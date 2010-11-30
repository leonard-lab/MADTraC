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
#include <string>

const char MT_ROBOT_NOT_CONNECTED = 0;
const char MT_ROBOT_CONNECTED = 1;

class MT_DataGroup;

class MT_RobotBase
{
public:
    /* Default ctor */
    MT_RobotBase(const char* name) : m_sName(name){};
    /* ctor with port name */
    MT_RobotBase(const char* port, const char* name) : m_sName(name){};

    /* dtor */
    virtual ~MT_RobotBase();

    /* Accessor Functions */
    virtual double GetX() const {return 0;}
    virtual double GetY() const {return 0;}
    virtual double GetTheta() const {return 0;}

	virtual void SetState(std::vector<double> state){};
	virtual std::vector<double> GetState(){std::vector<double> r; r.resize(0); return r;};

	virtual void SetControl(std::vector<double> control){};
	virtual std::vector<double> GetControl(){std::vector<double> u; u.resize(0); return u;};

    virtual const char* getInfo() const {return "MT_RobotBase...";};

    virtual unsigned char IsConnected() const
      { return MT_ROBOT_NOT_CONNECTED; };

    /* control functions */
    virtual void Update(){ Control(); };
    virtual void Control() {};
    virtual void Go() {};
    virtual void SafeStop() {};
    virtual void JoyStickControl(std::vector<double> js_axes,
                                 unsigned int js_buttons){};
    virtual void SendCommand(const char* command) {};
    virtual void SetAutonomousOn() {};
    virtual void SetAutonomousOff() {};
    virtual void AutoIDResponse() {};

    /* tracking update functions */
    virtual void Update(double x, double y, double theta){};
    virtual void Update(std::vector<double> state){};

    /* parameter access */
    virtual MT_DataGroup* GetParameters() { return m_pParameters; };

    friend class MT_AllRobotContainer;
    
protected:
    MT_DataGroup* m_pParameters;
    std::string m_sName;

    void SetParameters(MT_DataGroup* params, bool force = false);
    
private:
    
};



#endif // ROBOTBASE_H
