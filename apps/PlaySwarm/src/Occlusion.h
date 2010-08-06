/* NOTE This code is outdated!  See ../README */

#ifndef Occlusion_H
#define Occlusion_H

/*
 *  Occlusion.h
 *
 *  Created by Daniel Swain on 11/22/09.
 *
 */

#include "MT_Core.h"

void OCC_nextAOI();
void OCC_nextVisualAngle();
void CalculateAndDrawOcclusions(const std::vector<MT_BufferAgent*>& agents);

#endif // Occlusion_H
