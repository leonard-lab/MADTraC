#ifndef COMSEQUENCE_H
#define COMSEQUENCE_H

#include "MT/MT_GUI/support/Sequence.h"
 #include "MT/MT_Robot/io/ComIO.h"

const double MT_COM_SEQUENCE_DEFAULT_MIN_TIME = 0.05;

class COM_SEQ;

class MT_COMSequence
{
    friend class COM_SEQ;
private:
    MT_ComIO* m_pComPort;
    COM_SEQ* m_pSeq;

    FILE* m_pFile;

    double m_dMinComTime;
    std::vector<unsigned char*> m_vpData;
    std::vector<unsigned int> m_viNBytes;
    std::vector<bool> m_vbShifted;
    
    std::vector<double> m_vdRequestedTimes;
    std::vector<unsigned char*> m_vpRequestedData;
    std::vector<unsigned int> m_viRequestedNBytes;

    int pushSeqTime(double t);

protected:
    void onSeqEvent(unsigned int state);
    void onSeqDone(){onUserDone();};
public:
    static const bool s_bSkipBase = false;
    static const bool s_bDoBase = true;
    
    MT_COMSequence(const char* port, FILE* file = NULL);
    virtual ~MT_COMSequence();

    bool getIsConnected();
    bool forceReconnect(const char* port);

    int pushEvent(double t,
                  const unsigned char* data,
                  unsigned int n_bytes);

    void setMinDelay(double new_delay);

    void goSequence(bool force_stop = MT_SEQUENCE_NO_FORCE);
    bool getIsRunning() const;

    bool stopSequence();
    bool clearEvents(bool force_stop = MT_SEQUENCE_NO_FORCE);

    virtual bool onUserEvent(unsigned int state){return s_bDoBase;};
    virtual void onUserDone(){};

    std::string getEventTableAsString() const;
    
    bool sendData(const unsigned char* data, unsigned int nbytes);

    void setCOMFile(FILE* file){m_pFile = file;};

    std::vector<unsigned char> getDataForEvent(unsigned int index);

};

#endif // COMSEQUENCE_H
