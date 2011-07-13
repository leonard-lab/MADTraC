#include "MT_CalibrationDataFile.h"

#include <sstream>
#include <iomanip>
#include <time.h>

#include "MT/MT_Core/support/filesupport.h"
#include "MT/MT_Core/support/mathsupport.h"

static const std::string tag_line_start = "Camera calibration - ";
static const std::string tag_line_middle = " rows w/lengths [";
static const std::string tag_line_end = "] - created ";
static const unsigned int num_rows_expected = 9;
static const int row_lengths_expected[] = {-1, 2, 2, 1, 5, 3, 3, 3, 3};

static bool readAndValidateLine(FILE* f, int i, double* data)
{
    std::vector<double> row_data = MT_ReadDoublesToEndOfLine(f);

    if((int) row_data.size() != row_lengths_expected[i])
    {
        fprintf(stderr,
                "MT_CalibrationDataFile read error: Row length mismatch [b, %d]\n",
                i);
        return false;
    }

    for(int j = 0; j < row_lengths_expected[i]; j++)
    {
        data[j] = row_data[j];
    }

    return true;
}

static bool validateAndRead(FILE* f, MT_CalibrationData* data)
{
    std::string line1 = MT_TextToEndOfLine(f);

    int r = line1.substr(0, tag_line_start.length()).compare(tag_line_start);
    if(r != 0)
    {
        fprintf(stderr, "MT_CalibrationDataFile read error:  Mismatch [a] in head of file\n");
        return false;
    }

    line1 = line1.substr(tag_line_start.length());

    std::stringstream ss;
    ss.str(line1);
    int num_rows;
    std::string v;
    ss >> num_rows;

    if(num_rows != (int) num_rows_expected)
    {
        fprintf(stderr, "MT_CalibrationDataFile read error:  Number of rows mismatch [a]\n");
        return false;
    }

    int nr = ss.tellg();
    line1 = line1.substr(nr);

    r = line1.substr(0, tag_line_middle.length()).compare(tag_line_middle);
    if(r != 0)
    {
        fprintf(stderr, "MT_CalibrationDataFile read error:  Mismatch [b] in head of file\n");
        return false;
    }

    line1 = line1.substr(tag_line_middle.length());
    ss.str(line1);
    for(int i = 0; i < num_rows; i++)
    {
        ss >> nr;
        if(nr != row_lengths_expected[i])
        {
            fprintf(stderr,
                    "MT_CalibrationDataFile read error:  Row length mismatch [a, %d]\n",
                    i);
            return false;
        }
    }

    data->info = line1.substr(((int) ss.tellg()) + 4); /* strip "] - " */

    if(!readAndValidateLine(f, 1, data->f)){return false;};
    if(!readAndValidateLine(f, 2, data->c)){return false;};
    if(!readAndValidateLine(f, 3, &data->alpha)){return false;};
    if(!readAndValidateLine(f, 4, data->k)){return false;};
    if(!readAndValidateLine(f, 5, &(data->R[0]))){return false;};
    if(!readAndValidateLine(f, 6, &(data->R[3]))){return false;};
    if(!readAndValidateLine(f, 7, &(data->R[6]))){return false;};
    if(!readAndValidateLine(f, 8, data->T)){return false;};
    
    return true;
    
}

void MT_CopyCalibrationData(MT_CalibrationData* dest, const MT_CalibrationData& src)
{
    dest->info = std::string(src.info);
    memcpy(dest->f, src.f, 2*sizeof(double));
    memcpy(dest->c, src.c, 2*sizeof(double));
    dest->alpha = src.alpha;
    memcpy(dest->k, src.k, 5*sizeof(double));
    memcpy(dest->R, src.R, 9*sizeof(double));
    memcpy(dest->T, src.T, 3*sizeof(double));    
}

std::string MT_CalibrationDataToString(const MT_CalibrationData& calibData)
{
    std::string result;
    std::stringstream ss;

    ss << "Camera calibration:\n\t" << calibData.info << std::endl;
    ss << "\tFocal Parameters: " << calibData.f[0] << ", " << calibData.f[1] << std::endl;
    ss << "\tPrincipal Point: " << calibData.c[0] << ", " << calibData.c[1] << std::endl;
    ss << "\tSkew Coefficient: " << calibData.alpha << std::endl;
    ss << "\tRadial Distortion Ceofficients: " <<
        calibData.k[0] << ", " <<
        calibData.k[1] << ", " <<
        calibData.k[2] << ", " <<
        calibData.k[3] << ", " <<
        calibData.k[4] << ", " << std::endl;        
    ss << "\tRotation Matrix: \n\t  [" << 
        calibData.R[0] << " " <<
        calibData.R[1] << " " <<
        calibData.R[2] << "]\n\t  [" <<
        calibData.R[3] << " " <<
        calibData.R[4] << " " <<
        calibData.R[5] << "]\n\t  [" <<
        calibData.R[6] << " " <<
        calibData.R[7] << " " <<
        calibData.R[8] << "]\n";
    ss << "\tTranslation: \n\t  [" <<
       calibData.T[0] << " " <<
       calibData.T[0] << " " <<
        calibData.T[0] << "]\n";        

    return ss.str();
}

