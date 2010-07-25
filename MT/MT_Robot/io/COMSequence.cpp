#include "COMSequence.h"

#include <iostream>
#include <iomanip>
#include <sstream>

/*********************************************************************/
/* COM_SEQ is implemented here because it shouldn't be visible
 * gobally. */
class COM_SEQ : public MT_Sequence
{
private:
    MT_COMSequence* m_pCOMSeq;    
public:
    COM_SEQ(MT_COMSequence* in_seq)
        : MT_Sequence(),
          m_pCOMSeq(in_seq)
    {};

    ~COM_SEQ(){haltThread();};

    void onEvent(unsigned int state)
    {
        m_pCOMSeq->onSeqEvent(state);
    };

    void onDone()
    {
        m_pCOMSeq->onSeqDone();
    };
};
/*********************************************************************/


MT_COMSequence::MT_COMSequence(const char* port, FILE* file)
    : m_pComPort(NULL),
      m_pSeq(NULL),
      m_pFile(file),
      m_dMinComTime(MT_COM_SEQUENCE_DEFAULT_MIN_TIME)
{
    m_vpData.resize(0);
    m_viNBytes.resize(0);
    m_vbShifted.resize(0);
    
    if(port)
    {
        forceReconnect(port);
    }
}

MT_COMSequence::~MT_COMSequence()
{
    if(m_pFile)
    {
        fflush(m_pFile);
    }
    clearEvents();
    if(m_pSeq)
    {
        delete m_pSeq;
    }
    if(m_pComPort)
    {
        delete m_pComPort;
    }
}

bool MT_COMSequence::getIsConnected()
{
    return (m_pComPort && m_pComPort->IsConnected());
}

bool MT_COMSequence::forceReconnect(const char* port)
{
    if(!port)
    {
        return false;
    }

    delete m_pComPort;

    m_pComPort = new MT_ComIO(port, false, m_pFile);

    return m_pComPort->IsConnected();
}

int MT_COMSequence::pushEvent(double t,
                              const unsigned char* data,
                              unsigned int n_bytes)
{
    if(!m_pSeq)
    {
        m_pSeq = new COM_SEQ(this);
        m_pSeq->setMinInterval(m_dMinComTime);
    }

    bool ok = false;
    bool shifted = false;
    int ix;
    while(!ok)
    {
        ix = m_pSeq->pushTime(t);
    
        if(ix < 0)
        {
            if(ix == MT_SEQUENCE_ERR_EXISTS)
            {
                int jx = m_pSeq->getInterval(t);
                t = m_pSeq->getTimeFromIndex(jx) + m_dMinComTime;
                shifted = true;
            }
            else
            {
                fprintf(stdout, "Warning:  Event push error %d\n", ix);
                return ix;
            }
        }
        else
        {
            ok = true;
        }
    }

    unsigned char* t_data = (unsigned char *)calloc(n_bytes,
                                                    sizeof(unsigned char));
    memcpy(t_data, data, n_bytes);
    
    m_vpData.insert(m_vpData.begin() + ix, t_data);
    m_viNBytes.insert(m_viNBytes.begin() + ix, n_bytes);
    m_vbShifted.insert(m_vbShifted.begin() + ix, shifted);

    return ix;

}

bool MT_COMSequence::sendData(const unsigned char* data, unsigned int nbytes)
{
	if(!m_pComPort)
	{
		return false;
	}
	else
	{
		return (m_pComPort->SendData(data, nbytes) == 0);
	}
}

void MT_COMSequence::onSeqEvent(unsigned int state)
{
    if(state >= m_viNBytes.size() || state >= m_vpData.size())
    {
        return;
    }

    if(onUserEvent(state) != s_bSkipBase)
    {        
        sendData(m_vpData[state], m_viNBytes[state]);
    }
}

void MT_COMSequence::goSequence(bool force_stop)
{
    m_pSeq->goSequence(force_stop);
}

bool MT_COMSequence::getIsRunning() const
{
    if(!m_pSeq)
    {
        return false;
    }
    else
    {
        return m_pSeq->getIsRunning();
    }
}

bool MT_COMSequence::stopSequence()
{
    if(m_pSeq)
    {
        return m_pSeq->stopSequence();
    }
    else
    {
        return true;
    }
}

bool MT_COMSequence::clearEvents(bool force_stop)
{
    if(getIsRunning())
    {
        if(force_stop == MT_SEQUENCE_FORCE)
        {
            stopSequence();
        }
        else
        {
            return false;
        }
    }

    if(m_pSeq)
    {
        m_pSeq->clearTimes();
    }

    for(unsigned int i = 0; i < m_vpData.size(); i++)
    {
        free(m_vpData[i]);
    }
    m_vpData.resize(0);
    m_viNBytes.resize(0);
    m_vbShifted.resize(0);

    return true;
}

std::string MT_COMSequence::getEventTableAsString() const
{
    if(!m_pSeq)
    {
        return std::string("<empty>");
    }
    
    std::vector<double> times = m_pSeq->getTimes();
    std::ostringstream oss;

    for(unsigned int i = 0; i < times.size(); i++)
    {
        oss << "Event " << i << " : ";
        oss << setiosflags(ios::fixed) << setprecision(3) << times[i];
        oss << " : ";
        for(unsigned int j = 0; j < m_viNBytes[i]; j++)
        {
            oss << " " << (int) *(m_vpData[i] + j);
        }
        if(m_vbShifted[i])
        {
            oss << " : *";
        }

        oss << "\n";
    }

    return oss.str();

}
