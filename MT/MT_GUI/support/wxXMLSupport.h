#ifndef WXXMLSUPPORT_H
#define WXXMLSUPPORT_H

/** @file  wxXMLSupport.h
 *
 *  Created by Daniel Swain on 10/30/09.
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

#include "MT/MT_Core/fileio/XMLSupport.h"
#include "MT/MT_Core/primitives/DataGroup.h"

wxString MT_GetXMLPathForRobots();
wxString MT_GetXMLRootName();
wxString MT_GetXMLPathForApp();

/** Function to get the platform-independent directory for an XML file.  Creates the directory
    with permissions equivalent to 755 if it does not already exist.
    On OS X:  ~/Library/Application Support/<app name>/filename.xml
    On posix:  ~/.<app name>/filename.xml
    On Windows:  C:\Documents and Settings\<username>\Application Data\<app name>\filename.xml
    In case of failure to be able to write to the path, returns the filename without a path.
*/
wxString MT_GetXMLPath(const wxString& filename = wxEmptyString);

void MT_ReadWindowDataFromXML(const MT_XMLFile& xmlfile, const wxString& Key, wxWindow* window);
void MT_WriteWindowDataToXML(MT_XMLFile* xmlfile, const wxString& Key, wxWindow* window);

class MT_PathGroup
{
protected:
    wxString m_sGroupName;
    std::vector<wxString> m_vsNames;
    std::vector<wxString*> m_vpsPaths;
    
public:
    MT_PathGroup(const wxString& groupname);
    
    wxString GetGroupName() const;
    int GetPathIndexByName(const wxString& path) const;
    bool HavePath(const wxString& path) const;
    void AddPath(const wxString& name, wxString* path);
    wxString GetPathByName(const wxString& name) const;
    void SetPathByName(const wxString& name, const wxString& path);
    
    void ReadFromXML(const MT_XMLFile& xmlfile);
    void WriteToXML(MT_XMLFile* xmlfile);
    
};


#endif // WXXMLSUPPORT_H
