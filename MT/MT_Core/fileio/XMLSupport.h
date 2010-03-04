#ifndef XMLSUPPORT_H
#define XMLSUPPORT_H

/** @addtogroup MT_Core
 * @{
 */

/** @file
 *  XMLSupport.h
 *
 *  @brief Support for XML file i/o via tinyxml.
 *
 *  A couple support functions and a wrapper MT_XMLFile class are
 *  defined here.  The intention is to provide a simplified access to
 *  the functionality that tinyxml provides that is specialized to the
 *  needs of MT.
 *
 *  Note that there are further support functions defined in
 *  MT_GUI/wxXMLSupport.h - those functions depend upon wxWidgets
 *  objects, whereas these depend only upon standard c/c++, MT, and
 *  tinyxml. 
 *
 *  tinyxml is a compact, cross-platform library for reading and
 *  writing XML files.  The documentation for it is available at
 *  http://www.grinninglizard.com/tinyxmldocs/index.html
 *
 *  Because it is so small and simple to compile, tinyxml is included
 *  in the MT source tree in the MT_Core/3rdparty branch.
 *
 *  Created by Daniel Swain on 1/9/09.
 *
 *  @see wxXMLSupport.h
 *
 */

#include <string>

// we use the tinyxml library as an access point
// it is included as source code, giving us the most portability 
//  (as it is already an incredibly portable library)
#include "MT/MT_Core/3rdparty/tinyxml/tinyxml.h"

#include "MT/MT_Core/primitives/DataGroup.h"

/* TODO these should be moved into a support module */
/** Replace any spaces in the input string with the given replacement
 * character.  Default is an underscore. */
std::string MT_ReplaceSpacesInString(const std::string& input, char replacement = '_');
/** Find the given character in the string and replace it with
 * a space. */
std::string MT_ReplaceCharWithSpaceInString(const std::string& input, char replacee);

/** Searches parent_node for a node with the name node_name.  If it
 * exists, the value is replaced with new_value.  Otherwise a new node
 * is created. */
void MT_AddOrReplaceNodeValue(TiXmlElement* parent_node, const char* node_name, const char* new_value);

class MT_XMLFile;

/* TODO These should be changed to MT_ namespace. */
/** Function to read all information in an MT_DataGroup from an XML
 * file.  Only those keys found are modified.
 * @see MT_DataGroup
 * @see WriteDataGroupToXML */
void ReadDataGroupFromXML(const MT_XMLFile& xmlfile, MT_DataGroup* dg);
/** Function to write all information in an MT_DataGroup to an XML
 * file.
 * @see MT_DataGroup
 * @see ReadDataGroupFromXML */
void WriteDataGroupToXML(MT_XMLFile* xmlfile, MT_DataGroup* dg);

/** @class MT_XMLFile
 *
 * @brief Simplified interface to an XML file.
 *
 * Provides methods to read and write XML files using the tinyxml
 * library.  Usage is too complex to give a quick example here and
 * tends to be managed by other functions in MT.  For an example, see
 * the code for MT_FrameBase::writeXML() and MT_FrameBase::readXML().
 * In other words, you shouldn't need to access this object directly.
 * Instead create an MT_DataGroup and use ReadDataGroupFromXML and
 * WriteDataGroupToXML, or use an MT_ExperimentDataFile.  Even these
 * may not be necessary - for example MT_FrameBase::m_pPreferences is
 * read/written automatically as are the data groups in
 * MT_TrackerBase::m_vDataGroups. 
 *
 * @see ReadDataGroupFromXML
 * @see WriteDataGroupToXML
 * @see MT_ExperimentDataFile
 * @see XMLSupport.h
 * @see wxXMLSupport.h
 */
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
    /** Ctor to set the filename.  Does not read the file. */
    MT_XMLFile(const char* filename);
    /** Ctor for a blank object with no associated file. */
    MT_XMLFile();
    /** Dtor does not write the file.  This is done for safety
     * reasons - you may not WANT the data to be written. */
    ~MT_XMLFile();

    /** Set the file name.  Does not read the file. */
    void SetFilename(const char* filename);
    /** Query the file name. */
    const char* GetFilename() const;

    /** Read the file.  Makes sure the file has a valid XML structure
     * (specifically, a root element is checked for).  Returns false
     * on failure, true on succes.
     * @param filename Path of the file.  If NULL (default), the name
     * previously set by the ctor or SetFileName is used - or returns
     * false if neither was set. */
    bool ReadFile(const char* filename = NULL);
    /** Write the file to disk.  Returns true upon success, false on
     * failure. 
     * @param filename If specified, this filename is used, but the
     * object's filename (that reported by GetFileName()) is not
     * set.  This allows the file to be copied to any filename.  If
     * NULL, the GetFileName() name is used, or failure if it is not
     * set. */
    bool SaveFile(const char* filename = NULL);

    /** Initialize a new XML file with the given root name.  Returns
     * false if the file was previously initialized or no root name
     * is specified.  Returns true upon success. */
    bool InitNew(const char* rootname);
    
    /** Query the root name of the XML file.  Returns NULL if none is
     * available. */
    const char* Rootname() const;
    /** Return true if the XML file is properly initialized with a
     * valid root name.  Returns false if the file is not properly
     * initialized or has an invalid format and thus no root. */
    bool HasRoot() const;
    /** Return the root element of the XML file as a TiXmlElement
     * pointer.  This is the main method for manipulating the
     * contents of the XML file.  See the tinyxml documentation at
     * http://www.grinninglizard.com/tinyxmldocs/index.html */
    TiXmlElement* RootAsElement() const;
    /** Returns true if the root name of the file matches (as a
     * string) the given value.  Returns false otherwise.  This is a
     * good way to check if the XML file you've loaded has the right
     * *kind* of information - e.g. tracking data should have a root
     * name that is different than robot configuration parameters. */
    bool HasRootname(const char* checkroot) const;

    /** Return the first child element of the XML file's root as a
     * TiXmlHandle object.  Returns TiXmlHandle(0) if the file does
     * not have a root.  See the tinyxml documentation at
     * http://www.grinninglizard.com/tinyxmldocs/index.html */
    TiXmlHandle FirstChild(const char* value) const;
    
};

/** @} */

#endif // XMLSUPPORT_H
