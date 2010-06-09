#ifndef MT_GAMEPAD_H
#define MT_GAMEPAD_H

#include <stdio.h>

#define MT_GAMEPAD_USE_WX

#ifndef MT_GAMEPAD_USE_WX
#if defined(__APPLE__) || defined(MACOSX) || defined(__DARWIN__)

#define MT_GAMEPAD_MAC

const int MT_MAC_STICKMAX = 128;
const int MT_MAC_STICKMIN = -128;

#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/errno.h>
#include <sysexits.h>
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>
// #include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>

// Cookies
typedef struct cookie_struct
{
    IOHIDElementCookie gXAxisCookie;
    IOHIDElementCookie gYAxisCookie;
    IOHIDElementCookie gZAxisCookie;
    IOHIDElementCookie gWAxisCookie;
    IOHIDElementCookie gButton1Cookie;
    IOHIDElementCookie gButton2Cookie;
    IOHIDElementCookie gButton3Cookie;
    IOHIDElementCookie gButton4Cookie;
    IOHIDElementCookie gButton5Cookie;
    IOHIDElementCookie gButton6Cookie;
    IOHIDElementCookie gButton7Cookie;
    IOHIDElementCookie gButton8Cookie;
    IOHIDElementCookie gButton9Cookie;
    IOHIDElementCookie gButtonaCookie;
    IOHIDElementCookie gButtonbCookie;
    IOHIDElementCookie gButtoncCookie;
    IOHIDElementCookie gHatCookie;
    IOHIDElementCookie gU2Cookie;
    IOHIDElementCookie gU3Cookie;
    IOHIDElementCookie gU4Cookie;
} *cookie_struct_t;

#elif defined(_WIN32)

#ifndef MT_GAMEPAD_USE_WX
#define MT_GAMEPAD_USE_WX
#endif

#endif
#endif

#ifdef MT_GAMEPAD_USE_WX
#include <wx/joystick.h>
#endif

// Macro to set a bit of B, masked by M, to the value v
#define MT_SETBIT(B,M,v) (v) ? (B |= M) : (B &= (!M))

// Maximum value of analog stick (as a float)
const float MT_MAX_STICK_VAL_FLOAT = 128.0;

// Button Masks
const unsigned int MT_BUTTON1 = 0x001;
const unsigned int MT_BUTTON2 = 0x002;
const unsigned int MT_BUTTON3 = 0x004;
const unsigned int MT_BUTTON4 = 0x008;
const unsigned int MT_BUTTON5 = 0x010;
const unsigned int MT_BUTTON6 = 0x020;
const unsigned int MT_BUTTON7 = 0x040;
const unsigned int MT_BUTTON8 = 0x080;
const unsigned int MT_BUTTON9 = 0x100;
const unsigned int MT_BUTTONa = 0x200;
const unsigned int MT_BUTTONb = 0x400;
const unsigned int MT_BUTTONc = 0x800;

class MT_HIDGamePad
{
protected:
  
#ifdef MT_GAMEPAD_MAC
    cookie_struct_t gpCookies;
    io_object_t hidDevice;
    io_iterator_t hidObjectIterator;
    IOHIDDeviceInterface **hidDeviceInterface;
  
    // Queue for button states
    IOHIDQueueInterface** ButtonQueue;
    
    // Functions to set up the device - mostly copied from
    //  Apple sample code "hid_project"
    unsigned int FindHIDDevices();
    unsigned int FindGamePad();
    unsigned int CreateHIDDeviceInterface(IOHIDDeviceInterface*** hidDeviceInterfaceP);
    cookie_struct_t getHIDCookies(IOHIDDeviceInterface122 **handle);
    
    // Function to get a specific cookie value
    int getCookieValue(IOHIDElementCookie Cookie);  
  
#elif defined MT_GAMEPAD_USE_WX
    wxJoystick myJoystick;
#endif

    // Most recently updated states
    unsigned int ButtonStates;
    int X;
    int Y;
    int Z;
    int W;
    int Hat;
    int U2;
    int U3;
    int U4;
  

    float xcenter;
    float xnorm;
    float ycenter;
    float ynorm;
    float zcenter;
    float znorm;
    float wcenter;
    float wnorm;
  
    // should be within [-1,1]
    float Xf;
    float Yf;
    float Zf;
    float Wf;
  
    bool StatusGood;

public:
    MT_HIDGamePad();
    ~MT_HIDGamePad();
  
    // Call this to actually find the gamepad
    //  returns 0 if not found
    unsigned int Init();

    void Poll();
  
    bool GetStatus();
    void Disconnect();
  
#ifdef MT_GAMEPAD_MAC
    void CheckQueue();
#endif

    //////////////////////////////////////////////////////////////
    //  Functions to access the current value of a button state
    //    without polling the hardware (state could be old)
    //////////////////////////////////////////////////////////////
    int Button1State() const { return ButtonStates & MT_BUTTON1; }
    int Button2State() const { return ButtonStates & MT_BUTTON2; }
    int Button3State() const { return ButtonStates & MT_BUTTON3; }
    int Button4State() const { return ButtonStates & MT_BUTTON4; }
    int Button5State() const { return ButtonStates & MT_BUTTON5; }
    int Button6State() const { return ButtonStates & MT_BUTTON6; }
    int Button7State() const { return ButtonStates & MT_BUTTON7; }
    int Button8State() const { return ButtonStates & MT_BUTTON8; }
    int Button9State() const { return ButtonStates & MT_BUTTON9; }
    int ButtonaState() const { return ButtonStates & MT_BUTTONa; }
    int ButtonbState() const { return ButtonStates & MT_BUTTONb; }
    int ButtoncState() const { return ButtonStates & MT_BUTTONc; }

    int x() const { return X; }
    int y() const { return Y; }
    int z() const { return Z; }
    int w() const { return W; }
    float xf() const {return Xf;}
    float yf() const {return Yf;}
    float zf() const {return Zf;}
    float wf() const {return Wf;}
    int hat() const { return Hat; }
    int buttons() const { return ButtonStates; }

    int PollXAxis();
    int PollYAxis();
    int PollZAxis();
    int PollWAxis();

    int PollButton1State();
    int PollButton2State();
    int PollButton3State();
    int PollButton4State();
    int PollButton5State();
    int PollButton6State();
    int PollButton7State();
    int PollButton8State();
    int PollButton9State();
    int PollButtonaState();
    int PollButtonbState();
    int PollButtoncState();

    int PollHatState();

    int PollU2State();
    int PollU3State();
    int PollU4State();
};

typedef MT_HIDGamePad MT_gamepad;

#endif
