#ifndef GAMEPADCONTROLLER_H
#define GAMEPADCONTROLLER_H

/*
 *  GamePadController.h
 *
 *  Created by Dan Swain on 3/16/08.
 *
 */

#include "MT/MT_Robot/io/gamepad.h"
#include "MT/MT_Robot/robot/RobotBase.h"
#include "MT/MT_Core/support/mathsupport.h"  // The MT_SGN() macro is used here, as well as functions from math.h


const unsigned char MT_ROBOT_ASSIGNMENT_ERROR = 1;
const unsigned char MT_ROBOT_ASSIGNMENT_OK = 0;

const unsigned char MT_ROBOT_IN_USE = 1;
const unsigned char MT_ROBOT_FREED = 0;

class MT_GamePadController : public MT_HIDGamePad{
  
protected:
    MT_RobotBase* myXYRobot; /**< Robot assigned to the XY axis */
    MT_RobotBase* myWZRobot; /**< Robot assigned to the WZ axis */
  
    unsigned int connected;  /**< Allow for a poll of whether the gamepad is connected and operational or not. */
  
    /** Common initializations - sets parameter values to defaults
        and calls MT_HIDGamePad.Init() */
    void common_init();
  
    // Functions to allocate robots
    std::vector<MT_RobotBase*> AvailableRobots; /**< List of available robots */
    /** Searches the AvailableRobots and returns the ID if it is found */
    unsigned int robotID(MT_RobotBase* QueryBot);
    /** Attempts to cycle a robot to the next available position in the specified direction.
        Note that this is not a ring topology, i.e. we don't jump from the 0'th
        robot back up to the N'th one, we just stay at 0 if we try to go in the 
        -'ve direction. */
    void cycle_robot(MT_RobotBase*& botToChange, char direction);
    /** Common function used to assign a robot to an axis.
        @param RobotToChange Either myXYRobot or myWZrobot.
        @param NewBot The robot to assign to RobotToChange's axis.
    */
    unsigned char AssignRobot(MT_RobotBase*& RobotToChange, MT_RobotBase* NewBot); 
    /** Function to let the user know which robots are assigned to which axes. */
    void DisplayAssignedRobots() const;
  
public:
    /** Default constructor finds the joystick but assigns no robots. */
    MT_GamePadController();
    /** Constructor to assign a robot to the XY axis.
        @param setXYRobot A pointer to an instance of class MT_RobotBase corresponding to the robot we want to assign to the XY axis.
        @see myXYRobot
    */
    MT_GamePadController(MT_RobotBase* setXYRobot);
    /** Constructor to assign a robot to the XY axis and to the WZ axis.
        @param setXYRobot A pointer to an instance of class MT_RobotBase corresponding to the robot we want to assign to the XY axis.
        @param setWZRobot A pointer to an instance of class MT_RobotBase corresponding to the robot we want to assign to the WZ axis.
        @see myXYRobot
        @see myWZRobot
    */
    MT_GamePadController(MT_RobotBase* setXYRobot,
                         MT_RobotBase* SetWZRobot);
  
    // virtual destructor to make sure the gamepad destructor gets called
    virtual ~MT_GamePadController(){};
  
    /** Function to query whether the gamepad is connected and operational or not. */
    unsigned int IsConnected() const { return connected; };
  
    void SetParameters(float setMaxSpeed, float setMaxTurning, unsigned int setSpeedDeadBand, unsigned int setTurningDeadBand);

    /** Function to de-autonomize the robots on the gamepad axes for manual control. */
    void SeizeControl();
    /** Function to re-autonomize the robots on the gamepad axes. */
    void ReleaseControl();
  
    /** Function to add a robot to the list of robots available to the joystick.  Returns index of robot.*/
    unsigned int AddRobot(MT_RobotBase* newBot); 
    /** Function to remove a robot from the list of robots, if it is not already assigned to an axis. */
    unsigned int RemoveRobot(MT_RobotBase* BotToRemove);
    /** Function to remove a robot by index - we use this if we are managing the robot elsewhere. */
    unsigned int RemoveRobot(unsigned int i);
    /** Function to set the XY axis robot to the supplied MT_RobotBase. */
    unsigned char SetXYRobot(MT_RobotBase* setXYRobot);
    /** Function to set the WZ axis robot to the supplied MT_RobotBase. */
    unsigned char SetWZRobot(MT_RobotBase* setWZRobot);
    /** Function to return a pointer to the XY robot (for comparison) */
    MT_RobotBase* getXYRobot() const { return myXYRobot; };
    /** Function to return a pointer to the WZ robot (for comparison) */
    MT_RobotBase* getWZRobot() const { return myWZRobot; };
    /** Function to cycle the XY axis robot to the next available in the specified direction. 
        @param direction +1 for robot with the next highest ID, -1 for robot with the next lowest ID
        @see cycle_robot
        @see NextWZRobot
    */
    virtual void NextXYRobot(char direction);
    /** Function to cycle the WZ axis robot to the next available in the specified direction.
        @see NextXWRobot
    */
    virtual void NextWZRobot(char direction);
  
    /** Main function used to check the joystick state and act accordingly. */
    virtual void PollAndUpdate(bool DoControl = true);
  
};

#endif
