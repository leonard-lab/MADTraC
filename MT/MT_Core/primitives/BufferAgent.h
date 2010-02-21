#ifndef BUFFERAGENT_H
#define BUFFERAGENT_H

#include "MT/MT_Core/primitives/agent.h"
#include "MT/MT_Core/primitives/ringbuffer.h"

#include <vector>

class MT_BufferAgent : public MT_agent {
protected:
    std::vector<double> xplaybackbuffer;
    std::vector<double> yplaybackbuffer;
    std::vector<double> zplaybackbuffer;
    std::vector<double> thetaplaybackbuffer;

    void AppendBuffer(std::vector<double>& buffer, double value);

    long L;
    MT_ring_iterator current_position;
    unsigned int previous_position;
  
    int NumToAvg;  //< Number of points ahead and behind to average for heading
  
    double speed;

public:
    MT_BufferAgent();

    void Push_Back(double x, double y);
    virtual void Update();
    void AppendX(double x);
    void AppendY(double y);
    void AppendZ(double z);
    void FFWD(int NSteps);
    void RWND(int NSteps);
    virtual void integrate();
  
    void PopulateSpeedThetaBuffers();
  
    void estVel();
  
    virtual void Reset(double d1, double d2, double d3, double d4);

    virtual ~MT_BufferAgent(){ };
  
    double GetSpeed();
    long GetBufferLength();
  
    int GetBufferPosition();
  
    std::vector<double> speedplaybackbuffer;

};

#endif
