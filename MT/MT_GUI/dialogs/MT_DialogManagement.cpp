/*
 *  wxDialogManagement.cpp
 *
 *  Created by Daniel Swain on 10/26/09.
 *
 */

#include "MT_DialogManagement.h"

void MT_DialogInfo::Update()
{
  
    if(m_pDialog)
    {
        m_bShown = m_pDialog->IsShown();
        if(m_bShown)
        {
            m_pDialog->Update();
        }
    }
  
}

MT_DialogWithUpdate* MT_DialogGroup::GetDialog(unsigned int i) const
{
 
    if(i < 0 || i >= GetNumDialogs())
    {
        return NULL;
    }
    else
    {
        return m_pDialogInfos[i].m_pDialog;
    }
  
}

unsigned int MT_DialogGroup::GetNumDialogs() const
{
 
    return m_pDialogInfos.size();
  
}

bool MT_DialogGroup::ExistsInGroup(const string& key) const
{
    
    for(unsigned int i = 0; i < GetNumDialogs(); i++)
    {
        if(m_pDialogInfos[i].m_sKey == key)
        {
            return true;
        }
    }
  
    return false;

}

void MT_DialogGroup::RegisterAndShow(const string& key, MT_DialogWithUpdate* dialog)
{
  
    dialog->Show();
    MT_DialogInfo newinfo(key, dialog, true);
    m_pDialogInfos.push_back(newinfo);
  
}

void MT_DialogGroup::UpdateAll()
{

    for(unsigned int i = 0; i < GetNumDialogs(); i++)
    {
        m_pDialogInfos[i].Update();
    }
  
}
