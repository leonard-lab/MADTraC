#ifndef CameraUtility_H
#define CameraUtility_H

/*
 *  CameraUtility.h
 *
 *  Created by Daniel Swain on 1/24/10.
 *
 */

#include "MT_Core.h"
#include "MT_GUI.h"
#include "MT_Tracking.h"

class CameraUtilityFrame : public MT_TrackerFrameBase
{
    private:
    protected:
        /* we'll automatically start the tracker base when
         * we set up a capture so that it will tell us the
         * frame rate */
        virtual void onNewCapture();

    public:
        CameraUtilityFrame(wxFrame* parent, 
                wxWindowID id = wxID_ANY, 
                const wxString& title = wxT("Camera View"),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxSize(640,480),
                long style = MT_FIXED_SIZE_FRAME);
        virtual ~CameraUtilityFrame(){};

        /* we eliminate the "Start Tracking" button */
        virtual MT_ControlFrameBase* createControlDialog();

        /* don't need some of the extra file menu options */
        virtual void makeFileMenu(wxMenu* file_menu);

        /* avoids creating tracker menu */
        virtual void createUserMenus(wxMenuBar* menubar){};

};

class CameraUtilityApp : public MT_AppBase
{
    public:
        MT_FrameWithInit* createMainFrame()
        {
            return new CameraUtilityFrame(NULL);
        };
};

#endif // CameraUtility_H
