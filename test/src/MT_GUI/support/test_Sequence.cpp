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

    ~TestSeq1() {onDone();};

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
                times.size());
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

    printf("trying to delete\n");

    wxCriticalSection crit;
    crit.Enter();
    bool done = false;
    delete pSeq2;
    done = true;
    crit.Leave();

    while(!done){};
    
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


    return status;
}
