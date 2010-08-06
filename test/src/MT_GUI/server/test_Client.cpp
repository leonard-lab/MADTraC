#include "MT_Test.h"
#include "MT_wxTest.h"
#include "MT_TestApp.h"

#include "MT/MT_GUI/server/MT_Client.h"
#include "MT/MT_GUI/server/MT_ServerModules.h"

#include <iostream>

class test_frame : public MT_TestFrame
{
public:

    MT_Client* m_pClient;
    MT_SM_WindowProps* m_pSMWindowProps;
    
    void doTest(int argc, wxChar** argv);
    void finishTest();
};

IMPLEMENT_MT_TESTAPP(test_frame);

void test_frame::doTest(int argc, wxChar** argv)
{
    MT_TestFrame::SetExitOnKBHit();
    
    m_pClient = new MT_Client();

    m_pSMWindowProps = new MT_SM_WindowProps();
    m_pClient->addModule(m_pSMWindowProps);

    if(argc > 1)
    {
        m_pClient->enableDebugOutput();
    }

    MT_Server::err_code e = m_pClient->Connect("localhost", 3000);

    if(e != MT_Server::err_no_error)
    {
        m_iTestStatus = MT_TEST_ERROR;
        fprintf(stderr, "Error connecting to server.\n");
        Close();
    }

    for(unsigned int i = 0; i < 10; i++)
    {
        m_pClient->PingServer();
    }

    m_pClient->getMessageTable();

    std::cout << "Client Message Table\n" <<
        m_pClient->getMessageTableAsString();


    m_pClient->InitiateExchange();

    double t = m_pClient->getServerUptime();
    if(t < 0)
    {
        std::cerr << "Error getting uptime from server.  Response was "
                  << t << std::endl;
    }
    std::cout << "Uptime is " << t << std::endl;

    std::string MOTD = m_pClient->getMOTD();

    std::cout << "MOTD: " << MOTD << std::endl;

    int w = m_pSMWindowProps->getWinWidth(m_pClient->getSocket());
    int h = m_pSMWindowProps->getWinHeight(m_pClient->getSocket());
    int cw = m_pSMWindowProps->getClientWidth(m_pClient->getSocket());
    int ch = m_pSMWindowProps->getClientHeight(m_pClient->getSocket());
    int x = m_pSMWindowProps->getWindowX(m_pClient->getSocket());
    int y = m_pSMWindowProps->getWindowY(m_pClient->getSocket());


    std::cout << "Server window size is " << w << " x " << h << std::endl;
    std::cout << "Server window client size is " << cw
              << " x " << ch << std::endl;
    std::cout << "Server window position is " << x << ", " << y << std::endl;

    m_pSMWindowProps->requestWindowX(100, m_pClient->getSocket());
    m_pSMWindowProps->requestWindowY(100, m_pClient->getSocket());    

    m_pClient->EndExchange();
}

void test_frame::finishTest()
{
    delete m_pClient;
}
