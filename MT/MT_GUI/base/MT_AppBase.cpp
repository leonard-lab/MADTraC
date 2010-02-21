/*
 *  MT_AppBase.cpp
 *
 *  Created by Daniel Swain on 10/12/09.
 *
 */

#include "MT_AppBase.h"

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

BEGIN_EVENT_TABLE(MT_FrameWithInit, wxFrame)
END_EVENT_TABLE()

/* App ctor - don't usually want to do much here b/c various objects 
 * might not be totally initialized yet. */
MT_AppBase::MT_AppBase()
: wxApp()
{

    // safety measure.  we can check e.g. if(!m_pMainFrame) to see if it
    //  has been initialized yet
    m_pMainFrame = NULL;

}


/* App dtor - may want to do memory management or other cleanups here.
   Memory management is somewhat tricky with wxWidgets.  If we delete
   something here and get invalid access runtime errors, then we're 
   doing
   it wrong! */
MT_AppBase::~MT_AppBase()
{

}


/* Closest analog to an "int main" type of function (though the ctor 
 * is called first).  Usually at this point we can consider the app to 
 * be fully initialized and ready for us to take control of it. */
bool MT_AppBase::OnInit()
{

    /* The following is necessary to make the application behave 
     * properly when being run from the terminal on Mac OS X.  
     *
     * As per http://wiki.wxwidgets.org/WxMac_Issues
     *
     * Note: Still doesn't raise the window to the top.  Not sure how to 
     * fix this yet.
     */
#ifdef __APPLE__
    ProcessSerialNumber PSN;
    GetCurrentProcess(&PSN);
    TransformProcessType(&PSN,kProcessTransformToForegroundApplication);
#endif  

    /* this is for windows (specifically visual studio).  It redirects 
     * stdout (e.g. printf) to a console window - very useful for 
     * debugging, though you may want to compile it out for "release" 
     * versions.  The code is fairly standard, but borrowed from some 
     * website.  On OS X this is unnecessary as stdout appears in the 
     * debug terminal. */
#if defined(_DEBUG) && defined(MT_WIN32_DEBUG_CONSOLE) && defined(_WIN32)
    if(AllocConsole())
    {
        freopen("CONOUT$", "wt", stdout);
		freopen("CONOUT$", "wt", stderr);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                                FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

        printf("\n");
        printf(" MADTraC Debug Console\n");
        printf("\n");
    }
#endif

    m_pMainFrame = createMainFrame();

    if(m_pMainFrame)
    {
        /* call the virtual-safe initialization functions */
        m_pMainFrame->doMasterInitialization();

        // this line actually causes the window to be displayed
        m_pMainFrame->Show();    m_pMainFrame->Raise();

        /* The OnInit() function is called by some other internal function 
         * - so we need to return true to indicate things are OK (we'll 
         *   handle errors ourselves internally)*/
        return true;
    }
    else
    {
        fprintf(stderr, "Failed to create a main frame.  Exiting.\n");
        return false;
    }

}

/* Necessary to avoid a conflict between the way wx and OpenCV 
 * initialize OLE on windows.  See code.  Do not edit this function.  
 * */
bool MT_AppBase::Initialize(int &argc, wxChar** argv)
{

    // resolve the OLE problem, see: 
    // http://trac.wxwidgets.org/ticket/10505
#ifdef _WIN32
    CoUninitialize();
#endif

    // pass on to base class function
    return wxApp::Initialize(argc, argv);

}

#ifdef __APPLE__
void MT_AppBase::MacOpenFile(const wxString& filename)
{
    m_pMainFrame->handleOpenWithFile(filename);
}
#endif
