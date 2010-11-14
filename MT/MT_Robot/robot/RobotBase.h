#ifndef ROBOTBASE_H
#define ROBOTBASE_H

/*
 * RobotBase.h - Dan Swain, 11/13/10
 *
 * A base class for robots to work with MT_AllRobotContainer,
 * MT_GamePadController, and the associated dialogs.
 *
 */

#include <vector>

const char MT_ROBOT_NOT_CONNECTED = 0;
const char MT_ROBOT_CONNECTED = 1;

class MT_DataGroup;

class MT_RobotBase
{
public:
    /* Default ctor */
    MT_RobotBase() : m_pParameters(NULL) {};
    /* ctor with port name */
    MT_RobotBase(const char* port) : m_pParameters(NULL) {};

    /* dtor */
    virtual ~MT_RobotBase(){};

    /* Accessor Functions */
    virtual double GetX() const {return 0;}
    virtual double GetY() const {return 0;}
    virtual double GetTheta() const {return 0;}

    virtual const char* getInfo() const {return "MT_RobotBase...";};

    virtual unsigned char IsConnected() const
      { return MT_ROBOT_NOT_CONNECTED; };

    /* control functions */
    virtual void Update(){ Control(); };
    virtual void Control() {};
    virtual void SafeStop() {};

    /* tracking update functions */
    virtual void Update(double x, double y, double theta){};
    virtual void Update(std::vector<double> state){};

    virtual MT_DataGroup* GetParameters() { return m_pParameters; };

    friend class MT_AllRobotContainer;
    
protected:
    MT_DataGroup* m_pParameters;

    void SetParameters(MT_DataGroup* params, bool force = false);
    
private:
    
};



#endif // ROBOTBASE_H
