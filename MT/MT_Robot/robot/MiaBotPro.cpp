//------------------------------------------------------------
//  MiaBotPro.cpp - robofish - D.Swain 8/12/07
//  
//  Implementation of MT_MiaBotPro class.  Provides an interface
//   to the robot - inherits communications from MT_ComIO and
//   kinematics from SteeredParticle
//
//------------------------------------------------------------


//-----------------Headers------------------------------------

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Class definition
#include "MiaBotPro.h"

//-----------------Constructors-------------------------------

/** Default constructor: a unit speed particle on stderr.  All
    constructors should start paused and zero the encoders. */
// Default constructor (use stderr)
MT_MiaBotPro::MT_MiaBotPro()
    : MT_ComIO("stderr")
{

    c2m = MT_DEFAULT_C2M;
    wheelbase = MT_DEFAULT_WHEELBASE;
    LeftSpeedInt = 0;
    RightSpeedInt = 0;
    paused = 1;
    LeftCount = 0;
    RightCount = 0;
    speed = 0;
    omega = 0;

    // Zero the encoders
    ZeroCounters();

}

/** Constructor to specify the communications port.  All
    constructors should start paused and zero the encoders. */
// Constructor for specific port
MT_MiaBotPro::MT_MiaBotPro(const char* inComPortString)
    : MT_ComIO(inComPortString)
{

    c2m = MT_DEFAULT_C2M;
    wheelbase = MT_DEFAULT_WHEELBASE;
    LeftSpeedInt = 0;
    RightSpeedInt = 0;
    paused = 1;
    LeftCount = 0;
    RightCount = 0;
    speed = 0;
    omega = 0;

    // Zero the encoders
    ZeroCounters();
    printf("Counters Zeroed\n");

}

//-----------------Member Functions---------------------------

/** Function to send a command to the robot.  Takes care of
    wrapping the command in []'s in a fairly forgiving way. */
// Send a command to the robot wrapped in []'s
int MT_MiaBotPro::SendCommand(const char* cmd)
{

    // May need to up this if the command is ever longer than
    //  15 characters
    char scmd[15];

    // check for [ at front
    int first = (*cmd == '[');
    // check for ] at end
    int last = (*(cmd + strlen(cmd)-1) == ']');

    // do whatever we need to do to get the right syntax
    if(first && !last)
        sprintf(scmd,"%s]",cmd);
    if(!first && last)
        sprintf(scmd,"[%s",cmd);
    if(!first && !last)
        sprintf(scmd,"[%s]",cmd);
    if(first && last)
        strcpy(scmd,cmd);

    // let the user know if the bot is paused
#ifdef VERBOSE
    if(paused)
        printf("\t(Robot is paused)\n");
#endif

    // send the command
    return MT_ComIO::SendCommand(scmd);

}

/** Function to convert encoder counts to meters.  Uses the
    current value of the c2m variable to allow for adaption. */
// Convert encoder counts to meters
double MT_MiaBotPro::Counts2Meters(double counts) const
{
    // Note:  c2m is allowed to change!
    return c2m*counts;
}

/** Function to convert meters to encoder counts.  Uses the
    current value of the c2m variable to allow for adaption. */
// Convert meters to encoder counts
int MT_MiaBotPro::Meters2Counts(double meters) const
{
    // Note:  c2m is allowed to change!
    return ((int) (meters/c2m));
}

/** Function to set the left wheel speed variable.  This 
    limits the value to the +/- 2000 range. */
// Set left wheel speed variable
void MT_MiaBotPro::SetLeftSpeedInt(int LS)
{
    // Threshold to +/- 2000
    if(LS > 2000){ LeftSpeedInt = 2000; }
    else if(LS < -2000){ LeftSpeedInt = -2000; }
    else {LeftSpeedInt = LS; }
}

/** Function to set the right wheel speed variable.  This 
    limits the value to the +/- 2000 range. */
// Set right wheel speed variable
void MT_MiaBotPro::SetRightSpeedInt(int RS)
{
    // Threshold to +/- 2000
    if(RS > 2000){ RightSpeedInt = 2000; }
    else if(RS < -2000){ RightSpeedInt = -2000; }
    else {RightSpeedInt = RS; }
}


/** Function to safely set wheel speeds in counts/sec.
    Scales the value and sets the speed variables.  Executes
    the change if the robot is not paused.  Also updates the
    kinematic variables (speed, omega). */
