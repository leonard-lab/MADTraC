// Definitions for this class
#include "MT/MT_Core/primitives/BufferAgent.h"
// Needed for velocity estimation (includes math.h)
#include "MT/MT_Core/support/mathsupport.h"


/** Blank initializer.  Initialize the length, current position,
    and playback buffer sizes by setting them to zero.  Do whatever
    agent is supposed to do on init as well. */
MT_BufferAgent::MT_BufferAgent()
    : MT_agent()
{
    // L is the length of the buffer
    L = 0;
  
    // current_position tells us where we are in the current playback
    //   it is a MT_ring_iterator (see support/ringbuffer.h) - this takes 
    //   care of cycling through loops handily.
    current_position.init(0,0);
  
    // these are just std vectors
    xplaybackbuffer.resize(0);
    yplaybackbuffer.resize(0);
    zplaybackbuffer.resize(0);
  
    speed = 0;
  
    NumToAvg = 7;

}


/** Function to add (x,y) to the back of the playback buffers.
    Useful when initially loading the buffers from a file. */
void MT_BufferAgent::Push_Back(double x, double y)
{
    // Increment the length
    L++;
  
    // append x and y to the buffers
    xplaybackbuffer.push_back(x);
    yplaybackbuffer.push_back(y);
  
    // because current_position is a MT_ring_iterator,
    //  it needs to know how long the corresponding vector
    //  is and how full it is
    current_position.setlength(L);
    current_position.incfullto();
  
    NumToAvg = 7;
  
}


/** Function to independently append to the x buffer. */
void MT_BufferAgent::AppendX(double x)
{
    AppendBuffer(xplaybackbuffer, x);
}

/** Function to independently append to the y buffer. */
void MT_BufferAgent::AppendY(double y)
{
    AppendBuffer(yplaybackbuffer, y);
}

/** Function to independently append to the z buffer. */
void MT_BufferAgent::AppendZ(double z)
{
    AppendBuffer(zplaybackbuffer, z);
}


/** Utility function to handle the independent buffer appends.
    Tell it which buffer to append and what value.  It takes care of
    the iterators and length. */
void MT_BufferAgent::AppendBuffer(std::vector<double>& buffer, double value)
{
    // First append the buffer with the supplied value
    buffer.resize(buffer.size()+1,value);

    // set the length - but only if this makes it longer
    L = MT_MAX(L, (int) buffer.size());
  
    // because current_position is a MT_ring_iterator,
    //  it needs to know how long the corresponding vector
    //  is and how full it is
    current_position.setlength(L);
    current_position.incfullto();

}


/** Function to fast forward through NSteps positions in the buffer. */
void MT_BufferAgent::FFWD(int NSteps)
{
    // Just Update and integrate for each of the NSteps
    for(int i = 0; i < NSteps; i++){
        Update();
        integrate();
    }
}


/** Function to rewind through NSteps positions in the buffer. */
void MT_BufferAgent::RWND(int NSteps)
{
    // Because Update increments current_position, we decrement it twice
    //  (note that MT_ring_iterator -= is not yet implemented, so we
    //  call MT_ring_iterator-- twice.)
    for(int i = 0; i < NSteps; i++){
        current_position--;
        current_position--;
        Update();
        integrate();
    }
}


/** Integrate function - pushes the current position into
    the tail buffers. */
void MT_BufferAgent::integrate()
{
    // detects a wrap-around on the playback buffer
    if(abs((int) ((int) current_position - previous_position)) > L - 2)
    {
        // reset the tail buffer on wrap-around
        xbuffer->reset();
        ybuffer->reset();
        zbuffer->reset();
    } else
    {    
        // Update the tail buffers
        xbuffer->push(x());
        ybuffer->push(y());
        zbuffer->push(z());
    }
}


/** Update function - sets the position to the corresponding
    playback buffer entry, estimates the velocity, then increments
    the buffer position. */
void MT_BufferAgent::Update()
{
    // keep track of the previous position
    //  this allows us to detect when we cross
    //  the boundaries of the buffer
    previous_position = current_position;

    // Force the position to the current playback position
    setx(xplaybackbuffer.at(current_position));
    sety(yplaybackbuffer.at(current_position));
    setz(zplaybackbuffer.at(current_position));
  
    // Estimate the orientation and speed
    estVel();
  
    // Increment the current position - note that 
    //  because this is a MT_ring_iterator, it automagically
    //  takes care of wrapping around the end of the buffer
    current_position++;
  
}


/** Function to reset playback to the initial conditions and erase
    the tail buffers. */
void MT_BufferAgent::Reset(double d1, double d2, double d3, double d4)
{
  
    // Point us at the beginning of the playback buffer
    current_position = 0;

    // Tell agent to do whatever it needs to do to reset with
    //  the supplied arguments
    MT_agent::Reset(d1,d2,d3,d4);
  
    // agent::Reset assigns random initial conditions that we need to override
    setx(xplaybackbuffer.at(0));
    sety(yplaybackbuffer.at(0));
    setz(zplaybackbuffer.at(0));
  
    // get the new velocity
    estVel();

}


/** Function to estimate orientation and speed based on
    future and past positions. */
void MT_BufferAgent::estVel()
{
    // differentials of x and y
    double dx = 0;
    double dy = 0;
  
    // If buffer length <= 1, can't estimate a heading, so bail
    if(L <= 1)
        return;

    int cp;
    for( int iavg = 1; iavg <= NumToAvg; iavg++){
        cp = ((int) current_position);  // otherwise, cp +/- iavg will wrap around
        dx += 0.5*(xplaybackbuffer.at( MT_MIN( cp + iavg , L-1) )
                   - xplaybackbuffer.at( MT_MAX( cp - iavg, 0) ) );
        dy += 0.5*(yplaybackbuffer.at( MT_MIN( cp + iavg , L-1) )
                   - yplaybackbuffer.at( MT_MAX( cp - iavg, 0) ) );
    }
  
    dx /= ((float) (NumToAvg));
    dy /= ((float) (NumToAvg));
  
    speed = sqrt(dx*dx + dy*dy)/dT;
              
    // set the orientation and speed
    settheta(atan2(dy,dx));
    kinematics(dx,dy,0);
              
}

double MT_BufferAgent::GetSpeed()
{
  
    return speed;
  
}

long MT_BufferAgent::GetBufferLength()
{
 
    return L;
  
}

void MT_BufferAgent::PopulateSpeedThetaBuffers()
{
    current_position = 0;
  
    // current_position automatically gets wrapped around to 0, so
    //  look for that rather than current_position = L
    do
    {
        estVel();
        speedplaybackbuffer.push_back(speed);
        thetaplaybackbuffer.push_back(gettheta());
        current_position++;
    } while(current_position > 0);

}

int MT_BufferAgent::GetBufferPosition()
{
    return ((int) current_position);
}
