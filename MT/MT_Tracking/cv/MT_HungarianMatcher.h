#ifndef MT_HungarianMatcher_H
#define MT_HungarianMatcher_H

/*
 *  MT_HungarianMatcher.h
 *
 *  Created by Daniel Swain on 1/6/10.
 *
 */

/* using Brian Gerkey's implementation of the Hungarian algorithm 
 * see http://robotics.stanford.edu/~gerkey/tools/hungarian.html */
#include "MT/MT_Tracking/3rdparty/libhungarian-0.3/hungarian.h"

#include <vector>

const int MT_HM_SAME = -1;

/** 
 * @class MT_HungarianMatcher
 *
 * @brief Wrapper class for Brian Gerkey's c implementation
 * of the Hungarian matching algorithm.
 *
 * Wrapper class for Brian Gerkey's c implementation
 * of the Hungarian matching algorithm 
 * (http://robotics.stanford.edu/~gerkey/tools/hungarian.html).
 *
 * Given a cost/value matrix M, the algorithm calculates the optimal
 * row-to-column assignment vector a to either minimize or maximize
 * the total cost/value.  That is, it finds the values of a[i] such
 * that \sum_i M[i,a[i]] is either minimized or maximized.  Note that
 * i is the row and j is the column.
 *
 * The constructor really does nothing.  Call doInit
 * to initialize the matcher with the right dimensions and
 * algorithm type (maximize or minimize).  Use setValue
 * to stuff the cost/value array with values and then
 * call doMatch to execute the match.
 *
 */
class MT_HungarianMatcher
{
private:
    /* number of rows in the value matrix */
    unsigned int m_iRows;
    /* number of columns in the value matrix */
    unsigned int m_iCols;
    /* mode is either HUNGARIAN_MIN or HUNGARIAN_MAX */
    unsigned int m_iMode;

    /*
     * the solver evaluates over integers, but for the
     * most flexibility we start with double precision
     * data and later convert it to integers
     *
     * the mapping is calculated during doMatch and scales
     * the range [min data, max data] to [0, max integer value]
     */
    int* m_piValues;
    double* m_pdValues;

    /*
     * keep track of limits of the data for the 
     * double->int mapping. the m_bHaveLimits flag gets
     * reset each time we call doInit or doMatch
     */
    bool m_bHaveLimits;
    double m_dMinValue;
    double m_dMaxValue;

    /* structure used by the solver */
    hungarian_t m_HungarianStruct;

public:
    /** The ctor doesn't really do anything.  Call doInit to
     * set up the matcher.  This is done so that you can reuse
     * the same object if the parameters change. */
    MT_HungarianMatcher();

    /** The dtor frees the memory allocated for the cost/value
     * matrix, both double and int versions. */
    ~MT_HungarianMatcher();

    /** Initializes the solver.
     *
     * @param m Number of rows in matrix.
     * @param n Number of columns (default is m = n).
     * @param mode Either HUNGARIAN_MIN (minimize value, default) or
     *              HUNGARIAN_MAX (maximize value). */
    void doInit(int m, int n = MT_HM_SAME, int mode = HUNGARIAN_MIN);

    /** Set the value of M(i,j) if M is the cost/value matrix.
     *
     * @param i Row index
     * @param j Column index
     * @param v Value */
    void setValue(unsigned int i, unsigned int j, double v);

    /** Run the matching algorithm and calculate the assignment vector.
     *
     * @param assignment Pointer to vector in which to store results.
     *              assignment[i] = j means that column j should be 
     *              matched with row i. */
    void doMatch(std::vector<int>* assignment);
};

#endif // MT_HungarianMatcher_H
