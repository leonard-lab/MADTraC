#ifndef AGENT_H
#define AGENT_H
//------------------------------------------------------------
//  agent.h - glswarm/robofish - D.Swain 8/17/07
//  
//  A class from which all the other particle models can be
//   derived.  Includes basic properties like position and
//   color, simulation methods like integration and kinematics,
//   and methods to draw the representation.
//
//------------------------------------------------------------


//-----------------Headers------------------------------------

// Header for the MT_R3 class
#include "MT/MT_Core/primitives/R3.h"
// Header for the MT_ringbuffer class
#include "MT/MT_Core/primitives/ringbuffer.h"
// Header for the MT_Color class
#include "MT/MT_Core/primitives/Color.h"

//-----------------Constants----------------------------------

/* Defines the default length of buffers. */
// Default length of the buffers
const int MT_DEFAULT_TAIL_LENGTH = 500;
// Default draw size, in pixels
const double MT_DEFAULT_SIZE = 10;
// Invalid Identifier = -1
const int MT_INVALID_ID = -1;

//-----------------Lite Agent Type----------------------------

/* A class that includes the salient features of an agent
   (position, heading) but no member functions or buffer.
   Intended for use when these features are unnecessary 
   (for example in the tracking algorithm), resulting in
   a structure that is lite on memory but can be converted
   into an agent later. */
class MT_LiteAgent 
{
public:
    
    MT_R3 position;
    double theta;
    int Id;
    
    double x() const {return position.x;};
    double y() const {return position.y;};
    double z() const {return position.z;};
    
MT_LiteAgent() : position(0,0,0) { Id = MT_INVALID_ID;};
MT_LiteAgent(double setx, double sety, double settheta)
    : position(setx, sety, 0)
    {
        theta = settheta;
        Id = MT_INVALID_ID;
    };
MT_LiteAgent(double setx, double sety, double settheta, int setId)
    : position(setx, sety, 0)
    {
        theta = settheta;
        Id = setId;
    };
MT_LiteAgent(double setx, double sety, double setz, double settheta)
    : position(setx, sety, setz)
    {
        theta = settheta;
        Id = MT_INVALID_ID;
    };
    
};

//-----------------Class Definition---------------------------

/* A class from which we derive the other models from.  Sets
   up the basic framework for keeping track of and integratig
   a kinematic model and drawing on an OpenGL canvas. */
class MT_agent {
protected:
    // Protected data
    MT_R3 dp;                       /**< Vector differential.      */
    MT_R3 position;                 /**< Vector position.          */
    double theta;                 /**< Orientation Angle.  Note
                                     that in the agent model this
                                     does not correspond to any
                                     kinematic quantity.  It is
                                     used only for drawing.      */
    double dtheta;                /**< Differential of the
                                     orientation/heading angle.
                                     This should only be set by
                                     SteeredParticle or higher
                                     classes.                    */
  
    int glyph;                   /**< GL Listing number.        */
    MT_ringbuffer<double>* xbuffer; /**< x buffer.                 */
    MT_ringbuffer<double>* ybuffer; /**< y buffer.                 */
    MT_ringbuffer<double>* zbuffer; /**< z buffer.                 */
    int tail_length;             /**< Length of buffers.        */
    int NNeighbors;              /**< Number of neighbors.      */
    double SensingRadius;        /**< Maximum Sensing Radius.   */
    double BlindAngle;           /**< Blindspot angle (radians) */
  
    // Protected member functions
    void init_buffers(const int);
    void common_init();
  
public:
    // Public data
    int Id;                      /**< Identifier.               */
    double dT;                   /**< Timestep.                 */
    double size;                  /**< Drawing size (pixels)     */
    double aspect;
    double units_per_pixel;
    MT_Color myColor;              /**< Drawing color.  Set
                                    randomly on initialization. */
  
    MT_R3 getPos() const {return position;};
    virtual void setPos(MT_R3 inPos){position = inPos;};
    virtual void setPos(double inx, double iny){ setx(inx);  sety(iny); };
  
    // Constructors
    MT_agent();
    MT_agent(double scale);
    MT_agent(double inx, double iny, double inz);
  
    // Destructor
    virtual ~MT_agent();
  
    // Public member functions
    // Kinematics
    void kinematics(double indx, double indy, double indz);
    virtual void integrate();
    /** Prototype for a function to update the agent. */
    virtual void Update(){};
    /** Function to manually update the agent's state.
        Useful for "dummy" particles. */
    virtual void Update(double inx, double iny, double inz, double intheta)
    {setx(inx); sety(iny); setz(inz); theta = intheta;};
  
    // Accessors
    /** Function to get the position x coordinate. */
    double x() const {return position.x;};
    /** Function to get the position y coordinate. */
    double y() const {return position.y;};
    /** Function to get the position z coordinate. */
    double z() const {return position.z;};
    /** Function to get the orientation angle.     */
    double gettheta() const {return theta;};
    /** Function to set the position x coordinate. */
    void setx(double inx){position.x = inx;};
    /** Function to set the position y coordinate. */
    void sety(double iny){position.y = iny;};
    /** Function to set the position z coordinate. */
    void setz(double inz){position.z = inz;};
    /** Function to set the orientation angle.     */
    void settheta(double inth){theta = inth;};
    /** Function to set the maximum sensing radius.*/
    void setSensingRadius(double inR){SensingRadius = inR;};
    /** Function to set the blind angle.           */
    void setBlindAngle(double inB){BlindAngle = inB;};
    /** Funtion to set the size.  */
    void setSize(double inS){size = inS;};
    /** Function to get the size.  */
    double getSize() const {return size;};
    void setAspect(double inA){aspect = inA;};
    double getAspect() const {return aspect;};
    double getBlindAngle() const {return BlindAngle;};
  
    // Color Functions
    /** Function to set the particle color. */
    void setColor(double inR, double inG, double inB)
    {myColor.setR(inR); myColor.setG(inG); myColor.setB(inB);};
    /** Function to set the particle color (MT_Color supplied). */
    void setColor(const MT_Color& inColor){ myColor = inColor; }
    /** Function to get the red color value.   */
    double R() const {return myColor.R();};
    /** Function to get the green color value. */
    double G() const {return myColor.G();};
    /** Function to get the blue color value.  */
    double B() const {return myColor.B();};
  
    // Debugging
    virtual void spitup(const char* name) const;
  
    virtual void Reset(double xmin, double xmax, double ymin, double ymax);
  
    // Sensing Functions
    // These are the only quantities we should be able to know!
    double DistanceTo(const MT_agent* them);
    double DistanceTo(const MT_LiteAgent* them);
    double DistanceTo(const MT_R3& there);
    double RelAngle(const MT_agent* them);
    /** Function to find another agent's heading relative to
        our own. */
    double RelHeading(const MT_agent* them){return them->gettheta() - theta;};
  
    /** Prototype for a function to test whether another
        agent is a neighbor. */
    virtual int HasNeighbor(const MT_agent* them);
    /** Prototype for a function to react to neighbors. */
    virtual void Sense(const MT_agent* them){NNeighbors++;};
  
};



#endif
