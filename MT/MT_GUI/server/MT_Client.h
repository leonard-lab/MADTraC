#ifndef MT_CLIENT
#define MT_CLIENT

#include "wx/socket.h"
#include "MT_Server.h"

class MT_Client : public wxEvtHandler
{
public:

    typedef struct t_msg_table_entry
    {
        unsigned char server_code;
        unsigned char client_code;
        std::string description;
        std::string owner_name;
    } t_msg_table_entry;
    
private:
    
    wxSocketClient* m_pSocketClient;

    bool m_bDebugOutput;
    FILE* m_pDebugFile;

    bool m_bIsConnected;
    
    static const int s_iConnectionTimeout = 2; /* sec */

    std::vector<t_msg_table_entry> m_MessageTable;
    std::vector<MT_ServerModule*> m_vpModules;
    
protected:

public:
    MT_Client();
    ~MT_Client();

    bool addModule(MT_ServerModule* p_module);

    MT_Server::err_code Connect(wxString hostname,
                                long port = MT_Server::s_lDefaultPort);

    void enableDebugOutput(FILE* setfile = stdout)
    {m_bDebugOutput = true; m_pDebugFile = setfile;};
    void disableDebugOutput(){m_bDebugOutput = false;};

    bool InitiateExchange();
    bool EndExchange();
    
    bool PingServer();
    
    double getServerUptime();
    std::string getMOTD();

    void getMessageTable();

    std::string getMessageTableAsString();

    wxSocketClient* getSocket(){return m_pSocketClient;};

};

#endif // MT_CLIENT
