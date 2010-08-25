/*
 *  MT_FrameBase.cpp
 *
 *  Created by Daniel Swain on 12/7/09.
 *
 */

#include "MT_FrameBase.h"

#include "MT/MT_GUI/dialogs/MT_MovieExporterDialog.h"

/* static member functions for cursors */
const wxCursor& MT_GLCanvasBase::CursorZoom()
{
    static wxCursor cursor(wxCURSOR_MAGNIFIER);
    return cursor;
}
const wxCursor& MT_GLCanvasBase::CursorPan()
{
    static wxCursor cursor(wxCURSOR_HAND);
    return cursor;
}

/* height of the status bar */
static const double g_StatusBarFrac = 0.05;

/********************************************************************/
/*           MT_GLCanvasBase ctors and dtors                        */
/********************************************************************/

/* Note we attach events for the keyboard and mouse using
 * an event table.  Using "Connect" is in some sense better
 * but this method works well with keyboard ane mouse events
 * because EVT_CHAR and EVT_MOUSE_EVENTS catch pretty much
 * every keyboard and mouse event, whereas a separate call
 * to Connect calls would be necessary for e.g. 
 * wxEVT_KEY_DOWN, wxEVT_KEY_UP, etc etc. */
BEGIN_EVENT_TABLE(MT_GLCanvasBase, wxGLCanvas)
EVT_CHAR(MT_GLCanvasBase::onKeyboard)
EVT_MOUSE_EVENTS(MT_GLCanvasBase::onMouse)
END_EVENT_TABLE()

MT_GLCanvasBase::MT_GLCanvasBase(wxWindow* parent,
                                 wxWindowID id,
                                 const wxPoint& pos,
                                 const wxSize& size)
: wxGLCanvas(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE),
#ifdef MT_USE_IMAGES
    m_pCurrentImage(NULL),
#endif
    m_bZooming(false),
    m_bAutoZoom(false),
    m_bIsZoomed(false),
    m_bPanning(false),
    m_bGLInitialized(false),
    m_bNeedDraw(false),
    m_BackgroundColor(MT_DEFAULT_BG_COLOR),
    m_pMTParent(NULL),
    m_CurrentMouseRect(0,0,0,0),
    m_MouseDragHistory(),
    m_MouseDragHistoryWinCoords(),
    m_bMouseDragging(false),
    m_bFlagMouseUp(false),
    m_ViewportOrg(0,0,0,0),
    m_CurrentViewport(0,0,0,0),
    m_ObjectLimits(0,0,0,0),
    m_PanningReference(0,0,0,0),
    m_ViewportHistory(MT_DEFAULT_HISTORY_LENGTH)
{
    /* We connect the canvas events here so that they will
     * follow through to anywhere we place the canvas.  It
     * also simplifies the handling of events in the 
     * MT_FrameBase and its derived classes. 
     *
     * See the event table above for keyboard and mouse */
    wxGLCanvas::Connect(this->GetId(), 
                        wxEVT_PAINT, 
                        wxPaintEventHandler(MT_GLCanvasBase::onPaint));
    wxGLCanvas::Connect(this->GetId(),
                        wxEVT_SIZE,
                        wxSizeEventHandler(MT_GLCanvasBase::onSize));
    wxGLCanvas::Connect(this->GetId(),
                        wxEVT_ERASE_BACKGROUND,
                        wxEraseEventHandler(MT_GLCanvasBase::onEraseBackground));
}

MT_GLCanvasBase::~MT_GLCanvasBase()
{

    /* the base canvas doesn't allocate any memory for
     * itself, so there's really nothing to do here for now */

}

/********************************************************************/
/*           MT_GLCanvasBase private member functions               */
/********************************************************************/

void MT_GLCanvasBase::onPaint(wxPaintEvent& event)
{
    /* This line MUST be in this function.  Call it a quirk of wxWidgets,
    but the timer will fail to work without it. */
    wxPaintDC dc(this);

    /* only draw if it's necessary */
    if(!m_bNeedDraw)
    {
        return;
    }

    /* set this right away in case a call to doDraw() comes before
     * we're done (e.g. on another thread) */
    m_bNeedDraw = false;

#ifndef __WXMOTIF__
    if(!wxGLCanvas::GetContext())
    {
        return;
    }
#endif

    wxGLCanvas::SetCurrent();

    /* make sure the background color is right */
    glClearColor(m_BackgroundColor.R(), 
                 m_BackgroundColor.G(), 
                 m_BackgroundColor.B(),
                 m_BackgroundColor.A());

    /* this step is almost always the first one taken 
     * when drawing with OpenGL */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_FUNC);
    glDepthFunc(GL_LEQUAL);  
    glEnable(GL_DEPTH_TEST); 

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   

    /* this initialization should get called the first time. */
    if(!m_bGLInitialized)
    {
        doGLInitialization();
        m_bGLInitialized = true;
        lockCurrentViewportAsOriginal();
    }

    /* jump to the main (overridable) OpenGL drawing routine */
    doGLDrawing();

    /* this "enacts" the animation by swapping out a double buffer */
    wxGLCanvas::SwapBuffers();


}

void MT_GLCanvasBase::onSize(wxSizeEvent& event)
{

    wxGLCanvas::OnSize(event);
    doSizeCallback(event);

}

void MT_GLCanvasBase::onEraseBackground(wxEraseEvent& event)
{
    /* do nothing.  this is a standard wx hack to improve drawing. */
}

