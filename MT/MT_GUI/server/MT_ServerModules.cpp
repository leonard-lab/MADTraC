#include "MT_ServerModules.h"
#include "MT_Client.h"

MT_Server::t_msg_def* MT_SM_WindowProps::getMessageDefs()
{
    MT_Server::t_msg_def messages[9];

    messages[msg_WinWidth] = MT_Server::makeMessage(0,
                                                    msg_WinWidth,
                                                    "Window width",
                                                    this);
    messages[msg_WinHeight] = MT_Server::makeMessage(0,
                                                     msg_WinHeight,
                                                     "Window height",
                                                     this);
    messages[msg_ClientWidth] = MT_Server::makeMessage(0,
                                                     msg_ClientWidth,
                                                     "Client width",
                                                     this);
    messages[msg_ClientHeight] = MT_Server::makeMessage(0,
                                                     msg_ClientHeight,
                                                        "Client height",
                                                     this);
    messages[msg_GetWinX] = MT_Server::makeMessage(0,
                                                   msg_GetWinX,
                                                   "Window Pos X",
                                                   this);
    messages[msg_GetWinY] = MT_Server::makeMessage(0,
                                                   msg_GetWinY,
                                                   "Window Pos Y",
                                                   this);
    messages[msg_SetWinX] = MT_Server::makeMessage(0,
                                                   msg_SetWinX,
                                                   "Set Window Pos Y",
                                                   this);
    messages[msg_SetWinY] = MT_Server::makeMessage(0,
                                                   msg_SetWinY,
                                                   "Set Window Pos Y",
                                                   this);
    
    messages[msg_Sentinel] = MT_Server::sentinel_msg;

    return setMessages(messages);
}

bool MT_SM_WindowProps::handleMessage(MT_Server::t_msg msg_code,
                                      wxSocketBase* sock)
{
    if(msg_code == m_pMessages[msg_WinWidth].server_code)
    {
        sendWinWidth(sock);
        return true;
    }
    if(msg_code == m_pMessages[msg_WinHeight].server_code)
    {
        sendWinHeight(sock);
        return true;
    }
    if(msg_code == m_pMessages[msg_ClientWidth].server_code)
    {
        sendClientWidth(sock);
        return true;
    }
    if(msg_code == m_pMessages[msg_ClientHeight].server_code)
    {
        sendClientHeight(sock);
        return true;
    }
    if(msg_code == m_pMessages[msg_GetWinX].server_code)
    {
        sendWindowX(sock);
        return true;
    }
    if(msg_code == m_pMessages[msg_GetWinY].server_code)
    {
        sendWindowY(sock);
        return true;
    }
    if(msg_code == m_pMessages[msg_SetWinX].server_code)
    {
        setWindowX(sock);
        return true;
    }
    if(msg_code == m_pMessages[msg_SetWinY].server_code)
    {
        setWindowY(sock);
        return true;
    }
    
    return false;

}

void MT_SM_WindowProps::sendWinWidth(wxSocketBase* sock)
{
    wxSize sz = m_pFrame->GetSize();
    int w = sz.GetWidth();
    MT_SendInt(w, sock);
}

void MT_SM_WindowProps::sendWinHeight(wxSocketBase* sock)
{
    wxSize sz = m_pFrame->GetSize();
    int w = sz.GetHeight();
    MT_SendInt(w, sock);
}

void MT_SM_WindowProps::sendClientWidth(wxSocketBase* sock)
{
    wxSize sz = m_pFrame->GetClientSize();
    int w = sz.GetWidth();
    MT_SendInt(w, sock);
}

void MT_SM_WindowProps::sendClientHeight(wxSocketBase* sock)
{
    wxSize sz = m_pFrame->GetClientSize();
    int w = sz.GetHeight();
    MT_SendInt(w, sock);
}

int MT_SM_WindowProps::getWinWidth(wxSocketBase* sock)
{
    if(m_vMessageMap.size() <= msg_WinWidth){return 0;};

    MT_SendMessage(m_vMessageMap[msg_WinWidth], sock);
    return MT_ReadInt(sock);
}

int MT_SM_WindowProps::getWinHeight(wxSocketBase* sock)
{
    if(m_vMessageMap.size() <= msg_WinHeight){return 0;};

    MT_SendMessage(m_vMessageMap[msg_WinHeight], sock);
    return MT_ReadInt(sock);
}

int MT_SM_WindowProps::getClientWidth(wxSocketBase* sock)
{
    if(m_vMessageMap.size() <= msg_ClientWidth){return 0;};
    
    MT_SendMessage(m_vMessageMap[msg_ClientWidth], sock);
    return MT_ReadInt(sock);
}

int MT_SM_WindowProps::getClientHeight(wxSocketBase* sock)
{
    if(m_vMessageMap.size() <= msg_ClientHeight){return 0;};

    MT_SendMessage(m_vMessageMap[msg_ClientHeight], sock);    
    return MT_ReadInt(sock);
}

void MT_SM_WindowProps::sendWindowX(wxSocketBase* sock)
{
    wxPoint p = m_pFrame->GetPosition();
    int x = p.x;
    MT_SendInt(x, sock);
}
void MT_SM_WindowProps::sendWindowY(wxSocketBase* sock)
{
    wxPoint p = m_pFrame->GetPosition();
    int y = p.y;
    MT_SendInt(y, sock);
}
void MT_SM_WindowProps::setWindowX(wxSocketBase* sock)
{
    wxPoint p = m_pFrame->GetPosition();
    p.x = MT_ReadInt(sock);
    m_pFrame->Move(p);
}
void MT_SM_WindowProps::setWindowY(wxSocketBase* sock)
{
    wxPoint p = m_pFrame->GetPosition();
    p.y = MT_ReadInt(sock);
    m_pFrame->Move(p);
}

int MT_SM_WindowProps::getWindowX(wxSocketBase* sock)
{
    if(m_vMessageMap.size() <= msg_GetWinX){return 0;};

    MT_SendMessage(m_vMessageMap[msg_GetWinX], sock);
    return MT_ReadInt(sock);
}
int MT_SM_WindowProps::getWindowY(wxSocketBase* sock)
{
    if(m_vMessageMap.size() <= msg_GetWinY){return 0;};

    MT_SendMessage(m_vMessageMap[msg_GetWinY], sock);
    return MT_ReadInt(sock);
}
int MT_SM_WindowProps::requestWindowX(int X, wxSocketBase* sock)
{
    if(m_vMessageMap.size() <= msg_SetWinX){return 0;};

    MT_SendMessage(m_vMessageMap[msg_SetWinX], sock);
    MT_SendInt(X, sock);
    return 0;
}
int MT_SM_WindowProps::requestWindowY(int Y, wxSocketBase* sock)
{
    if(m_vMessageMap.size() <= msg_SetWinY){return 0;};

    MT_SendMessage(m_vMessageMap[msg_SetWinY], sock);
    MT_SendInt(Y, sock);
    return 0;
}
