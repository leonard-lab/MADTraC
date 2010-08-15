#include "MT_Server.h"
#include "MT/MT_Core/support/mathsupport.h"

#include "MT_Client.h"

#include <iostream>

const MT_Server::t_msg_def MT_Server::sentinel_msg = {0, 0, NULL, NULL};

MT_Server::t_msg_def MT_Server::builtin_msgs[] =
{
    {msg_END, msg_END, "End exchange", NULL},
    {msg_ACK, msg_ACK, "Acknowledge", NULL},
    {msg_OK, msg_OK, "OK", NULL},
    {msg_ERR, msg_ERR, "Error", NULL},
    {msg_UNKNOWN, msg_UNKNOWN, "Unknown response", NULL},
    {msg_UPTIME, msg_UPTIME, "Request server uptime", NULL},
    {msg_MOTD, msg_MOTD, "Message of the day", NULL},
    MT_Server::sentinel_msg /* sentinel */
};

MT_Server::t_msg_def* MT_ServerModule::setMessages(MT_Server::t_msg_def* messages)
{
    if(m_pMessages)
    {
        free(m_pMessages);
    }

    int count = MT_Server::countNumMessages(messages);

    m_pMessages = (MT_Server::t_msg_def *)calloc(count,
                                                 sizeof(MT_Server::t_msg_def));

    memcpy(m_pMessages, messages, count*sizeof(MT_Server::t_msg_def));

    return m_pMessages;
    
}


// IDs
enum
{
    MT_SERVER_ID = 100,
    MT_SOCKET_ID
};

static wxString get_socket_IPAddress(wxSocketBase* sock);

BEGIN_EVENT_TABLE(MT_Server, wxEvtHandler)
EVT_SOCKET(MT_SERVER_ID,  MT_Server::onServerEvent)
EVT_SOCKET(MT_SOCKET_ID,  MT_Server::onSocketEvent)
END_EVENT_TABLE()

bool MT_Server::m_bDebugOutput = false;
FILE* MT_Server::m_pDebugFile = stdout;

MT_Server::MT_Server()
: m_pSocketServer(NULL),
    m_bBusy(false),
    m_iNumClients(0),
    m_dStartupTime(MT_getTimeSec()),
    m_sMOTD("MADTraC Server v0.0000000!")
{
}

bool MT_Server::registerModule(MT_ServerModule* pModule)
{
    if(!pModule || !(pModule->doInit()))
    {
        return false;
    }

    t_msg_def* new_messages = pModule->getMessageDefs();
    if(!new_messages) { return false; };

    return addMessagesToTable(new_messages);
}

MT_Server::t_msg MT_Server::getFirstAvailableMessageCode()
{
    unsigned int sz = m_vMessageTable.size();
    /* the first message should go on 1, not 0 */
    if(sz == 0)
    {
        sz = 1;
    }
    return sz;
}

bool MT_Server::addMessagesToTable(t_msg_def* add_messages)
{
    if(!add_messages)
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Null pointer passed to MT_Server::addMessagesToTable");
        }
        return false;
    }

    t_msg c_msg_code = getFirstAvailableMessageCode();

    t_msg_def* c_msg = add_messages;
    t_msg_def c_msg_copy;
    while(!isSentinelMessage(c_msg))
    {
        c_msg->server_code = c_msg_code++;
        memcpy(&c_msg_copy, c_msg, sizeof(t_msg_def));
        m_vMessageTable.push_back(c_msg_copy);
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Registered message definition: %s with server code %d, "
                    "client code %d, and handler %ld\n",
                    c_msg_copy.description,
                    c_msg_copy.server_code,
                    c_msg_copy.client_code,
                    (long int) c_msg_copy.handler);
        }
        c_msg++;
   }

    return true;
    
}

MT_Server::t_msg_def MT_Server::makeMessage(unsigned char server_code,
                                            unsigned char client_code,
                                            const char* desc,
                                            MT_ServerModule* handler)
{
    t_msg_def message = {server_code, client_code, desc, handler};
    return message;
}

bool MT_Server::isSentinelMessage(t_msg_def* p_message_def)
{

    return (bool) (p_message_def && (p_message_def->server_code == 0)
                   && (p_message_def->client_code == 0)
                   && !p_message_def->description
                   && !p_message_def->handler);
}

