/*
 *  CLFSupport.cpp
 *  MADTraC
 *
 *  Created by Daniel Swain on 6/3/10.
 *
 */

#include "CLFSupport.h"

#include "MT/MT_Core/support/mathsupport.h"

MT_CLFtoAgentReader::MT_CLFtoAgentReader(const char* filename,
                                         const char* x_field_name,
                                         const char* y_field_name,
                                         const char* z_field_name)
#ifdef MT_HAVE_CLF
    : m_CLF(filename, "r")
#endif
{
    readFile(x_field_name, y_field_name, z_field_name);
}

void MT_CLFtoAgentReader::readFile(const char* x_field_name,
                                   const char* y_field_name,
                                   const char* z_field_name)
{
#ifndef MT_HAVE_CLF
    return;
#else
    if(m_CLF.GetLastStatus() == CLF_ERROR)
    {
        return;
    }

    int nagents = m_CLF.GetNumberOfTracks();
    int nt = m_CLF.GetNumberOfFrames();

    bool have_x = m_CLF.HasFieldNamed(x_field_name);
    bool have_y = m_CLF.HasFieldNamed(y_field_name);
    bool have_z = m_CLF.HasFieldNamed(z_field_name);

    if(!have_x || !have_y)
    {
        return;
    }

    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;

    for(int i = 0; i < nagents; i++)
    {
        x = m_CLF.ReadTrackAsDoubleSTL(x_field_name, i);
        y = m_CLF.ReadTrackAsDoubleSTL(y_field_name, i);
        if(have_z)
        {
            z = m_CLF.ReadTrackAsDoubleSTL(z_field_name, i);
        }
        m_vpAgents.push_back(new MT_BufferAgent());
        for(unsigned int j = 0; j < x.size(); j++)
        {
            m_vpAgents[i]->AppendX(x[j]);
            m_vpAgents[i]->AppendY(y[j]);
            m_BoundingBox.ShowX(x[j]);
            m_BoundingBox.ShowY(y[j]);
            if(have_z)
            {
                m_vpAgents[i]->AppendZ(z[j]);
                m_BoundingBox.ShowZ(z[j]);
            }
            else
            {
                m_vpAgents[i]->AppendZ(0.0);
                m_BoundingBox.ShowZ(0.0);
            }
        }
    }

#endif // MT_HAVE_CLF    
}
