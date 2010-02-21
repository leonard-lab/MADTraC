#ifndef WXMADTRACSERVER_H
#define WXMADTRACSERVER_H

#include "wx/socket.h"

#define MT_SERVER_DEBUG

// IDs

const long MT_SERVER_DEFAULT_PORT = 3000;
const long MT_SERVER_NO_INIT = -1;

enum
{
    MT_SERVER_ID = 100,
    MT_SOCKET_ID
};

typedef enum MT_message_type
{
    MT_MSG_END = 1,   
    MT_MSG_ACKNOWLEDGE,
    MT_MSG_IMGHEIGHT,
    MT_MSG_IMGWIDTH,
    MT_MSG_POSITION,
    MT_MSG_FULLSTATE/*,
                   MT_MSG_ORIENTATION*/
} MT_message_type;

// class

class MT_Server : public wxEvtHandler
{
protected:
    long m_lPort;
  
public:

    MT_Server(long port = MT_SERVER_DEFAULT_PORT);
    ~MT_Server();

    bool doInit(long port = MT_SERVER_DEFAULT_PORT);

    /*void SetTracker(predexpTracker * tracker);*/

    void OnServerEvent(wxSocketEvent& event);
    void OnSocketEvent(wxSocketEvent& event);

    void Test1(wxSocketBase* sock);
    void Test2(wxSocketBase* sock);
    void Test3(wxSocketBase* sock);
    void Communicate(wxSocketBase* sock);

    MT_message_type ReceiveMessage(wxSocketBase* sock, char* buffer);
    void SendMessage(wxSocketBase* sock, char* buffer, int message);
    void SendAcknowledge(wxSocketBase* sock, char* buffer);
    bool GetAcknowledge(wxSocketBase* sock, char* buffer);

    void TranslateToBuffer(char* buffer, int value);
    int TranslateFromBuffer(char* buffer);

private:

    wxSocketServer* m_server;
    bool m_busy;
    int m_numClients;

    DECLARE_EVENT_TABLE()
};

#endif