int MT_Server::countNumMessages(t_msg_def* message_list)
{
    int count = 0;
    t_msg_def* c_msg = message_list;
    while(!isSentinelMessage(c_msg))
    {
        count++;
        c_msg++;
    }
    return ++count;
}

bool MT_Server::initMessageTable()
{
    m_vMessageTable.resize(0);
    /* adding a blank handler at 0 simplifies indexing below */
    m_vMessageTable.push_back(sentinel_msg);
    return addMessagesToTable(builtin_msgs);
}

double MT_Server::getUptime()
{
    return MT_getTimeSec() - m_dStartupTime;
}

MT_Server::err_code MT_Server::doInit(long port)
{
    if(port <= 0)
    {
        return err_invalid_port;
    }
    
    m_lPort = port;

    if(!initMessageTable())
    {
        fprintf(stderr,
                "MT_Server Fatal Error: Failed to initialize message table.\n");
        return err_msg_table_init_failed;
    }
  
    // address defaults to localhost:0
    wxIPV4address addr;
    addr.Service(m_lPort);
    
    /* wxSOCKET_REUSEADDR should avoid most errors about not being
     * able to use the port after a crash */
    m_pSocketServer = new wxSocketServer(addr, wxSOCKET_REUSEADDR);
    
    m_pSocketServer->SetFlags(wxSOCKET_REUSEADDR);

    if (!m_pSocketServer->Ok())
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Failed to initialize server on port %ld.\n\n", m_lPort);
        }
        return err_init_failed;
    }
    else
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile, "Server listening on port %ld.\n\n", m_lPort);
        }
    }

    m_pSocketServer->SetEventHandler(*this, MT_SERVER_ID);
    m_pSocketServer->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_pSocketServer->Notify(true);

    m_bBusy = false;
    m_iNumClients = 0;

    return err_no_error;

}

void MT_Server::onServerEvent(wxSocketEvent& event)
{

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "Server event: ");
        fflush(m_pDebugFile);

        // check for connection event
        switch(event.GetSocketEvent())
        {
        case wxSOCKET_CONNECTION:
            fprintf(m_pDebugFile, "wxSOCKET_CONNECTION.\n");
            break;
        default:
            fprintf(m_pDebugFile, "Unexpected event.\n");
            break;
        }
    }

    wxSocketBase* sock;

    // accept new connection if one is in queue, else exit
    // non-blocking accept from Accept(false)
    sock = m_pSocketServer->Accept(false);

    if(sock)
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "New client connection to %s accepted.\n"
                    "  Now have %d connections.\n",
                    (const char *) get_socket_IPAddress(sock).mb_str(),
                    m_iNumClients+1);
        }
    }
    else
    {
        fprintf(stderr, "Error: couldn't accept a new connection.\n\n");
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Error: Couldn't accept a new connection.\n");
        }
        return;
    }

    sock->SetEventHandler(*this, MT_SOCKET_ID);
    sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    sock->Notify(true);

    m_iNumClients++;
}

void MT_Server::onSocketEvent(wxSocketEvent& event)
{

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "Client event: ");
        fflush(m_pDebugFile);

        // connection event
        switch(event.GetSocketEvent())
        {
        case wxSOCKET_INPUT:
            fprintf(m_pDebugFile, "wxSOCKET_INPUT.\n");
            break;
        case wxSOCKET_LOST:
            fprintf(m_pDebugFile, "wxSOCKET_LOST.\n");
            break;
        default:
            fprintf(m_pDebugFile, "Unexpected event.\n");
            break;
        }
    }

    wxSocketBase* sock = event.GetSocket();

    // process event
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_INPUT:
    {
        /* disable input events, so that this exchange doesn't trigger
         * wxSocketEvent again */
        sock->SetNotify(wxSOCKET_LOST_FLAG);

        // which function to execute?
        t_cmd c;
        sock->Read(&c, 1);

        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Received command %X from %s.\n",
                    c,
                    (const char *) get_socket_IPAddress(sock).mb_str());
        }

        switch (c)
        {
        case cmd_Init:
            Communicate(sock);
            break;
        case cmd_Ping:
            PingReply(sock);
            break;
        case cmd_List:
            SendMessageList(sock);
            break;
        case msg_ACK:  /* ignore spurious ACKs */
            break;
        default:
            if(m_bDebugOutput)
            {
                fprintf(m_pDebugFile,
                        "Error:  Unrecognized command %X from %s.\n",
                        c,
                        (const char*) get_socket_IPAddress(sock).mb_str());
            }
            MT_SendMessage(msg_UNKNOWN, sock);
        }

        // enable input events again
        sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
        break;
    }
    case wxSOCKET_LOST:
    {
        m_iNumClients--;
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Lost connection to client %s\n",
                    (const char*) get_socket_IPAddress(sock).mb_str());
        }
        sock->Destroy();
        break;
    }
    default: ;
    }
}

