#include <string>
#include <iostream>
#include <sstream>
#include <string.h>

#include "MT_Test.h"

#include "MT/MT_Core/fileio/ExperimentDataFile.h"
#include "MT/MT_Core/support/filesupport.h"

bool g_show_only_errors = true;

const char* const XDFTestDir = "XDFTestDir";
const char* const XDFTestName = "XDFTestDir/XDFtest.xdf";

int main(int argc, char** argv)
{
    MT_RMDIR(XDFTestDir);
    MT_mkdir(XDFTestDir);
    
    MT_ExperimentDataFile XDF1;

    std::cerr << "This should be followed by an MT_ExperimentDataFile init error\n";
    bool r = XDF1.initForReading("test_nonexist.xdf");

    if(r != MT_XDF_ERROR)
    {
        std::cerr << "When attempting to open a non-existant file for reading, did not give an error.\n";
    }

    r = MT_ExperimentDataFile::isXDF("test_nonexist.xdf");
    if(r)
    {
        std::cerr << "Asking if a non-existent file is an XDF returns true.\n";
    }

    r = XDF1.initAsNew(XDFTestName);
    if(r != MT_XDF_OK)
    {
        std::cerr << "Couldn't create testXDF.xdf\n";
        return 0;
    }

    r = XDF1.addDataStream("test stream 1", "test.dat");
    if(r != MT_XDF_OK)
    {
        std::cerr << "Couldn't add stream \"test stream 1\"\n";
    }

    return 0;
    
}
