#ifndef MT_ExperimentDataFile_H
#define MT_ExperimentDataFile_H

/*
 *  MT_ExperimentDataFile.h
 *
 *  Created by Daniel Swain on 12/15/09.
 *
 */

#include <vector>
#include <string>

#include "MT/MT_Core/fileio/XMLSupport.h"
#include "MT/MT_Core/primitives/DataGroup.h"

const bool MT_XDF_READ_ONLY = true;
const bool MT_XDF_READ_WRITE = false;

const bool MT_XDF_OK = true;
const bool MT_XDF_ERROR = false;

const unsigned int MT_XDF_MAX_PARAM_NAME_LENGTH = 256;

const bool MT_XDF_OVERWRITE = true;
const bool MT_XDF_NO_OVERWRITE = false;

const char* const MT_XDF_XML_ROOT = "TrackingDataExperimentData";
const char* const MT_XDF_XML_FILES_KEY = "DataFiles";
const char* const MT_XDF_XML_PARAMS_KEY = "Parameters";
const char* const MT_XDF_XML_USERNOTE_KEY = "User_Note";
const char* const MT_XDF_XML_VIDEOSOURCE_KEY = "Video_Source";

class MT_ExperimentDataFile
{
private:
    MT_XMLFile m_XMLFile;
    TiXmlElement* m_FilesNode;
    TiXmlElement* m_ParametersNode;

    std::vector<FILE*> m_vpDataFiles;
    std::vector<std::string> m_vFileNames;
    std::vector<std::string> m_vNames;

    std::vector<bool> m_vWroteThisTimeStep;

    std::vector<unsigned int> m_viCheckedOut;

    bool m_bReadOnly;
    mutable bool m_bStatus;

    bool setError() const {m_bStatus = MT_XDF_ERROR; return m_bStatus;};
    bool setOK() const {m_bStatus = MT_XDF_OK; return m_bStatus;};

    bool init(const char* filename, bool asreadonly, bool force_overwrite);

    int findStreamIndexByLabel(const char* label) const;

    TiXmlElement* getFilesNode();
    TiXmlElement* getParametersNode();

    void registerFile(const char* label, const char* filename);

protected:

public:
    MT_ExperimentDataFile();
    ~MT_ExperimentDataFile();

    bool initAsNew(const char* filename, bool force_overwrite = MT_XDF_OVERWRITE);
    bool initForReading(const char* filename);

    bool getStatus() const {return m_bStatus;};
    void clearStatus(){m_bStatus = MT_XDF_OK;};

    bool addDataStream(const char* label, 
                       const char* filename = NULL,
                       bool force_overwrite = MT_XDF_OVERWRITE);

    FILE* checkoutStreamByName(const char* stream_name);
    void releaseStream(FILE* file);

    bool writeData(const char* stream_name,
                   const std::vector<double>& data,
                   const char* format_str = "%11.10e ");

    bool writeParameterToXML(const char* param_name,
                             const char* value);

    bool writeDataGroupToXML(MT_DataGroup* dg);

    void flushTimeStep();

    bool getParameterString(const char* param_name, 
                            std::string* result)
        const;

    bool getDoubleValue(const char* stream_name,
                        int time_index,
                        double* value) 
        const;

    void writeXML();

};

#endif // MT_ExperimentDataFile_H
        