void MT_GLCanvasBase::onKeyboard(wxKeyEvent& event)
{

    /* make sure the drawing gets updated */
    doDraw();

    bool do_keys = doKeyboardCallback(event);

    if(do_keys == MT_DO_BASE_KEY)
    {
        switch(event.GetKeyCode())
        {
        case 'a':
        {
            toggleAutoZoom(); 
            break; 
        }
        case 'z':
            toggleZoomMode();  break;
        case 'Z':
            resetZoom();  break;
        case 'P':
            togglePanMode();  break;
        }
    }

    /* makes sure the rest of the wx event system processes this event */
    event.Skip();

}

void MT_GLCanvasBase::onMouse(wxMouseEvent& event)
{

    static double firstx = 0;
    static double firsty = 0;

    long x = event.GetX();
    long y = wxGLCanvas::GetClientSize().y - event.GetY();

    /* make sure we draw if the mouse is pressed or dragging */
    if(event.IsButton() || event.Dragging())
    {
        doDraw();
    }

    double vx, vy;
    convertWin2ViewXY(x, y, &vx, &vy);

    bool do_mouse = MT_DO_BASE_MOUSE;
    if(!(m_bZooming || m_bPanning))
    {
        do_mouse = doMouseCallback(event, vx, vy);
    }

    if(do_mouse == MT_DO_BASE_MOUSE)
    {
        if(event.LeftDown())
        {
            firstx = vx;
            firsty = vy;
            m_MouseDragHistory.clearHistory();
            m_MouseDragHistory.pushXY(vx, vy);
            m_MouseDragHistoryWinCoords.clearHistory();
            m_MouseDragHistoryWinCoords.pushXY(x, y);
            if(m_bPanning)
            {
                m_PanningReference = m_CurrentViewport;
            }
        }

        if(event.LeftIsDown())
        {
            m_bMouseDragging = true;
            m_CurrentMouseRect.xmin = MT_MIN(firstx, vx);
            m_CurrentMouseRect.xmax = MT_MAX(firstx, vx);
            m_CurrentMouseRect.ymin = MT_MIN(firsty, vy);
            m_CurrentMouseRect.ymax = MT_MAX(firsty, vy);
            m_MouseDragHistory.pushXY(vx, vy);
            m_MouseDragHistoryWinCoords.pushXY(x, y);
        }

        if(event.LeftUp())
        {
            m_bMouseDragging = false;
            m_bFlagMouseUp = true;
        }

    }

    /* makes sure the rest of the wx event system processes this event */
    event.Skip();

}

void MT_GLCanvasBase::toggleAutoZoom()
{
    doDraw();
    if(m_bAutoZoom)
    {
        resetZoom();
    }
    else
    {
        setAutoZoomOn();
    }
}

void MT_GLCanvasBase::toggleZoomMode()
{
    doDraw();
    m_bZooming = !m_bZooming;
    m_bAutoZoom = false;
}

void MT_GLCanvasBase::togglePanMode()
{
    if(!m_bAutoZoom)
    {
        doDraw();
        m_bPanning = !m_bPanning;
    }
}

void MT_GLCanvasBase::resetZoom()
{
    doDraw();
    m_bAutoZoom = false;
    m_bZooming = false;
    /* set the viewport back to the original */
    setViewport(m_ViewportOrg);
    m_bIsZoomed = false;

    /* resets the cursor to the default arrow */
    wxGLCanvas::SetCursor(wxNullCursor);
}

void MT_GLCanvasBase::zoomTo(const MT_Rectangle& zoom_viewport)
{
    m_bIsZoomed = true;
    setViewport(zoom_viewport);
}

void MT_GLCanvasBase::convertWin2ViewXY(long win_x, 
                                        long win_y,
                                        double* view_x, 
                                        double* view_y) const
{
    *view_x = m_CurrentViewport.xmin + 
        ((double) win_x)*(m_CurrentViewport.xmax - 
                          m_CurrentViewport.xmin)/((double) GetClientSize().x);
    *view_y = m_CurrentViewport.ymin + 
        ((double) win_y)*(m_CurrentViewport.ymax - 
                          m_CurrentViewport.ymin)/((double) GetClientSize().y);
}

void MT_GLCanvasBase::convertWin2ViewDXDY(long win_dx, 
                                          long win_dy,
                                          double* view_dx, 
                                          double* view_dy) const
{
    *view_dx = ((double) win_dx)*(m_CurrentViewport.xmax - 
                                  m_CurrentViewport.xmin)/((double) GetClientSize().x);
    *view_dy = ((double) win_dy)*(m_CurrentViewport.ymax - 
                                  m_CurrentViewport.ymin)/((double) GetClientSize().y);
}


/********************************************************************/
/*           MT_GLCanvasBase protected member functions             */
/********************************************************************/

void MT_GLCanvasBase::paintImage()
{
#ifdef MT_USE_IMAGES
    if(m_pCurrentImage)
    {
        glTranslatef(0,0,-0.01);
        /* Note we paint to the ORIGINAL viewport so that zooming
         * works correctly with respect to the image as well
         * as the rest of the drawing */
        MT_DisplayCVImage(m_pCurrentImage, 
                       m_ViewportOrg.xmin,
                       m_ViewportOrg.xmax,
                       m_ViewportOrg.ymin,
                       m_ViewportOrg.ymax);
        glTranslatef(0,0,0.01);
    }
#endif
}

void MT_GLCanvasBase::tellObjectLimits(const MT_Rectangle& object_limits, double exp_factor)
{
    MT_Rectangle ol = object_limits;
    MT_CalculateAspectRectangle(wxGLCanvas::GetClientSize().x, 
                             wxGLCanvas::GetClientSize().y, 
                             &ol.xmin, 
                             &ol.xmax, 
                             &ol.ymin, 
                             &ol.ymax,
                             exp_factor);
    m_ObjectLimits = ol;
}

