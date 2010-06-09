#ifndef ALLROBOTCONTAINER_H
#define ALLROBOTCONTAINER_H

/*
 *  AllRobotContainer.h
 *
 *  Created by Daniel Swain on 12/6/08.
 *
 *  Simple container class for robots.  Well suited to a situation
 *  with a fixed maximum number of robots MT_MAX_NROBOTS (i.e. any number 
 *  of robots up to MT_MAX_NROBOTS). 
 *
 *  Also contains functions to read/write an xml file with parameters
 *  for the robots and gamepad controller.
 *
 */
#include <vector>            // for std::vector
#include <string>


#include "MT/MT_Robot/robot/SteeredRobot.h"    // base robot class used
#include "MT/MT_Core/primitives/Color.h"  // for robot colors

// safely define MT_USE_XML if you want to use XML (recommended)
#ifndef MT_USE_XML
#define MT_USE_XML
#endif

#ifdef MT_USE_XML
#include "MT/MT_Core/fileio/XMLSupport.h"  // for MT_XMLFile wrapper and TinyXML includes
#endif

#ifndef MT_HAVE_ROBOT_MODULE
#define MT_HAVE_ROBOT_MODULE
#endif

/************* Constants ****************************/

const unsigned int MT_MAX_NROBOTS = 7;   // maximum number of robots (determined by bluetooth for us)
const int MT_NO_GAMEPAD = -1;            // indicates robot is not attached to gamepad
const int MT_NOT_TRACKED = -1;           // indicates robot is not being tracked
const int MT_NONE_AVAILABLE = -1;        // indicates no robot is available for tracking

// Robot short names - for user display, etc
const std::string MT_DefaultRobotName[] = {"mid617",
                                        "MiaBot 2", 
                                        "MiaBot 3",
                                        "MiaBot 4",
                                        "MiaBot 5",
                                        "MiaBot 6",
                                        "MiaBot 7"};

// standard colors for drawing robots
const MT_Color MT_RobotColors[] = {MT_Red, 
                              MT_Green, 
                              MT_Blue, 
                              MT_Brown, 
                              MT_Orange, 
                              MT_Purple, 
                              MT_White};

#ifndef _WIN32
// default robot resource port names
//   - on OS X these tend to be e.g. /dev/tty.Miabot2-BluetoothSerial-1
const std::string MT_DefaultPortName[] = {"/dev/tty.mid617-BluetoothSerial-1", 
                                       "/dev/tty.Miabot2-BluetoothSerial-1", 
                                       "/dev/tty.Miabot3-BluetoothSerial-1", 
                                       "/dev/tty.Miabot4-BluetoothSerial-1", 
                                       "/dev/tty.Miabot5-BluetoothSerial-1", 
                                       "/dev/tty.Miabot6-BluetoothSerial-1", 
                                       "/dev/tty.Miabot7-BluetoothSerial-1"};
#else
// default robot resource port names - on MSW these tend to be e.g. COM0
const std::string MT_DefaultPortName[] = {"COM0", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6"};
#endif

// type and constants for flagging/checking connection status changes
//  (helps to manage gamepad)
typedef unsigned int MT_robot_status_change;
const MT_robot_status_change MT_ROBOT_NO_CHANGE = 0;
const MT_robot_status_change MT_ROBOT_JUST_CONNECTED = 1;
const MT_robot_status_change MT_ROBOT_JUST_DISCONNECTED = 2;

// macro to safely index a robot (limits to MT_MAX_NROBOTS)
#define MT_SAFE_ROBOT_INDEX(i) ((i) < MT_MAX_NROBOTS) ? (i) : (MT_MAX_NROBOTS-1)


/******************** Class Definition *****************************/
class MT_AllRobotContainer
{
protected:
    // vector containing actual robot pointers as they are added
    std::vector<MT_SteeredRobot*> myRobots;
    
public:
    // constructor - initialized to no robots and default parameter values
    MT_AllRobotContainer();
    // destructor - does nothing - NOTE robots not deleted here!
    //   they can be deleted by calling ClearBot() (one at a time)
    ~MT_AllRobotContainer();
    
    // gamepad parameters
    float MaxGamePadSpeed;        // speed (m/s) when stick is fully forward or backward
    float MaxGamePadTurningRate;  // turning rate (rad/s) when stick is tilted fully
    
    std::string PortName[MT_MAX_NROBOTS];   // port names for robots
    std::string RobotName[MT_MAX_NROBOTS];  // display names for robots
    int GamepadIndex[MT_MAX_NROBOTS];    // index for gamepad cycling
    
    std::string LastCommand;  // last command sent manually, use to store for display/recall
    
    int GetNumAvailable() const;  // get the number of robots that are connected
    
    int TrackingIndex[MT_MAX_NROBOTS];  // index for tracking - i.e. the i'th robot should
                                     //  get its status updates from TrackedObject[TrackingIndex[i]]
    int GetNumTracked() const;       // count number of robots where TrackingIndex != MT_NOT_TRACKED
    int GetNextUntracked() const;    // find first robot in list where TrackingIndex != MT_NOT_TRACKED
    void ClearTrackingIDs();         // set all tracking IDs to MT_NOT_TRACKED
    
    MT_robot_status_change StatusChange[MT_MAX_NROBOTS];  // keep track of connection status changes
    void ClearStatusChanges();                      // set all status changes to MT_ROBOT_NO_CHANGE
    
    void SetBot(unsigned int i, MT_SteeredRobot* newBot);  // if robot i is connected, delete it.
                                                        //  newBot is then in the i'th position
    void ClearBot(unsigned int i);                      // safely disconnect i'th robot
    
    MT_SteeredRobot*& operator[](unsigned int i);  // Get the i'th robot pointer
    MT_SteeredRobot*& GetRobot(unsigned int i);    // Get the i'th robot pointer
    
    bool IsPhysical(unsigned int i) const;    // return true if i'th robot pointer != NULL
    bool IsConnected(unsigned int i) const;   // return true if i'th robot is connected
    
    // set the x, y, and theta state of the i'th robot (e.g. for use in conjunction with tracker)
    void UpdateState(unsigned int i, float xpos, float ypos, float theta);
    float GetX(unsigned int i) const;          // access X position of i'th robot
    float GetY(unsigned int i) const;          // access Y position of i'th robot
    float GetHeading(unsigned int i) const;    // access heading (theta) of i'th robot
    
    // set the speed (m/s) and turning rate (rad/s) of the i'th robot (e.g. for control)
    void SetSpeedOmega(unsigned int i, float setspeed, float setomega);
    
};


/****************** XML Support Functions *********************/
#ifdef MT_USE_XML
bool MT_WriteRobotXML(MT_AllRobotContainer* robots, float maxspeed, float maxturningrate, MT_XMLFile* xmlfile, bool strict_root = true);
bool MT_ReadRobotXML(MT_AllRobotContainer* robots, float* maxspeed, float* maxturningrate, MT_XMLFile* xmlfile, bool strict_root = true);
#endif // MT_USE_XML

#endif // ALLROBOTCONTAINER_H
