#ifndef MT_PARAMETERDIALOG_H
#define MT_PARAMETERDIALOG_H
/*
 *  MT_ParameterDialog.h
 *
 *  A simple parameter display/editing dialog.  Supply the parameters
 *  as std::vector<double>& ParamVector and the names as  
 *  wxString[] ParamNames and the dialog shows this list with the values
 *  and gives the user a chance to edit them and click OK/Apply/Cancel.
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
      
    bool m_bDestroyGroupOnClose;
    
public:
    MT_DataGroupDialog(MT_DataGroup* datagroup, 
                       wxFrame* parent, 
                       bool DestroyGroupOnClose = MT_NO_DESTROY_GROUP_ON_CLOSE,
                       void (*pcallbackfunction)(void *ParentObject) = NULL, 
                       const wxPoint& pos = wxDefaultPosition, 
                       const wxSize& size = wxDefaultSize);
    ~MT_DataGroupDialog(){};
    
    void OnApplyButtonClicked(wxCommandEvent& event);
    void OnOKButtonClicked(wxCommandEvent& event);
    void OnCancelButtonClicked(wxCommandEvent& event);
    
    void OnTextCheck(wxCommandEvent& event);
    void OnColorButtonClicked(wxCommandEvent& event);
    
    void Update(){UpdateValues();};
    void UpdateValues();
    void WriteValues();
    
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
    
#endif // MT_PARAMETERDIALOG_H
