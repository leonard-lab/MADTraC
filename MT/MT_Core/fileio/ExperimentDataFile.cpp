/*
 *  MT_ExperimentDataFile.cpp
 *
 *  Created by Daniel Swain on 12/15/09.
 *
 */

#include "ExperimentDataFile.h"

#include <sys/stat.h>

static const int MT_XDF_STREAM_INDEX_ERROR = -1;
static const bool MT_XDF_READ_ONLY = true;
static const bool MT_XDF_READ_WRITE = false;

#ifndef _WIN32
  static std::string path_sep = "/";
#else
  #include <direct.h>
  #define mkdir(a,b) _mkdir(a)
  static std::string path_sep = "\\";
#endif

static bool file_is_available(const char* name, const char* method = "r")
{
  FILE* tmp_file = fopen(name, method);
  if(tmp_file)
  {
    fclose(tmp_file);
    return true;
  }
  else
  {
    return false;
  }
}

static void mkdir_if_necessary(const char* dir_name)
{
    struct stat st;
    if(stat(dir_name, &st) == 0)
    {
        return;
    }
    else
    {
        mkdir(dir_name, 0777);
    }
}

/*static std::string directory_from_path(const std::string& path)
  {
  size_t found = path.find_last_of("/\\");
  return path.substr(0,found);
  }*/

static std::string dir_from_xdf(const std::string& xdf_path)
{
    return xdf_path.substr(0,xdf_path.length()-4) + path_sep;
}

static std::string fix_extension(const std::string& path)
{
    unsigned int l = path.length();
    if(l <= 5 || (path.substr(l-5, l-1) != ".xdf"))
    {
        return path + ".xdf";
    }
    else
    {
        return path;
    }
}

MT_ExperimentDataFile::MT_ExperimentDataFile()
    : m_XMLFile(),
      m_FilesNode(NULL),
      m_ParametersNode(NULL),
      m_vpDataFiles(0),
      m_vFileNames(0),
      m_vNames(0),
      m_vWroteThisTimeStep(0),
      m_viCheckedOut(0),
      m_bReadOnly(true),
      m_bStatus(MT_XDF_ERROR)
{
}

bool MT_ExperimentDataFile::init(const char* filename, 
                              bool asreadonly,
                              bool force_overwrite)
{
    m_bReadOnly = asreadonly;

    std::string _filename = fix_extension(std::string(filename));

    bool exists = file_is_available(_filename.c_str());

    if(m_bReadOnly == MT_XDF_READ_WRITE &&
       force_overwrite == MT_XDF_NO_OVERWRITE &&
       exists)
    {
        /* check to make sure the file doesn't exist */
        m_bStatus = MT_XDF_ERROR;
        fprintf(stderr,
                "File %s exists.  Try "
                "initAsNew(filename, MT_XDF_READ_WRITE, MT_XDF_OVERWRITE) "
                "if you really mean to overwrite this file.\n",
                _filename.c_str());
        return setError();
    }

    /* create or open the XML file */
    if(exists)
    {
        if(!m_XMLFile.ReadFile(_filename.c_str()))
        {
            fprintf(stderr,
                    "Error reading MT_ExperimentDataFile "
                    "%s.\n",
                    _filename.c_str());
            m_bStatus = MT_XDF_ERROR;
            return setError();
        }
        if(!m_XMLFile.HasRootname(MT_XDF_XML_ROOT))
        {
            fprintf(stderr,
                    "Error: File %s does not have the proper root name\n"
                    "  Root is %s, should be %s\n",
                    _filename.c_str(),
                    m_XMLFile.Rootname(),
                    MT_XDF_XML_ROOT);
            m_bStatus = MT_XDF_ERROR;
            return setError();
        }
    } 
    else
    {
        m_XMLFile.SetFilename(_filename.c_str());
        if(!m_XMLFile.InitNew(MT_XDF_XML_ROOT))
        {
            fprintf(stderr,
                    "Error:  Unable to create root %s in %s\n",
                    MT_XDF_XML_ROOT,
                    _filename.c_str());
            m_bStatus = MT_XDF_ERROR;
            return setError();
        }
    }
    return setOK();

}

bool MT_ExperimentDataFile::initAsNew(
    const char* filename,
    bool force_overwrite)
{
    return init(filename, MT_XDF_READ_WRITE, force_overwrite);
}

