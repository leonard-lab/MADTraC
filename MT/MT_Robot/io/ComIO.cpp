//------------------------------------------------------------
//  ComIO.cpp - robofish - D.Swain 8/12/07
//  
//  Defines a class that provides platform-independent
//   implementation of a serial port designed for use
//   with the MT_MiaBotPro but suitably generic for use
//   in other applications.
//
//------------------------------------------------------------


//-----------------Headers------------------------------------

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Header for this class
#include "ComIO.h"
//#include "com/Serial.h"

// Headers for the POSIX implementation using read()/write()
#ifndef _WIN32
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#endif

// This causes problems in windows for some reason
#ifndef _WIN32
using namespace std;
#endif


//-----------------Constants----------------------------------
//! Defined if we want to see every IO command at stdout
//#define COM_DEBUG


//-----------------Code---------------------------------------

// Default constructor sets up to spit out to stdout (or wherever printf points)
MT_ComIO::MT_ComIO()
    : m_pFile(NULL)
{

    // Set the port
    strcpy(PortString,"stderr");
    // Common initializations of the port
    ComInit();

}

// Constructor to initialize on a specific port
/* Constructor to set up on a specific port by supplying
    a string descriptor of the resource. */
MT_ComIO::MT_ComIO(const char* inComPortString, bool handshaking, FILE* file)
    : m_pFile(file)
{

    // Set the port
    strcpy(PortString,inComPortString);
    // Common initializations of the port
    ComInit(handshaking);

}

// Common initialization of the com port
/* Function to initialize the communications port.  Called by
    each of the constructors. */
void MT_ComIO::ComInit(bool handshaking)
{

    // Assume that we are connected, set to zero if an error occurs
    //   -- this seems like a bad idea, but I can't think of a simple way
    //      to do the opposite
    connected = 1;
  
    // TODO This should eventually return an int for error checking

    printf("Initializing MT_ComIO on %s\n",PortString);

    // Do this only if we aren't using stderr
    if((strcmp(PortString,"STDERR") != 0) && 
       (strcmp(PortString,"stderr") != 0) ){

#ifndef _WIN32  // POSIX implementation 

        struct termios options;
        printf("Opening device %s\n",PortString);

        // Mostly taken from various websites, these are fairly
        //  standard settings that seem to work pretty well
        fd = open(PortString, O_RDWR | O_NOCTTY | O_NDELAY);
        if(fd < 0){ printf("Error opening port (will proceed anyways)\n"); connected = 0; }
        tcgetattr(fd,&options);
        cfsetispeed(&options,B9600);
        cfsetospeed(&options,B9600);
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_cflag &= -PARENB;
        options.c_cflag &= -CSTOPB;
        options.c_cflag &= -CSIZE;
        options.c_cflag |= CS8;
        // Set up for a 1 sec timeout
        options.c_cc[VMIN]  = 0;
        options.c_cc[VTIME] = 10;
        tcsetattr(fd,TCSANOW,&options);
        // read/write issues will block program flow
        fcntl(fd, F_SETFL, 0);

        // Note the timeout and blocking setup was necessary to get reads
        //  to operate properly on the MT_MiaBotPro (not necessary for writes)

#else   // Windows implementation using CSerial class

        // Taken from CSerial class "Hello World" example
        LONG lLastError = ERROR_SUCCESS;
        printf("Opening port %s\n", PortString);

        /* Attempt to open the port  */
		/* We need to handle the possibility that we're building in Unicode mode
		 * here.  If we are, then the simplest thing to do is copy the string
		 * char-for-char to a wide character array and pass that array to the
		 * serial port class.  Otherwise there is a byte alignment problem and
		 * the serial port won't connect. */
#ifdef UNICODE
		wchar_t t[512];
		for(unsigned int i = 0; i < strlen(PortString) + 1; i++)
		{
			t[i] = PortString[i];
		}
		lLastError = serial.Open(t, 0, 0, false);
#else
        lLastError = serial.Open((LPCTSTR)(PortString),0,0,false);
#endif
        if (lLastError != ERROR_SUCCESS){
            printf("Unable to open COM-port\n");
            connected = 0;
        }

        // Setup the serial port (9600,N81) using hardware handshaking
        lLastError = serial.Setup(CSerial::EBaud9600,CSerial::EData8,
                                  CSerial::EParNone,CSerial::EStop1);
        if (lLastError != ERROR_SUCCESS){
            printf("Unable to set COM-port setting\n");
            connected = 0;
        }

        // Setup handshaking
		if(handshaking)
		{
			lLastError = serial.SetupHandshaking(CSerial::EHandshakeHardware);
        	if (lLastError != ERROR_SUCCESS){
            	printf("Unable to set COM-port handshaking\n");
            	connected = 0;
        	}
		}

        // Setup the read method to nonblocking (prevents computer hangs)
        lLastError = serial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
        if(lLastError != ERROR_SUCCESS){
            printf("Unable to set COM-port timeout method\n");
            connected = 0;
        }

#endif

    } else {  // what to do if we want to use stderr

        strcpy(PortString,"stderr");
        printf("Opening (pseudo) COM port on stdout\n");
        fd = 0;  // fd = 0 corresponds to stderr
        connected = 1;  // report this as connected...

    }

    if(m_pFile)
    {
        fprintf(m_pFile,
                "This file is a mirror of output to a COM "
                "port opened on %s by MADTraC\n",
                PortString);
    }

}


/* Destructor - closes the port automatically when the
    instance is deleted (or the program closes). */