// Set speed in counts/sec
void MT_MiaBotPro::SetSpeedC(int LeftSpeed, int RightSpeed)
{
    // Set variables
    SetLeftSpeedInt(LeftSpeed/50);    // factor of 50 due to robot internal scaling
    SetRightSpeedInt(RightSpeed/50);  //  MBP Manual pg 8
    // Update kinematics
    speed = 0.5*Counts2Meters(LeftSpeed + RightSpeed);
    omega = (float) (LeftSpeed - RightSpeed);
      
    // Go, if not paused
    if(!paused)
        ActivateSpeed();

}

/** Function to safely set wheel speeds in meters/sec.
    Scales the value and sets the speed variables.  Executes
    the change if the robot is not paused.  Also updates the
    kinematic variables (speed, omega). */
// Set speed in meters/sec
void MT_MiaBotPro::SetSpeedM(double LeftSpeed, double RightSpeed)
{
    // Set variables  
    SetLeftSpeedInt(Meters2Counts(LeftSpeed)/50);    // factor of 50 due to robot internal scaling
    SetRightSpeedInt(Meters2Counts(RightSpeed)/50);  //  MBP Manual pg 8
    // Update Kinematics
    speed = 0.5*(LeftSpeed + RightSpeed);
    omega = (LeftSpeed - RightSpeed)/wheelbase;

    // Go, if not paused
    if(!paused)
        ActivateSpeed();

}

/** Function to report our identity to the user. */
// Debugging report function
void MT_MiaBotPro::spitup(const char* name) const
{

    printf("\t Is a MT_MiaBotPro on ComPort:  %s\n", MT_ComIO::getComPort());

}

/** Function to set the kinematic parameters and effect
    them onto the robot. */
// Set kinematics
void MT_MiaBotPro::SetKinematics(double inspeed, double inomega)
{

    speed = inspeed;
    omega = inomega;

    // Invert the kinematics to get wheel speeds
    double lspeed = inspeed + 0.5*wheelbase*inomega;
    double rspeed = inspeed - 0.5*wheelbase*inomega;
    // Set wheel speeds
    SetSpeedM(lspeed,rspeed);

}

/** Function to unpause the robot.  Robot will start moving
    according to the Left/RightSpeedInt variables, which should
    reflect the kinematic model. */
// Unpause and go!
void MT_MiaBotPro::Go()
{
    paused = 0;
    ActivateSpeed();
}

/** Safely pause and stop the robot.  */
// Stop safely
void MT_MiaBotPro::Pause()
{
    if(!paused)
        Stop();
    paused = 1;
}

/** Actually tells the robot to start moving at the current
    stored wheel speeds.  NOTE that this is a private member
    function ON PURPOSE - we should always use the wrapper
    command Go() to obey the pause state so that the robot
    does not start moving unexpectedly. */
// Start - NOT SAFE - only call when not paused
void MT_MiaBotPro::ActivateSpeed()
{

    // Build the command from the current speed int variables
    char Command[15];
    sprintf(Command,"=%d,%d",LeftSpeedInt,RightSpeedInt);

    // Send the command, but let us know if it doesn't go well
    if(SendCommand(Command)){ printf("Error Sending Speed Command\n");}

}

unsigned char MT_MiaBotPro::TestConnection()
{
  
    char result[] = "<test#xx>\n";
  
    SendCommand("t");
    int status = MT_ComIO::ReadString(result);

    if(!status)
    {
        // status = 0 implies the read was successful -> still connected;
        return 1;
    }
    else
    {
        // not connected!
        MT_ComIO::FlagDisconnect();
        return 0;
    }
  
}

/** Function to query the MiaBot Pro's encoder counts.  This version
    just updates our locally-stored values. */
// Update our knowledge of the encoder counts
void MT_MiaBotPro::QueryCounters()
{
    // String representing the format of the data we expect back
    char result[] = "<: -dddddddddd:+dddddddddd>\n";

    SendCommand(":");
    MT_ComIO::ReadString(result);
    int r = sscanf(result,"%*s %11d:%11d>",&LeftCount,&RightCount);

}

/** Function to query the MiaBot Pro's encoder counts and return
    (by reference) their values to the user. */
// Query the counters and report the values
void MT_MiaBotPro::QueryCounters(int* lCount, int* rCount)
{
    // Execute the localized version
    QueryCounters();
    // Report the values we've found
    *lCount = LeftCount;
    *rCount = RightCount;
}

/** Function to zero the encoder values.  We may also want to take
    care of any special tasks involving dead reckoning. */
// Zero the counters
void MT_MiaBotPro::ZeroCounters()
{
    // take care of any dead reckoning tasks

    // Send the command to zero the counters on the bot
    SendCommand(";0:0");
}
