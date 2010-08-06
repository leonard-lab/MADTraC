#include "MT_Client.h"

#include <iostream>
#include <sstream>
#include <iomanip>

MT_Client::MT_Client()
    : m_pSocketClient(NULL),
      m_bDebugOutput(false),
      m_pDebugFile(stdout),
      m_bIsConnected(false)
{
}

MT_Client::~MT_Client()
{
    if(m_pSocketClient)
    {
        m_pSocketClient->Destroy();
    }
}

MT_Server::err_code MT_Client::Connect(wxString hostname,
                                       long port)
{
    if(port <= 0)
    {
        return MT_Server::err_invalid_port;
    }

    if(!m_pSocketClient)
    {
        m_pSocketClient = new wxSocketClient();
    }
        
    wxIPV4address addr;
    addr.Hostname(hostname);
    addr.Service(port);

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile,
                "Attempting to connect to server at %s:%ld\n",
                hostname.mb_str(),
                port);
    }

    /* non-blocking connect */
    m_pSocketClient->Connect(addr, false);
    /* wait  */
    m_pSocketClient->WaitOnConnect(10);

    if(!m_pSocketClient->IsConnected())
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Failure to connect to server at %s:%ld\n",
                    hostname.mb_str(),
                    port);
        }
        return MT_Server::err_connect_failed;
    }
    else
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Success connecting to server at %s:%ld\n",
                    hostname.mb_str(),
                    port);
        }
        m_bIsConnected = true;
        return MT_Server::err_no_error;
    }
}

bool MT_Client::PingServer()
{
    if(!m_bIsConnected)
    {
        return false;
    }

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile,
                "Initiating ping to server.\n");
    }

    MT_Server::t_msg reply = MT_SendCommand(MT_Server::cmd_Ping,
                                            m_pSocketClient);

    if(reply != MT_Server::msg_OK)
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Error in ping reply.  Got %d, expecting %d\n",
                    reply,
                    MT_Server::msg_OK);
        }
        return false;
    }
    else
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Success.\n");
        }
        MT_SendAck(m_pSocketClient);
    }
}

double MT_Client::getServerUptime()
{
    if(!m_bIsConnected)
    {
        return -1.0;
    }

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "Getting uptime from server.\n");
    }

    double t = -1.0;

    bool sent =  MT_SendMessage(MT_Server::msg_UPTIME,
                                m_pSocketClient);

    if(!sent)
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Error:  Unable to send message to server..\n");
        }
        return t;
    }
        
    t = MT_ReadDouble(m_pSocketClient);
    
    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "Uptime was %f\n", t);
    }
    
    return t;

}

bool MT_Client::InitiateExchange()
{
    if(!m_bIsConnected)
    {
        return false;
    }

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile,
                "Initiating exchange with server.\n");
    }

    MT_Server::t_cmd init = MT_Server::cmd_Init;
    MT_Server::t_msg reply = MT_Server::msg_ERR;
    m_pSocketClient->Write(&init, 1);
    m_pSocketClient->Read(&reply, 1);

    if(reply != MT_Server::msg_OK)
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Error in exchange reply.  Got %d, expecting %d\n",
                    reply,
                    MT_Server::msg_OK);
        }
        return false;
    }
    else
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Succes.\n");
        }
        MT_SendAck(m_pSocketClient);
        return true;
    }
}

bool MT_Client::EndExchange()
{
    if(!m_bIsConnected)
    {
        return false;
    }

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile,
                "Ending exchange with server.\n");
    }

    MT_Server::t_msg msg = MT_Server::msg_END;
    MT_Server::t_msg reply = MT_Server::msg_ERR;
    m_pSocketClient->Write(&msg, 1);
    m_pSocketClient->Read(&reply, 1); /* ack */
    m_pSocketClient->Read(&reply, 1); /* end */

    MT_SendAck(m_pSocketClient);

    if(reply != MT_Server::msg_END)
    {
        if(m_bDebugOutput)
        { 
            fprintf(m_pDebugFile,
                    "Error in exchange reply.  Got %d, expecting %d\n",
                    reply,
                    MT_Server::msg_END);
        }
        return false;
    }
    else
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Succes.\n");
        }
        return true;
    }    
}