bool MT_ExperimentDataFile::initForReading(const char* filename)
{
    return init(filename, MT_XDF_READ_ONLY, MT_XDF_NO_OVERWRITE);
}

MT_ExperimentDataFile::~MT_ExperimentDataFile()
{
    /* close all the files */
    for(unsigned int i = 0; i < m_vpDataFiles.size(); i++)
    {
        if(!m_viCheckedOut[i])
        {
            /* fflush may not be necessary, but to make sure */
            fflush(m_vpDataFiles[i]);
            fclose(m_vpDataFiles[i]);
        }
    }

    /* write the XML */
    m_XMLFile.SaveFile();

}

int MT_ExperimentDataFile::findStreamIndexByLabel(const char* name) const
{
    for(unsigned int i = 0; i < m_vNames.size(); i++)
    {
        if(!strcmp(name, m_vNames[i].c_str()))
        {
            return i;
        }
    }

    return MT_XDF_STREAM_INDEX_ERROR;
}

TiXmlElement* MT_ExperimentDataFile::getFilesNode()
{
    if(!m_FilesNode)
    {
        m_FilesNode = m_XMLFile.FirstChild(MT_XDF_XML_FILES_KEY).Element();
        /* create if necessary */
        if(!m_FilesNode)
        {
            m_FilesNode = new TiXmlElement(MT_XDF_XML_FILES_KEY);
            m_XMLFile.RootAsElement()->LinkEndChild(m_FilesNode);
        }
    }
    return m_FilesNode;
}

TiXmlElement* MT_ExperimentDataFile::getParametersNode()
{
    if(!m_ParametersNode)
    {
        m_ParametersNode = m_XMLFile.FirstChild(MT_XDF_XML_PARAMS_KEY).Element();
        /* create if necessary */
        if(!m_ParametersNode)
        {
            m_ParametersNode = new TiXmlElement(MT_XDF_XML_PARAMS_KEY);
            m_XMLFile.RootAsElement()->LinkEndChild(m_ParametersNode);
        }
    }
    return m_ParametersNode;
}

void MT_ExperimentDataFile::registerFile(
    const char* label,
    const char* filename)
{
    std::string _label(label);

    /* the label can't have any spaces */
    _label = MT_ReplaceSpacesInString(_label);

    MT_AddOrReplaceNodeValue(getFilesNode(), _label.c_str(), filename);
}

FILE* MT_ExperimentDataFile::checkoutStreamByName(const char* stream_name)
{
    if(m_bStatus == MT_XDF_ERROR)
    {
        return NULL;
    }

    int i = findStreamIndexByLabel(stream_name);
    if(i == MT_XDF_STREAM_INDEX_ERROR)
    {
        return NULL;
    }

    m_viCheckedOut[i]++;

    return m_vpDataFiles[i];

}

void MT_ExperimentDataFile::releaseStream(FILE* file)
{
    for(unsigned int i = 0; i < m_vpDataFiles.size(); i++)
    {
        if(file == m_vpDataFiles[i])
        {
            if(m_viCheckedOut[i])
            {
                m_viCheckedOut[i]--;
            }
        }
    }
}

bool MT_ExperimentDataFile::addDataStream(const char* label,
                                       const char* filename,
                                       bool force_overwrite)
{
    if(m_bStatus == MT_XDF_ERROR)
    {
        return setError();
    }

    /* if this is already one of our streams, return true
     * without doing anything */
    if(findStreamIndexByLabel(label) != MT_XDF_STREAM_INDEX_ERROR)
    {
        return setOK();
    }

    /* if filename == NULL, use filename = label */
    std::string _filename(filename);
    if(!filename)
    {
        _filename = label;
    }

    std::string dir = dir_from_xdf(m_XMLFile.GetFilename());
    _filename = dir + _filename;
    mkdir_if_necessary(dir.c_str());

    /* check to see if filename exists (for reading) */
    bool exists = file_is_available(_filename.c_str(), "r");

    /* if it does and force_overwrite is false, then
     * return false. if force_overwrite is true, then
     * overwrite the file. */
    if(exists)
    {
        if(force_overwrite == MT_XDF_NO_OVERWRITE)
        {
            fprintf(stderr,
                    "File %s already exists.  If you are sure you want "
                    "to overwrite it, try addDataStream(label, filename, MT_XDF_OVERWRITE).\n",
                    _filename.c_str());
            return setError();
        }
    }

    /* make sure we can write to the file (this will erase the file if it's there!) */
    FILE* fp = fopen(_filename.c_str(), "w");
    if(!fp)
    {
        fprintf(stderr,
                "Error: Can't write to file %s.  Unable to "
                "add to experiment data stream.\n",
                _filename.c_str());
        return setOK();
    }

    /* register the file in the XML master */
    registerFile(label, _filename.c_str());
    /* save the file */
    m_XMLFile.SaveFile();

    /* set up the various registers */
    m_vpDataFiles.push_back(fp);
    m_vFileNames.push_back(_filename);
    m_vNames.push_back(label);
    m_vWroteThisTimeStep.push_back(false);
    m_viCheckedOut.push_back(0);

    /* if everything goes well, return true */
    return setOK();

}

