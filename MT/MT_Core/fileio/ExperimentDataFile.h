#ifndef MT_ExperimentDataFile_H
#define MT_ExperimentDataFile_H

/** @addtogroup MT_Core
 * @{ */

/** @file
 *  MT_ExperimentDataFile.h
 *
 *  Created by Daniel Swain on 12/15/09.
 *
 */

#include <vector>
#include <string>

#include "MT/MT_Core/fileio/XMLSupport.h"
#include "MT/MT_Core/primitives/DataGroup.h"

/** @var const bool MT_XDF_OK
 * Returned by various MT_ExperimentDataFile methods to indicate
 * success or an OK status. */
const bool MT_XDF_OK = true;
/** @var const bool MT_XDF_ERROR
 * Returned by various MT_ExperimentDataFile methods to indicate an
 * error. */
const bool MT_XDF_ERROR = false;

/* QUARANTINE - should be deleted if next build is OK */
/* const unsigned int MT_XDF_MAX_PARAM_NAME_LENGTH = 256; */

/** @var const bool MT_XDF_OVERWRITE
 * Use with MT_ExperimentDataFile methods to indicate a file should be
 * overwritten if it already exists. */
const bool MT_XDF_OVERWRITE = true;
/** @var const bool MT_XDF_NO_OVERWRITE
 * Use with MT_ExperimentDataFile methods to indicate a file should
 * not be overwritten if it already exists. */
const bool MT_XDF_NO_OVERWRITE = false;

/* Constants used to write XML files.  Shouldn't be needed by the
 * user. */
const char* const MT_XDF_XML_ROOT = "TrackingDataExperimentData";
const char* const MT_XDF_XML_FILES_KEY = "DataFiles";
const char* const MT_XDF_XML_PARAMS_KEY = "Parameters";
const char* const MT_XDF_XML_USERNOTE_KEY = "User_Note";
const char* const MT_XDF_XML_VIDEOSOURCE_KEY = "Video_Source";

/** @class MT_ExperimentDataFile
 *
 * @brief Standardized data outputter.
 *
 * Creates and maintains a set of files representing output from an
 * experiment (real or numerical).  These consist of a master
 * XML-formatted ".xdf" file that contains human-readable information
 * relevant to the actual data and a series of files containing lines
 * of space-separated data, where each line represents one write to
 * the file (for example one time step).
 *
 * In MT, ExperimentDataFile is typically abbreviated as XDF.
 *
 * TODO Should give a short example of how to use XDF.
 */
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
    /** Create a new XDF data structure.  This is a blank constructor,
     * you should call initForNew or initForReading to initialize the
     * actual file. */
    MT_ExperimentDataFile();
    /** Destructor flushes output streams and closes each owned data
     *  file and saves the .xdf file.  Will not close files that are
     *  checked out.
     *  @see checkoutStreamByName
     *  @see releaseStream */
    ~MT_ExperimentDataFile();

    /** Initialize the XDF for writing with the given filename.
     * Returns MT_XDF_OK on success and MT_XDF_ERROR on error.  Prints
     * useful information to stderr to indicate source of error.
     * @param filename Path to the XDF file.  Needs to be in a
     * location that the program can write to.  Data files will be
     * located in the same directory as the XDF file.
     * @param force_overwrite Pass MT_XDF_NO_OVERWRITE to avoid
     * overwriting existing data.  Otherwise data will be overwritten
     * if it already exists.
     */
    bool initAsNew(const char* filename, bool force_overwrite = MT_XDF_OVERWRITE);

    /* Initialize for reading.  UNTESTED. */
    bool initForReading(const char* filename);

    /** Returns MT_XDF_OK if the file is configured successfully and
     * MT_XDF_ERROR otherwise. */
    bool getStatus() const {return m_bStatus;};
    /** Force the status to return to MT_XDF_OK.  Use cautiously - you
     * should know why the status is MT_XDF_ERROR if you are going to
     * change it. */
    void clearStatus(){m_bStatus = MT_XDF_OK;};

    /** Add a data file to the XDF.  Opens and maintains a FILE
     * pointer to the given file.  Returns MT_XDF_OK if successful,
     * MT_XDF_ERROR otherwise.  Fails to continue if the status was
     * MT_XDF_ERROR previously.
     * @param label A meaningful label used in the XDF to index the
     * data file.
     * @param filename Name of the output file.  label is used if
     * filename is NULL or missing.
     * @param force_overwrite Pass MT_XDF_NO_OVERWRITE to avoid
     * overwriting existing data.  Otherwise data will be overwritten
     * if it already exists.
     */
    bool addDataStream(const char* label, 
                       const char* filename = NULL,
                       bool force_overwrite = MT_XDF_OVERWRITE);

    /** Find the stream (file) by looking for its label (as was
     * specified in addDataStream) and return the FILE pointer.  This
     * will block the XDF from writing to this file or closing it.
     * @see releaseStream
     */
    FILE* checkoutStreamByName(const char* stream_name);
    /** Release the FILE stream from user control, in order to allow
     * the XDF to continue writing to it and to close the file upon
     * deletion of the instance.
     * @see checkoutStreamByName
     */
    void releaseStream(FILE* file);

    /** Write data to the stream according to its label.  Returns
     * MT_XDF_OK if successful, MT_XDF_ERROR otherwise.  The XDF keeps
     * track of which streams get written at each time step.  
     * @param stream_name label of the stream. MT_XDF_ERROR is
     * returned if no stream is found with this name.
     * @param data Vector of data to be written.
     * @param format_str Format string to be used when writing data.
     * Default is "%11.10e ".
     *
     * @see flushTimeStep
     */
    bool writeData(const char* stream_name,
                   const std::vector<double>& data,
                   const char* format_str = "%11.10e ");

    /** Write a single parameter to the XDF file.  This could be,
     * e.g. a setting used in the experiment or the name of an
     * associated file.
     * @param param_name Name of the parameter.
     * @param value Value of the parameter as a string.
     */
    bool writeParameterToXML(const char* param_name,
                             const char* value);

    /** Write an MT_DataGroup to the XDF file. */
    bool writeDataGroupToXML(MT_DataGroup* dg);

    /** Loop through each of the data streams and flush the output
     * stream.  Also, if the stream was not written to during that
     * time step, a NaN is written. */
    void flushTimeStep();

    /** Look for a parameter in the XDF.  Returns MT_XDF_OK if
     * successful, MT_XDF_ERROR otherwise.
     * @param param_name Name of the parameter.
     * @param result String where the result is stored.
     *
     * @see writeParameterToXML
     */
    bool getParameterString(const char* param_name, 
                            std::string* result)
        const;
    bool getParameterAsDouble(const char* param_name, double* val) const;
    bool getParameterNames(std::vector<std::string>* params_list) const;

    /* UNTESTED */
    bool getDoubleValue(const char* stream_name,
                        int time_index,
                        double* value) 
        const;

    /** Save the XML portion to disk.  Gets called automatically on
     * destructor. */
    void writeXML();

};

/* @} */

#endif // MT_ExperimentDataFile_H
        
