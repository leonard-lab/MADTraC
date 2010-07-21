#ifndef SEQUENCE_H
#define SEQUENCE_H

/** @file
 * Sequence.h
 *
 * Defines the MT_Sequence class.
 *
 */
    

#include <vector>
/* using wxThread for sequence thread */
#include "wx/thread.h"
/* for getTimeSec */
#include "MT/MT_Core/support/mathsupport.h"

/** @see MT_Sequence::goSequence */
const bool MT_SEQUENCE_NO_FORCE = false;
/** @see MT_Sequence::goSequence */
const bool MT_SEQUENCE_FORCE = true;

/** @see MT_Sequence::pushTime */
const int MT_SEQUENCE_ERR_LOCKED = -1;
/** @see MT_Sequence::pushTime */
const int MT_SEQUENCE_ERR_NEGATIVE = -2;
/** @see MT_Sequence::pushTime */
const int MT_SEQUENCE_ERR_EXISTS = -3;

/** @see MT_Sequence::setMinInterval */
const double MT_SEQUENCE_DEFAULT_MIN_INTERVAL = 0.002;  /* sec */

/* forward declaration */
class MT_Sequence;

/* This is the thread class used by MT_Sequence.  It's not documented
 * in Doxygen b/c the end-user won't have access to it. */
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

/** @class MT_Sequence
 *
 * A time-based sequence that is executed by a separate thread for
 * (relatively) precise timing.
 *
 * The main concept is to load the sequence with a list of event
 * times with MT_Sequence::pushTime (which can be added out of
 * order).  Then, when the sequence is started with
 * MT_Sequence::doSequence, a thread is spawned that triggers a call
 * to the virtual function MT_Sequence::onEvent after each event
 * time.  After the thread is done, it calls the virtual function
 * MT_Sequence::onDone.
 *
 * The sequence can be forced to stop with MT_Sequence::stopSequence.
 *
 * Example Usage:
 * First, derive a class from MT_Sequence:
 * @code
 * // Derive from MT_Sequence to override event fcns
 * class mySequence : public MT_Sequence
 * {
 * private:
 *     // will be the recipient of event notifications
 *     SomeObj* m_pParent;
 * public:
 *     // ctor should set parent
 *     mySequence(SomeObj* parent)
 *      : MT_Sequence(), m_pParent(parent){};
 *
 *     // Sequence event handler
 *     void onEvent(unsigned int state)
 *       {m_pParent->handleEvent(state);};
 *       
 *     // Finish event handler
 *     void onDone(){m_pParent->handleFinished();};
 * };
 * @endcode
 *
 * Then, wherever you initialize the parent, initialize the sequence
 * object by pushing event times to it.
 * @code
 * m_pSequence = new mySequence(this);
 *
 * // three events, spaced 0.1 sec apart 
 * m_pSequence->pushTime(0.1);
 * m_pSequence->pushTime(0.2);
 * m_pSequence->pushTime(0.3);
 * // add event at 0.15 (note the order is enforced automatically)
 * m_pSequence->pushTime(0.15);
 * // final sequence times will be [0.1 0.15 0.2 0.3]
 * @endcode
 *
 * Later, when we want the sequence to "go":
 * @code
 * m_pSequence->doSequence();
 * @endcode
 *
 * The event handlers could be as simple as
 * @code
 * void SomeObj::handleEvent(unsigned int state)
 * {
 *     // "state" is the index of the time of the most recent event
 *     //  i.e. it counts up from 0 to the number of events - 1
 *     printf("Event %d occurred\n", state);
 * }
 *
 * void SomeObj::handleFinished()
 * {
 *     printf("The sequence is done.\n");
 * }
 * @endcode
 *     
 */
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
    /* these two functions get called directly by the thread and
     * should *only* get called by the thread.  This allows us to do
     * whatever housekeeping we need to do.  flagEvent is currently a
     * passthrough to onEvent, but this leaves us room to modify
     * later. */
    void flagEvent(unsigned int state){onEvent(state);};
    void flagThreadIsDone();

