#include "MT_Server.h"

static wxString get_socket_IPAddress(wxSocketBase* sock);

BEGIN_EVENT_TABLE(MT_Server, wxEvtHandler)
EVT_SOCKET(MT_SERVER_ID,  MT_Server::OnServerEvent)
EVT_SOCKET(MT_SOCKET_ID,  MT_Server::OnSocketEvent)
END_EVENT_TABLE()

MT_Server::MT_Server(long port)
: m_server(NULL)
{
    if(port > 0)
    {
        doInit(port);
    }
}

bool MT_Server::doInit(long port)
{
    // set parent frame for text functions
/*      m_trackerframe = trackerframe;
        m_frame = frame;*/
    m_lPort = port;
  
    // address defaults to localhost:0
    wxIPV4address addr;
    addr.Service(m_lPort);
  
    m_server = new wxSocketServer(addr);

    if (!m_server->Ok())
    {
        fprintf(stderr, "Failed to initialize server on port %ld.\n\n", m_lPort);
        return false;
    }
    else
    {
        fprintf(stdout, "Server listening on port %ld.\n\n", m_lPort);
    }

    m_server->SetEventHandler(*this, MT_SERVER_ID);
    m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_server->Notify(true);

    m_busy = false;
    m_numClients = 0;

    return true;

    /*m_tracker = NULL;*/
}

/*void MT_Server::SetTracker(predexpTracker* tracker)
  {
  m_tracker = tracker;
  }*/

void MT_Server::Test1(wxSocketBase* sock)
{
    unsigned char len = 204;

    fprintf(stdout, "Start Test 1.\n");

    sock->SetFlags(wxSOCKET_WAITALL);

    // read size and handshake
    sock->Read(&len, 1);
    char* buf = new char[1];
    buf[0] = 'a';
    sock->Write(buf, 1);
    delete[] buf;

    fprintf(stdout, "Data length is %d.\n", len);
    buf = new char[len];

    // read data
    sock->Read(buf, len);
    fprintf(stdout, "Data is");
    for (int i = 0; i < len; i++)
        fprintf(stdout, " %d", buf[i]);
    fprintf(stdout, ".\n");

    // write data back
    sock->Write(buf, len);
    delete[] buf;

    fprintf(stdout, "End Test 1.\n\n");
}

void MT_Server::Communicate(wxSocketBase* sock)
{
    char buffer[2];
    buffer[0] = 0;
    buffer[1] = 0;

    MT_message_type instr;

    sock->SetFlags(wxSOCKET_WAITALL);

    bool commEnd = false;
    while (!commEnd)
    {
        // get instruction
        instr = ReceiveMessage(sock, buffer);

        // process request
        switch (instr)
        {
        case MT_MSG_ACKNOWLEDGE:
            break;
        case MT_MSG_END:
            commEnd = true;
            break;
        case MT_MSG_IMGHEIGHT:
            //SendMessage(sock, buffer, m_trackerframe
            break;
        case MT_MSG_POSITION:
        {
            // send number of robots
            int numRobots = 10;
            int x, y;
            SendMessage(sock, buffer, numRobots);
            for (int i = 0; i < numRobots; i++)
            {
                /*tracker->*/
                x = i;
                y = numRobots - i;
                // get positions, send x then y
                SendMessage(sock, buffer, x);
                SendMessage(sock, buffer, y);
            }
        }
        break;
        default: 
            fprintf(stderr, "Unknown instruction receieved from client.\n");
        }
    }

}

MT_message_type MT_Server::ReceiveMessage(wxSocketBase* sock, char* buffer)
{
    sock->Read(buffer, 2);
    MT_message_type instr = (MT_message_type) TranslateFromBuffer(buffer);

    // send receipt
    if(instr != MT_MSG_ACKNOWLEDGE)
    {
        SendAcknowledge(sock, buffer);
    }

#ifdef MT_SERVER_DEBUG
    fprintf(stdout, "Received message %d from client, and acknowledged.\n", (int)instr);
#endif

    return instr;
}

void MT_Server::SendMessage(wxSocketBase* sock, char* buffer, int message)
{
    TranslateToBuffer(buffer, message);
    sock->Write(buffer, 2);

    // check for receipt
    if (!GetAcknowledge(sock, buffer))
        return;

#ifdef MT_SERVER_DEBUG
    fprintf(stdout, "Sent message %d to client, and received acknowledgement.\n", message);
#endif
}

