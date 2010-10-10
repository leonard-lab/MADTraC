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

std::string MT_EnsurePathHasExtension(const std::string& path,
                                      const std::string& extension);

bool MT_PathIsAbsolute(const std::string& path);
std::string MT_CalculateRelativePath(const std::string& from_path,
                                     const std::string& to_path);

bool MT_CatTextFile(const char* filename);

    
std::string MT_TextFileToString(const char* filename);

std::vector<std::string> MT_TextFileToStringVector(const char* filename);

std::string MT_TextToEndOfLine(FILE* fp);
std::vector<double> MT_ReadDoublesToEndOfLine(FILE* fp);
int MT_GetNumberOfLinesInFile(const char* filename);
    
/** Returns true if the given file can be opened with the given method
 *  (default is "r") */
bool MT_FileIsAvailable(const char* name, const char* method = "r");

/** Create the specified directory with given mode (posix-only,
 * default is O755).  If the directory exists, nothing is done. */
void MT_mkdir(const char* dir_name, unsigned int mode = 0755);

int MT_RMDIR(const char* dir_name);
#endif // FILESUPPORT_H
