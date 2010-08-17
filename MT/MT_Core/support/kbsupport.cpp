#include "kbsupport.h"

#include <stdio.h>
#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
static struct termios initial_settings, new_settings;
static int peek_character = -1;
#else
#include <conio.h>
#endif

void MT_keyboard_init()
{
#ifndef _WIN32
    tcgetattr(0,&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
#endif
}

void MT_keyboard_close()
{
#ifndef _WIN32
    tcsetattr(0, TCSANOW, &initial_settings);
#endif
}

int MT_kbhit()
{

#ifndef _WIN32
    char ch;
    int nread;

    if(peek_character != -1)
        return 1;
    new_settings.c_cc[VMIN]=0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0,&ch,1);
    new_settings.c_cc[VMIN]=1;
    tcsetattr(0,TCSANOW,&new_settings);

    if(nread == 1){
        peek_character = ch;
        return 1;
    }
    return 0;
#else
    return _kbhit();
#endif
}

int MT_getch(){
#ifndef _WIN32
    char ch;

    if(peek_character != -1){
        ch = peek_character;
        peek_character = -1;
        return ch;
    }
    int r = read(0,&ch,1);
    if(r != 1)
    {
        fprintf(stderr,
                "Warning:  MT_getch():  Could not read character "
                "from stdin.  Returning 0.\n");
        return 0;
    }
    return ch;
#else
    return _getch();
#endif
}
