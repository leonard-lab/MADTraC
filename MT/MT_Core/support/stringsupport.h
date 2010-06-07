#ifndef STRINGSUPPORT_H
#define STRINGSUPPORT_H

/** @addtogroup MT_Core */
/**  @{ */

/** @file stringsupport.h
 *  MADTraC
 *
 *  Created by Daniel Swain on 6/7/10.
 *
 */

#include <string>

#ifndef isdigit
#define isdigit MT_isdigit
#endif

bool MT_isdigit(char c);

/* --------- String analysis functions --------------------- */

/** Return values for MT_hasFormatString.
 * @see MT_hasFormatString
 */
typedef enum
{
    MT_FORMAT_STRING_INVALID = 0, /**< No valid format string found.     */
    MT_FORMAT_STRING_CHAR,        /**< Char (%c) format string found.    */
    MT_FORMAT_STRING_STRING,      /**< String (%s) format string found.  */
    MT_FORMAT_STRING_INT,         /**< Integer-type format string found. */
    MT_FORMAT_STRING_FLOAT        /**< Float-type format string found.   */
} MT_FORMAT_STRING_TYPE;

/** Path separator for this platform.  On Windows = '\', otherwise =
 * '/' */
#ifdef _WIN32
const char MT_PathSeparator = '\';
#else
const char MT_PathSeparator = '/';
#endif

/** Checks a string to determine if it contains a valid
 * printf-style format string (e.g. %5.4f, %d, etc).  I've done my
 * best to check against any valid formatter.  Returns an enum 
 * (MT_FORMAT_STRING_TYPE) indicating the <i>type</i> of formatter found,
 * or 0 (MT_FORMAT_STRING_INVALID) if no valid format string is found.
 *
 * @param string_to_check Null-terminated string to check for format.
 * @param p_startof Integer to get the index of the start of the format
 *      string.  Ignored if NULL (default) or no valid format string is found.
 * @param p_endof Integer to get the index of the end of the format
 *      string.  Ignored if NULL (default) or no valid format string is found.
 */
MT_FORMAT_STRING_TYPE MT_hasFormatString(const char* string_to_check, 
        int* p_startof = NULL, 
        int* p_endof = NULL);

/** Function to determine the file extension of a string.  The
 * extension is taken as everything past the last instance of '.' that
 * is not before a path separator (/ or \).
 * Otherwise "" is returned.
 *
 * @see MT_PathSeparator
 */
std::string MT_GetFileExtension(const std::string& path);

/** Function to check if a path has a given extension.  Uses
 * MT_GetFileExtension to determine extension.
 *
 * @see MT_GetFileExtension
 */
bool MT_PathHasFileExtension(const char* path, const char* extension);

/** @} */

#endif // STRINGSUPPORT_H
