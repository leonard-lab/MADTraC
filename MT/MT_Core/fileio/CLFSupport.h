#ifndef CLFSUPPORT_H
#define CLFSUPPORT_H

/*
 *  CLFSupport.h
 *  MADTraC
 *
 *  Created by Daniel Swain on 6/3/10.
 *
 */

#ifdef MT_HAVE_CLF
#include "CLFpp.h"
#endif

#include "MT/MT_Core/primitives/BufferAgent.h"
#include "MT/MT_Core/primitives/BoundingBox.h"

class MT_CLFtoAgentReader
{
private:
    MT_BoundingBox m_BoundingBox;
    std::vector<MT_BufferAgent*> m_vpAgents;
#ifdef MT_HAVE_CLF
    CLFpp m_CLF;
#endif

public:
    MT_CLFtoAgentReader(const char* filename,
                        const char* x_field_name = "x",  
                        const char* y_field_name = "y",  
                        const char* z_field_name = "z");
    void readFile(const char* x_field_name = "x",
                  const char* y_field_name = "y",
                  const char* z_field_name = "z");
    std::vector<MT_BufferAgent*> getAgents() const
    {return m_vpAgents;};
    MT_BoundingBox getBoundingBox() const {return m_BoundingBox;};
};

#endif // CLFSUPPORT_H
