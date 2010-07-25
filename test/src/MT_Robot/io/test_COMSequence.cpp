#include <iostream>
#include <iomanip>
#include <sstream>

#include "MT_Test.h"
#include "MT_wxTest.h"

#include "MT/MT_Robot/io/COMSequence.h"
#include "MT/MT_Core/support/filesupport.h"
#include "MT/MT_Core/support/stringsupport.h"
#include "MT/MT_Core/support/mathsupport.h"

bool g_ShowOnlyErrors = true;

void TRY_PUSH_EVENT(MT_COMSequence* seq,
                    double t,
                    const unsigned char* data,
                    unsigned int n_bytes,
                    int expected_index,
                    int* p_status_in)
{
    int e;
    e = seq->pushEvent(t, data, n_bytes);
    if(e != expected_index)
    {
        cerr << "    MT_COMSequence Error:  Unexepcted result " << e <<
            ", expecting " << expected_index << endl;
        *p_status_in = MT_TEST_ERROR;
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

    FILE* f = fopen("output.txt", "w");
    if(!f)
    {
        fprintf(stderr, "Could not open output file.\n");
        return MT_TEST_ERROR;
    }

    MT_TEST_START("MT_Sequence");

    double t0 = MT_getTimeSec();
    double t;
    
    MT_COMSequence* pSeq1 = new MT_COMSequence("stderr", f);

    t = MT_getTimeSec() - t0;

    try
    {
        if(!pSeq1->clearEvents())
        {
            cerr << "    MT_COMSequence Error: Couldn't clear empty events.\n";
            status = MT_TEST_ERROR;
        }
    }
    catch (int e)
    {
        cerr << "    MT_COMSequence Error:  Exception " << e <<
            " while trying to clear empty events.\n";
        status = MT_TEST_ERROR;
    }

    unsigned char d[] = {65, 66, 67};
    if(!pSeq1->sendData(d, 3))
    {
        cerr << "    MT_COMSequence Error: Couldn't send data to port.\n";
        status = MT_TEST_ERROR;
    }

    unsigned char d2[] = {'b', '@', 'T'};
    unsigned char d3[] = {'w', 'o', 'o', 't'};

    TRY_PUSH_EVENT(pSeq1, 0.1, d, 3, 0, &status);
    TRY_PUSH_EVENT(pSeq1, 0.2, d2, 3, 1, &status);
    TRY_PUSH_EVENT(pSeq1, 0.23, d2, 3, 2, &status);
    TRY_PUSH_EVENT(pSeq1, 0.22, d3, 4, 3, &status);

    if(!g_ShowOnlyErrors)
    {
        cout << pSeq1->getEventTableAsString();
    }

    pSeq1->setMinDelay(0.025);

    if(!g_ShowOnlyErrors)
    {
        cout << pSeq1->getEventTableAsString();
    }

    pSeq1->goSequence();

    while(pSeq1->getIsRunning()){wxMilliSleep(10);};

    pSeq1->clearEvents();

    /* make sure we send a newline to clean up the output window. */
    unsigned char d4[] = {'\n','d','o','n','e','\n'};
    if(!pSeq1->sendData(d4, 6))
    {
        cerr << "    MT_COMSequence Error: Couldn't send data to port.\n";
        status = MT_TEST_ERROR;
    }
    
    delete pSeq1;

    fclose(f);

    if(!g_ShowOnlyErrors)
    {
        cout << "\n============== file output =============\n" ;
        
        MT_CatTextFile("output.txt");

        /* Note we should probably check the output against what's
         * expected.  This is a skeleton of how to tokenize the
         * output log file. */
        /*
        std::vector<std::string> lines =
        MT_TextFileToStringVector("output.txt");
      
        std::vector<std::string> second = MT_SplitString(lines[1],
        std::string(" | "));
      
        cout <<
        MT_StringVectorToString(second);
        */
    }
    
    return status;
}