/********************************************************************/
/*           MT_GLCanvasBase public member functions                */
/********************************************************************/
void MT_GLCanvasBase::setViewport(const MT_Rectangle& requested)
{

    wxSize client_size = wxGLCanvas::GetClientSize();
    MT_Rectangle r = requested;

    /* if the request is (0,0,0,0) (the default) then the viewport
     * will have lower left corner at 0,0 and the x and y extents
     * corresponding to the width and height of the client area
     * in pixels.  That is, the drawing coordinate frame will
     * correspond exactly to the window coordinates except that
     * the origin will be at the bottom left rather than the top
     * left. */
    if(requested == MT_BlankRectangle)
    {
        if(m_CurrentViewport == MT_BlankRectangle)
        {
            r = MT_Rectangle(0, client_size.x, 0, client_size.y);
        }
        else
        {
            r = m_CurrentViewport;
        }
    }
    /* even if we requested a specific viewport, we need to make
     * sure that the coordinates will have the right aspect
     * ratio. */
    else
    {
        MT_CalculateAspectRectangle(client_size.x,
                                 client_size.y,
                                 &r.xmin,
                                 &r.xmax,
                                 &r.ymin,
                                 &r.ymax);
    }

    /* if the new viewport is the same as the current one (with some
     * leeway - the MT_Rectangle == operator uses MT_IsEqual from mathsupport.h) 
     * we don't need to do anything */
    if(r == m_CurrentViewport)
    {
        return;
    }

    /* sets the OpenGL viewport (the area OpenGL will draw on) equal
     * to the entirety of the client area */
    glViewport(0, 0, client_size.x, client_size.y);

    /* Sets up an overhead view of the scene and defines the 
     * coordinates of the left and right sides of the viewport 
     * in drawing coordinates.  
     * i.e. client_size.x on the screen = viewport_size.x 
     *    in drawing coordinates 
     * note we also set the z (into and out of the screen) limits 
     *    somewhat arbitrarily at +/- 0.1 units  */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(r.xmin, 
            r.xmax, 
            r.ymin, 
            r.ymax, 
            -MT_Z_EXTENTS, 
            MT_Z_EXTENTS);

    /* push the most recent viewport */
    m_ViewportHistory.Push(r);

    /* store the viewport for quick access */
    m_CurrentViewport = r;

    doDraw();

}

void MT_GLCanvasBase::doGLInitialization()
{
    /* Initializes our OpenGL lists - precompiled
     * chunks of instruction useful for efficiently drawing
     * primitive objects such as circles, arrows, fish...*/
    MT_InitGLLists();  /* declared in MT/MT_Core/gl/glSupport.h */

    wxGLCanvas::SetCurrent();
    glEnable(GL_DEPTH_TEST);

    /* Set the viewport to the default, i.e. equivalent to
     * screen coordinates */
    setViewport();

    /* Initialize texture for painting images
     *  (if we're using them) */
#ifdef MT_USE_IMAGES
    MT_TextureInitForCV(&m_itexID);
#endif

    /* initialize the parent if its available */
    if(m_pMTParent)
    {
        m_pMTParent->doUserGLInitialization();
    }

}

void MT_GLCanvasBase::doGLDrawing()
{

    paintImage();

    if(m_pMTParent)
    {
        m_pMTParent->doUserGLDrawing();
    }

    if(m_bZooming && !m_bAutoZoom)
    {
        wxGLCanvas::SetCursor(MT_GLCanvasBase::CursorZoom());
    }

    if(m_bPanning)
    {
        wxGLCanvas::SetCursor(MT_GLCanvasBase::CursorPan());
    }

    if(!m_bAutoZoom && m_bMouseDragging && m_bZooming)
    {
        /* calculates a rectangle with the correct aspect
         * ratio given the window width and height.
         * This way the zoom rectangle will preserve the
         * coordinate system's aspect ratio.
         * see MT/MT_Core/gl/glSupport.h */
        MT_CalculateAspectRectangle(wxGLCanvas::GetClientSize().x, 
                                 wxGLCanvas::GetClientSize().y, 
                                 &m_CurrentMouseRect.xmin, 
                                 &m_CurrentMouseRect.xmax, 
                                 &m_CurrentMouseRect.ymin, 
                                 &m_CurrentMouseRect.ymax);
        /** Draws the calculated rectangle for user feedback */
        MT_DrawRectangle(m_CurrentMouseRect.xmin, /* lower left point */
                      m_CurrentMouseRect.ymin, 
                      /*width*/   m_CurrentMouseRect.xmax - m_CurrentMouseRect.xmin,  
                      /*height*/  m_CurrentMouseRect.ymax - m_CurrentMouseRect.ymin);
    }

    /* happens when we have just released the mouse */
    if(!m_bAutoZoom && m_bFlagMouseUp && m_bZooming)
    {
        double rect_min = MT_MIN_ZOOM_RECT*
            (m_CurrentViewport.ymax - m_CurrentViewport.ymin);
        /* want to make sure the specified rectangle is big enough,
         * avoids annoying spurious zooms when you accidentally click
         * the mouse for a very brief instant */
        if( m_CurrentMouseRect.GetWidth() > rect_min 
            && m_CurrentMouseRect.GetHeight() > rect_min)
        {
            zoomTo(m_CurrentMouseRect);
        }

        /* and exit zooming mode */
        m_bZooming = false;
        /* reset the cursor to the normal arrow */
        wxGLCanvas::SetCursor(wxNullCursor);
    }

    if(m_bAutoZoom && (m_ObjectLimits != MT_BlankRectangle))
    {
        doAutoZoom();
    }

    if(m_bPanning)
    {
        if(m_bMouseDragging)
        {
            double x1, y1, x2, y2;
            m_MouseDragHistoryWinCoords.getFirstXY(&x2, &y2);
            m_MouseDragHistoryWinCoords.getMostRecentXY(&x1, &y1);
            double dx, dy;
            convertWin2ViewDXDY(x2-x1, y2-y1, &dx, &dy);
            setViewport(m_PanningReference + MT_Rectangle(dx, dx, dy, dy));
        }

        if(m_bFlagMouseUp)
        {
            m_bPanning = false;
            /* reset the cursor to the normal arrow */
            wxGLCanvas::SetCursor(wxNullCursor);
        }
    }

    /* makes sure that the mouse-up flag doesn't "stick" accidentally */
    if(m_bFlagMouseUp)
    {
        m_bFlagMouseUp = false;
        m_MouseDragHistory.clearHistory();
        m_MouseDragHistoryWinCoords.clearHistory();
    }

}


