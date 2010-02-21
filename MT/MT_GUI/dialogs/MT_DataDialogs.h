#ifndef MT_DATADIALOGS_H
#define MT_DATADIALOGS_H

/*
 *  MT_DataDialogs.h
 *
 *  Created by Daniel Swain on 10/10/09.
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

#include <wx/listctrl.h>

#include "MT/MT_Core/primitives/DataGroup.h"
#include "MT/MT_GUI/support/wxSupport.h"
#include "MT/MT_GUI/dialogs/MT_DialogManagement.h"

#ifndef MT_SCROLLBAR_WIDTH
#define MT_SCROLLBAR_WIDTH 18
#endif

#define MT_MAX_LISTDIALOG_HEIGHT 347
#define MT_MAX_LISTDIALOG_WIDTH 518

#include <string>
#include <vector>
using namespace std;

class MT_DataDialog : public MT_DialogWithUpdate
{
protected:
    wxListCtrl* m_pListControl;
    wxArrayString m_asHeadings;

public:
    MT_DataDialog(const wxArrayString& Headings,
                  const wxArrayString& Values,
                  const wxString& DialogTitle,
                  wxFrame* parent,
                  const wxPoint& pos = wxDefaultPosition);
    ~MT_DataDialog();
    
    void Update(const wxArrayString& Values);
    virtual void Update(){};
    
};

class MT_VectorDataGroupDialog : public MT_DataDialog
{
protected:
    MT_VectorDataGroup* m_pVectorDataGroup;
public:
    MT_VectorDataGroupDialog(MT_VectorDataGroup* group, wxFrame* parent, const wxPoint& pos = wxDefaultPosition);
    
    void Update();
};

typedef MT_VectorDataGroupDialog MT_DataReportDialog;

/* A 2 column data dialog.  Intended that the first column
   contains names of variables and the second column contains
   the actual values.  Create with the ctor then update with
   Update. */
class MT_DataDialog2Col : public MT_DialogWithUpdate
{
protected:
    wxListCtrl* m_pListControl;
public:
    MT_DataDialog2Col(const wxArrayString& Names,
                      const wxArrayString& Values,
                      const wxString& DialogTitle,
                      wxFrame* parent,
                      const wxPoint& pos = wxDefaultPosition);
    ~MT_DataDialog2Col();
    
    void Update(const wxArrayString& Names, const wxArrayString& Values);
    virtual void Update(){};
    
};

#endif // MT_DATADIALOGS_H