bool MT_ExperimentDataFile::writeData(const char* stream_name,
                                   const std::vector<double>& data,
                                   const char* format_str)
{
    if(m_bStatus == MT_XDF_ERROR)
    {
        return setError();
    }

    /* find the file index by label */
    int i = findStreamIndexByLabel(stream_name);

    /* if not found, return false */
    if(i == MT_XDF_STREAM_INDEX_ERROR)
    {
        fprintf(stderr, "Error:  No stream with label %s\n", stream_name);
        return setError();
    }

    FILE* fp = m_vpDataFiles[i];
    unsigned int n_data = data.size();

    /* if the data vector's size is 0, write NaN */
    if(n_data == 0)
    {
        fprintf(fp, "NaN \n");
    }
    else
    {
        /* otherwise write the data to the file */
        for(unsigned int i = 0; i < n_data; i++)
        {
            fprintf(fp, format_str, data[i]);
        }
        /* append a newline at the end of the data */
        fprintf(fp, "\n");
    }

    /* mark that we've written to this file this time step */
    m_vWroteThisTimeStep[i] = true;

    /* return true to indicate success */
    return setOK();

}

bool MT_ExperimentDataFile::writeParameterToXML(const char* param_name,
                                             const char* value)
{
    if(m_bStatus == MT_XDF_ERROR)
    {
        return setError();
    }

    std::string _label(param_name);

    /* the label can't have any spaces */
    _label = MT_ReplaceSpacesInString(_label);

    MT_AddOrReplaceNodeValue(getParametersNode(), _label.c_str(), value);

    /* save the file */
    m_XMLFile.SaveFile();

    return setOK();
}

bool MT_ExperimentDataFile::writeDataGroupToXML(MT_DataGroup* dg)
{
    if(m_bStatus == MT_XDF_ERROR)
    {
        return setError();
    }
    WriteDataGroupToXML(&m_XMLFile, dg);
    return setOK();
}

void MT_ExperimentDataFile::flushTimeStep()
{
    if(m_bStatus == MT_XDF_ERROR)
    {
        return;
    }

    /* loop through data files. */
    /* any file not written to during this time step gets
     * a NaN written on that line */
    unsigned int nfiles = m_vpDataFiles.size();
    for(unsigned int i = 0; i < nfiles; i++)
    {
        if(!m_vWroteThisTimeStep[i])
        {
            fprintf(m_vpDataFiles[i], "NaN \n");
        }
        /* do an fflush to be safe */
        fflush(m_vpDataFiles[i]);
        /* clear the write flags to indicate that
         * we're ready to move on to the next time step */
        m_vWroteThisTimeStep[i] = false;
    }
}

bool MT_ExperimentDataFile::getParameterString(const char* param_name,
                                            std::string* result)
    const 
{
    TiXmlElement* pElem = m_XMLFile.FirstChild(MT_XDF_XML_PARAMS_KEY).FirstChild(param_name).Element();
    if(!pElem)
    {
        /* just return error - don't use setError because this
         * just means the key is not there - it's not a problem
         * with the file itself. */
        return MT_XDF_ERROR;
    }
    else
    {
        *result = std::string(pElem->GetText());
        return MT_XDF_OK;
    }
}

bool MT_ExperimentDataFile::getDoubleValue(const char* stream_name,
                                        int time_index,
                                        double* value) const
{
    return MT_XDF_ERROR;
}

void MT_ExperimentDataFile::writeXML()
{

    if(m_bStatus == MT_XDF_ERROR)
    {
        return;
    }

    /* write the XML file to disk. */
    m_XMLFile.SaveFile();

}