void MT_Server::SendMessageList(wxSocketBase* sock)
{

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile,
                "Client has requested message table.\n");
    }
    
    sock->SetFlags(wxSOCKET_WAITALL);

    t_msg ok = msg_OK;
    sock->Write(&ok, 1);

    /* send the number of messages */
    MT_SendInt(m_vMessageTable.size(), sock);

    for(unsigned int i = 0; i < m_vMessageTable.size(); i++)
    {
        MT_SendMessage(m_vMessageTable[i].server_code, sock);
        MT_SendMessage(m_vMessageTable[i].client_code, sock);
        if(m_vMessageTable[i].description)
        {
            MT_SendString(std::string(m_vMessageTable[i].description), sock);
        }
        else
        {
            MT_SendString(std::string("!NULL!"), sock);
        }
        if(m_vMessageTable[i].handler)
        {
            MT_SendString(m_vMessageTable[i].handler->getName(), sock);
        }
        else
        {
            MT_SendString("MT_Server", sock);
        }
    }
    
}

void MT_Server::PingReply(wxSocketBase* sock)
{
    sock->SetFlags(wxSOCKET_WAITALL);
    MT_SendMessage(msg_OK, sock);
}

void MT_Server::Communicate(wxSocketBase* sock)
{

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "Initiating exchange with client %s\n",
                (const char*) get_socket_IPAddress(sock).mb_str());
    }
    
    t_msg instr;

    sock->SetFlags(wxSOCKET_WAITALL);

    MT_SendMessage(msg_OK, sock);

    bool commEnd = false;
    while (!commEnd)
    {
        if(!sock->IsConnected())
        {
            commEnd = true;
        }
        
        // get instruction
        instr = MT_ReceiveMessage(sock);

        if(instr == msg_END)
        {
            break;
        }

        bool message_handled = false;
        if(instr <= m_vMessageTable.size())
        {
            if(!m_vMessageTable[instr].handler)
            {
                if(m_bDebugOutput)
                {
                    fprintf(m_pDebugFile,
                            "Handling built-in message\n");
                }
                message_handled = handleMessage(instr, sock);
            }
            else
            {
                if(m_bDebugOutput)
                {
                    fprintf(m_pDebugFile,
                            "Delegating message to handler %ld\n",
                            (long int) m_vMessageTable[instr].handler);
                }
                message_handled =
                    m_vMessageTable[instr].handler->handleMessage(instr, sock);
            }
        }
        else
        {
            if(m_bDebugOutput)
            {
                fprintf(m_pDebugFile,
                        "Unknown message %d receieved from client %s.\n",
                        instr,
                        (const char*) get_socket_IPAddress(sock).mb_str());
            }
        }
        
        if(!message_handled)
        {
            MT_SendMessage(msg_UNKNOWN, sock);
        }

    }

    if(sock->IsConnected())
    {
        MT_SendMessage(msg_END, sock);
    }

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "Exchange with client %s ended.\n",
                (const char*) get_socket_IPAddress(sock).mb_str());
    }
    
}

bool MT_Server::handleMessage(t_msg msg_code, wxSocketBase* sock)
{
    bool handled = false;

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile,
                "Handling built-in message %d\n",
                msg_code);
    }
    
    switch(msg_code)
    {
    case msg_UPTIME:
        sendUptime(sock);
        handled = true;
        break;
    case msg_MOTD:
        sendMOTD(sock);
        handled = true;
        break;
    default:
        break;
    };
    
    if(!handled && m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "Unhandled message %d.\n", msg_code);
    }

    if(!handled)
    {
        fprintf(stderr, "MT_Server Error:  Unhandled message %d\n", msg_code);
    }
    
    return handled;
}

