#ifndef MT_SERVER_MODULES
#define MT_SERVER_MODULES

#include "wx/frame.h"

#include "MT_Server.h"



class MT_SM_WindowProps : public MT_ServerModule
{
private:
    wxFrame* m_pFrame;
    
protected:
    bool handleMessage(MT_Server::t_msg msg_code, wxSocketBase* sock);
    MT_Server::t_msg_def* getMessageDefs();

    enum
    {
        msg_WinWidth = 0,
        msg_WinHeight,
        msg_ClientWidth,
        msg_ClientHeight,
        msg_GetWinX,
        msg_GetWinY,
        msg_SetWinX,
        msg_SetWinY,
        msg_Sentinel
    } msg_index;

public:
    MT_SM_WindowProps() : MT_ServerModule("WindowProps"), m_pFrame(NULL){};
    MT_SM_WindowProps(MT_Server* pServer, wxFrame* pFrame)
        : MT_ServerModule(pServer, "WindowProps"), m_pFrame(pFrame){};

    /* server methods */
    void sendWinWidth(wxSocketBase* sock);
    void sendWinHeight(wxSocketBase* sock);
    void sendClientWidth(wxSocketBase* sock);
    void sendClientHeight(wxSocketBase* sock);
    void sendWindowX(wxSocketBase* sock); 
    void sendWindowY(wxSocketBase* sock);
    void setWindowX(wxSocketBase* sock);
    void setWindowY(wxSocketBase* sock);
    
    /* client methods */
    int getWinWidth(wxSocketBase* sock);
    int getWinHeight(wxSocketBase* sock);    
    int getClientWidth(wxSocketBase* sock);
    int getClientHeight(wxSocketBase* sock);
    int getWindowX(wxSocketBase* sock);
    int getWindowY(wxSocketBase* sock);
    int requestWindowX(int X, wxSocketBase* sock);
    int requestWindowY(int Y, wxSocketBase* sock);

};


#endif // MT_SERVER_MODULES