void MT_GLCanvasBase::doSizeCallback(wxSizeEvent& event)
{
    /* do nothing.  only here fore possible future implementation. */
}

bool MT_GLCanvasBase::doKeyboardCallback(wxKeyEvent& event)
{
    if(m_pMTParent)
    {
        return m_pMTParent->doKeyboardCallback(event);
    }
    else
    {
        return MT_DO_BASE_KEY;
    }
}

bool MT_GLCanvasBase::doMouseCallback(wxMouseEvent& event, double vx, double vy)
{
    if(m_pMTParent)
    {
        return m_pMTParent->doMouseCallback(event, vx, vy);
    }
    else
    {
        return MT_DO_BASE_MOUSE;
    }
}

void MT_GLCanvasBase::doAutoZoom()
{

    if(!m_bAutoZoom || m_ObjectLimits == MT_BlankRectangle)
    {
        m_bAutoZoom = false;
        return;
    }

    const unsigned int Navg = 3;
    const double forgetting_factor = 0.99;

    const unsigned int Navg_center = 3;
    const double forgetting_factor_center = 0.99;

    double xc_sum = m_ObjectLimits.GetXCenter();
    double yc_sum = m_ObjectLimits.GetYCenter();

    double w = 1.0;
    double width_sum = m_ObjectLimits.GetWidth();
    double height_sum = m_ObjectLimits.GetHeight();
    double wsum = w;

    MT_Rectangle v;
    for(unsigned int i = 1; i < Navg; i++)
    {
        if(i >= m_ViewportHistory.Size())
        {
            break;
        }
        v = m_ViewportHistory.AtHistory(i);
        w *= forgetting_factor;
        width_sum += w*v.GetWidth();
        height_sum += w*v.GetHeight();
        wsum += w;
    }

    width_sum /= wsum;
    height_sum /= wsum;

    w = 1.0;
    wsum = w;
    for(unsigned int i = 1; i < Navg_center; i++)
    {
        if(i >= m_ViewportHistory.Size())
        {
            break;
        }
        v = m_ViewportHistory.AtHistory(i);
        w *= forgetting_factor_center;
        xc_sum += w*v.GetXCenter();
        yc_sum += w*v.GetYCenter();
        wsum += w;
    }
    xc_sum /= wsum;
    yc_sum /= wsum;

    setViewport(MT_Rectangle(xc_sum - 0.5*width_sum,
                         xc_sum + 0.5*width_sum,
                         yc_sum - 0.5*height_sum,
                         yc_sum + 0.5*height_sum));


}

/********************************************************************/
/*           MT_FrameBase ctors and dtors                           */
/********************************************************************/


MT_FrameBase::MT_FrameBase(wxFrame* parent,
                           wxWindowID id,
                           const wxString& title,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style)
  : MT_FrameWithInit(parent, id, title, pos, size, style),
    m_pControlFrame(NULL),        
    m_sScreenSaveDirectory(wxGetCwd()),  /* directories default to working directory */
    m_sScreenSavePath(wxGetCwd()),
    m_sMovieExportDirectory(wxGetCwd()),
#ifdef MT_USE_IMAGES
    m_pCurrentFrame(NULL),
    m_pCurrentScreen(NULL),
#endif
    m_bDoTimedEvents(false),
    m_bPaused(false),
    m_bDrawStatusBar(false),
    m_bQuitOnReset(false),
    m_bMakingMovie(false),
    m_iFramePeriod_msec(MT_DEFAULT_FRAME_PERIOD),
    m_bDoQuitWasCalled(false),
    m_ClientSize(size),
    m_pTimer(NULL),
    m_pCanvas(NULL),
    m_CmdLineParser(),    
    m_XMLSettingsFile(MT_GetXMLPathForApp().mb_str()),
    m_PathGroup(wxT("Directories"))
{
    m_sDescriptionText = wxString(wxT("Application description needs to be set")) +
        wxString(wxT(" in implementation.  Set m_sDescriptionText.\n"));

    wxFrame::Connect(wxID_ANY,
                     wxEVT_SIZE,
                     wxSizeEventHandler(MT_FrameBase::onSize));

    wxSizeEvent dummy_event;
    onSize(dummy_event);

    wxFrame::Connect(wxID_ANY,
                     wxEVT_CLOSE_WINDOW,
                     wxCloseEventHandler(MT_FrameBase::onClose));

    /* client size gets clobbered by the initial call to onSize,
     * so reset it here -- the client size should get set
     * appropriately during doMasterInitialization */
    m_ClientSize = size;

}

MT_FrameBase::~MT_FrameBase()
{

    delete m_pTimer;

    delete m_pPreferences;

#ifdef MT_USE_IMAGES
    if(m_pCurrentScreen)
    {
        cvReleaseImage(&m_pCurrentScreen);
    }
#endif

}

