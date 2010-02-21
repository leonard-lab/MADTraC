#ifndef MIABOTPRO_H
#define MIABOTPRO_H
//------------------------------------------------------------
//  MiaBotPro.h - robofish - D.Swain 8/12/07
//  
//  Header file for MT_MiaBotPro class.  Provides an interface
//   to the robot - inherits communications from MT_ComIO
//
//------------------------------------------------------------


//-----------------Headers------------------------------------

// Communications class
#include "ComIO.h"

//-----------------Constants----------------------------------

//! Number of Meters per Count, as specified by the MBP user manual
static const double MT_DEFAULT_C2M = 3.9884e-5;
//! Wheel base as specified by MBP user manual = 7 cm
static const double MT_DEFAULT_WHEELBASE = 0.07;

/* A class that provides an interface to the MiaBotPro.  Takes
   care of unit conversion and command protocols.  Keeps track of
   robot encoder counts.  Inherits from
   MT_ComIO - For communications on a serial port
*/
class MT_MiaBotPro : private MT_ComIO {

private:
    // Private Member Functions
    void SetLeftSpeedInt(int LS);
    void SetRightSpeedInt(int RS);
    void ActivateSpeed();
    /** Send the stop command.  Use only as an Estop since this
        does not set the paused flag */
    void Stop(){ SendCommand("s"); };

    // Private Data
    int LeftSpeedInt;    /**<  Left wheel speed command  [robot units]  */
    int RightSpeedInt;   /**<  Right wheel speed command [robot units]  */
    int paused;          /**<  Indicates whether the robot is paused
                            (default TRUE)                        */
    long int LeftCount;  /**<  Left wheel encoder counts                */
    long int RightCount; /**<  Right wheel encoder counts               */

protected:
    // Protected member functions
    double Counts2Meters(double counts) const;
    int Meters2Counts(double meters) const;

    // Protected data
    double c2m;          /**<  Counts to meters conversion factor       */
    double wheelbase;    /**<  Wheelbase length (meters)                */
    float speed;
    float omega;

public:
    // constructors
    MT_MiaBotPro();
    //MT_MiaBotPro(int inComPort);   // disabled for now
    MT_MiaBotPro(const char* inComPortString);

    // Destructor
    /** The destructor stops the robot */
    virtual ~MT_MiaBotPro(){ Pause(); };

    // Public Member Functions
    virtual void spitup(const char* name) const;

    // Low-Level Control/Vocabulary Functions
    // simple one-char commands defined inline
    // These are generally safe, since they call only finite time commands
    //   Be careful with DoSequence and be aware of the motion parameters
    //   There is enough rope here to hang yourself
    /** Test Communications.  Sends back a test string.         */
    virtual void SendTest(){    SendCommand("t"); }; 
    /** Query Version.  Sends back a firmware version.          */
    virtual void SendVersion(){ SendCommand("?"); }; 
    /** Stepwise forward.  See MBP Manual pg 11.                */
    virtual void Forward(){     SendCommand("^"); }; 
    /** Stepwise backward.  See MBP Manual pg 11.               */
    virtual void Backward(){    SendCommand("v"); }; 
    /** Stepwise left turn.  See MBP Manual pg 11.              */
    virtual void Left(){        SendCommand("<"); }; 
    /** Stepwise right turn.  See MBP Manual pg 11.             */
    virtual void Right(){       SendCommand(">"); }; 
    /** Execute current sequence.  See MBP Manual pg 13.        */
    virtual void DoSequence(){  SendCommand("~"); }; 
    /** Clear Stored Sequence.  See MBP Manual pg 13.           */
    virtual void ClrSequence(){ SendCommand("$"); }; 
    int SendCommand(const char* cmd);

  
    // 
    unsigned char TestConnection();


    // Higher level functions
    virtual void QueryCounters();
    virtual void QueryCounters(int* lCount, int* rCount);
    virtual void ZeroCounters();
    // These commands are restricted by the pause state
    virtual void SetSpeedC(int LeftSpeed, int RightSpeed);
    virtual void SetSpeedM(double LeftSpeed, double RightSpeed);
    virtual void SetKinematics(double inspeed, double inomega);

    // Action commands - affect speed changes, etc
    virtual void Go();    
    virtual void Pause(); 
  
    // Safe functions we can pass-through to MiaBot (or MT_ComIO)
    virtual const char* getComPort() const {return MT_ComIO::getComPort();};
    virtual unsigned char IsConnected() const { return MT_ComIO::IsConnected(); };

};

#endif
