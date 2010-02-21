#ifndef XMLSUPPORT_H
#define XMLSUPPORT_H

/*
 *  XMLSupport.h
 *
 *  Created by Daniel Swain on 1/9/09.
 *  
 *  Defines a class MT_XMLFile that is a convenience
 *  wrapper for the tinyxml functions.
 *
 */

#include <string>

// we use the tinyxml library as an access point
// it is included as source code, giving us the most portability 
//  (as it is already an incredibly portable library)
#include "MT/MT_Core/3rdparty/tinyxml/tinyxml.h"

#include "MT/MT_Core/primitives/DataGroup.h"

std::string MT_ReplaceSpacesInString(const std::string& input, char replacement = '_');
std::string MT_ReplaceCharWithSpaceInString(const std::string& input, char replacee);

void MT_AddOrReplaceNodeValue(TiXmlElement* parent_node, const char* node_name, const char* new_value);

class MT_XMLFile;

void ReadDataGroupFromXML(const MT_XMLFile& xmlfile, MT_DataGroup* dg);
void WriteDataGroupToXML(MT_XMLFile* xmlfile, MT_DataGroup* dg);

class MT_XMLFile
{
private:
    
    TiXmlDocument m_xmldoc;
    TiXmlHandle m_hRoot;
    char* m_pfilename;
    
    bool m_fileread;
    bool m_hasroot;
    
    void common_init();
    
public:
    
    MT_XMLFile(const char* filename);
    MT_XMLFile();
    ~MT_XMLFile();
    
    void SetFilename(const char* filename);
    const char* GetFilename() const;
    
    bool ReadFile(const char* filename = NULL);
    bool SaveFile(const char* filename = NULL);
    
    bool InitNew(const char* rootname);
    
    const char* Rootname() const;
    bool HasRoot() const;
    TiXmlElement* RootAsElement() const;
    bool HasRootname(const char* checkroot) const;
    
    TiXmlHandle FirstChild(const char* value) const;
    
};

#endif // XMLSUPPORT_H
