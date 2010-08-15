#ifndef MT_SERVER
#define MT_SERVER

#include "wx/socket.h"

#include <vector>
#include <string>

class MT_ServerModule;
class MT_Client;

class MT_Server : public wxEvtHandler
{
    friend class MT_ServerModule;
public:
    typedef unsigned char t_cmd;
    typedef unsigned char t_msg;

    typedef struct t_msg_def
    {
        unsigned char server_code;
        unsigned char client_code;
        const char* description;
        MT_ServerModule* handler;
    } t_msg_def;

    static const long s_lDefaultPort = 3000;
    static const long s_lNoInit = -1;

    static const t_cmd cmd_Init = 0xFE;
    static const t_cmd cmd_Ping = 0xFD;
    static const t_cmd cmd_List = 0xFC;
    
    typedef enum
    {
        err_no_error,
        err_invalid_port,
        err_init_failed,
        err_ack_failed,
        err_connect_failed,
        err_msg_table_init_failed
    } err_code;

    /* NOTE: when adding a built-in message, you should add the code
     * here AND the definition in builtin_msgs in the source file */
    enum
    {
        msg_END = 1,
        msg_ACK,
        msg_OK,
        msg_ERR,
        msg_UNKNOWN,
        msg_UPTIME,
        msg_MOTD,
    } builtin_msg_codes;

    /* defined below */
    static const t_msg_def sentinel_msg;
    static t_msg_def builtin_msgs[];
            
private:

    wxSocketServer* m_pSocketServer;
    bool m_bBusy;
    int m_iNumClients;

    static bool m_bDebugOutput;
    static FILE* m_pDebugFile;

    double m_dStartupTime;
    double getUptime();

    std::vector<t_msg_def> m_vMessageTable;
    std::vector<MT_ServerModule*> m_vpModules;
    bool addMessagesToTable(t_msg_def* p_msgs);
    bool initMessageTable();
    
    void onServerEvent(wxSocketEvent& event);
    void onSocketEvent(wxSocketEvent& event);

    /* Command responses */
    void Communicate(wxSocketBase* sock);
    void PingReply(wxSocketBase* sock);
    void SendMessageList(wxSocketBase* sock);
    
    void sendUptime(wxSocketBase* sock);
    void sendMOTD(wxSocketBase* sock);

    bool handleMessage(t_msg msg_code, wxSocketBase* sock);
    
protected:
    long m_lPort;
    std::string m_sMOTD;
    
    t_msg ReceiveMessage(wxSocketBase* sock);
    err_code SendMessage(wxSocketBase* sock,
                         t_msg message);
    static void SendAcknowledge(wxSocketBase* sock);
    static err_code GetAcknowledge(wxSocketBase* sock);

    t_msg getFirstAvailableMessageCode();
    
public:

    MT_Server();
    ~MT_Server();

    err_code doInit(long port = s_lDefaultPort);

    static void enableDebugOutput(FILE* setfile = stdout)
    {m_bDebugOutput = true; m_pDebugFile = setfile;};
    static void disableDebugOutput(){m_bDebugOutput = false;};

    bool registerModule(MT_ServerModule* pModule);    

    static int countNumMessages(t_msg_def* message_list);
    static bool isSentinelMessage(t_msg_def* p_message_def);
    static t_msg_def makeMessage(unsigned char server_code,
                                 unsigned char client_code,
                                 const char* desc,
                                 MT_ServerModule* handler);

    static void sendInt(int val, wxSocketBase* sock);
    static void sendString(std::string str, wxSocketBase* sock);
    static void sendCharArray(const char* arr,
                       unsigned int length,
                       wxSocketBase* sock);

    static int readInt(wxSocketBase* sock);

private:
    DECLARE_EVENT_TABLE()    
};

MT_Server::t_msg MT_SendCommand(MT_Server::t_cmd command, wxSocketBase* sock);
bool MT_SendMessage(MT_Server::t_msg message, wxSocketBase* sock);
MT_Server::t_msg MT_ReceiveMessage(wxSocketBase* sock, bool force_ack = false);
void MT_SendAck(wxSocketBase* sock);
bool MT_GetAck(wxSocketBase* sock);
bool MT_SendInt(int val, wxSocketBase* sock);
int MT_ReadInt(wxSocketBase* sock);
bool MT_SendString(std::string str, wxSocketBase* sock);
bool MT_SendCharArray(const char* arr,
                      unsigned int length,
                      wxSocketBase* sock);
bool MT_SendDouble(double val, wxSocketBase* sock);
bool MT_SendDoubleArray(double* arr, unsigned int num, wxSocketBase* sock);
double MT_ReadDouble(wxSocketBase* sock);
std::string MT_ReadString(wxSocketBase* sock);

class MT_ServerModule
{
    friend class MT_Server;
    friend class MT_Client;
private:

protected:
    
    MT_Server* m_pServer;    
    MT_Server::t_msg_def* m_pMessages;
    std::string m_sName;
    
    virtual bool handleMessage(MT_Server::t_msg msg_code,
                       wxSocketBase* sock)
    {return false;};

    std::vector<unsigned char> m_vMessageMap;
    
    virtual MT_Server::t_msg_def* getMessageDefs(){return m_pMessages;};
    virtual MT_Server::t_msg_def* setMessages(MT_Server::t_msg_def* messages);

public:
    MT_ServerModule(const char* name)
        : m_pServer(NULL), m_pMessages(NULL), m_sName(name) {};
    MT_ServerModule(MT_Server* pServer, const char* name)
        : m_pServer(pServer), m_pMessages(NULL), m_sName(name) {};
    virtual ~MT_ServerModule(){if(m_pMessages) free(m_pMessages);};

    virtual bool doInit() {return true;};

    std::string getName(){return m_sName;};

    bool isForClient(){return (m_pServer == NULL);};
};

#endif
