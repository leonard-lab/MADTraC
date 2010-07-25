#ifndef FILESUPPORT_H
#define FILESUPPORT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

const char* const MT_FILE_READ_ERROR_STRING = "__MT_ERROR__";

bool MT_CatTextFile(const char* filename);
std::string MT_TextFileToString(const char* filename);
std::vector<std::string> MT_TextFileToStringVector(const char* filename);

std::string MT_StringVectorToString(const std::vector<std::string>& input,
                                    const std::string& sep = std::string("\n"));
std::vector<std::string> MT_SplitString(const std::string& input,
                                        const std::string& split_on);

#endif // FILESUPPORT_H
