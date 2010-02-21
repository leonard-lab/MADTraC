#ifndef COMIO_H
#define COMIO_H

//------------------------------------------------------------
//  ComIO.h - robofish - D.Swain 8/12/07
//  
//  Defines a class that provides platform-independent
//   implementation of a serial port designed for use
//   with the MT_MiaBotPro but suitably generic for use
//   in other applications.
//
//------------------------------------------------------------


//-----------------Headers------------------------------------


// Includes for the CSerial class
#ifdef _WIN32
#define WINDOWS
#include <tchar.h>
#include <windows.h>
#include "Win32Com/Serial.h"
#endif

//-----------------Constants----------------------------------
/** Use to send output to the console instead of a port */
// Console
const char MT_STDERR[] = "stderr";

/* A cross-platform communications port class.  For POSIX it uses 
   the read() and write() functions from unistd.h.  For Windows it
   uses an instance of the CSerial class example written by 
   Ramon de Klein on codeproject.com:  
   http://www.codeproject.com/system/serial.asp
   The decision on which to use is based on whether _WIN32 is
   #define'd or not
*/
class MT_ComIO {
private:    
    // Member functions
    void ComInit();
    
    // Variables
    //! String descriptor of port device
    char PortString[100];
    //! Port File descriptor (linux/mac)
    int fd;

#ifdef _WIN32
    //! Instance of serial control class (windows)
    CSerial serial;
#endif
  
    //! Keep track of whether or not this is connected
    unsigned char connected;

public:
    // Constructors
    MT_ComIO();  // default = stderr
    MT_ComIO(const char* inComPortString);

    // Destructor (closes port)
    virtual ~MT_ComIO();

    //Communication Functions
    int SendCommand(const char* cmd);
    int ReadString(char* result);

    //Interface Functions
    // Report the ComPort
    virtual const char* getComPort() const;
  
    // Query whether or not this port is connected
    virtual unsigned char IsConnected() const { return connected;};
    // Allow devices to tell us we're not connected
    void FlagDisconnect(){connected = 0;};
  
};

#endif
