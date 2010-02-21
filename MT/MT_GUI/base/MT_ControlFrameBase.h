#ifndef MT_CONTROLFRAMEBASE_H
#define MT_CONTROLFRAMEBASE_H

/** @file MT_ControlFrameBase.h
 *
 *  Created by Daniel Swain on 12/11/09 from wxControlFrameCore.h
 *
 */

#include "MT/MT_GUI/base/MT_FrameBase.h"

#if !wxUSE_STATUSBAR
#error "You need to set wxUSE_STATUSBAR to 1 to compile this program"
#endif // wxUSE_STATUSBAR

#include <wx/statusbr.h>

enum
{
    MT_CF_ID_BUTTON_PLAYPAUSE = wxID_HIGHEST + 1,
    MT_CF_ID_BUTTON_STEP,

    MT_CF_ID_HIGHEST_ /* leave this as the last ID! */
};

/** @def MT_CF_ID_HIGHEST
 * @hideinitializer
 * Highest ID claimed by the MT control frame base class.  
 * If you need to define IDs for your derived class,
 * define them like this:
 * @code
 * enum
 * {
 *   ID_MY_BUTTON = MT_CF_ID_HIGHEST + 1,
 *   ID_MY_MENU_ITEM,
 *   etc...
 * };
 * @endcode
 *
 * @note This <em>shouldn't</em> be necessary in classes
 * that are not derived from MT_ControlFrameBase.
 */
#define MT_CF_ID_HIGHEST MT_CF_ID_HIGHEST_

/* Flags to specify which buttons to have */
#define MT_CF_PLAYPAUSE  0x01
#define MT_CF_STEP       0x02
/* Default buttons = no auto-ID button */
#define MT_CF_DEFAULT_BUTTONS MT_CF_PLAYPAUSE | MT_CF_STEP

/* Forward declaration of MT_FrameBase so MT_ControlFrameBase knows about it. */
class MT_FrameBase;

/** @class MT_ControlFrameBase
 *
 * @brief Base control dialog with Play/Pause etc buttons.
 */
class MT_ControlFrameBase : public wxFrame
{
    friend class MT_FrameBase;
private:
    /* UI element pointers */
    /* button elements */
    wxButton* m_pButtonPlayPause;
    wxButton* m_pButtonStep;

    /* Member data */
    bool m_bDoEvents;
    int m_iButtons;
    wxString m_sStatusText;
    
protected:
    /* pointer to the owner frame */
    MT_FrameBase* m_pParentFrame;
   
    /* Functions to manually set UI */
    void setStatusText(const wxString& statustext);
   
public:
    /* Constructor */
    MT_ControlFrameBase(MT_FrameBase* parent,
                        int Buttons = MT_CF_DEFAULT_BUTTONS,
                        const wxPoint& pos = wxDefaultPosition, 
                        const wxSize& size = wxSize(150,300));
    virtual ~MT_ControlFrameBase(){};

    int getButtons() const {return m_iButtons;};

    virtual void doMasterInitialization();

    /* UI Creation */
    /* CreateButtons should create the buttons on the panel in the sizer
       and return the number of buttons created (for sizing purposes) */
    virtual unsigned int createButtons(wxBoxSizer* pSizer, wxPanel* pPanel);
    /* Button callbacks */
    virtual void onButtonPlayPausePressed(wxCommandEvent& event);
    virtual void onButtonStepPressed(wxCommandEvent& event);
    
    
    /* Functions that propagate events from the main frame */
    virtual void doQuit();
    virtual void onPlayPause(bool paused);
    virtual void enableButtons();
    
    DECLARE_EVENT_TABLE()
    
        };

#endif // MT_CONTROLFRAMEBASE_H
