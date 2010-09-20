#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

#include "MT_Test.h"

#include "MT/MT_Core/fileio/ExperimentDataFile.h"

const char* xdf_filename = "test";

const double dt = 0.01;

const unsigned int N = 2;
const unsigned int Nt = 100;

void gen_data(double t, unsigned int i, double* x, double* y)
{
    *x = ((double) i) + sin(t);
    *y = ((double) i) + cos(t);
}

int main(int argc, char** argv)
{
    int status = MT_TEST_SUCCESS;

    /**************************************************/
    MT_TEST_START("ExperimentDataFile");

    MT_DataGroup data_group("Test data group");
    double d = 3.1415;
    int i = 42;
    std::string s("May the Force be with you.");

    data_group.AddDouble("Value of pi", &d);
    data_group.AddInt("The ultimate answer", &i);
    data_group.AddString("Old Jedi addage", &s);

    MT_ExperimentDataFile* XDF = new MT_ExperimentDataFile();

    bool success = XDF->initAsNew(xdf_filename);
    char val[20];
    sprintf(val, "%4.3f", dt);

    XDF->writeParameterToXML("This is a parameter", "It has a string value");
    XDF->writeParameterToXML("dt", val);

    XDF->writeDataGroupToXML(&data_group);

    std::vector<double> X;
    std::vector<double> Y;

    X.resize(N);
    Y.resize(N);

    XDF->addDataStream("X data", "x.dat");
    XDF->addDataStream("Y data", "y.dat");

    double x, y;
    double t = 0;
    for(unsigned int nt = 0; nt < Nt; nt++)
    {
        for(unsigned int ix = 0; ix < N; ix++)
        {
            gen_data(t, ix, &x, &y);
            X[ix] = x;
            Y[ix] = y;
        }
        XDF->writeData("X data", X);
        XDF->writeData("Y data", Y);
        XDF->flushTimeStep();
        t += dt;
    }

    delete XDF;

    MT_ExperimentDataFile* rXDF = new MT_ExperimentDataFile();

    success = rXDF->initForReading(xdf_filename);
    if(!success)
    {
        fprintf(stderr,
                "    Error reading file %s\n",
                xdf_filename);
        status = MT_TEST_ERROR;
    }

    std::string pval("error");

    double rdt = 100;
    
    rXDF->getParameterAsDouble("dt", &rdt);
    printf("read: dt %f\n", rdt);

    std::vector<std::string> params;
    std::vector<std::string> file_labels;
    std::vector<std::string> file_names;
    rXDF->getParameterNames(&params);
    rXDF->getFilesFromXML(&file_labels, &file_names);

    std::cout << "Parameters in XDF:" << std::endl;
    for(unsigned int i = 0; i < params.size(); i++)
    {
        std::cout << params[i] << std::endl;
    }

    std::cout << "Files in XDF:" << std::endl;
    for(unsigned int i = 0; i < file_labels.size(); i++)
    {
        std::cout << file_labels[i] << ": " <<
            file_names[i] << std::endl;
    }

    X.resize(0);
    Y.resize(0);

    double xp, yp, tp;
    tp = 0;
    std::cout << "First lines of data:" << std::endl;
    for(unsigned int l = 0; l < 101; l++)
    {
        rXDF->readNextLineOfDoublesFromStream("X data", &X);
        rXDF->readNextLineOfDoublesFromStream("Y data", &Y);
        std:: cout << "line " << l;
        for(unsigned int i = 0; i < X.size(); i++)
        {
            gen_data(tp, i, &xp, &yp);
            std::cout << " (" << X[i] << ", " << Y[i] << ") ";
            std::cout << "== (" << xp << ", " << yp << ")? ";
            if(MT_IsEqual(X[i], xp) && MT_IsEqual(Y[i], yp))
            {
                std::cout << "yes. ";
            }
            else
            {
                std::cout << "no. ";
            }
        }
        tp += rdt;
        std::cout << std::endl;
    }

    std::cout << "Number of lines in X data file " <<
        rXDF->getNumberOfLinesInStream("X data") << std::endl;
    std::cout << "Number of lines in X data file " <<
        rXDF->getNumberOfLinesInStream("X data") << std::endl;

    delete rXDF;

    return status;

}