/********************************************************************/
/*           MT_FrameBase private methods                           */
/********************************************************************/

void MT_FrameBase::doMasterInitialization()
{
    m_pTimer = createTimer();
    setTimer(MT_DEFAULT_FRAME_PERIOD);

    createMenus();

    /* UI Creation */
    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);  
    m_pCanvas = new MT_GLCanvasBase(this, 
                                    MT_ID_CANVAS,
                                    wxDefaultPosition, 
                                    m_ClientSize);
    m_pCanvas->Show();
    hbox->Add(m_pCanvas);
    SetSizerAndFit(hbox);
    m_pCanvas->setMTParent((MT_FrameBase *)this);
    /* make sure the frame size is updated */
    setSizeByClient(m_ClientSize.x, m_ClientSize.y);

    /* Extra Frame Creation */
    m_pControlFrame = createControlDialog();
    if(m_pControlFrame)
    {
        m_pControlFrame->doMasterInitialization();
        m_pControlFrame->Show();
    }

    createUserDialogs();

    m_PathGroup.AddPath(wxT("Screen_Save_Directory"), &m_sScreenSaveDirectory);
    m_PathGroup.AddPath(wxT("Movie_Export_Directory"), &m_sMovieExportDirectory);

    m_pPreferences = new MT_DataGroup("General Preferences");
    m_pPreferences->AddColor("Background Color", &(m_pCanvas->m_BackgroundColor));
    m_pPreferences->AddBool("Quit On Reset", &m_bQuitOnReset);


    /* initialize the command line parser -
     * needs to happen before initUserData so that there is a chance
     * to override switches */
    /* refuse / as a switch, so we can use them for paths */
    m_CmdLineParser.SetSwitchChars(wxT("-")); 

    /* requests for help are handled automatically by setting
     * the wxCMD_LINE_OPTION_HELP flag here */
    m_CmdLineParser.AddSwitch(wxT("h"), 
                              wxT("help"),
                              wxT("Display this help message."),
                              wxCMD_LINE_OPTION_HELP);
    m_CmdLineParser.AddSwitch(wxT("1"),
                              wxT("quit-on-reset"),
                              wxT("Quit on reset."));
    m_CmdLineParser.AddOption(wxT("m"),
                              wxT("make-movie"),
                              wxT("Record uncompressed AVI."),
                              wxCMD_LINE_VAL_STRING,
                              wxCMD_LINE_PARAM_OPTIONAL);

    initUserData();

    /* Set the description of the program given if "-h" or "--help"
       is an argument - i.e. the usage message 
       happens after initUserData in case it was set there. */
    m_CmdLineParser.SetLogo(m_sDescriptionText);
    m_CmdLineParser.SetCmdLine(wxTheApp->argc, wxTheApp->argv);
    m_CmdLineParser.Parse();

    /* deal with requests for help right here */

    readXML();
    handleCommandLineArguments(wxTheApp->argc, wxTheApp->argv);

    /* not m_bQuitOnReset = m_CmdLineParser.Found("1") so it does not
     * clobber the preference */
    if(m_CmdLineParser.Found(wxT("1")))
    {
        m_bQuitOnReset = true;
    }

    wxString v;
    if(m_CmdLineParser.Found(wxT("m"), &v))
    {
        setupMovie(v);
    }

    /* get frame info from XML */
    registerDialogForXML(this);
    registerDialogForXML(m_pControlFrame);

}

void MT_FrameBase::createMenus()
{
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* file_menu = new wxMenu;
    wxMenu* edit_menu = new wxMenu;
    wxMenu* view_menu = new wxMenu;
    wxMenu* help_menu = new wxMenu;

    makeFileMenu(file_menu);
    menuBar->Append(file_menu, wxT("&File"));

    makeEditMenu(edit_menu);
    if(edit_menu->GetMenuItemCount())
    {
        menuBar->Append(edit_menu, wxT("Edit"));
    }

    makeViewMenu(view_menu);
    if(view_menu->GetMenuItemCount())
    {
        menuBar->Append(view_menu, wxT("View"));
    } 

    createUserMenus(menuBar);

    makeHelpMenu(help_menu);
    if(help_menu->GetMenuItemCount())
    {
        menuBar->Append(help_menu, wxT("&Help"));
    }

    wxFrame::SetMenuBar(menuBar);

}

void MT_FrameBase::doTimedEvents()
{

    doUserTimedEvents();

    if(!m_bPaused)
    {
        doStep();
    }

    /* tells the canvas to do its drawing */
    m_pCanvas->Refresh(false);

#ifdef MT_USE_IMAGES
    if(m_bMakingMovie && 
       MT_GLBufferToIplImage(GetClientSize().x, 
                          GetClientSize().y, m_pCurrentScreen) == MT_SAVER_OK)
    {
        m_MovieExporter.saveFrame(m_pCurrentScreen);
    }
#endif

}

void MT_FrameBase::doStep()
{

    doUserStep();

    m_DialogGroup.UpdateAll();

    ensureDraw();

}

void MT_FrameBase::writeXML()
{

    /* see if the file has a root */
    if(m_XMLSettingsFile.HasRoot())
    {
        // if it does, make sure this is the right kind of file
        if(!m_XMLSettingsFile.HasRootname(MT_GetXMLRootName().mb_str()))
        {
            return;
        }
    }
    else
    {
        /* file doesn't have a root - it's probably a new xml file, so initialize it */
        m_XMLSettingsFile.InitNew(MT_GetXMLRootName().mb_str());
    }

    m_PathGroup.WriteToXML(&m_XMLSettingsFile);

    WriteDataGroupToXML(&m_XMLSettingsFile, m_pPreferences);

    writeUserXML();

    /* actually write the parameters to disk */
    m_XMLSettingsFile.SaveFile();

    return;
}

