/*
 *  MT_HungarianMatcher.cpp
 *
 *  Created by Daniel Swain on 1/6/10.
 *
 */

#include "MT/MT_Tracking/cv/MT_HungarianMatcher.h"
#include "MT/MT_Core/support/mathsupport.h"

/* 
 * This seems to work OK so far.  If you have problems, try
 * setting it to a lower value (e.g. 2^8, etc) but know
 * that this will set the resolution of the 
 * algorithm.  We aren't doing anything fancy in terms of
 * quantization, just an affine transformation, so if
 * there are a bunch of values in the range [0, 1] and
 * one outlier at 1e6 you could end up with a bunch of
 * integers at 0 and one at HM_MAX_VALUE 
 */
const double HM_MAX_VALUE = 1e6;//_max_int;

/* uncomment to display some potential debugging information */
// #define HM_DEBUG 

MT_HungarianMatcher::MT_HungarianMatcher()
  : m_iRows(0),
    m_iCols(0),
    m_iMode(HUNGARIAN_MIN),
    m_piValues(NULL),
    m_pdValues(NULL),
    m_bHaveLimits(false)
{
}

MT_HungarianMatcher::~MT_HungarianMatcher()
{
    if(m_piValues)
    {
        free(m_piValues);
        free(m_pdValues);
    }
}

void MT_HungarianMatcher::doInit(int m, int n, int mode)
{
    if(n == MT_HM_SAME)
    {
        n = m;
    }

    /* 
     * libhungarian can't deal with m > n, so we'll force m = n
     * so the program doesn't crash.  This will cause bad results
     * but shouldn't crash the program. 
     */
    if(m > n)
    {
        fprintf(stderr,
                "MT_HungarianMatcher Error:  Cannot have more rows "
                "(%d) than columns (%d).\n", m, n);
        n = m;
    }

    m_iRows = m;
    m_iCols = n;

    m_bHaveLimits = false;

    /* allows us to reuse the same instance by calling doInit again */
    if(m_piValues)
    {
        free(m_piValues);
        free(m_pdValues);
    }

    m_piValues = (int *)malloc(sizeof(int)*m*n);
    m_pdValues = (double *)malloc(sizeof(double)*m*n);
    
    m_iMode = mode;
}

void MT_HungarianMatcher::setValue(unsigned int i, unsigned int j, double v)
{

    /* force i, j in the proper range */
    i = MT_MIN(i, m_iRows);
    j = MT_MIN(j, m_iCols);
    m_pdValues[i*m_iRows + j] = v;

    /* we're going to use these limits to calculate 
     * the double->integer scaling */
    if(!m_bHaveLimits)
    {
        m_dMinValue = v;
        m_dMaxValue = v;
        m_bHaveLimits = true;
    }
    m_dMinValue = MT_MIN(v, m_dMinValue);
    m_dMaxValue = MT_MAX(v, m_dMaxValue);

}

void MT_HungarianMatcher::doMatch(std::vector<int>* assignments)
{
    /* 
     * bail if the result vector has not been properly initialized,
     * this must be done outside the algorithm (so that the calling
     * code can manage it) 
     */
    if(!assignments)
    {
        return;
    }

    /* 
     * The algorithm needs to work with integers, so we'll apply an
     * affine transformation to compress the double value range into
     * the range [0, HM_MAX_VALUE] with integer values 
     */
    const double scale = HM_MAX_VALUE/(m_dMaxValue - m_dMinValue);
    const double offset = m_dMinValue;
    m_bHaveLimits = false;
    double temp;
    for(unsigned int i = 0; i < m_iRows*m_iCols; i++)
    {
        /* casting this as a double first avoids some serious 
         * floating point -> integer conversion errors */
        temp = scale*(m_pdValues[i] - offset);
        m_piValues[i] = temp;

#ifdef HM_DEBUG
        printf("%f, %f, (i,j) %f -> %d\n", 
               scale, 
               offset, 
               m_pdValues[i], 
               m_piValues[i]);
#endif

    }

    /* 
     * Runs the Hungarian solver.  The init call copies the values from
     * the second argument.  It also allocates some intermediate 
     * arrays/matrices, so we need to call hungarian_fini below 
     *  Note - this may affect performance, but short of hacking the
     *          solver this is the best solution
     */
    hungarian_init(&m_HungarianStruct, (int *)m_piValues, m_iRows, m_iCols, m_iMode); 
    hungarian_solve(&m_HungarianStruct);

#ifdef HM_DEBUG
    hungarian_print_rating(&m_HungarianStruct);
    hungarian_print_assignment(&m_HungarianStruct);
#endif

    /* if the problem was not feasible, return without modifying
     * the assignment vector (this should only very rarely happen) */
    if(!hungarian_check_feasibility(&m_HungarianStruct))
    {
        return;
    }

    /* otherwise copy the assignment vector into the given vector */
    assignments->assign(m_HungarianStruct.a, m_HungarianStruct.a + m_iRows);

    /* frees memory allocated by the solver, see above */
    hungarian_fini(&m_HungarianStruct);
}
