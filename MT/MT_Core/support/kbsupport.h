#ifndef KBSUPPORT
#define KBSUPPORT

/*
 * kbsupport.h
 *
 * Defines a number of platform-independent keyboard functions
 * 1) MT_getch() to act as the normal getch()
 * 2) MT_kbhit() to act as the normal kbhit()
 *
 * On POSIX-like (e.g. OSX, linux) systems you need to call
 * MT_keyboard_init() and MT_keyboard_close() to start and stop the
 * interface with the curses system.
 *
 * On MSW/DOS (DOS untested) these are pass-throughs to getch() and kbhit()
 *
 * Created DTS 9/4/07
 *  - Modified for new MT DTS 8/1/10
 *
 */

void MT_keyboard_init();
int MT_kbhit();
int MT_getch();
void MT_keyboard_close();

#endif // KBSUPPORT
