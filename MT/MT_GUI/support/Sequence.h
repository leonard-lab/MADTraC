#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <vector>
#include "wx/thread.h"
#include "MT/MT_Core/support/mathsupport.h"

const bool MT_SEQUENCE_NO_FORCE = false;
const bool MT_SEQUENCE_FORCE = true;

const int MT_SEQUENCE_ERR_LOCKED = -1;
const int MT_SEQUENCE_ERR_NEGATIVE = -2;
const int MT_SEQUENCE_ERR_EXISTS = -3;

const double MT_SEQUENCE_DEFAULT_MIN_INTERVAL = 0.002;  /* sec */

class MT_Sequence;

class MT_SequenceThread : public wxThread
{
private:
    double m_dT0;
    double m_dT;
    unsigned int m_iCurrentState;
    std::vector<double> m_vdTimes;

    MT_Sequence* m_pSequence;

public:
    MT_SequenceThread(MT_Sequence* parent_sequence,
                      std::vector<double> times);
    void* Entry();
    void OnExit();
};

class MT_Sequence
{
    friend class MT_SequenceThread;
private:
    std::vector<double> m_vdEventTimes;
    double m_dMinInterval;

    MT_SequenceThread* m_pSequenceThread;

    bool m_bIsRunning;
    bool m_bTimesLock;
    bool m_bThreadExitedNormally;

protected:
    void flagEvent(unsigned int state){onEvent(state);};
    void flagThreadIsDone();

public:
    MT_Sequence();
    MT_Sequence(std::vector<double> set_times,
                double min_interval = MT_SEQUENCE_DEFAULT_MIN_INTERVAL);

    virtual ~MT_Sequence();

    void setMinInterval(
        double min_interval = MT_SEQUENCE_DEFAULT_MIN_INTERVAL);

    virtual void setTimes(std::vector<double> set_times);
    std::vector<double> getTimes() const {return m_vdEventTimes;};

    int getInterval(double t);

    int pushTime(double t);

    bool getIsRunning() const {return m_bIsRunning;};

    virtual bool goSequence(bool force_stop = MT_SEQUENCE_NO_FORCE);
    virtual bool stopSequence();

    virtual void onEvent(unsigned int state){printf("Event %d\n", state);};
    virtual void onDone(){printf("Thread is done.\n");};

};

#endif // SEQUENCE_H
