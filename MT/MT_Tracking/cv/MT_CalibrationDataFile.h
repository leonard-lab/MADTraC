#ifndef CALIBRATIONDATAFILE_H
#define CALIBRATIONDATAFILE_H

#include <string>

#include <cv.h>

typedef struct MT_CalibrationData
{
    std::string info;
    double f[2];
    double c[2];
    double alpha;
    double k[5];
    double R[9];
    double T[3];

    MT_CalibrationData()
    {
        info = std::string("N/A");
        f[0] = f[1] = 0;
        c[0] = c[1] = 0;
        alpha = 0;
        k[0] = k[1] = k[2] = 0;
        R[0] = R[1] = R[2] =
            R[3] = R[4] = R[5] =
            R[6] = R[7] = R[8] = 0;
        T[0] = T[1] = T[2] = 0;
    }    
} MT_CalibrationData;

void MT_CopyCalibrationData(MT_CalibrationData* dest, const MT_CalibrationData& src);

std::string MT_CalibrationDataToString(const MT_CalibrationData& calibData);

bool MT_CalibrationDataToOpenCVCalibration(const MT_CalibrationData& calibData,
                                           CvMat* cameraMatrix,
                                           CvMat* distCoeffs,
                                           CvMat* rvec,
                                           CvMat* tvec,
                                           CvMat* R = NULL);
bool MT_OpenCVCalibrationToCalibrationData(const CvMat* cameraMatrix,
                                           const CvMat* distCoeffs,
                                           const CvMat* rvec,
                                           const CvMat* tvec,
                                           MT_CalibrationData* calibData);

typedef enum
{
    MT_CALIB_READ,
    MT_CALIB_WRITE
} MT_CalibrationLoadMode;

class MT_CalibrationDataFile
{
public:
    MT_CalibrationDataFile(const char* filename,
                           MT_CalibrationLoadMode mode = MT_CALIB_READ);
    ~MT_CalibrationDataFile();

    bool didLoadOK() const;
    bool getCalibration(MT_CalibrationData* calibData);
    bool writeCalibration(const MT_CalibrationData& calibData);

private:
    bool m_bDidLoadOK;
    MT_CalibrationData m_Data;
    MT_CalibrationLoadMode m_Mode;
    std::string m_sFileName;

};

#endif // CALIBRATIONDATAFILE_H
