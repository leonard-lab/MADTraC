/*
 *  RobotBasicGUI.cpp
 *
 *  Created by Daniel Swain on 1/4/10.
 *
 */

#include "SampleRobot.h"

/* This tells the wx system (and the compiler) which application
 * class is our main class. */
IMPLEMENT_APP(RobotBasicApp)


/* ctor - see header file documentation */
RobotBasicFrame::RobotBasicFrame(wxFrame* parent,
                                 wxWindowID id,
                                 const wxString& title, 
                                 const wxPoint& pos, 
                                 const wxSize& size,     
                                 long style)
/* pass on arguments to base class ctor - note it's generally
 * better c++ to initialize variables this way than to use e.g.
 * m_iDefaultNumToTrack = 1 in the body of the code. */
: MT_RobotFrameBase(parent, id, title, pos, size, style),
    m_iDefaultNumToTrack(1),      /* value will get changed during execution */
    m_bNumToTrackInCmdLine(false) /* gets set true if user passes -n */
{
    /* nothing else to do here */
}


/* called after ctor - see header file documentation */
void RobotBasicFrame::initUserData()
{
    /* initializes frame-level tracking variables, command line
     * parameters, etc. */
    MT_RobotFrameBase::initUserData();

    /* Adds m_iDefaultNumToTrack to the preferences that get automatically
     * written to / read from the preferences XML and displayed in the
     * preferences dialog.  First argument is the label/key and second
     * is a pointer to the data.  It's AddUInt because the data is
     * an unsigned integer (as opposed to AddBool, AddDouble, AddInt, etc) */
    m_pPreferences->AddUInt("DefaultNumToTrack", &m_iDefaultNumToTrack);

    /* Adds the "-n" or "--num-objects" option to the command line parser
     * so that it will be automatically detected. */
    m_CmdLineParser.AddOption("n",     /* "short" description -n */
                              "num-objects",                 /* "long" description --num-objects */
                              "Number of Objects to Track",  /* description shown for -h help */
                              wxCMD_LINE_VAL_NUMBER,         /* the value of the argument is a number */
                              wxCMD_LINE_PARAM_OPTIONAL);    /* this is an optional command 
                                                                line argument */

}


/* This function gets called after initUserData and after the
 * CmdLineParser has been parsed.  Note the argc and argv
 * are provided but it's preferred to use the CmdLineParser
 * functionality as shown. */
void RobotBasicFrame::handleCommandLineArguments(int argc, char** argv)
{
    /* call the base command line handler, which will handle all
     * the standard command line arguments */
    MT_RobotFrameBase::handleCommandLineArguments(argc, argv);
}


/* gets called when the user presses "start tracker" or passes
 * the -T or --Track-now command line arguments.  Needs to 
 * initialize the tracker, which should be derived from
 * MT_TrackerBase and set a pointer to it as m_pTracker. 
 * In this case our tracker is a GYSegmenter and it also
 * needs to know how many objects we're going to track. */
void RobotBasicFrame::initTracker()
{

    /* create a new tracker object.  also want to keep a copy
     * of the pointer that is cast as the GYTracker so that
     * we can access its member functions without having to
     * re-cast the m_pTracker pointer.  Keeping multiple
     * copies of a pointer is relatively cheap and safe (as long
     * as we are careful to keep them in the same scope), 
     * but having to cast up is dangerous and slow. */
    //m_pGYTracker = new GYSegmenter(m_pCurrentFrame);
	m_YATracker = new Segmenter(m_pCurrentFrame);
    /* the m_pTracker pointer is how the base class accesses
     * the tracker for all of the base functionality, so
     * we need to cast our pointer down to an MT_TrackerBase */
    //m_pTracker = (MT_TrackerBase *) m_pGYTracker;
	m_pTracker = (MT_TrackerBase *) m_YATracker;

    /* ask for the number of objects to track if the user did not
     * specify it at the command line with the -T / --Track-now 
     * option. */
    if(!m_bNumToTrackInCmdLine)
    {
        /* use the MT_NumericInputDialog class, which provides
         * a simplified way to ask the user for numeric input */
        MT_NumericInputDialog* dlg = new MT_NumericInputDialog(
            this,             /* parent is this frame */
            wxID_ANY,         /* ID is unimportant */
            wxT("Segmenter"), /* Title for the dialog */
            wxT("Enter Number of Objects to Track"), /* Prompt */
            (double) m_iDefaultNumToTrack, /* default number in the dlg */
            0,                   /* minimum admissible input */
            (double) MT_max_uint,  /* maximum admissible input */
            MT_INPUT_INTEGER);      /* input must be an integer */
        /* show the dialog and catch the result, which is either
         * wxID_OK if user pressed OK or wxID_CANCEL if user pressed
         * cancel */
        int result = dlg->ShowModal();

        /* if the user pressed OK, ask the dialog what the input was */
        if(result == wxID_OK)
        {
            m_iDefaultNumToTrack = dlg->GetValue();
        }

        /* we're done with the dialog so destroy it. this method
         * is preferred to the delete operator for wx objects
         * because it safely handles any pending events.  You
         * can think of this as equivalent to "delete dlg;" */
        dlg->Destroy();
    }

    /* set the number of objects to track in the segmenter, which is
     * an option specific to this derivative of MT_TrackerBase - hence
     * it needs to be done here */
    //m_pGYTracker->setNumObjects(m_iDefaultNumToTrack);

    /* note - do NOT call MT_TrackerBase::initTracker, which is
     * a placeholder function that sets m_pTracker to NULL! */
}

