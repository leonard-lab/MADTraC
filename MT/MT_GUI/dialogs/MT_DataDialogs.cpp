/*
 *  MT_DataDialogs.cpp
 *
 *  Created by Daniel Swain on 10/10/09.
 *
 */

#include "MT/MT_GUI/dialogs/MT_DataDialogs.h"
#include "MT/MT_GUI/support/wxSupport.h"   // for MT_StringVectorToWxArrayString
#include "MT/MT_Core/support/mathsupport.h"  // for MT_MIN

MT_DataDialog::MT_DataDialog(const wxArrayString& Headings,
                             const wxArrayString& Values,
                             const wxString& DialogTitle,
                             wxFrame* parent,
                             const wxPoint& pos)
  : MT_DialogWithUpdate(parent, 
                        wxID_ANY, 
                        DialogTitle, 
                        pos, 
                        wxSize(MT_MIN(100*Headings.GetCount()+MT_SCROLLBAR_WIDTH, MT_MAX_LISTDIALOG_WIDTH), 
                               MT_MAX_LISTDIALOG_HEIGHT), 
                        wxDEFAULT_DIALOG_STYLE)
{
  
    m_pListControl = new wxListCtrl(this, 
                                    wxID_ANY, 
                                    wxDefaultPosition, 
                                    GetClientSize(), 
                                    wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
    m_asHeadings = Headings;
  
    Update(Values);
  
}
           
MT_DataDialog::~MT_DataDialog()
{
  
}


void MT_DataDialog::Update(const wxArrayString& Values)
{
  
    wxArrayString values = Values;
  
    static bool have_columns = false;
  
    if(!have_columns)
    {
        wxListItem itemCol;
        itemCol.SetText(m_asHeadings[0]);
        m_pListControl->InsertColumn(0,itemCol);
        for(unsigned int i = 1; i < m_asHeadings.GetCount(); i++)
        {
            itemCol.SetText(m_asHeadings[i]);
            itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
            m_pListControl->InsertColumn(i,itemCol);
        }
    }
    
    m_pListControl->DeleteAllItems();
    unsigned int ncols = m_asHeadings.GetCount();
    
    long nrows;
    if(values.GetCount() == 0)
    {
        nrows = 1;
        for(unsigned int i = 0; i < ncols; i++)
        {
            values.Add(wxT("N/A"));
        }
    }
    nrows = (long) floor((float) values.GetCount()/m_asHeadings.GetCount());
    m_pListControl->Hide();
  
    long tmp;
  
    for(long row = 0; row < nrows; row++)
    {
        tmp = m_pListControl->InsertItem(row, values[row*ncols]);
        m_pListControl->SetItemData(tmp, row*ncols);
        for(unsigned int col = 1; col < ncols; col++)
        {
            if(row*ncols + col >= values.GetCount())
            {
                continue;
            }
            m_pListControl->SetItem(tmp, col, values[row*ncols + col]);
        } 
    }
  
    m_pListControl->Show();
  
    wxSize client_size = m_pListControl->GetClientSize();
    int col_width = (int) ((1.0/((float) ncols))*(client_size.x - MT_SCROLLBAR_WIDTH));
    if(!have_columns)
    {
        for(unsigned int i = 0; i < ncols; i++)
        {
            m_pListControl->SetColumnWidth(i, col_width);
        }
    }
  
    have_columns = true;
    
}

MT_VectorDataGroupDialog::MT_VectorDataGroupDialog(MT_VectorDataGroup* group, 
                                                   wxFrame* parent, 
                                                   const wxPoint& pos)
  : MT_DataDialog(MT_StringVectorToWxArrayString(group->GetNameStringArray()), 
                  MT_StringVectorToWxArrayString(group->GetStringValueArray()), 
                  MT_StringToWxString(group->GetGroupName()), 
                  parent, 
                  pos)
{
    m_pVectorDataGroup = group;
}

void MT_VectorDataGroupDialog::Update()
{
  
    MT_DataDialog::Update(MT_StringVectorToWxArrayString(m_pVectorDataGroup->GetStringValueArray()));
  
}

MT_DataDialog2Col::MT_DataDialog2Col(const wxArrayString& Names,
                                     const wxArrayString& Values,
                                     const wxString& DialogTitle,
                                     wxFrame* parent,
                                     const wxPoint& pos)
  : MT_DialogWithUpdate(parent, 
                        wxID_ANY, 
                        DialogTitle, 
                        pos, 
                        wxSize(200, MT_MIN(20*Names.GetCount() + 47, MT_MAX_LISTDIALOG_HEIGHT)), 
                        wxCAPTION | wxMINIMIZE_BOX | wxCLOSE_BOX)
{
  
    m_pListControl = new wxListCtrl(this, 
                                    wxID_ANY, 
                                    wxDefaultPosition, 
                                    GetClientSize(), 
                                    wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
  
    Update(Names, Values);
  
}

MT_DataDialog2Col::~MT_DataDialog2Col()
{
  
}

void MT_DataDialog2Col::Update(const wxArrayString& Names, const wxArrayString& Values)
{
  
    static bool have_columns = false;
  
    if(!have_columns)
    {
        wxListItem itemCol;
        itemCol.SetText(wxT("Name"));
        itemCol.SetImage(-1);
        m_pListControl->InsertColumn(0,itemCol);
    
        itemCol.SetText(wxT("Value"));
        itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
        m_pListControl->InsertColumn(1,itemCol);
    }
  
    have_columns = true;
  
    m_pListControl->DeleteAllItems();
  
    m_pListControl->Hide();
  
    long NItems = MT_MIN(Names.GetCount(), Values.GetCount());
    long tmp;
  
    for(long i = 0; i < NItems; i++)
    {
        tmp = m_pListControl->InsertItem(i, Names[i]);
        m_pListControl->SetItemData(tmp, i);
        m_pListControl->SetItem(tmp, 1, Values[i]);
    }
  
    m_pListControl->Show();
  
    wxSize client_size = m_pListControl->GetClientSize();
    int col_width = (int) (0.5*(client_size.x - MT_SCROLLBAR_WIDTH));
    m_pListControl->SetColumnWidth(0, col_width);
    m_pListControl->SetColumnWidth(1, col_width);
  
}
