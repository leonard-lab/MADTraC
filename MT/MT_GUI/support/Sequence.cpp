#include "Sequence.h"

#include <algorithm>

#include "wx/utils.h"

/* simple sorting function */
static bool sort_func(double x, double y)
{
    return x < y;
}

MT_SequenceThread::MT_SequenceThread(MT_Sequence* parent_sequence,
                                     std::vector<double> times)
    : wxThread(),
      m_dT0(0),
      m_dT(0),
      m_iCurrentState(0),
      m_vdTimes(times),
      m_pSequence(parent_sequence)
{
}

void* MT_SequenceThread::Entry()
{
    /* initial time */
    m_dT0 = MT_getTimeSec();

    while(1)
    {
        /* makes sure the thread exits when necessary */
        if(wxThread::TestDestroy())
        {
            m_pSequence->flagThreadIsDestroyed();
            return NULL;
        }
        
        /* current time */
        m_dT = MT_getTimeSec() - m_dT0;

        if(m_dT >= m_vdTimes[m_iCurrentState])
        {
            m_pSequence->flagEvent(m_iCurrentState++);
            if(m_iCurrentState >= m_vdTimes.size())
            {
                /* done */
                m_pSequence->flagThreadIsDone();
                return NULL;
            }
        }

        /* theoretically this reduces the resolution to no better than
         * 1 msec, but it also reduces the CPU load significantly, so
         * if you really really need higher resolution and don't care
         * if CPU load goes up significantly, try commenting this */
        wxThread::Sleep(1);  /* 1 msec */

        /* makes sure the thread exits when necessary */
        if(wxThread::TestDestroy())
        {
            m_pSequence->flagThreadIsDestroyed();
            return NULL;
        }

    }

    return NULL;
}

void MT_SequenceThread::OnExit()
{
}

MT_Sequence::MT_Sequence()
    : m_vdEventTimes(0),
      m_dMinInterval(MT_SEQUENCE_DEFAULT_MIN_INTERVAL),
      m_pSequenceThread(NULL),
      m_bIsRunning(false),
      m_bTimesLock(false),
      m_bThreadExitedNormally(false),
      m_bThreadIsDestroyed(false)
{
}

MT_Sequence::~MT_Sequence()
{
    haltThread();
}

void MT_Sequence::haltThread()
{

    if(m_bIsRunning && m_pSequenceThread)
    {
        m_bThreadIsDestroyed = false;
        
        m_pSequenceThread->Delete();

        unsigned int attempts = 0;
        
        while(!m_bThreadIsDestroyed && attempts++ < 100){wxMilliSleep(10);};
        if(attempts >= 100)
        {
            fprintf(stderr, "WARNING - MT_Sequence::haltThread dould not stop"
                    "thread within 1 second.  Errors may result.\n");
        }

        onDone();
    }

    m_bIsRunning = false;
    m_bTimesLock = false;

}

void MT_Sequence::flagThreadIsDone()
{
    m_bThreadExitedNormally = true;
    m_bIsRunning = false;
    m_bTimesLock = false;
    stopSequence();
    onDone();
}

void MT_Sequence::flagThreadIsDestroyed()
{
    m_bThreadIsDestroyed = true;
}

void MT_Sequence::setMinInterval(double min_interval)
{
    if(min_interval <= 0)
    {
        return;
    }
    else
    {
        m_dMinInterval = min_interval;
        /* reset the times so that they obey the new min interval */
        std::vector<double> current_times = m_vdEventTimes;
        setTimes(current_times);
    }
}

void MT_Sequence::setTimes(std::vector<double> set_times)
{
    /* don't allow this if the times are locked */
    if(m_bTimesLock)
    {
        return;
    }

    /* clear existing times */
    m_vdEventTimes.resize(0);

    /* add the new times */
    for(unsigned int i = 0; i < set_times.size(); i++)
    {
        pushTime(set_times[i]);
    }
}

int MT_Sequence::getInterval(double t)
{
    if((t < 0) || (m_vdEventTimes.size() == 0))
    {
        return MT_SEQUENCE_ERR_NEGATIVE;
    }

    if(t < m_vdEventTimes[0])
    {
        return 0;
    }

    for(unsigned int i = 1; i < m_vdEventTimes.size(); i++)
    {
        if((t > m_vdEventTimes[i - 1]) && (t <= m_vdEventTimes[i]))
        {
            return i;
        }
    }
    
    return m_vdEventTimes.size() - 1;
}

int MT_Sequence::pushTime(double t)
{

    if(m_bTimesLock)
    {
        return MT_SEQUENCE_ERR_LOCKED;
    }
    
    if(t < 0.5*m_dMinInterval)
    {
        return MT_SEQUENCE_ERR_NEGATIVE;
    }

    if(m_vdEventTimes.size() == 0)
    {
        m_vdEventTimes.push_back(t);
        return m_vdEventTimes.size()-1;
    }

    for(unsigned int i = 0; i < m_vdEventTimes.size(); i++)
    {
        if(fabs(t - m_vdEventTimes[i]) < 0.5*m_dMinInterval)
        {
            return MT_SEQUENCE_ERR_EXISTS;
        }
    }

    m_vdEventTimes.push_back(t);
    /* sort_func defined above */
    sort(m_vdEventTimes.begin(), m_vdEventTimes.end(), sort_func);

    return getInterval(t);

}

bool MT_Sequence::goSequence(bool force_stop)
{
    if(m_bIsRunning)
    {
        /* already running */        
        if(!force_stop)
        {
            return false;
        }
        else
        {
            stopSequence();
        }
    }

    if(m_vdEventTimes.size() == 0)
    {
        return false;
    }

    m_pSequenceThread = new MT_SequenceThread(this, m_vdEventTimes);
    m_pSequenceThread->Create();
    if(m_pSequenceThread->Run() != wxTHREAD_NO_ERROR)
    {
        fprintf(stderr, "MT_Sequence: Error running thread.\n");
        return false;
    }
    
    m_bIsRunning = true;
    m_bTimesLock = true;

    return true;

}

bool MT_Sequence::stopSequence()
{
    if(m_bIsRunning)
    {
        haltThread();
    }

    m_pSequenceThread = NULL;
    m_bThreadExitedNormally = false;

    return true;
}
