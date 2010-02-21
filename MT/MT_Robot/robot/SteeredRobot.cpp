#include <stdio.h>


#include "SteeredRobot.h"

MT_SteeredRobot::MT_SteeredRobot()
    : MT_MiaBotPro(), position()
{

    world_scale = 1.0;
    Autonomous = 0;
    init_buffers(MT_DEFAULT_TAIL_LENGTH);

}


MT_SteeredRobot::MT_SteeredRobot(const char* onComPort)
  : MT_MiaBotPro(onComPort), position()
{

    world_scale = 1.0;
    Autonomous = 0;
    init_buffers(MT_DEFAULT_TAIL_LENGTH);

}


MT_SteeredRobot::MT_SteeredRobot(const char* onComPort, double inscale)
    : MT_MiaBotPro(onComPort), position()
{

    world_scale = inscale;
    Autonomous = 0;
    init_buffers(MT_DEFAULT_TAIL_LENGTH);

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
        
};