static bool validateOpenCVCalibration(const CvMat* cameraMatrix,
                                      const CvMat* distCoeffs,
                                      const CvMat* rvec,
                                      const CvMat* tvec,
                                      const CvMat* R)
{
    if(!cameraMatrix || !distCoeffs || !rvec || !tvec)
    {
        fprintf(stderr, "OpenCV Calibration Validation Error:  NULL pointer\n");
        return false;
    }

    if(cameraMatrix->rows != 3 || cameraMatrix->cols != 3)
    {
        fprintf(stderr,
                "OpenCV Calibration Validation Error:  Wrong camera matrix format\n");
        return false;
    }

    if(!( (distCoeffs->rows == 5 && distCoeffs->cols == 1)
          || (distCoeffs->cols == 5 && distCoeffs->rows == 1)))
    {
        fprintf(stderr,
                "OpenCV Calibration Validation Error:  Wrong distortion coefficient format\n");
        return false;
    }

    if(!( (rvec->rows == 3 && rvec->cols == 1)
          || (rvec->rows == 1 && rvec->cols == 3)))
    {
        fprintf(stderr,
                "OpenCV Calibration Validation Error:  Wrong rotation vector format\n");
        return false;
    }

    if(!( (tvec->rows == 3 && tvec->cols == 1)
          || (tvec->rows == 1 && tvec->cols == 3)))
    {
        fprintf(stderr,
                "OpenCV Calibration Validation Error:  Wrong translation vector format\n");
        return false;
    }

    if(R && (R->rows != 3 || R->cols != 3))
    {
        fprintf(stderr,
                "OpenCV Calibration Validation Error:  Wrong rotation matrix format\n");
        return false;
    }

    return true;
}

bool MT_CalibrationDataToOpenCVCalibration(const MT_CalibrationData& calibData,
                                           CvMat* cameraMatrix,
                                           CvMat* distCoeffs,
                                           CvMat* rvec,
                                           CvMat* tvec,
                                           CvMat* R)
{
    if(!validateOpenCVCalibration(cameraMatrix, distCoeffs, rvec, tvec, R))
    {
        return false;
    }

    cvZero(cameraMatrix);
    cvSetReal2D(cameraMatrix, 0, 0, calibData.f[0]);
    cvSetReal2D(cameraMatrix, 1, 1, calibData.f[1]);
    cvSetReal2D(cameraMatrix, 0, 2, calibData.c[0]);
    cvSetReal2D(cameraMatrix, 1, 2, calibData.c[1]);
    cvSetReal2D(cameraMatrix, 0, 1, calibData.alpha);
    cvSetReal2D(cameraMatrix, 2, 2, 1.0);

    cvSetReal1D(distCoeffs, 0, calibData.k[0]);
    cvSetReal1D(distCoeffs, 1, calibData.k[1]);
    cvSetReal1D(distCoeffs, 2, calibData.k[2]);
    cvSetReal1D(distCoeffs, 3, calibData.k[3]);
    cvSetReal1D(distCoeffs, 4, calibData.k[4]);

    CvMat* Rt = cvCreateMat(3, 3, CV_32FC1);

    cvSetReal2D(Rt, 0, 0, calibData.R[0]);
    cvSetReal2D(Rt, 0, 1, calibData.R[1]);
    cvSetReal2D(Rt, 0, 2, calibData.R[2]);
    cvSetReal2D(Rt, 1, 0, calibData.R[3]);
    cvSetReal2D(Rt, 1, 1, calibData.R[4]);
    cvSetReal2D(Rt, 1, 2, calibData.R[5]);
    cvSetReal2D(Rt, 2, 0, calibData.R[6]);
    cvSetReal2D(Rt, 2, 1, calibData.R[7]);
    cvSetReal2D(Rt, 2, 2, calibData.R[8]);

    if(R)
    {
        cvCopy(Rt, R);
    }

    cvRodrigues2(Rt, rvec);

    cvSetReal1D(tvec, 0, calibData.T[0]);
    cvSetReal1D(tvec, 1, calibData.T[1]);
    cvSetReal1D(tvec, 2, calibData.T[2]);

    cvReleaseMat(&Rt);

    return true;
}

