#ifndef MT_AppBase_H
#define MT_AppBase_H

/** @addtogroup MT_Base 
 * @{ */

/** @file
 *  MT_AppBase.h
 *
 *  Created by Daniel Swain on 10/12/09.
 *
 *  An attempt to make the App class transparent.  Based on current MT_AppBase
 *  implementation.
 *
 */

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

// pops up a console in windows and directs stdout (i.e. printf) to it
//   very useful for debugging
#ifdef _WIN32
/** \def MT_WIN32_DEBUG_CONSOLE If this is defined under Windows, a console
    window is displayed to which e.g. printf is redirected. */
#define MT_WIN32_DEBUG_CONSOLE
#endif

/** \def MT_FIXED_SIZE_FRAME 
    This is the same as wxDEFAULT_FRAME_STYLE except 
    without a maximize button and not sizable. */
#define MT_FIXED_SIZE_FRAME wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN
/** \def MT_FIXED_SIZE_FRAME_NO_CLOSEBOX 
    This is the same as wxDEFAULT_FRAME_STYLE 
    except without a maximize or close button and not sizable. */
#define MT_FIXED_SIZE_FRAME_NO_CLOSEBOX wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN

/* The size of window elements is not always consistent and we sometimes
 * calculate window sizes based upon them.  Here are a few specific ones. */
#ifdef __APPLE__
const int MT_BUTTON_HEIGHT = 20;  /* works on OS X 10.5 at least */
#else
const int MT_BUTTON_HEIGHT = 26;  /* tested on windows 7 so far... */
#endif

/** @class MT_FrameWithInit
 *
 * @brief Base class for windows with a manually-callable initialization.
 *
 * A wxFrame-derived class with initialization.  The 
 * MT_FrameWithInit class is derived from wxFrame with 
 * a single constructor (the most commonly used here) 
 * that passes parameters on to the wxFrame constructor.  
 * It has a single virtual function, doMasterInitialization(), 
 * that gets called by MT_AppBase.  The intention here 
 * is to avoid problems associated with calling virtual 
 * functions from a constructor and yet keep the frame 
 * type used by MT_AppBase as basic as possible. 
 *
 */
class MT_FrameWithInit : public wxFrame
{
public:
    /** This constructor is a straight pass-through to the equivalent
        wxFrame constructor.  There is no actual code in the constructor. 
        @see FIXED_SIZE_FRAME
        @see FIXED_SIZE_FRAME_NO_CLOSEBOX */
MT_FrameWithInit(wxFrame* parent,
                 wxWindowID id = wxID_ANY,
                 const wxString& title = wxT("Window"),
                 const wxPoint& pos = wxDefaultPosition, 
                 const wxSize& size = wxSize(640,480),     
                 long style = MT_FIXED_SIZE_FRAME)
    :wxFrame(parent, id, title, pos, size, style){};

    /** This function is called by MT_AppBase after the frame is created.
        This is where you should place initialization code that isn't
        accounted for in the virtual functions of wxFrameCore etc. */
    virtual void doMasterInitialization(){};

    /** This function gets called by MT_AppBase when a file is supplied
        to it by e.g. "open with" from finder on Mac.  
        @param filename The full path of the file that was provided. */
    virtual void handleOpenWithFile(const wxString& filename){}

    DECLARE_EVENT_TABLE()

        };


/** @class MT_AppBase
 *
 * @brief Base class for applications.
 *
 * The MT_AppBase class is the main entry 
 * point for our wxWidgets applications.  
 * This class is derived from wxApp and 
 * doesn't do a whole lot other than to 
 * create and display a main frame and 
 * handle some potential OS-specific issues. 
 *
 */
class MT_AppBase : public wxApp
{

private:

    /* pointer to main frame */
    MT_FrameWithInit* m_pMainFrame;

public:

    /** The constructor doesn't really do anything here. */
    MT_AppBase();
    /** The destructor doesn't really do anything here. */
    ~MT_AppBase();

    /** This function calls CreateMainFrame and if a suitable result (not NULL)
        is given it calls the doMasterInitialization() function and shows the frame. 
        Also handles showing console on Windows. */
    bool OnInit();

    /** This is the only virtual function and the most important for the
        user - override this to return a pointer to any frame derived from
        MT_FrameWithInit and that will be treated as the application's main
        frame.  This version returns NULL, which will cause the program
        to automatically exit. */
    virtual MT_FrameWithInit* createMainFrame(){return NULL;};

    /** Necessary to avoid a conflict between the way wx and OpenCV 
        initialize OLE on windows.  See code.  Do not edit this function. */
    bool Initialize(int& argc, wxChar **argv);

#ifdef __APPLE__
    /** Handles the system requesting to open a file with this
        application.  For example, when the user right clicks a file in finder
        and selects "Open With" or when double clicking a file that this
        program is associated with.  Note to get this work you also need
        to add "Document Types" in the "Properties" tab of the info dialog
        for the target of interest.  See the PlaySwarm target for an example,
        where this is set to accept .dat files. 

        This function calls m_pMainFrame->handleOpenWithFile(filename)

        @param filename The full path of the file that was provided. 
        @see MT_FrameWithInit::handleOpenWithFile */
    void MacOpenFile(const wxString& filename);
#endif __APPLE__


};


/* @} */

#endif // MT_AppBase_H
