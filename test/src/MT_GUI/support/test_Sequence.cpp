#include "MT_Test.h"
#include "MT_wxTest.h"

#include "MT/MT_GUI/support/Sequence.h"

bool g_ShowOnlyErrors = true;

unsigned int g_iState = 0;
unsigned int g_iCount = 0;
bool g_bSeqFinished = false;

class TestSeq1 : public MT_Sequence
{
private:
    double m_dT0;
    double m_dTp;
public:
    TestSeq1() : MT_Sequence(), m_dT0(MT_getTimeSec()), m_dTp(m_dT0){};

    ~TestSeq1() {haltThread();};

    void onEvent(unsigned int state)
    {
        double ta = MT_getTimeSec() - m_dT0;
        double te = MT_getTimeSec() - m_dTp;
        
        if(!g_ShowOnlyErrors)
        {
            fprintf(stdout,
                    "    Sequence state %d, took %f, total %f\n",
                    state,
                    te,
                    ta);
        }
        m_dTp = MT_getTimeSec();
        g_iState = state;
        g_iCount++;
    };

    void onDone()
    {
        if(!g_ShowOnlyErrors)
        {
            fprintf(stdout,
                    "    Sequence is done.\n");
        }
        g_bSeqFinished = true;
    };
};

void CHECK_SEQUENCE_TIMES(MT_Sequence* sequence,
                          double* expected_times,
                          unsigned int n_expected,
                          int* p_in_status)
{
    std::vector<double> times = sequence->getTimes();

    if(times.size() != n_expected)
    {
        *p_in_status = MT_TEST_ERROR;
        fprintf(stderr,
                "    MT_Sequence Error: Expected to have %d"
                " events, but had %d\n",
                n_expected,
                (int) times.size());
        return;
    }

    for(unsigned int i = 0; i < n_expected; i++)
    {
        if(!MT_IsEqual(expected_times[i], times[i]))
        {
            *p_in_status = MT_TEST_ERROR;
            fprintf(stderr,
                    "    MT_Sequence Error: Expected time %d"
                    " to be %f, but it was %f\n",
                    i,
                    expected_times[i],
                    times[i]);
        }
    }

    if(!g_ShowOnlyErrors)
    {
        fprintf(stdout,
                "    Times are (expected)\n");
        for(unsigned int i = 0; i < n_expected; i++)
        {
            fprintf(stdout,
                    "      %f (%f)\n",
                    times[i],
                    expected_times[i]);
        }
    }
            
}