void MT_FrameBase::readXML()
{

    /* try to read the xml file */
    if(!m_XMLSettingsFile.ReadFile())
    {
        /* if not, bail with an error */
        fprintf(stderr, "Could not read XML file %s\n", m_XMLSettingsFile.GetFilename());

        /* try creating a new file */
        if(!m_XMLSettingsFile.InitNew((MT_GetXMLRootName()).mb_str()))
        {
            fprintf(stderr, "Can't even create a new XML file.  There may be problems.\n");
            return;
        }

        return;
    }

    /* make sure this is the right kind of file */
    if(!m_XMLSettingsFile.HasRootname(MT_GetXMLRootName().mb_str()))
    {
        return;
    }

    m_PathGroup.ReadFromXML(m_XMLSettingsFile);

    ReadDataGroupFromXML(m_XMLSettingsFile, m_pPreferences);

    readUserXML();

    return;
}

void MT_FrameBase::onChildMove(wxCommandEvent& event)
{
    /* once we ask the app to quit, the windows are no longer valid.
     * even worse - as of wx 2.8.11, this event seems to get triggered
     * when closing the application.  by exiting here, we make sure
     * not to cause an access violation. */
    if(m_bDoQuitWasCalled)
    {
        return;
    }
    
    wxWindow* win = dynamic_cast<wxWindow*>(event.GetEventObject());
    MT_WriteWindowDataToXML(&m_XMLSettingsFile, win->GetLabel(), win);
}

void MT_FrameBase::onSize(wxSizeEvent& event)
{
#ifdef MT_USE_IMAGES
    if(m_pCurrentScreen)
    {
        cvReleaseImage(&m_pCurrentScreen);
    }
    m_pCurrentScreen = cvCreateImage(cvSize(GetClientSize().x, GetClientSize().y), IPL_DEPTH_8U, 3);
#endif
    m_ClientSize = GetClientSize();
    /* should redraw when resized */
    ensureDraw();
}

void MT_FrameBase::onClose(wxCloseEvent& event)
{
    /* makes sure that the doQuit stuff gets called once and only once */
    if(!m_bDoQuitWasCalled)
    {
        doQuit(false);
    }

    event.Skip();
}

/********************************************************************/
/*           MT_FrameBase protected methods                         */
/********************************************************************/

void MT_FrameBase::doQuit(bool force_close)
{

    /* our OnClose handler checks this to make sure that we don't
     * try to call doQuit more than once. */
    m_bDoQuitWasCalled = true;

    m_pTimer->Stop();
    m_bDoTimedEvents = false;
    m_bPaused = true;

    // write config xml
    writeXML();

    doUserQuit();

    if(m_pControlFrame)
    {
        m_pControlFrame->doQuit();
    }

    /* normally, we want this to close the window, but
     * we don't want that to happen if the close event has
     * already been triggered and is calling this function 
     * itself */
    if(force_close)
    {
        wxFrame::Close(true);
    }

}

void MT_FrameBase::registerUpdatableDialog(
    MT_DialogWithUpdate* dialog,
    const string& key)
{
    if(!m_DialogGroup.ExistsInGroup(key))
    {
        m_DialogGroup.RegisterAndShow(key, dialog);
    }
}

void MT_FrameBase::registerDialogForXML(wxWindow* dialog)
{
    if(!dialog)
    {
        return;
    }

    MT_ReadWindowDataFromXML(m_XMLSettingsFile, 
                          dialog->GetLabel(), 
                          dialog); 
    dialog->Connect(wxID_ANY, 
                    wxEVT_MOVE, 
                    wxCommandEventHandler(MT_FrameBase::onChildMove),
                    NULL,
                    this); /* it's important that the "this" parameter
                              is passed to the Connect call so that
                              it knows which instance of MT_FrameBase
                              to refer to */
}

void MT_FrameBase::togglePause()
{
    m_bPaused = !m_bPaused;
    if(m_pControlFrame)
    {
        m_pControlFrame->onPlayPause(m_bPaused);
    }
    onPauseToggled(m_bPaused);
}

void MT_FrameBase::setControlFrameStatusText(const wxString& text)
{
    if(m_pControlFrame)
    {
        m_pControlFrame->setStatusText(text);
    }
}

void MT_FrameBase::saveScreen(char* filename)
{

    wxSize cs = GetClientSize();
    int w = cs.x;
    int h = cs.y;
    if(!filename)
    {
        int result = MT_SaveFileDialog(this,
                                       m_sScreenSaveDirectory,
                                       wxT("Select file name for screen save."),
                                       MT_FILTER_IMAGE_FILES,
                                       &m_sScreenSavePath,
                                       &m_sScreenSaveDirectory);
        if(result != wxID_OK)
        {
            return;
        }
        MT_SaveGLBuffer(w, h, m_sScreenSavePath.mb_str());
    }
    else
    {
        MT_SaveGLBuffer(w, h, filename);
    }

}

void MT_FrameBase::displayCheatSheet()
{
    wxString cst = getCheatSheetText();
    if(!cst || cst.IsEmpty() || (cst == wxT("")))
    {
        cst = wxT("No help for this application yet.");
    }
    MT_CheatSheetFrame* csf = new MT_CheatSheetFrame(this, cst);
    registerDialogForXML(csf);
    csf->Show(true);
}

