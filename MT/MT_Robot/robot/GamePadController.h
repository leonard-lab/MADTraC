#ifndef GAMEPADCONTROLLER_H
#define GAMEPADCONTROLLER_H

/*
 *  GamePadController.h
 *  glswarm
 *
 *  Created by Dan Swain on 3/16/08.
 *
 */

#include "MT/MT_Robot/io/gamepad.h"
#include "MT/MT_Robot/robot/SteeredRobot.h"
#include "MT/MT_Core/support/mathsupport.h"  // The MT_SGN() macro is used here, as well as functions from math.h

const float MT_DEFAULT_MAX_SPEED = 0.35;             // speeds in meters/sec
const float MT_DEFAULT_MAX_TURNING_RATE = 8.0;       // turning rates in rad/sec (based on 7 cm wheelbase)
const float MT_MAX_ALLOWED_SPEED = 4.0;              // max rates: see DTS notebook #2, 1/2-1/5 2009
const float MT_MAX_ALLOWED_TURNING_RATE = 114.3;
const float MT_DEFAULT_SPEED_DEADBAND = 0;
const float MT_DEFAULT_TURNING_DEADBAND = 0;

const unsigned char MT_ROBOT_ASSIGNMENT_ERROR = 1;
const unsigned char MT_ROBOT_ASSIGNMENT_OK = 0;

const unsigned char MT_ROBOT_IN_USE = 1;
const unsigned char MT_ROBOT_FREED = 0;

/* A class that uses an MT_HIDGamePad to control one or two robots - one each for the XY and WZ axes.
   Forward speed and turning rate are calculated by determining what proportion of the stick's
   travel outside of a deadband is covered and multiplying by a maximum rate - i.e. the transition from
   deadband to outside of deadband is smooth, not abrupt.  The user can also press buttons 5-8 (at the
   top of the gamepad) to switch robots.  The class keeps a list of robots that are available to it.
*/
class MT_GamePadController : public MT_HIDGamePad{
  
protected:
    MT_SteeredRobot* myXYRobot; /**< Robot assigned to the XY axis */
    MT_SteeredRobot* myWZRobot; /**< Robot assigned to the WZ axis */
  
    float MaxForwardSpeed; /**< Maximum forward speed when stick is throttled [m/sec]  */
    float MaxTurningRate;  /**< Maximum turning rate when stick is throttled [rad/sec] */
  
    float ForwardSpeedNormalization;  /**< = MT_MAX_STICK_VAL_FLOAT - SpeedDeadBand   */
    float TurningRateNormalization;   /**< = MT_MAX_STICK_VAL_FLOAT - TurningDeadBand */
  
    unsigned int SpeedDeadBand;    /**< DeadBand for speed (y,w) axis   */
    unsigned int TurningDeadBand;  /**< Deadband for turning (x,z) axis */
  
    unsigned int connected;  /**< Allow for a poll of whether the gamepad is connected and operational or not. */
  
    /** Common initializations - sets parameter values to defaults
        and calls MT_HIDGamePad.Init() */
    void common_init();
  
    // Functions to allocate robots
    std::vector<MT_SteeredRobot*> AvailableRobots; /**< List of available robots */
    /** Searches the AvailableRobots and returns the ID if it is found */
    unsigned int robotID(MT_SteeredRobot* QueryBot);
    /** Attempts to cycle a robot to the next available position in the specified direction.
        Note that this is not a ring topology, i.e. we don't jump from the 0'th
        robot back up to the N'th one, we just stay at 0 if we try to go in the 
        -'ve direction. */
    void cycle_robot(MT_SteeredRobot*& botToChange, char direction);
    /** Common function used to assign a robot to an axis.
        @param RobotToChange Either myXYRobot or myWZrobot.
        @param NewBot The robot to assign to RobotToChange's axis.
    */
    unsigned char AssignRobot(MT_SteeredRobot*& RobotToChange, MT_SteeredRobot* NewBot); 
    /** Function to let the user know which robots are assigned to which axes. */
    void DisplayAssignedRobots() const;
  
    /** Speed calculation - takes into account deadbands and limits. */
    // Defined in-line for quick reference
    float CalculateSpeed(int StickValue) const { 
        StickValue = DeadBand(StickValue,SpeedDeadBand);
        return (((float) StickValue)/(ForwardSpeedNormalization))*MaxForwardSpeed;
    }
  