void MT_Server::sendMOTD(wxSocketBase* sock)
{
    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "Client %s requested MOTD, "
                "which should be %s.\n",
                (const char*) get_socket_IPAddress(sock).mb_str(),
                m_sMOTD.c_str());
    }

    MT_SendString(m_sMOTD, sock);
    
}

void MT_Server::sendUptime(wxSocketBase* sock)
{
    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "Client %s requested uptime, "
                "which should be %f.\n",
                (const char*) get_socket_IPAddress(sock).mb_str(),
                getUptime());
    }
    double t = getUptime();
    MT_SendDouble(t, sock);
}

MT_Server::~MT_Server()
{

    for(unsigned int i = 0; i < m_vpModules.size(); i++)
    {
        delete m_vpModules[i];
    }
    
    /* if the server was initialized, we need to clean it up */
    if(m_pSocketServer)
    {
        /* safely wait for pending events then destroy */
        m_pSocketServer->Destroy();
    }

}


MT_Server::t_msg MT_SendCommand(MT_Server::t_cmd command, wxSocketBase* sock)
{
    sock->Write(&command, sizeof(MT_Server::t_cmd));
    MT_Server::t_msg reply = MT_Server::msg_ERR;
    sock->Read(&reply, sizeof(MT_Server::t_cmd));
    return reply;
}

bool MT_SendMessage(MT_Server::t_msg message, wxSocketBase* sock)
{
    sock->Write(&message, sizeof(MT_Server::t_cmd));
    return MT_GetAck(sock);
}

MT_Server::t_msg MT_ReceiveMessage(wxSocketBase* sock, bool force_ack)
{
    MT_Server::t_msg instr = MT_Server::msg_ERR;
    sock->Read(&instr, sizeof(MT_Server::t_cmd));

    // send acknowledge unless this was an ack or it was explicitly
    // asked for
    if(force_ack || instr != MT_Server::msg_ACK)
    {
        MT_SendAck(sock);
    }

    return instr;
}

void MT_SendAck(wxSocketBase* sock)
{
    MT_Server::t_msg ack = MT_Server::msg_ACK;
    sock->Write(&ack, sizeof(MT_Server::t_cmd));
}

bool MT_GetAck(wxSocketBase* sock)
{
    MT_Server::t_msg buffer;
    sock->Read(&buffer, sizeof(MT_Server::t_cmd));
    return (buffer == MT_Server::msg_ACK);
}

bool MT_SendInt(int val, wxSocketBase* sock)
{
    sock->Write(&val, sizeof(int));
    return MT_GetAck(sock);
}

int MT_ReadInt(wxSocketBase* sock)
{
    int result;
    sock->Read(&result, sizeof(int));
    MT_SendAck(sock);
    return result;    
}

bool MT_SendDouble(double val, wxSocketBase* sock)
{
    sock->Write(&val, sizeof(double));
    return MT_GetAck(sock);
}

bool MT_SendDoubleArray(double* arr, unsigned int num, wxSocketBase* sock)
{
    sock->Write(arr, num*sizeof(double));
    return MT_GetAck(sock);
}

double MT_ReadDouble(wxSocketBase* sock)
{
    double result;
    sock->Read(&result, sizeof(double));
    MT_SendAck(sock);
    return result;
}

bool MT_SendString(std::string str, wxSocketBase* sock)
{
    return MT_SendCharArray(str.c_str(), str.length()+1, sock);
}

bool MT_SendCharArray(const char* arr,
                      unsigned int length,
                      wxSocketBase* sock)
{
    MT_SendInt(length, sock);
    
    /* we need a non-const version of the string */
    char* c = (char *)calloc(length, sizeof(char));
    memcpy(c, arr, length*sizeof(char));
    sock->Write(c, length*sizeof(char));
    free(c);

    return MT_GetAck(sock);
}

std::string MT_ReadString(wxSocketBase* sock)
{
    unsigned int length = MT_ReadInt(sock);

    char* arr = (char *)calloc(length, sizeof(char));
    sock->Read(arr, length*sizeof(char));
    
    MT_SendAck(sock);

    std::string result(arr);

    free(arr);
    
    return result;    
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
        return wxT("unknown client");
    }
}

