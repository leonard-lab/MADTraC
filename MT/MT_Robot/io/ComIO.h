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
    void ComInit(bool handshaking = true);
    
    // Variables
    // String descriptor of port device
    char PortString[100];
    // Port File descriptor (linux/mac)
    int fd;

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
    MT_ComIO(const char* inComPortString, bool handshaking = true);

    /** Destructor - closes the port. */
    virtual ~MT_ComIO();

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