int main(int argc, char** argv)
{
    WX_CONSOLE_APP_INIT;

    if(argc > 1)
    {
        g_ShowOnlyErrors = false;
    }

    int status = MT_TEST_SUCCESS;

    MT_TEST_START("MT_Sequence");

    TestSeq1* pSeq1 = new TestSeq1();
    pSeq1->pushTime(0.01);
    pSeq1->pushTime(0.03);
    pSeq1->pushTime(0.02);
    pSeq1->pushTime(0.015);
    pSeq1->pushTime(0.010001);

    double exp1[] = {0.01, 0.015, 0.02, 0.03};

    CHECK_SEQUENCE_TIMES(pSeq1,
                         exp1,
                         4,
                         &status);

    g_iState = 0;
    double t0 = MT_getTimeSec();
    double t;
    
    pSeq1->goSequence();

    while(!g_bSeqFinished){wxMilliSleep(1);}

    t = MT_getTimeSec() - t0;

    if(g_iState != 3)
    {
        fprintf(stderr,
                "    Sequence did not finish properly\n"
                "      Final state was %d\n",
                g_iState);
        status = MT_TEST_ERROR;
    }

    if(!g_ShowOnlyErrors)
    {
        fprintf(stdout,
                "    Sequence took %f sec\n",
                t);
    }

    /* run the sequence again */
    g_iState = 0;
    g_bSeqFinished = false;
    t0 = MT_getTimeSec();
    
    pSeq1->goSequence();

    while(!g_bSeqFinished){wxMilliSleep(1);}

    t = MT_getTimeSec() - t0;

    if(g_iState != 3)
    {
        fprintf(stderr,
                "    Sequence did not finish properly (second run)\n"
                "      Final state was %d\n",
                g_iState);
        status = MT_TEST_ERROR;
    }

    if(!g_ShowOnlyErrors)
    {
        fprintf(stdout,
                "    Sequence took %f sec\n",
                t);
    }

    /* the sequence shouldn't run if we clear the times */
    g_iState = 0;
    g_bSeqFinished = false;
    unsigned int count_now = g_iCount;
    t0 = MT_getTimeSec();

    pSeq1->clearTimes();
    pSeq1->goSequence();

    wxMilliSleep(10);

    if(g_iCount != count_now)
    {
        fprintf(stderr,
                "    Sequence should not have run with empty event set");
        status = MT_TEST_ERROR;
    }

    delete pSeq1;

    TestSeq1* pSeq2 = new TestSeq1;

    g_iState = 0;
    g_bSeqFinished = false;
    count_now = g_iCount;
    t0 = MT_getTimeSec();

    pSeq2->pushTime(3.0);
    pSeq2->goSequence();

    wxMilliSleep(100);

    /* sequence shouldn't allow us to add times while it's running */
    int err = pSeq2->pushTime(10.0);
    if(err >= 0)
    {
        fprintf(stderr,
                "    Sequence did not respond with an error "
                "while attempting to push a time while it "
                "should have been running.\n");
        status = MT_TEST_ERROR;
    }
    if(err != MT_SEQUENCE_ERR_LOCKED)
    {
        fprintf(stderr,
                "    Sequence responded with wrong error "
                "while attempting to push a time while it "
                "should have been running.  Returned %d"
                " when it should have returned %d\n",
                err,
                MT_SEQUENCE_ERR_LOCKED);
        status = MT_TEST_ERROR;
    }

    delete pSeq2;

    t = MT_getTimeSec() - t0;

    /* deleting the sequence should stop the sequence,
     * this should also stop it before the full 3 sec */
    if(!g_bSeqFinished)
    {
        fprintf(stderr,
                "    Sequence did not stop properly upon"
                " deletion.\n");
        status = MT_TEST_ERROR;
    }

    if(t > 2.9)
    {
        fprintf(stderr,
                "    Sequence did not stop immediately."
                "  Ran for %f sec\n", t);
        status = MT_TEST_ERROR;
    }

    if(!g_ShowOnlyErrors)
    {
        fprintf(stdout,
                "    Sequence stopped with time %f sec.\n",
                t);
    }

    TestSeq1* pSeq3 = new TestSeq1;

    g_iState = 0;
    g_bSeqFinished = false;
    count_now = g_iCount;
    t0 = MT_getTimeSec();

    pSeq3->pushTime(3.0);
    pSeq3->goSequence();

    wxMilliSleep(100);

    pSeq3->stopSequence();

    t = MT_getTimeSec() - t0;

    /* deleting the sequence should stop the sequence,
     * this should also stop it before the full 3 sec */
    if(!g_bSeqFinished)
    {
        fprintf(stderr,
                "    Sequence did not stop properly upon"
                " stopSequence.\n");
        status = MT_TEST_ERROR;
    }

    if(t > 2.9)
    {
        fprintf(stderr,
                "    Sequence did not stop immediately."
                "  Ran for %f sec\n", t);
        status = MT_TEST_ERROR;
    }

    if(!g_ShowOnlyErrors)
    {
        fprintf(stdout,
                "    Sequence stopped with time %f sec.\n",
                t);
    }

    pSeq3->clearTimes();

    int o = pSeq3->pushTime(0.1);
    if(o < 0)
    {
        fprintf(stderr,
                "    MT_Sequence Error:  Sequence did not allow us"
                " to add a time when it should have.  Error was"
                " %d\n",
                o);
        status = MT_TEST_ERROR;
    }

    double exp2[] = {0.1};

    CHECK_SEQUENCE_TIMES(pSeq3,
                         exp2,
                         1,
                         &status);

    g_bSeqFinished = false;

    t0 = MT_getTimeSec();
    pSeq3->goSequence();

    if(pSeq3->goSequence())
    {
        fprintf(stderr,
                "    MT_Sequence Error:  Sequence allowed to run twice"
                " when it shouldn't have.\n");
        status = MT_TEST_ERROR;
    }
        

    while(!g_bSeqFinished){wxMilliSleep(1);};

    t = MT_getTimeSec() - t0;

    if(t > 0.15)
    {
        fprintf(stderr,
                "    MT_Sequence Error:  Sequence ran with excessive length."
                "  Ran for %f sec\n", t);
        status = MT_TEST_ERROR;
    }

    if(!g_ShowOnlyErrors)
    {
        fprintf(stdout,
                "    Sequence took %f sec\n",
                t);
    }

    delete pSeq3;

    TestSeq1* pSeq4 = new TestSeq1;

    g_iState = 0;
    g_bSeqFinished = false;
    count_now = g_iCount;
    t0 = MT_getTimeSec();
    g_bSeqFinished = false;

    pSeq4->pushTime(2.0);
    pSeq4->goSequence();

    wxSleep(1.0);

    if(!g_ShowOnlyErrors)
    {
        fprintf(stdout,
                "    Resetting sequence.  You will see a "
                "\"Sequence is done.\" message, but that "
                "is because we are resetting.\n");
    }

    if(!pSeq4->goSequence(MT_SEQUENCE_FORCE))
    {
        fprintf(stderr,
                "    MT_Sequence Error:  Sequence did not allow"
                " forced restart.\n");
        status = MT_TEST_ERROR;
    }

    /* the above will call onDone, which sets this true, but
     * we want to see how long the entire sequence takes. */
    g_bSeqFinished = false;

    while(!g_bSeqFinished){wxMilliSleep(1);};
    
    t = MT_getTimeSec() - t0;

    if(t < 2.9)
    {
        fprintf(stderr,
                "    MT_Sequence Error:  Sequence did not appear to restart."
                "  Ran for %f sec\n", t);
        status = MT_TEST_ERROR;
    }

    if(!g_ShowOnlyErrors)
    {
        fprintf(stdout,
                "    Sequence took %f sec\n",
                t);
    }

    delete pSeq4;

    return status;
}