wxString MT_FrameBase::getCheatSheetTextFromFile()
{
    wxString cheatsheettextfile = MT_GetApplicationResourcePath(
        wxTheApp->GetAppName() + wxT("CheatSheet.txt"), 
        wxT(""));  
    wxString result = MT_wxStringFromTextFile(cheatsheettextfile);
    if(result.StartsWith(wxT("Error")))
    {
        result = wxString(wxT("Could not find cheat sheet text file.  ")) + 
            wxString(wxT("Should be called ")) + wxTheApp->GetAppName() + 
            wxString(wxT("CheatSheet.txt"));
    }
    return result;
}

wxString MT_FrameBase::getUsageTextFromFile()
{
    wxString usagetextfile = MT_GetApplicationResourcePath(
        wxTheApp->GetAppName() + wxT("Usage.txt"),
        wxT(""));  
    wxString result = MT_wxStringFromTextFile(usagetextfile);
    if(result.StartsWith(wxT("Error")))
    {
        result = wxString(wxT("Could not find usage text file.  ")) +
            wxString(wxT("Should be called ")) + wxTheApp->GetAppName() + 
            wxString(wxT("Usage.txt"));
    }
    return result;
}

MT_TimerBase* MT_FrameBase::createTimer()
{
    return new MT_TimerBase(this);
}

void MT_FrameBase::setTimer(int period_msec)
{
    if(!m_pTimer)
    {
        return;
    }

    if((period_msec == 0) && (m_iFramePeriod_msec == 0))
    {
        m_iFramePeriod_msec = MT_DEFAULT_FRAME_PERIOD;
    }

    if(period_msec > 0)
    {
        m_iFramePeriod_msec = period_msec;
    }

    m_bDoTimedEvents = true;
    m_pTimer->Start(m_iFramePeriod_msec);
}

void MT_FrameBase::stopTimer()
{
    if(!m_pTimer)
    {
        return;
    }

    m_pTimer->Stop();
}

void MT_FrameBase::setSizeByClient(int client_width, int client_height, int pad_height)
{

    /* scales the size to fit the screen by dividing it by 2 repeatedly */
    wxSize DesiredSize = MT_FitSizeToScreen(wxSize(client_width, client_height));

    /* The new size is the desired canvas size (size of the paintable area)
       + the extra size required by window borders, menus, etc, calculated by taking
       the difference between the current window size and the current canvas size*/
    wxSize NewSize = DesiredSize + wxFrame::GetSize() - m_pCanvas->GetSize() + wxSize(0, pad_height);

    /* we need to call SetSizeHints and setsize to make sure the window is
       allowed to actually be that size */
    //wxFrame::SetSizeHints(NewSize);
    wxFrame::SetSize(NewSize);
    /* we need to resize the canvas as well as the window, but the canvas
       gets the desired size */
    m_pCanvas->SetSizeHints(DesiredSize);
    m_pCanvas->SetSize(0, 0, DesiredSize.x, DesiredSize.y);  

    setViewport(MT_Rectangle(0, client_width, 0, client_height));

    wxSizeEvent dummy_event;
    onSize(dummy_event);

}

void MT_FrameBase::drawStatusBar(double frac)
{
    if(m_bDrawStatusBar && frac >= 0)
    {
        double xmin = getXMin();
        double xmax = getXMax();
        double ymin = getYMin();
        double ymax = getYMax();

        glColor4f(0,0,0,0.5);
        glBegin(GL_QUADS);
        glVertex2f(xmin, ymin);
        glVertex2f(xmax, ymin);
        glVertex2f(xmax, ymin + g_StatusBarFrac*(ymax - ymin));
        glVertex2f(xmin, ymin + g_StatusBarFrac*(ymax - ymin));
        glEnd();

        glColor4f(0,0,1,1);
        glBegin(GL_QUADS);
        glVertex2f(xmin, ymin);
        glVertex2f(frac*xmax, ymin);
        glVertex2f(frac*xmax, ymin + g_StatusBarFrac*(ymax - ymin));
        glVertex2f(xmin, ymin + g_StatusBarFrac*(ymax - ymin));
        glEnd();

    }
}

void MT_FrameBase::setupMovie(const wxString& filename)
{
    if(m_MovieExporter.getIsActive())
    {
        if(m_bMakingMovie)
        {
            m_MovieExporter.deactivate();
            m_bMakingMovie = false;
        }
        else
        {
            m_bMakingMovie = true;
        }
    }
    else
    {
        if(filename == wxEmptyString)
        {
            MT_MovieExporterDialog* dlg = new MT_MovieExporterDialog(
                m_sMovieExportDirectory,
                MT_ME_CV_VIDEO_WRITER,
                GetClientSize().x,
                GetClientSize().y,
                0,
                25,
                this);
            registerDialogForXML(dlg);
            int result = dlg->ShowModal();
            if(result == wxID_OK)
            {
                dlg->getInfo(&m_MovieExporter, &m_sMovieExportDirectory);
                dlg->Destroy();
                MT_ShowMessageDialog(this,
                                     wxT("Press 'm' to start recording"));
            }
            else
            {
                dlg->Destroy();
            }
        }
        else
        {
            m_MovieExporter.initForCvVideoWriter(filename.mb_str(), 
                                                 25, 
                                                 GetClientSize().x,
                                                 GetClientSize().y,
                                                 0);
            m_bMakingMovie = true;
        }
    }
}

/********************************************************************/
/*           MT_FrameBase public methods                            */
/********************************************************************/

