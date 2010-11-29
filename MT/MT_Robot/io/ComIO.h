#ifndef COMIO_H
#define COMIO_H

/** @addtogroup MT_Robot
 * @{
 * 
 * @file ComIO.h
 *
 * @brief Class for serial communications on COM ports.
 *
 * Defines a class that provides platform-independent implementation
 * of a serial port designed for use with the MT_MiaBotPro but
 * suitably generic for use in other applications.
 *
 * On Windows platforms, this uses the 3rd Party library library
 * Win32Com found in MT/MT_Robot/3rdparty/Win32Com, written by Ramon
 * de Klein as a codeproject.com tutorial:
 * http://www.codeproject.com/system/serial.asp 
 *
 * On POSIX platforms, uses standard POSIX serial port access methods.
 * 
 */ 

/* History:
 * Created DTS 8/12/07 (part of old robofish)
 */

//-----------------Headers------------------------------------

// Includes for the CSerial class
#ifdef _WIN32
#define WINDOWS
#include <tchar.h>
#include <windows.h>
#include "MT/MT_Robot/3rdparty/Win32Com/Serial.h"
#endif

//-----------------Constants----------------------------------
/** Use to send output to the console instead of a port */
const char MT_STDERR[] = "stderr";

#ifdef _WIN32

typedef enum
{
    MT_Baud110 = CSerial::EBaud110,
    MT_Baud300 = CSerial::EBaud300,
    MT_Baud600 = CSerial::EBaud600,
    MT_Baud1200 = CSerial::EBaud1200,
    MT_Baud2400 = CSerial::EBaud2400,
    MT_Baud4800 = CSerial::EBaud4800,
    MT_Baud9600 = CSerial::EBaud9600,
    MT_Baud14400 = CSerial::EBaud14400,
    MT_Baud19200 = CSerial::EBaud19200,
    MT_Baud38400 = CSerial::EBaud38400,
    MT_Baud56000 = CSerial::EBaud56000,
    MT_Baud57600 = CSerial::EBaud57600,
    MT_Baud115200 = CSerial::EBaud115200,
    MT_Baud128000 = CSerial::EBaud128000,
    MT_Baud256000 = CSerial::EBaud256000        
} MT_Baudrate;

#else  // not _WIN32

#include <termios.h>
typedef enum
{
    MT_Baud0    = B0,   
    MT_Baud50   = B50,
    MT_Baud75   = B75,
    MT_Baud110  = B110,
    MT_Baud134  = B134,     
    MT_Baud150  = B150,     
    MT_Baud200  = B200,     
    MT_Baud300  = B300,     
    MT_Baud600  = B600,     
    MT_Baud1200 = B1200,    
    MT_Baud1800 = B1800,    
    MT_Baud2400 = B2400,    
    MT_Baud4800 = B4800,    
    MT_Baud9600 = B9600,    
    MT_Baud19200= B19200,   
    MT_Baud38400= B38400   
} MT_Baudrate;

#endif  // _WIN32

/** @class MT_ComIO
 *
 * @brief Cross-platform Serial I/O class. 
 * 
 * A cross-platform communications port class.  For
 * POSIX it uses the read() and write() functions from unistd.h.  For
 * Windows it uses an instance of the
 * <a href="http://www.codeproject.com/system/serial.asp">
 * CSerial class example </a> written by Ramon de Klein on
 * http://codeproject.com 
 *
 * The decision on which to use is based on whether _WIN32 is
 * define'd or not.
 *
 * For best performance, the port should be opened and closed once
 * during program execution rather than once each read/write cycle.
 * There can be significant overhead in the open/close process.
 *
 * Example of usage:
 * @code
 * // Create the port
 * MT_ComIO port("COM4");                      // Windows COM4, or
 * MT_ComIO port("/dev/tty.SomeSerialDevice"); // POSIX, or
 * MT_ComIO port(MT_STDERR);                   // stderr, for debug purposes
 *
 * // Make sure the connection happened 
 * if(!port.IsConnected())
 * {
 *     return;
 * }
 *
 * port.SendCommand("[^]");  // example MiaBot command...
 *
 * char result = "1234567890";  // read back buffer of length 10 + 1
 * // read back at most 10 characters
 * port.ReadString(result);
 *
 * printf("COM port %s read result %s\n", port.getComPort(), result);
 *
 * // port is closed when it goes out of scope 
 * @endcode
 *
 * NOTE: This is one of the oldest classes in MT and therefore
 * disobeys some of the naming conventions used in the newer classes
 * (e.g. methods should start with a lower case verb).  This may be
 * fixed in a future version, but not without warning and/or backwards
 * compatibility.
 */
class MT_ComIO {
private:    
    // Member functions
    void ComInit(bool handshaking = true, MT_Baudrate = MT_Baud9600);
    
    // Variables
    // String descriptor of port device
    char PortString[100];
    // Port File descriptor (linux/mac)
    int fd;

    FILE* m_pFile;
    double m_dT0;

#ifdef _WIN32
    // Instance of serial control class (windows)
    CSerial serial;
#endif
  
    // Keep track of whether or not this is connected
    unsigned char connected;

public:
    /** Default ctor uses stderr as the output rather than an actual
     * COM port. */
    MT_ComIO();  // default = stderr
    /** Ctor to connect to a specific COM port, e.g.
     * @code
     * MT_ComIO("COM4");  // Windows, hardware handshaking or
     * MT_ComIO("COM4", true); // Windows, no handshaking or
     * MT_ComIO("/dev/tty.MiaBot4-BluetoothSerial-1"); // POSIX
     * @endcode
     * @param inComPortString COM Port name, e.g. "COM4", etc.
     * @param handshaking Pass false to disable handshaking
     * @see IsConnected to test status.
     */
    MT_ComIO(const char* inComPortString,
             bool handshaking = true,
             MT_Baudrate baud_rate = MT_Baud9600,
             FILE* file = NULL);

    /** Destructor - closes the port. */
    virtual ~MT_ComIO();

    /** Set a file for output mirroring */
    void SetFile(FILE* file){m_pFile = file;};

    /** Send a null-terminated string to the COM port.  Returns 0 on
     * success, 1 on error.  Converts data to binary data and uses
     * SendCommand. 
     */
    int SendCommand(const char* cmd);
    /** Send binary data (0-255) to the COM port.  Returns 0 on
     * success, 1 on error.
     * @param data Binary data to send.
     * @param n_bytes Number of bytes to send. */
    int SendData(const unsigned char* data, unsigned long n_bytes);

    /* TODO This should really be more robust. */
    /** Read a string from the COM port.  The maximum length of the
     * result is determined by strlen(result).  Uses ReadData and
     * converts the result to chars. */
    int ReadString(char* result);
    /** Read max_length bytes from the COM port. */
    int ReadData(unsigned char* result, unsigned long max_length);

    //Interface Functions

    /** Retrieves the name of the COM port, e.g. COM4 or
     *  /dev/tty.MiaBot-etc. */
    virtual const char* getComPort() const;

    /* TODO This just reports the status from Connect, there is
     *  probably a better way to query the port. */
    /** Query whether or not the port is connected. */
    virtual unsigned char IsConnected() const { return connected;};

    /* Force the port to report a disconnect (does not actually
     * disconnect the port). */
    void FlagDisconnect(){connected = 0;};
  
};

/** @} */

#endif