void MT_Server::SendAcknowledge(wxSocketBase* sock, char* buffer)
{
    TranslateToBuffer(buffer, MT_MSG_ACKNOWLEDGE);
    sock->Write(buffer, 2);
}

bool MT_Server::GetAcknowledge(wxSocketBase* sock, char* buffer)
{
    sock->Read(buffer, 2);
    if (TranslateFromBuffer(buffer) != MT_MSG_ACKNOWLEDGE)
    {
        fprintf(stdout, "Handshake failed; client may not have received data.\n");
        return false;
    }

    return true;
}

void MT_Server::TranslateToBuffer(char* buffer, int value)
{
    buffer[1] = value >> 8;
    buffer[0] = value - buffer[1];
}

int MT_Server::TranslateFromBuffer(char* buffer)
{
    return (buffer[1] << 8) + buffer[0];
}

void MT_Server::OnServerEvent(wxSocketEvent& event)
{
    /*if (m_tracker == NULL)
      {
      #ifdef MT_SERVER_DEBUG
      fprintf(stdout, "Refusing connection, tracker is not yet initialized.\n");
      #endif
      return;
      }*/

#ifdef MT_SERVER_DEBUG
    fprintf(stdout, "Server event: ");

    // check for connection event
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
        fprintf(stdout, "wxSOCKET_CONNECTION.\n");
        break;
    default:
        fprintf(stdout, "Unexpected event.\n");
        break;
    }
#endif

    wxSocketBase* sock;

    // accept new connection if one is in queue, else exit
    // non-blocking accept from Accept(false)
    sock = m_server->Accept(false);

    if(sock)
    {
        wxString message = "New client connection to "
            + get_socket_IPAddress(sock) + " accepted.\n";
        fprintf(stdout, message.c_str());
        fprintf(stdout, "have %d connections\n\n", m_numClients+1);
    }
    else
    {
        fprintf(stderr, "Error: couldn't accept a new connection.\n\n");
        return;
    }

    sock->SetEventHandler(*this, MT_SOCKET_ID);
    sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    sock->Notify(true);

    m_numClients++;
}

void MT_Server::OnSocketEvent(wxSocketEvent& event)
{
#ifdef MT_SERVER_DEBUG
    fprintf(stdout, "Client event: ");

    // connection event
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_INPUT:
        fprintf(stdout, "wxSOCKET_INPUT.\n");
        break;
    case wxSOCKET_LOST:
        fprintf(stdout, "wxSOCKET_LOST.\n");
        break;
    default:
        fprintf(stdout, "Unexpected event.\n");
        break;
    }
#endif

    wxSocketBase* sock = event.GetSocket();

    // process event
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_INPUT:
    {
        /* disable input events, so that test doesn't trigger
         * wxSocketEvent again */
        sock->SetNotify(wxSOCKET_LOST_FLAG);

        // which function to execute?
        unsigned char c;
        sock->Read(&c, 1);
#ifdef MT_SERVER_DEBUG
        fprintf(stdout, "Received start command, %d.\n", c);
#endif

        switch (c)
        {
#ifdef MT_SERVER_DEBUG
        case 0xBE:
            Test1(sock);
            break;
            //case 0xCE:
            //  Test2(sock);
            //  break;
            //case 0xDE:
            //  Test3(sock);
            //  break;
#endif
        case 0xFE:
            Communicate(sock);
            break;
        default:
            fprintf(stdout, "Unknown test ID received from client.\n\n");
        }

        // enable input events again
        sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
        break;
    }
    case wxSOCKET_LOST:
    {
        m_numClients--;
        wxString message = "Lost connection to "
            + get_socket_IPAddress(sock) + ". Deleting socket.\n\n";
        fprintf(stdout, message.c_str());
        sock->Destroy();
        break;
    }
    default: ;
    }
}

MT_Server::~MT_Server()
{
    /* if the server was initialized, we need to clean it up */
    if(m_server)
    {
        /* safely wait for pending events then destroy */
        m_server->Destroy();
    }
}

static wxString get_socket_IPAddress(wxSocketBase* sock)
{
    wxIPV4address address;
    if(sock && sock->GetPeer(address))
    {
        return address.IPAddress();
    }
    else
    {
        return "unknown client";
    }
}