// Destructor should close the port
MT_ComIO::~MT_ComIO()
{
    printf("Closing port %s\n", PortString);

    if(connected)
    {
#ifndef WINDOWS
        close(fd);
#else
        serial.Close();
#endif
    }

    if(m_pFile)
    {
        fprintf(m_pFile,
                "Closing COM port from destructor.\n");
    }

}


/* Function to write a string out on the port. */
// Write a string out on the port
int MT_ComIO::SendCommand(const char* cmd)
{
    unsigned char* data =
        (unsigned char*) calloc(strlen(cmd), sizeof(unsigned char));

    unsigned long n_bytes = strlen(cmd);
    for(unsigned long i = 0; i < n_bytes; i++)
    {
        data[i] = cmd[i];
    }

    int r = SendData(data, n_bytes);

    free(data);

    return r;
}
    
int MT_ComIO::SendData(const unsigned char* data, unsigned long n_bytes)
{
    if(m_pFile)
    {
        fprintf(m_pFile, "Attempting to write ");
        for(unsigned int i = 0; i < n_bytes; i++)
        {
            fprintf(m_pFile, "%d, ", data[i]);
        }
        fprintf(m_pFile, " (%d bytes).\n", n_bytes);
    }
    // Inform the user what's going on.  Always do this if 
    //  COM_DEBUG is defined, otherwise only if the port
    //  is set up as stderr.
#ifdef COM_DEBUG
    if(strcmp(PortString,MT_STDERR) != 0)
        printf("Sending Command \"%s\" to %s.\n",cmd,PortString);
#endif

#ifndef WINDOWS

    // Posix implementation using unistd.h et al

    // n is how many bites are successfully written
    int n = write(fd, data, n_bytes);
    if(!fd){ printf("\n"); }      // fd = 0 means stdout, print a newline
    // to avoid clutter

    // report an error if n < 0 or n does not match
    //   the number of bytes we expect
    if(n < 0 || (unsigned int) n != n_bytes){
        printf("Byte mismatch error writing command\n\t\"%s\" to %s\n",
               data,PortString);
        if(m_pFile){fprintf(m_pFile, "Byte mismatch error!\n");};
        return 1;
    }

#else

    // Windows implementation using CSerial

    LONG lLastError = ERROR_SUCCESS;
    lLastError = serial.Write(data, n_bytes);
    if(lLastError != ERROR_SUCCESS){
        printf("Serial error writing command\n\t\"%s\" to %s\n",
               data,PortString);
        if(m_pFile){fprintf(m_pFile, "Serial Error!\n");};
        return 1;
    }

#endif

    if(m_pFile){fprintf(m_pFile, "Success.\n");};
    // All is well, return 0
    return 0;

}


/* Function to allow us to query the port specifier */
// Return the name of the port
const char* MT_ComIO::getComPort() const
{
    return PortString;
}


/* Function to read back a string of known length.  Should be
    supplied with a string of the same length as the one which we
    expect to get back. */
// Read a string from the port
int MT_ComIO::ReadString(char* result)
{
    if(m_pFile)
    {
        fprintf(m_pFile,
                "Attempting to read into string %s\n",
                result);
    }
    unsigned long length = strlen(result);
    unsigned char* data =
        (unsigned char*) calloc(length, sizeof(unsigned char));
    memset(data, 0, length);

    int r = ReadData(data, length);

    for(unsigned long i = 0; i < length; i++)
    {
        result[i] = data[i];
    }

    if(m_pFile)
    {
        fprintf(m_pFile,
                "Got %s, error status %d\n",
                result,
                r);
    }

    free(data);

    return r;
}

int MT_ComIO::ReadData(unsigned char* result, unsigned long max_length)
{

    if(m_pFile)
    {
        fprintf(m_pFile,
                "Attempting to read up to %d bytes\n",
                max_length);
    }
    // Inform the user what's going on.  Always do this if 
    //  COM_DEBUG is defined, otherwise only if the port
    //  is set up as stderr.
#ifdef COM_DEBUG
    if(strcmp(PortString,MT_STDERR) != 0)
        printf("Attempting to read %d bytes on port %s.\n",
               max_length, PortString);
#endif
    if(strcmp(PortString,MT_STDERR) == 0)
    {
        fprintf(m_pFile, "But not doing it because port is stderr\n");
        return 0;
    }

#ifndef WINDOWS  // POSIX implementation

    // number of read characters so far
    unsigned int N = 0;
    // number of characters reported by read
    //  (initialized != 0)
    int n = 1;

    // read characters one at a time until either 
    //  read says there are no more (n <= 0) or
    //  we have gotten all we asked for (N = length)
    while(n > 0 && N < max_length){

        // get a character into the buffer
        char buff[2];
        n = read(fd,buff,1);
        // if we got anything, put it in the result
        if(n)
            result[N++] = buff[0];

    }

    // report an error if we didn't get the number of characters
    //  we expected
    if(N < max_length){
        printf("Problem reading from %s (error %d).\n",
               PortString, errno);
        if(m_pFile)
        {
            fprintf(m_pFile, "Error reading data - too few bytes read\n");
        }
        return 1;
    }

#else  // Windows implementation using CSerial

    // Initialize to no error
    LONG lLastError = ERROR_SUCCESS;
    // read (length) bytes from the port
    lLastError = serial.Read(result, max_length);
    // Let us know if there is an eror
    if(lLastError != ERROR_SUCCESS){
        printf("Serial error recieving %d bytes on %s (error %d)\n",
               max_length,PortString,lLastError);
        if(m_pFile)
        {
            fprintf(m_pFile, "Serial Error Reading!\n");
        }
        return 1;
    }

#endif

    // hopefully return 0
    return 0;

}
