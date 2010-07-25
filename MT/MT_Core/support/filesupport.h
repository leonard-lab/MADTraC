#ifndef FILESUPPORT_H
#define FILESUPPORT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

/** Path separator for this platform.  On Windows = '\', otherwise =
 * '/' */
#ifdef _WIN32
const char MT_PathSeparator = '\\';
#else
const char MT_PathSeparator = '/';
#endif

const char* const MT_FILE_READ_ERROR_STRING = "__MT_ERROR__";

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

bool MT_CatTextFile(const char* filename);

std::string MT_TextFileToString(const char* filename);

std::vector<std::string> MT_TextFileToStringVector(const char* filename);

#endif // FILESUPPORT_H