    /** Turning rate calculation - takes into account deadbands and limits. */
    float CalculateTurningRate(int StickValue) const {
        StickValue = DeadBand(StickValue,TurningDeadBand);
        return (((float) StickValue)/(TurningRateNormalization))*MaxTurningRate;
    }
  
    /** Returns how far the value is outside of the deadband. */
    static int DeadBand(int StickValue, unsigned int DeadBand)
    {
        int result = abs(StickValue) - DeadBand;
        if(result > 0)
            return MT_SGN(StickValue)*result;
        else
            return 0;
    }  
  
public:
    /** Default constructor finds the joystick but assigns no robots. */
    MT_GamePadController();
    /** Constructor to assign a robot to the XY axis.
        @param setXYRobot A pointer to an instance of class MT_SteeredRobot corresponding to the robot we want to assign to the XY axis.
        @see myXYRobot
    */
    MT_GamePadController(MT_SteeredRobot* setXYRobot);
    /** Constructor to assign a robot to the XY axis and to the WZ axis.
        @param setXYRobot A pointer to an instance of class MT_SteeredRobot corresponding to the robot we want to assign to the XY axis.
        @param setWZRobot A pointer to an instance of class MT_SteeredRobot corresponding to the robot we want to assign to the WZ axis.
        @see myXYRobot
        @see myWZRobot
    */
    MT_GamePadController(MT_SteeredRobot* setXYRobot, MT_SteeredRobot* SetWZRobot);
  
    // virtual destructor to make sure the gamepad destructor gets called
    virtual ~MT_GamePadController(){};
  
    /** Function to query whether the gamepad is connected and operational or not. */
    unsigned int IsConnected() const { return connected; };
  
    /** Function to set the joystick motion control parameters.
        @param setMaxSpeed Maximum speed [m/sec] when the joystick is pressed fully forward or backward.
        @param setMaxTurning Maximum turning rate [rad/sec] when the joystick is pressed fully left or right.
        @param setSpeedDeadBand Deadband for speed, e.g. if |Y| < Deadband, then speed = 0.
        @param setTurningDeadBand Deadbandf or turning, e.g. if |X| < Deadband, then turning = 0.
        @see MaxForwardSpeed
        @see MaxTurningRate
        @see SpeedDeadBand
        @see TurningDeadBand
    */
    void SetParameters(float setMaxSpeed, float setMaxTurning, unsigned int setSpeedDeadBand, unsigned int setTurningDeadBand);
    /** Function to just set the deadbands.
        @see SetParameters
    */
    void SetDeadBands(unsigned int setSpeedDeadBand, unsigned int setTurningDeadBand);
    /** Function to just set the maximum speed and turning rate.
        @see SetParameters
    */
    void SetMaxRates(float setMaxSpeed, float setMaxTurning);
    void SetMaxSpeedMPS(float setMaxSpeed);
    void SetMaxTurningRateRADPS(float setMaxTurningRate);
    float GetMaxSpeedMPS() const;
    float GetMaxTurningRateRADPS() const;
  
    /** Function to de-autonomize the robots on the gamepad axes for manual control. */
    void SeizeControl();
    /** Function to re-autonomize the robots on the gamepad axes. */
    void ReleaseControl();
  
    /** Function to add a robot to the list of robots available to the joystick.  Returns index of robot.*/
    unsigned int AddRobot(MT_SteeredRobot* newBot); 
    /** Function to remove a robot from the list of robots, if it is not already assigned to an axis. */
    unsigned int RemoveRobot(MT_SteeredRobot* BotToRemove);
    /** Function to remove a robot by index - we use this if we are managing the robot elsewhere. */
    unsigned int RemoveRobot(unsigned int i);
    /** Function to set the XY axis robot to the supplied MT_SteeredRobot. */
    unsigned char SetXYRobot(MT_SteeredRobot* setXYRobot);
    /** Function to set the WZ axis robot to the supplied MT_SteeredRobot. */
    unsigned char SetWZRobot(MT_SteeredRobot* setWZRobot);
    /** Function to return a pointer to the XY robot (for comparison) */
    MT_SteeredRobot* getXYRobot() const { return myXYRobot; };
    /** Function to return a pointer to the WZ robot (for comparison) */
    MT_SteeredRobot* getWZRobot() const { return myWZRobot; };
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
