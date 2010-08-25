#ifndef MT_PARAMETERDIALOG_H
#define MT_PARAMETERDIALOG_H

/** @addtogroup MT_GUI
 * @{
 */

/** @file
 *  MT_ParameterDialog.h
 *
 *  @brief Dialog for editing values in an MT_DataGroup
 *
 *  Created by Daniel Swain on 8/3/09.
 *  Modified from Kalman dialog to be general by DTS on 8/12/09
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

#include "MT/MT_GUI/dialogs/MT_DialogManagement.h"
#include "MT/MT_Core/primitives/DataGroup.h"

#include <vector>
#include <string>
using namespace std;

const bool MT_DESTROY_GROUP_ON_CLOSE = true;
const bool MT_NO_DESTROY_GROUP_ON_CLOSE = false;

/** @class MT_DataGroupDialog
 *
 * @brief Dialog for editing values in an MT_DataGroup.
 *
 * Displays the values in an MT_DataGroup and allows for their
 * editing unless they are read-only.  If an element has the
 * GetDialogIgnore flag set true (this must be done by calling
 * MT_DataGroup::SetDialogIgnore), it is omitted from the dialog.
 *
 * Currently all of the data types supported by MT_DataGroup are
 * supported by the dialog.
 * 
 * Numeric and string types are displayed in a standard text editing
 * control.  Numeric elements are automatically validated for numeric
 * contents.  Upon clicking "Apply" the values are set in the data
 * group and the resulting values are displayed - therefore the
 * potentially clamped values will be displayed.
 *
 * MT_Choice elements are displayed as a combo box with the string
 * values of the choices.
 *
 * MT_Color elements display a button that will show a system-native
 * color selection dialog box when pressed.
 *
 * Boolean elements display a check box.
 * 
 */
class MT_DataGroupDialog : public MT_DialogWithUpdate
{
protected:
    MT_DataGroup* m_pDataGroup;
    std::vector<wxTextCtrl*> m_pParamTextCtrls;
    std::vector<wxCheckBox*> m_pCheckBoxCtrls;
    std::vector<wxChoice*> m_pChoiceCtrls;
    std::vector<wxButton*> m_pColorButtons;
    std::vector<int> m_viIndexMap;
    void (*m_pCallBackFunction)(void* ParentObject);
    unsigned int m_iNParams;
    wxFrame* m_pParent;
      
    bool m_bDestroyGroupOnClose;
    
public:
    /** Ctor constructs the dialog but does not show it.
     * @param datagroup The MT_DataGroup whos data should be
     * displayed.
     * @param parent The parent wxFrame.  Usually "this" in the
     * calling code.
     * @param DestroyGroupOnClose Pass MT_DESTROY_GROUP_ON_CLOSE to
     * cause the group to be destroyed when the dialog closes.  This
     * can allow a temporary group to be created and used in a dialog
     * so that the group exists as long as the dialog is visible.  By
     * default, the group is not destroyed when the dialog closes.
     * @param pcallbackfunction Callback function to be called when
     * the OK or Apply buttons are pressed.  Normally this is NULL.
     * The callback function must be a statically linkable function.
     * The argument to the function should be the parent object.
     * @param pos Initial position of the window.  Not needed.
     * @param size Initial size of the window.  Not needed.  The
     * actual size will be set according to the number of elements in
     * the MT_DataGroup. */
    MT_DataGroupDialog(MT_DataGroup* datagroup, 
                       wxFrame* parent, 
                       bool DestroyGroupOnClose = MT_NO_DESTROY_GROUP_ON_CLOSE,
                       void (*pcallbackfunction)(void *ParentObject) = NULL, 
                       const wxPoint& pos = wxDefaultPosition, 
                       const wxSize& size = wxDefaultSize);
    ~MT_DataGroupDialog(){};

    /* TODO these should be private... */
    void OnApplyButtonClicked(wxCommandEvent& event);
    void OnOKButtonClicked(wxCommandEvent& event);
    void OnCancelButtonClicked(wxCommandEvent& event);
    void OnTextCheck(wxCommandEvent& event);
    void OnColorButtonClicked(wxCommandEvent& event);

    /** Calls UpdateValues.  If the dialog is registered with an
     * MT_DialogGroup, this will get called when the group is
     * updated.  Otherwise you can call it manually.  Not necessary
     * if the values aren't expected to change programmatically - so,
     * for example, parameters shouldn't really need to be updated.
     * @see UpdateValues */
    void Update(){UpdateValues();};
    /** Updates the values in the controls based on the value
     * returned by MT_DataGroup::GetStringValue
     * @see Update */
    virtual void UpdateValues();

    /** Saves the values in the dialog to the data group.  Gets
     * called when the user presses OK or Apply, but can be called
     * manually by code as well. */
    virtual void WriteValues();
    
};

class MT_ParameterDialog : public MT_DialogWithUpdate
{
    
protected:
    std::vector<wxStaticText*> m_pParamNamesText;
    std::vector<wxTextCtrl*> m_pParamTextCtrl;

    int nparams;
    std::vector<double>* m_pParams;
    void (*pCallBackFunction)(void* ParentObject);
        
public:
    MT_ParameterDialog(std::vector<double>&  ParamVector,
                       const wxString* ParamNames,
                       void (*inpCallBackFunction)(void* ParentObject),
                       const wxString& FrameCaption,
                       wxFrame* parent,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxSize(340,5*30 + 60));
    MT_ParameterDialog(std::vector<double>&  ParamVector,
                       const std::vector<string>& ParamNames,
                       void (*inpCallBackFunction)(void* ParentObject),
                       const wxString& FrameCaption,
                       wxFrame* parent,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxSize(340,5*30 + 60));
    ~MT_ParameterDialog();
    
    void OnApplyButtonClicked(wxCommandEvent& event);
    void OnOKButtonClicked(wxCommandEvent& event);
    void OnCancelButtonClicked(wxCommandEvent& event);
    
    virtual void SetParams();
        
    DECLARE_EVENT_TABLE()
    
        };

/** @} */
    
#endif // MT_PARAMETERDIALOG_H
