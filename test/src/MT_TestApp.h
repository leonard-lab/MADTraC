#ifndef MT_TESTAPP
#define MT_TESTAPP

/* Standard WX include block */
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
/* End standard WX include block */

#include <wx/app.h>  // for wxApp

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

#include "MT/MT_Core/support/kbsupport.h"
#include <iostream>

class MT_KBHitThread : public wxThread
{
private:
    wxFrame* m_pFrame;
public:
    MT_KBHitThread(wxFrame* pFrame) : m_pFrame(pFrame) { MT_keyboard_init();};
    void* Entry()
    {
        while(1)
        {
            if(wxThread::TestDestroy())
            {
                MT_keyboard_close();
                return NULL;
            }

            if(MT_kbhit())
            {
                MT_keyboard_close();
                m_pFrame->Close();
                return NULL;
            }

            wxThread::Sleep(1);
        }
    };
    void OnExit()
    {
        MT_keyboard_close();
    };
};
    
class MT_TestFrame : public wxFrame
{
public:
    MT_KBHitThread* m_pKBHitThread;
    int m_iTestStatus;
    
    MT_TestFrame(const wxString& title = wxT("Test"),
                 const wxPoint& pos = wxPoint(-100,-100), /* off
                                                           * screen */
                 const wxSize& size = wxSize(50, 50),
                 long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
        : wxFrame(NULL, /* no parent */
                  wxID_ANY,
                  title,
                  pos,
                  size,
                  style),
          m_pKBHitThread(NULL),
          m_iTestStatus(MT_TEST_SUCCESS)
    {
        wxFrame::Connect(wxID_ANY,
                         wxEVT_CLOSE_WINDOW,
                         wxCloseEventHandler(MT_TestFrame::onClose));
    };
    ~MT_TestFrame(){};

    void onClose(wxCloseEvent& event){finishTest(); event.Skip();};

    virtual void doTest(int argc, wxChar** argv){};
    virtual void finishTest(){};

    void SetExitOnKBHit()
    {
        m_pKBHitThread = new MT_KBHitThread(this); 
        m_pKBHitThread->Create(); 
        if(m_pKBHitThread->Run() != wxTHREAD_NO_ERROR) 
        { 
                std::cout << "Could not create waiting thread.\n"; 
        }
    };


};

#ifndef NO_CONSOLE
#define TEST_DEBUG_CONSOLE
#endif

class MT_TestApp : public wxApp
{
public:
    MT_TestApp()
        : wxApp()
    {};

    ~MT_TestApp(){};

    MT_TestFrame* m_pFrame;
    virtual MT_TestFrame* createTestFrame();

    bool OnInit()
    {
#ifdef __APPLE__
        ProcessSerialNumber PSN;
        GetCurrentProcess(&PSN);
        TransformProcessType(&PSN,kProcessTransformToForegroundApplication);
#endif  

#if defined(TEST_DEBUG_CONSOLE) && defined(_WIN32)
        if(AllocConsole())
        {
            freopen("CONOUT$", "wt", stdout);
            freopen("CONOUT$", "wt", stderr);

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                                    FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

            fprintf(stdout, "\n");
            fprintf(stdout, " MADTraC Debug Console\n");
            fprintf(stdout, "\n");
            fprintf(stdout, "    Warning:  Do NOT close this window manually!\n");
            fprintf(stdout, "\n");
            fprintf(stdout, "\n");
        }
#endif

        m_pFrame = createTestFrame();

        if(m_pFrame)
        {
            m_pFrame->doTest(wxTheApp->argc, wxTheApp->argv);
            m_pFrame->Show();
            m_pFrame->Raise();
            return true;
        }
        else
        {
            fprintf(stderr, "Failed to create a main frame.  Exiting.\n");
            return false;
        }
    };

    bool Initialize(int &argc, wxChar** argv)
    {

        // resolve the OLE problem, see: 
        // http://trac.wxwidgets.org/ticket/10505
#ifdef _WIN32
        CoUninitialize();
#endif

        // pass on to base class function
        return wxApp::Initialize(argc, argv);

    };
    
    int OnRun(){wxApp::OnRun(); return m_pFrame->m_iTestStatus;};
};

#define IMPLEMENT_MT_TESTAPP(test_frame_class) \
    MT_TestFrame* MT_TestApp::createTestFrame(){return new test_frame_class;}; \
    IMPLEMENT_APP(MT_TestApp);
    

#endif // MT_TESTAPP
