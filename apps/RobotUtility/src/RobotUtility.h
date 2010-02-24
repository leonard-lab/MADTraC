#ifndef RobotUtility_H
#define RobotUtility_H

/*
 *  RobotUtility.h
 *
 *  Created by Daniel Swain on 12/31/09.
 *
 */

#include "MT_Core.h"
#include "MT_GUI.h"
#include "MT_Tracking.h"
#include "MT_Robot.h"

class RobotUtilityApp : public MT_AppBase
{
public:
    MT_FrameWithInit* createMainFrame()
    {
        return new MT_JoyStickFrame(NULL, NULL, MT_JSF_STANDALONE);
    }
};

#endif // RobotUtility_H
