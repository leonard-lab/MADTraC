#ifndef MT_DIALOGMANAGEMENT_H
#define MT_DIALOGMANAGEMENT_H

/*
 *  wxDialogManagement.h
 *
 *  Created by Daniel Swain on 10/26/09.
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

#include <vector>
#include <string>
using namespace std;

/* a wxDialog derivative class that has update functions */
class MT_DialogWithUpdate : public wxDialog
{
protected:
    bool m_bAcceptUpdate;
public:
MT_DialogWithUpdate(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE)
    : wxDialog(parent, id, title, pos, size, style){m_bAcceptUpdate = true;};
    
    virtual void Update(){};
};

/* class to keep track of the status of a dialog */
class MT_DialogInfo
{
public:
    MT_DialogWithUpdate* m_pDialog;
    bool m_bShown;
    string m_sKey;
    
    MT_DialogInfo(){m_sKey = "unknown"; m_pDialog = NULL; m_bShown = false;};
    MT_DialogInfo(const string& key, MT_DialogWithUpdate* dlg, bool shown = false){m_sKey = key; m_pDialog = dlg; m_bShown = shown;};
    
    void Update();
    void SafeDestroyDialog(){if(m_pDialog){m_pDialog->Destroy();}};
    
};

class MT_DialogGroup
{
protected:
    std::vector<MT_DialogInfo> m_pDialogInfos;
public:
    MT_DialogGroup(){m_pDialogInfos.resize(0);};
    
    MT_DialogWithUpdate* GetDialog(unsigned int i) const;
    unsigned int GetNumDialogs() const;
    bool ExistsInGroup(const string& key) const;
    void RegisterAndShow(const string& key, MT_DialogWithUpdate* dialog);
    void UpdateAll();
    
    void SafeDestroyDialogs(){for(unsigned int i = 0; i < m_pDialogInfos.size(); i++){m_pDialogInfos[i].SafeDestroyDialog();}};
    
};

#endif