bool MT_FrameBase::doKeyboardCallback(wxKeyEvent& event)
{

    /* Standard key responses */
    char key = event.GetKeyCode();

    wxCommandEvent dummy_event;

    switch(key)
    {
    case ' ':
        doStep();  break;
    case 'p':
        togglePause();  break;
        /*    case 'P':
              onMenuFilePreferences(dummy_event); break;*/
    case 'q':
        doQuit();  break;
    case '<':  /* On a mac this is the left arrow too for some reason */
        doStep(); break;
    case 'm':
        setupMovie();
    }

    return MT_DO_BASE_KEY;

}

bool MT_FrameBase::doMouseCallback(wxMouseEvent& event, 
                                   double viewport_x, 
                                   double viewport_y)
{
    long y = event.GetY();

    if(y > (1.0-g_StatusBarFrac)*GetClientSize().y)
    {
        m_bDrawStatusBar = true;
    }
    else
    {
        m_bDrawStatusBar = false;
    }
    ensureDraw();

    return MT_DO_BASE_MOUSE;
}

void MT_FrameBase::onMenuFileSaveScreen(wxCommandEvent& event)
{
    saveScreen();
}

void MT_FrameBase::onMenuFileCreateMovie(wxCommandEvent& event)
{
    setupMovie();
}

void MT_FrameBase::onMenuFileQuit(wxCommandEvent& event)
{
    doQuit();
}

void MT_FrameBase::onMenuFilePreferences(wxCommandEvent& event)
{
    MT_DataGroupDialog* dlg = new MT_DataGroupDialog(m_pPreferences, this);
    registerDialogForXML(dlg);
    dlg->Show(true);
}

void MT_FrameBase::onMenuViewZoom(wxCommandEvent& event)
{
    setZoomOn();
}

void MT_FrameBase::onMenuViewUnzoom(wxCommandEvent& event)
{
    resetZoom();
}

void MT_FrameBase::onMenuViewAutoZoom(wxCommandEvent& event)
{
    setAutoZoomOn();
}

void MT_FrameBase::onMenuHelpCheatSheet(wxCommandEvent& event)
{
    displayCheatSheet();
}

wxString MT_FrameBase::getCheatSheetText()
{
    return m_sDescriptionText + wxT("\n") + 
        getUsageTextFromFile() + wxT("\n") + 
        getCheatSheetTextFromFile();
}

MT_ControlFrameBase* MT_FrameBase::createControlDialog()
{
    return NULL;
}

void MT_FrameBase::makeFileMenu(wxMenu* file_menu)
{

#ifndef MT_NO_OPENCV    
    file_menu->Append(MT_ID_MENU_FILE_SAVESCREEN, wxT("Save Screen to Image File..."));
    wxFrame::Connect(MT_ID_MENU_FILE_SAVESCREEN,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_FrameBase::onMenuFileSaveScreen));

    file_menu->Append(MT_ID_MENU_FILE_CREATEMOVIE, wxT("Create Movie..."));
    wxFrame::Connect(MT_ID_MENU_FILE_CREATEMOVIE,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_FrameBase::onMenuFileCreateMovie));
#endif    

    file_menu->Append(wxID_PREFERENCES, wxT("Preferences"));
    wxFrame::Connect(wxID_PREFERENCES, 
                     wxEVT_COMMAND_MENU_SELECTED, 
                     wxCommandEventHandler(MT_FrameBase::onMenuFilePreferences));

    file_menu->AppendSeparator();

    file_menu->Append(wxID_EXIT, wxT("E&xit\t"));
    wxFrame::Connect(wxID_EXIT,
        wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MT_FrameBase::onMenuFileQuit));
}

void MT_FrameBase::makeViewMenu(wxMenu* view_menu)
{

    view_menu->Append(MT_ID_MENU_VIEW_ZOOM, 
                      wxT("&Zoom Window"));
    view_menu->Append(MT_ID_MENU_VIEW_UNZOOM, 
                      wxT("&Reset Zoom"));
    view_menu->Append(MT_ID_MENU_VIEW_AUTOZOOM,
                      wxT("&Auto Zoom (experimental)"));

    wxFrame::Connect(MT_ID_MENU_VIEW_ZOOM,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_FrameBase::onMenuViewZoom));
    wxFrame::Connect(MT_ID_MENU_VIEW_UNZOOM,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_FrameBase::onMenuViewUnzoom));
    wxFrame::Connect(MT_ID_MENU_VIEW_AUTOZOOM,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_FrameBase::onMenuViewAutoZoom));
}

void MT_FrameBase::makeHelpMenu(wxMenu* help_menu)
{
    help_menu->Append(MT_ID_MENU_HELP_CHEATSHEET, 
                      wxT("Cheat Sheet"));

    wxFrame::Connect(MT_ID_MENU_HELP_CHEATSHEET,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_FrameBase::onMenuHelpCheatSheet));
}

void MT_FrameBase::doReset(bool hard_reset)
{
    if(m_bQuitOnReset && hard_reset)
    {
        doQuit();
    }
}

/********************************************************************/
/*           MT_CheatSheetFrame ctors and dtors                        */
/********************************************************************/

MT_CheatSheetFrame::MT_CheatSheetFrame(wxFrame* parent, const wxString& help_text)
    :wxDialog(parent, 
              wxID_ANY, 
              wxTheApp->GetAppName() + wxT(" Help Cheat Sheet"), 
              wxDefaultPosition, 
              wxDefaultSize, 
              wxDEFAULT_DIALOG_STYLE)
{
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    vbox->Add(new wxTextCtrl(this, wxID_ANY, help_text, wxDefaultPosition, wxSize(500,500), wxTE_MULTILINE | wxTE_READONLY | wxTE_LEFT), 0, wxALL | wxALIGN_LEFT, 10);
    vbox->Add(CreateButtonSizer(wxOK), 0, wxALL | wxALIGN_RIGHT, 10);

    SetSizerAndFit(vbox);

}