std::string MT_Client::getMOTD()
{
    if(!m_bIsConnected)
    {
        return std::string("!error: not connected!");
    }

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "Getting MOTD from server.\n");
    }

    MT_Server::t_msg motd = MT_Server::msg_MOTD;
    MT_Server::t_msg ack = MT_Server::msg_ACK;
    std::string MOTD("!error: couldn't get MOTD!");
    
    m_pSocketClient->Write(&motd, 1);
    m_pSocketClient->Read(&ack, 1);

    MOTD = MT_ReadString(m_pSocketClient);
    
    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile, "MOTD was %s\n", MOTD.c_str());
    }

    return MOTD;
    
}

void MT_Client::getMessageTable()
{
    if(!m_bIsConnected){return;};

    if(m_bDebugOutput)
    {
        fprintf(m_pDebugFile,
                "Requesting message table.\n");
    }

    MT_Server::t_cmd list_cmd = MT_Server::cmd_List;
    MT_Server::t_msg reply = MT_Server::msg_ERR;
    m_pSocketClient->Write(&list_cmd, 1);
    m_pSocketClient->Read(&reply, 1);

    if(reply != MT_Server::msg_OK)
    {
        if(m_bDebugOutput)
        {
            fprintf(m_pDebugFile,
                    "Error in exchange reply.  Got %d, expecting %d\n",
                    reply,
                    MT_Server::msg_OK);
        }
        return;
    }

    //MT_SendAck(m_pSocketClient);

    int n_messages = MT_ReadInt(m_pSocketClient);

    t_msg_table_entry c_entry;
    for(unsigned int i = 0; i < n_messages; i++)
    {
        /* force ack because the message might *be* an ack */
        c_entry.server_code = MT_ReceiveMessage(m_pSocketClient, true);
        c_entry.client_code = MT_ReceiveMessage(m_pSocketClient, true);
        c_entry.description = MT_ReadString(m_pSocketClient);
        c_entry.owner_name = MT_ReadString(m_pSocketClient);
        m_MessageTable.push_back(c_entry);
    }

    std::string mod_name;
    unsigned char s_c;
    /* skip the first one */
    for(unsigned int i = 1; i < m_MessageTable.size(); i++)
    {
        mod_name = m_MessageTable[i].owner_name;
        s_c = m_MessageTable[i].server_code;
        if(mod_name.compare("MT_Server") == 0)
        {
            /* TODO should also message map server... */
        }
        else
        {
            for(unsigned int j = 0; j < m_vpModules.size(); j++)
            {
                if(mod_name.compare(m_vpModules[j]->getName()) == 0)
                {
                    m_vpModules[j]->m_vMessageMap.push_back(s_c);
                }
            }
        }
    }

}

std::string MT_Client::getMessageTableAsString()
{
    std::ostringstream s;

    unsigned int w_d = 30;
    unsigned int w_c = 12;
    unsigned int w_m = 20;

    s << std::left << std::setw(w_d) << "Header" <<
        std::right <<
        std::setw(w_c) << "Server Code" <<
        std::setw(w_c) << "Client Code" << 
        std::left << " " <<
        std::setw(w_m) << "Module Name" << std::endl;
    
    
    for(unsigned int i = 0; i < m_MessageTable.size(); i++)
    {
        std::ostringstream s2;
        s2 << "'" << m_MessageTable[i].description << "'";
        s << std::left <<
            std::setw(w_d) << s2.str() <<
            std::right <<
            std::setw(w_c - 3) << (int) m_MessageTable[i].server_code <<
            "   " << 
            std::setw(w_c - 3) << (int) m_MessageTable[i].client_code <<
            "   " <<
            std::left << " " <<
            std::setw(w_m) << m_MessageTable[i].owner_name <<
            std::endl;
    }
    return s.str();
}

bool MT_Client::addModule(MT_ServerModule* p_module)
{
    if(!p_module->isForClient()){return false;};

    m_vpModules.push_back(p_module);
}
