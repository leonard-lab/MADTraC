#include "MT_Test.h"
#include "MT_wxTest.h"
#include "MT_TestApp.h"

#include <iostream>
#include <string>

#include "MT/MT_GUI/server/MT_Server.h"
#include "MT/MT_GUI/server/MT_ServerModules.h"

class test_frame : public MT_TestFrame
{
public:

    MT_Server* m_pServer;
    
    void doTest(int argc, wxChar** argv);
    void finishTest();
};

IMPLEMENT_MT_TESTAPP(test_frame);
    
void test_frame::doTest(int argc, wxChar** argv)
{
    MT_TestFrame::SetExitOnKBHit();

    m_pServer = new MT_Server;
    if(argc > 1)
    {
        m_pServer->enableDebugOutput();
    }

    if(m_pServer->doInit() == MT_Server::err_no_error)
    {
        std::cout << "Server is listening.\n\tPres any key to exit.\n";
    }

    m_pServer->registerModule(new MT_SM_WindowProps(m_pServer, this));

}

void test_frame::finishTest()
{
    printf("Goodbye.\n");

    
    delete m_pServer;
    
}
