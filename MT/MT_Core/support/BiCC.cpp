#include "BiCC.h"

#include <algorithm>

unsigned int MT_BiCC::getNumberOfLabels(const std::vector<unsigned int>& labels)
{
    return *std::max_element(labels.begin(), labels.end());
}

MT_BiCC::MT_BiCC(unsigned int rows, unsigned int cols)
    : m_vuiLabelMatrix(rows*cols, 0),
      m_vuiLabelVector(rows + cols, 0),
      m_iNRows(rows),
      m_iNCols(cols),
      m_iCurrentLabel(0)
{
    doInit(rows, cols);
}

void MT_BiCC::doInit(unsigned int rows, unsigned int cols)
{
    m_vuiLabelMatrix.assign(rows*cols, 0);
    m_vuiLabelVector.assign(rows + cols, 0);
    m_iNRows = rows;
    m_iNCols = cols;
    m_iCurrentLabel = 0;
}

unsigned int MT_BiCC::getRowLabel(unsigned int i)
{
    if(i < m_iNRows)
    {
        return m_vuiLabelVector[i];
    }
    return 0;
}

unsigned int MT_BiCC::getColLabel(unsigned int j)
{
    if(j < m_iNCols)
    {
        return m_vuiLabelVector[m_iNRows + j];
    }
    return 0;
}

unsigned int MT_BiCC::getLabelElement(unsigned int k)
{
    if(k < m_vuiLabelVector.size())
    {
        return m_vuiLabelVector[k];
    }
    return 0;
}

int MT_BiCC::findComponents(const std::vector<unsigned int>& adj_sub)
{
    m_vuiAdj = adj_sub;

    int empty_row;
    m_iCurrentLabel = 0;

    /* move along the rows (top), follow any components */    
    for(unsigned int i = 0; i < m_iNRows; i++)
    {
        empty_row = 1;
        for(unsigned int j = 0; j < m_iNCols; j++)
        {
            if(areAdjacent(i, j))
            {
                empty_row = 0;
            }

            if(unlabeledAdjacent(i, j))
            {
                m_iCurrentLabel++;
                setLabelM(i, j);
                setLabel(i, j);
                followRow(i);
                followCol(j);
            }
        }
        if(empty_row)
        {
            m_iCurrentLabel++;
            setRowLabel(i);
        }
    }

    for(unsigned int j = 0; j < m_iNCols; j++)
    {
        if(m_vuiLabelVector[m_iNRows + j] == 0)
        {
            m_vuiLabelVector[m_iNRows + j] = ++m_iCurrentLabel;
        }
    }

    return m_iCurrentLabel;
}

void MT_BiCC::followCol(unsigned int j)
{
    for(unsigned int i = 0; i < m_iNRows; i++)
    {
        if(unlabeledAdjacent(i, j))
        {
            setLabelM(i, j);
            setLabel(i, j);
            followRow(i);
            followCol(j);
        }
    }
}

void MT_BiCC::followRow(unsigned int i)
{
    for(unsigned int j = 0; j < m_iNCols; j++)
    {
        if(unlabeledAdjacent(i, j))
        {
            setLabelM(i, j);
            setLabel(i, j);
            followCol(j);
            followRow(i);
        }
    }
}

bool MT_BiCC::areAdjacent(unsigned int i, unsigned int j)
{
    return (m_vuiAdj[i*m_iNCols + j] > 0);
}

bool MT_BiCC::unlabeledAdjacent(unsigned int i, unsigned int j)
{
    return (m_vuiAdj[i*m_iNCols + j] > 0) && (m_vuiLabelMatrix[i*m_iNCols + j] == 0);
}

void MT_BiCC::setLabelM(unsigned int i, unsigned int j)
{
    m_vuiLabelMatrix[i*m_iNCols + j] = m_iCurrentLabel;
}

void MT_BiCC::setLabel(unsigned int i, unsigned int j)
{
    m_vuiLabelVector[i] = m_iCurrentLabel;
    m_vuiLabelVector[m_iNRows + j] = m_iCurrentLabel;
}

void MT_BiCC::setRowLabel(unsigned int i)
{
    m_vuiLabelVector[i] = m_iCurrentLabel;
}
