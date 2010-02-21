/*
 *  XMLSupport.cpp
 *
 *  Created by Daniel Swain on 1/9/09.
 *
 */

#include "XMLSupport.h"

std::string MT_ReplaceSpacesInString(const std::string& input, const char replacement)
{
    std::string result = input;
    for(unsigned int i = 0; i < result.length(); i++)
    {
        if(result[i] == ' ')
        {
            result[i] = replacement;
        }
    }
    return result;
}

std::string ReplaceCharWithSpaceInString(const std::string& input, const char replacee)
{
    std::string result = input;
    for(unsigned int i = 0; i < result.length(); i++)
    {
        if(result[i] == replacee)
        {
            result[i] = ' ';
        }
    }
    return result;

}

void MT_AddOrReplaceNodeValue(TiXmlElement* parent_node, const char* node_name, const char* new_value)
{
  
    if(!parent_node)
    {
        return;
    }
  
    TiXmlNode* new_node;
    TiXmlNode* old_node;
  
    old_node = parent_node->FirstChild(node_name);
    new_node = new TiXmlElement(node_name);
    new_node->LinkEndChild(new TiXmlText(new_value));
  
    if(old_node)
    {
        parent_node->ReplaceChild(old_node, *new_node);
        /* ReplaceChild copies new_node, so we need to delete ours */
        delete new_node;
    }
    else
    {
        parent_node->LinkEndChild(new_node);
        /* LinkEndChild does NOT copy new_node -> the document now owns it,
         * so we should NOT delete it */
    }
  
}

void ReadDataGroupFromXML(const MT_XMLFile& xmlfile, MT_DataGroup* dg)
{
  
    if(!dg)  /* can't have a null group... */
    {
        return;
    }
  
    std::string NodeName = "Data_Group_" + MT_ReplaceSpacesInString(dg->GetGroupName(),'_');
  
    TiXmlElement* pElem = xmlfile.FirstChild(NodeName.c_str()).FirstChild().Element();
    for(/* pElem already initialized */; pElem; pElem = pElem->NextSiblingElement())
    {
        const char *pKey = pElem->Value();
        const char *pText = pElem->GetText();
        if(pKey && pText)
        {
            std::string fixed_key = ReplaceCharWithSpaceInString(pKey, '_');
            dg->SetStringValueByName(fixed_key, std::string(pText));
        }
    }
  
}

void WriteDataGroupToXML(MT_XMLFile* xmlfile, MT_DataGroup* dg)
{
  
    if(!dg)
    {
        return;
    }
  
    std::string NodeName = "Data_Group_" + MT_ReplaceSpacesInString(dg->GetGroupName(),'_');
    TiXmlElement* msgs = xmlfile->FirstChild(NodeName.c_str()).Element();
    if(!msgs)
    {
        msgs = new TiXmlElement(NodeName.c_str());
        xmlfile->RootAsElement()->LinkEndChild(msgs);
    }
    TiXmlNode* msg;
    TiXmlNode* old_msg;
    for(unsigned int i = 0; i < dg->GetGroupSize(); i++)
    {
        std::string curr_name = MT_ReplaceSpacesInString(dg->GetNameString(i), '_');
        old_msg = msgs->FirstChild(curr_name.c_str());
        msg = new TiXmlElement(curr_name.c_str());
        std::string curr_value = dg->GetStringValue(i);
        msg->LinkEndChild(new TiXmlText(curr_value.c_str()));
        if(old_msg)
        {
            msgs->ReplaceChild(old_msg, *msg);
            /* ReplaceChild copies msg, so we need to delete ours */
            delete msg;
        }
        else
        {
            msgs->LinkEndChild( msg );
            /* LinkEndChild does NOT copy msg -> the document now owns it,
            * so we should NOT delete it */
        }
    }  
  
}

MT_XMLFile::MT_XMLFile(const char* filename)
  : m_hRoot(0)
{
  
    common_init();
  
    SetFilename(filename);
  
}

MT_XMLFile::MT_XMLFile()
  : m_hRoot(0),
  m_pfilename(NULL)
{
  
    common_init();
  
}


void MT_XMLFile::common_init()
{
  
    m_fileread = false;
    m_hasroot = false;
    m_pfilename = NULL;
  
}
  
MT_XMLFile::~MT_XMLFile()
{
  
    if(m_pfilename)
    {
        free(m_pfilename);
    }
  
}

void MT_XMLFile::SetFilename(const char* filename)
{
  
    if(m_pfilename)
    {
        free(m_pfilename);
    }
  
    unsigned int length = strlen(filename) + 1;
    m_pfilename = (char *) malloc(length);
    strcpy(m_pfilename, filename);

}

const char* MT_XMLFile::GetFilename() const
{
  
    return m_pfilename;
  
}

bool MT_XMLFile::ReadFile(const char* filename)
{
  
    m_fileread = false;
  
    if(filename)
    {
        SetFilename(filename);
    }

    if(!m_xmldoc.LoadFile(m_pfilename))
    {
        fprintf(stderr, "Load Error:  %s\n", m_xmldoc.ErrorDesc());
        return false;
    }

    // check to make sure document has a root
    TiXmlHandle hDoc(&m_xmldoc);
    TiXmlElement* elem = hDoc.FirstChildElement().ToElement();
    if(!elem)
    {
        return false;
    }

    m_hRoot = TiXmlHandle(elem);
    m_hasroot = true;
    m_fileread = true;
  
    return true;

}

bool MT_XMLFile::SaveFile(const char* filename)
{

    bool status;
  
    if(!filename)
    {
        if(m_pfilename)
        {
            status =  m_xmldoc.SaveFile(m_pfilename);
        }
        else
        {
            /* can't save a file without a filename */
            return false;
        }
    }
    else
    {
        // note we do not set the filename, allows us to save the file with any filename
        status = m_xmldoc.SaveFile(filename);
    }
      
    return status;
    
}

bool MT_XMLFile::InitNew(const char* rootname)
{

    if(!m_xmldoc.NoChildren() || !rootname)
    {
        return false;
    }
  
    m_xmldoc.LinkEndChild(new TiXmlDeclaration("1.0", "", ""));
  
    TiXmlElement *newroot = new TiXmlElement(rootname);
    m_xmldoc.LinkEndChild(newroot);
    m_hRoot = TiXmlHandle(newroot);
    m_hasroot = true;
  
    return true;

}

const char*  MT_XMLFile::Rootname() const
{
  
    return m_hRoot.ToElement()->Value();

}

bool MT_XMLFile::HasRoot() const
{
  
    return m_hasroot;
  
}

TiXmlElement* MT_XMLFile::RootAsElement() const
{
  
    return m_hRoot.ToElement();
  
}

bool MT_XMLFile::HasRootname(const char* checkroot) const
{

    const char* myroot = Rootname();
  
    if(!strcmp(myroot,checkroot))
    {
        return true;
    }
    else
    {
        return false;
    }

}

TiXmlHandle MT_XMLFile::FirstChild(const char* value) const
{
  
    if(m_hasroot)
    {
        return m_hRoot.FirstChild(value);
    }
    else
    {
        return TiXmlHandle(0);
    }

}
