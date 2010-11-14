#include "RobotBase.h"

#include "MT/MT_Core/primitives/DataGroup.h"

void MT_RobotBase::SetParameters(MT_DataGroup* params, bool force)
{
    if(m_pParameters && !force)
    {
        return;
    }
    else
    {
        if(m_pParameters)
        {
            delete m_pParameters;
            m_pParameters = NULL;
        }
        m_pParameters = params;
    }
    
}