bool MT_OpenCVCalibrationToCalibrationData(const CvMat* cameraMatrix,
                                           const CvMat* distCoeffs,
                                           const CvMat* rvec,
                                           const CvMat* tvec,
                                           MT_CalibrationData* calibData)
{
    if(!validateOpenCVCalibration(cameraMatrix, distCoeffs, rvec, tvec, NULL))
    {
        return false;
    }

    if(!calibData)
    {
        return false;
    }

    calibData->f[0] = cvGetReal2D(cameraMatrix, 0, 0);
    calibData->f[1] = cvGetReal2D(cameraMatrix, 1, 1);
    calibData->c[0] = cvGetReal2D(cameraMatrix, 0, 2);
    calibData->c[1] = cvGetReal2D(cameraMatrix, 1, 2);
    calibData->alpha = cvGetReal2D(cameraMatrix, 0, 1);

    calibData->k[0] = cvGetReal1D(distCoeffs, 0);
    calibData->k[1] = cvGetReal1D(distCoeffs, 1);
    calibData->k[2] = cvGetReal1D(distCoeffs, 2);
    calibData->k[3] = cvGetReal1D(distCoeffs, 3);
    calibData->k[4] = cvGetReal1D(distCoeffs, 4);

    CvMat* Rt = cvCreateMat(3, 3, CV_32FC1);

    cvRodrigues2(rvec, Rt);

    calibData->R[0] = cvGetReal2D(Rt, 0, 0);
    calibData->R[1] = cvGetReal2D(Rt, 0, 1);
    calibData->R[2] = cvGetReal2D(Rt, 0, 2);    
    calibData->R[3] = cvGetReal2D(Rt, 1, 0);
    calibData->R[4] = cvGetReal2D(Rt, 1, 1);
    calibData->R[5] = cvGetReal2D(Rt, 1, 2);    
    calibData->R[6] = cvGetReal2D(Rt, 2, 0);
    calibData->R[7] = cvGetReal2D(Rt, 2, 1);
    calibData->R[8] = cvGetReal2D(Rt, 2, 2);

    calibData->T[0] = cvGetReal1D(tvec, 0);
    calibData->T[1] = cvGetReal1D(tvec, 1);
    calibData->T[2] = cvGetReal1D(tvec, 2);    

    return true;
}

MT_CalibrationDataFile::MT_CalibrationDataFile(const char* filename,
    MT_CalibrationLoadMode mode)
    : m_bDidLoadOK(false),
      m_Data(),
      m_Mode(mode),
      m_sFileName(filename)
{
    if(mode == MT_CALIB_READ)
    {
        if(!MT_FileIsAvailable(filename, "r"))
        {
            fprintf(stderr, "MT_CalibrationDataFile Error:  Could not load %s", filename);
            return;
        }

        FILE* f = fopen(filename, "r");

        m_bDidLoadOK = validateAndRead(f, &m_Data);

        fclose(f);
    }
    else
    {
        if(!MT_FileIsAvailable(filename, "wa"))
        {
            fprintf(stderr, "MT_CalibrationDataFile Error:  Could not load %s", filename);
            return;
        }
        m_bDidLoadOK = true;
    }
    
}

MT_CalibrationDataFile::~MT_CalibrationDataFile()
{
}

bool MT_CalibrationDataFile::didLoadOK() const
{
    return m_bDidLoadOK;
}

bool MT_CalibrationDataFile::getCalibration(MT_CalibrationData* calibData)
{

    if(!calibData || !m_bDidLoadOK || (m_Mode != MT_CALIB_READ))
    {
        return false;
    }

    MT_CopyCalibrationData(calibData, m_Data);
    
    return true;
}

bool MT_CalibrationDataFile::writeCalibration(const MT_CalibrationData& calibData)
{
    if(!m_bDidLoadOK || (m_Mode != MT_CALIB_WRITE))
    {
        return false;
    }

    MT_CopyCalibrationData(&m_Data, calibData);

    std::ofstream f(m_sFileName.c_str());

    f << tag_line_start << num_rows_expected << tag_line_middle;
    for(unsigned int i = 0; i < num_rows_expected; i++)
    {
        f << " " << row_lengths_expected[i];
    }

    if(m_Data.info.length() == 0 || m_Data.info.compare("N/A") == 0)
    {        
        time_t nowtime;
        time(&nowtime);
    
        f << tag_line_end << ctime(&nowtime); // already includes newline (?)
    }
    else
    {
        f << tag_line_end << m_Data.info << std::endl;
    }

    f << m_Data.f[0] << " " << m_Data.f[1] << std::endl;
    f << m_Data.c[0] << " " << m_Data.c[1] << std::endl;
    f << m_Data.alpha << std::endl;
    f << m_Data.k[0] << " " << m_Data.k[1] << " " << m_Data.k[2] << " "
      << m_Data.k[3] << " " << m_Data.k[4] << std::endl;
    f << m_Data.R[0] << " " << m_Data.R[1] << " " << m_Data.R[2] << std::endl;
    f << m_Data.R[3] << " " << m_Data.R[4] << " " << m_Data.R[5] << std::endl;
    f << m_Data.R[6] << " " << m_Data.R[7] << " " << m_Data.R[8] << std::endl;
    f << m_Data.T[0] << " " << m_Data.T[1] << " " << m_Data.T[2] << std::endl;
    
    f.close();

    return true;
}
