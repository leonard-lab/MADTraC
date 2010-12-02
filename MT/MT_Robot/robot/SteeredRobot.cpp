#include <stdio.h>

#include "MT/MT_Core/support/mathsupport.h"
#include "MT/MT_Core/primitives/DataGroup.h"

#include "SteeredRobot.h"

MT_SteeredRobot::MT_SteeredRobot()
    : MT_MiaBotPro(),
      MT_RobotBase("Anonymous"),
      position()
{
    init();
}


MT_SteeredRobot::MT_SteeredRobot(const char* onComPort, const char* name)
        : MT_MiaBotPro(onComPort),
        MT_RobotBase(onComPort, name),
        position()
{
    init();
}


MT_SteeredRobot::MT_SteeredRobot(const char* onComPort,
                                 double inscale,
                                 const char* name)
        : MT_MiaBotPro(onComPort),
          MT_RobotBase(onComPort, name),
          position()
{
    init();
    world_scale = inscale;
}

void MT_SteeredRobot::init()
{
    m_dMaxSpeed = SR_DEFAULT_MAX_SPEED;
    m_dMaxTurningRate = SR_DEFAULT_MAX_TURNING_RATE;
    m_dSpeedDeadBand = SR_DEFAULT_DEADBAND;
    m_dTurningRateDeadBand = SR_DEFAULT_DEADBAND;    
    world_scale = 1.0;
    Autonomous = 0;
    init_buffers(MT_DEFAULT_TAIL_LENGTH_ROBOT);

    m_pParameters = new MT_DataGroup(std::string(m_sName));
    m_pParameters->AddDouble("Max Speed",
                             &m_dMaxSpeed,
                             MT_DATA_READWRITE,
                             0,
                             SR_MAX_ALLOWED_SPEED);
    m_pParameters->AddDouble("Max Turning Rate",
                             &m_dMaxTurningRate,
                             MT_DATA_READWRITE,
                             0,
                             SR_MAX_ALLOWED_TURNING_RATE);
    m_pParameters->AddDouble("Speed Deadband",
                             &m_dSpeedDeadBand,
                             MT_DATA_READWRITE,
                             0,
                             1.0);
    m_pParameters->AddDouble("Turning Deadband",
                             &m_dTurningRateDeadBand,
                             MT_DATA_READWRITE,
                             0,
                             1.0);
}

MT_SteeredRobot::~MT_SteeredRobot()
{
    // delete buffers
    delete xbuffer;
    delete ybuffer;
}


void MT_SteeredRobot::init_buffers(const int length)
{
    xbuffer = new MT_ringbuffer<float>(length);
    ybuffer = new MT_ringbuffer<float>(length);
}

double MT_SteeredRobot::GetX() const
{
    return position.x;
}

double MT_SteeredRobot::GetY() const
{
    return position.y;
}

double MT_SteeredRobot::GetTheta() const
{
    return theta;
}

void MT_SteeredRobot::Update(float inx, float iny, float intheta)
{
    position.x = inx;
    position.y = iny;
    position.z = 0;
    theta = intheta;
}

void MT_SteeredRobot::SetSpeed(double inspd)
{
    SetKinematics(inspd, omega);
}


void MT_SteeredRobot::SetOmega(double inomega)
{
    SetKinematics(speed, inomega);
}


void MT_SteeredRobot::SetSpeedOmega(double inspd, double inomega)
{
    SetKinematics(inspd, inomega);
}


void MT_SteeredRobot::Go()
{
    MT_MiaBotPro::Go();
}


void MT_SteeredRobot::Pause()
{
    MT_MiaBotPro::Pause();
}


void MT_SteeredRobot::Donut(double spd, double radius)
{
    SetKinematics(spd, spd/radius);
}


void MT_SteeredRobot::spitup(const char* name) const
{
    MT_MiaBotPro::spitup(name);

    printf("\t is a MT_SteeredRobot with scale %f\n",world_scale);

}

void MT_SteeredRobot::QueryCounters()
{
    MT_MiaBotPro::QueryCounters();
}

void MT_SteeredRobot::QueryCounters(int* Left, int* Right)
{
    MT_MiaBotPro::QueryCounters(Left, Right);
}

void MT_SteeredRobot::Control()
{ 
    
    if(Autonomous)
    {
        SetSpeedOmega( cspeed, comega ); 
    }
        
}

void MT_SteeredRobot::JoyStickControl(std::vector<double> js_axes,
                                      unsigned int js_buttons)
{

    double X = js_axes[0];
    double Y = js_axes[1];
    double speed = MT_DeadBandAndScale(Y, m_dSpeedDeadBand, m_dMaxSpeed);
    double omega = MT_DeadBandAndScale(X,
                                       m_dTurningRateDeadBand,
                                       m_dMaxTurningRate);
    SetSpeedOmega(speed, omega);
}
