#ifdef __MSC_VER
/* Manual set up of definitions for Visual Studio
   (unless you can figure out a way to export the
    preprocessor definitions to a post-build event
    script... let me know!) */

#ifndef WIN32
#define WIN32
#endif

#ifndef _WINDOWS
#define _WINDOWS
#endif

#ifndef WINVER
#define WINVER 0x0400
#endif

#ifndef __WXMSW__
#define __WXMSW__
#endif

#ifndef wxUSE_GUI
#define wxUSE_GUI
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#endif /* _MSC_VER */
