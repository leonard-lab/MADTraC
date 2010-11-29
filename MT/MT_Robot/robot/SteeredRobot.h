#ifndef STEEREDROBOT_H
#define STEEREDROBOT_H

/************************************************************
 * SteeredRobot.h
 *
 * Defines the MT_SteeredRobot class.  This is a MT_MiaBotPro with
 * only speed and turning rate controls (not direct wheel
 * rate controls).  This should suffice as a basic class
 * to use for most MT_MiaBotPro applications while still guarding
 * the low-level functionality of MT_MiaBotPro.
 *
 * - Modified from FishBot.h  DTS 8/10/09
 *
 ************************************************************/

// Inherits from the (more basic) MT_MiaBotPro class.
#include "MiaBotPro.h"
#include "RobotBase.h"

// Default length of the buffers
static const int MT_DEFAULT_TAIL_LENGTH_ROBOT = 500; 
// Header for the MT_R3 class
#include "MT/MT_Core/primitives/R3.h"
// Header for the MT_ringbuffer class
#include "MT/MT_Core/primitives/ringbuffer.h"

const double SR_DEFAULT_MAX_SPEED = 0.35;             // speeds in meters/sec
const double SR_DEFAULT_MAX_TURNING_RATE = 8.0;       // turning rates in rad/sec (based on 7 cm wheelbase)
const double SR_MAX_ALLOWED_SPEED = 4.0;              // max rates: see DTS notebook #2, 1/2-1/5 2009
const double SR_MAX_ALLOWED_TURNING_RATE = 114.3;
const double SR_DEFAULT_DEADBAND = 0.05;

class MT_SteeredRobot : public MT_MiaBotPro, public MT_RobotBase {   
protected:
  
    MT_R3 position;
    float theta;
    void init_buffers(const int length);
    MT_ringbuffer<float>* xbuffer;
    MT_ringbuffer<float>* ybuffer;
  
    //! scaling factor:  [world meters] = world_scale*[internal units]
    double world_scale;
        
    //! comega: control omega
    double comega;
    double cdtheta;
  
    //! cspeed: control speed
    double cspeed;
        
    unsigned char Autonomous;

    double m_dMaxTurningRate;
    double m_dMaxSpeed;
    double m_dSpeedDeadBand;
    double m_dTurningRateDeadBand;

    void init();
  
public:
  
    //     NOTE: there is no MT_SteeredRobot(comport, speed, omega) constructor
    //             as this seems like a bad idea (wait for the program to
    //             be in control of things to start the robot moving)
  
    //! default constructor (use stdout)
    MT_SteeredRobot();
    //! constructor to specify com port
    MT_SteeredRobot(const char* onComPort, const char* name);
    //! constructor to specify com port and scaling factor
    MT_SteeredRobot(const char* onComPort, double inscale, const char* name);
    // dtor
    virtual ~MT_SteeredRobot();
  
    // function to display the name of this robot (and its port)
    void spitup(const char* name) const;
  
    //! Set kinematic speed
    void SetSpeed(double inspd);
    //! Set kinematic turning rate
    void SetOmega(double inomega);
    //! Set kinematic speed and turning rate
    void SetSpeedOmega(double inspd, double inomega);
        
    // flag autonomous mode on
    void SetAutonomousOn(){ Autonomous = 1; };
    // flag autonomous mode off
    void SetAutonomousOff(){ Autonomous = 0; };
  
    void Go();     //! Provide access to the go command
    void Pause();  //! Provide access to the pause command
  
    //! Do a donut and wow the audience :)
    void Donut(double spd, double radius);
  
    // functions to get counter values
    void QueryCounters();
    void QueryCounters(int* Left, int* Right);
  
    // accessor functions
    double GetX() const;
    double GetY() const;
    double GetTheta() const;
  
    // Safe functions we can pass-through to MiaBot (or MT_ComIO)
    virtual const char* getComPort() const {return MT_MiaBotPro::getComPort();};
    virtual unsigned char IsConnected() const { return MT_MiaBotPro::IsConnected(); };
  
    //! Calculation of Control
    virtual void Update(){ Control(); };
    virtual void Update(float inx, float iny, float intheta);
    void Update(std::vector<double> state){Update(state[0], state[1], state[2]);};
        
    //! Apply Control
    virtual void Control();

    void SafeStop(){SetSpeedOmega(0,0);};
    void AutoIDResponse(){SetSpeedOmega(0.1, 0);};

    void JoyStickControl(std::vector<double> js_axes, unsigned int js_buttons);
        
};

#endif // STEEREDROBOT_H