public:
    /** Constructor.  The default constructor initializes the sequence
     * with an empty 
     * set of event times and minimum interval equal to
     * MT_SEQUENCE_DEFAULT_MIN_INTERVAL */
    MT_Sequence();

    /** Destructor.  The destructor will attempt to make sure that the
     * thread is 
     * deleted.  It will try 100 times to call stopSequence before
     * giving up.  If all is well, either the sequence was already
     * stopped or this will succeed on the first try.  The retrial is
     * a safety measure. */
    virtual ~MT_Sequence();

    /** Set the minimum allowed interval in seconds.  This enforces a
     * minimum 
     * allowed interval length, which may be desirable because of
     * either a 
     * known limitation of the timing hardware or of the target
     * hardware (for example serial devices that can only handle a
     * certain amount of bandwidth).  When this is called, the
     * existing times are checked to enforce this. This is
     * accomplished by copying the existing times and then resetting
     * them via setTimes.
     *
     * The minimum, MT_SEQUENCE_DEFAULT_MIN_INTERVAL, is chosen based
     * on the resolution of the timing hardware on most systems, which
     * is 1 msec (slightly worse under windows).  It's value is 0.002
     * sec. */
    void setMinInterval(
        double min_interval = MT_SEQUENCE_DEFAULT_MIN_INTERVAL);
    /** Returns the current minimum interval */
    double getMinInterval() const {return m_dMinInterval;};

    /** Set event times (STL vector version).  Existing event times
     * will be deleted.  New event times will be added one at a time
     * using pushTime in the order supplied.  Will get ignored if the
     * sequence is currently running.  */
    virtual void setTimes(std::vector<double> set_times);

    /** Get a vector of the existing (sorted) event times. */
    std::vector<double> getTimes() const {return m_vdEventTimes;};
    /** Clear the event times.  Ignored if a sequence thread is
     * running */
    void clearTimes(){if(!m_bIsRunning){m_vdEventTimes.resize(0);}};

    /** Query the interval in which time t (seconds) occurrs.  Returns
     * the index i such that EventTimes[i - 1] <= t < EventTimes[i].
     * If t is negative, returns MT_SEQUENCE_ERR_NEGATIVE.
     */
    int getInterval(double t);

    /** Add a single event time.
     *  - If the sequence is currently
     * running, this time is ignored and MT_SEQUENCE_ERR_LOCKED is
     * returned.
     *  - If t is negative or less than half of the minimum
     * allowed interval, MT_SEQUENCE_ERR_NEGATIVE is returned.
     *  - If t is too close to an existing event time (see below),
     *  then MT_SEQUENCE_ERR_EXISTS is returned.
     *  - If the time is successfully added, the times are sorted and
     *  the result of getInterval(t) is returned - i.e. the index of
     *  the time once it is inserted.
     *
     * Before adding the time, the existing times are checked to make
     * sure that there is not an existing tE such that |t - tE| < half
     * of the minimum allowed interval.  I.e. event times cannot be
     * too close together. */
    int pushTime(double t);

    /** Returns true of the sequence thread is running. */
    bool getIsRunning() const {return m_bIsRunning;};

    /** Start the sequence.  If the sequence is running and
     * force_stop is false (MT_SEQUENCE_NO_FORCE, the default), then
     * nothing occurrs and the function returns false.
     * If force_stop is true (MT_SEQUENCE_FORCE), the existing running
     * sequence thread is stopped and a new sequence thread is
     * started.  If the event times are empty, no thread is started
     * and false is returned.  If the thread starts successfully, true
     * is returned.  Otherwise false is returned. */
    virtual bool goSequence(bool force_stop = MT_SEQUENCE_NO_FORCE);
    /** Stops an existing sequence thread and triggers onDone. */
    virtual bool stopSequence();

    /** Event trigger callback.  This function is called whenever the
     * sequence thread elapses to the next event time.  The value of
     * state is the value of the index of the last event time.
     * I.e. it starts at zero and counts up to the number of event
     * times minus one. */
    virtual void onEvent(unsigned int state){};
    /** Sequence finished callback.  This gets called when the
     * sequence thread is stopped - either by naturally ending or by a
     * call to stopSequence.  Use this function to return any outputs
     * to safe states. */
    virtual void onDone(){};

};

#endif // SEQUENCE_H
