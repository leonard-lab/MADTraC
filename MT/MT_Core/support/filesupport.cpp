#include "filesupport.h"

#include <sys/stat.h>
#include <sstream>

#include "stringsupport.h"

#include "mathsupport.h" /* for MT_MIN */

#ifndef _WIN32
#else
  #include <direct.h>
  #define mkdir(a,b) _mkdir(a)
#endif

bool MT_CatTextFile(const char* filename)
{
    std::string file_text = MT_TextFileToString(filename);

    if(file_text != std::string(MT_FILE_READ_ERROR_STRING))
    {
        std::cout << file_text;
        return true;
    }
    else
    {
        return false;
    }
}

std::string MT_TextFileToString(const char* filename)
{
    std::vector<std::string> lines = MT_TextFileToStringVector(filename);

    if(lines.size() == 0)
    {
        return std::string(MT_FILE_READ_ERROR_STRING);
    }
    else
    {
        return MT_StringVectorToString(lines);
    }
    
}

std::vector<std::string> MT_TextFileToStringVector(const char* filename)
{
    std::vector<std::string> lines;
    std::string line;
    lines.resize(0);

    std::ifstream readfile(filename);

    if(readfile.is_open())
    {
        while(!readfile.eof())
        {
            getline(readfile, line);
            lines.push_back(line);
        }
        readfile.close();
    }

    return lines;

}

int MT_GetNumberOfLinesInFile(const char* filename)
{
    std::string line;
    std::ifstream readfile(filename);
    int nlines = 0;

    if(readfile.is_open())
    {
        while(!readfile.eof())
        {
            getline(readfile, line);
            nlines++;
        }
        readfile.close();
    }
    return nlines - 1;
}

std::string MT_TextToEndOfLine(FILE* fp)
{

    std::string r;
    if(!fp)
    {
        r = "MT_TextToEndOfLineInFile Error:  Null file descriptor";
        std::cerr << r << std::endl;
        return r;
    }

    std::stringstream ss;
    char c = fgetc(fp);
    while(c != '\n' && c != EOF)
    {
        ss << c;
        c = fgetc(fp);
    }

    r = ss.str();
    
    return r;
}

std::vector<double> MT_ReadDoublesToEndOfLine(FILE* fp)
{
    std::string s = MT_TextToEndOfLine(fp);
    std::vector<double> D;
    double d;
    std::stringstream ss;

    D.resize(0);
    ss.str(s);
    while(ss >> d)
    {
        D.push_back(d);
    }
    return D;
}

std::string MT_GetFileExtension(const std::string& path)
{
    std::string::size_type idx = path.rfind('.');
    std::string::size_type idx2 = path.rfind(MT_PathSeparator);
    std::string return_value = "";

    if(idx != std::string::npos
       && (idx2 == std::string::npos || (idx2 < idx)))
    {
        return_value = path.substr(idx+1);
    }

    return return_value;
}

bool MT_PathIsAbsolute(const std::string& path)
{

    /* look for *:/ or *:\ */
    bool win_abs_path =  (path.size() >= 3) && (path[1] == ':')
        && ((path[2] == '/') || (path[2] == '\\'));

    /* look for /(*) */
    bool posix_abs_path = (path.size() >= 1) && (path[0] == '/');

    return win_abs_path || posix_abs_path;

}

std::string MT_CalculateRelativePath(const std::string& from_path,
                                     const std::string& to_path)
{
    std::string::size_type from_length = from_path.size();
    std::string::size_type to_length = to_path.size();

    if(from_length == 0 || to_length == 0)
    {
        return from_path;
    }

    if(MT_PathIsAbsolute(from_path) != MT_PathIsAbsolute(to_path))
    {
        return from_path;
    }

#ifdef _WIN32    
    int common_levels = -1;
#else
    int common_levels = 0;
#endif
    unsigned int i = 0;
    unsigned int last_same_index = 0;

    for(i = 0; i < MT_MIN(from_length, to_length); i++)
    {
        if(from_path[i] != to_path[i])
        {
            break;
        }
        
        if(from_path[i] == MT_PathSeparator)
        {
            last_same_index = i;
            common_levels++;
        }
    }

    std::string common_path = from_path.substr(0,last_same_index);
    std::string diff_path_to = to_path.substr(last_same_index);
    std::string diff_path_from = from_path.substr(last_same_index);

    if(diff_path_to[0] == MT_PathSeparator)
    {
        diff_path_to = diff_path_to.substr(1);
    }
    if(diff_path_from[0] == MT_PathSeparator)
    {
        diff_path_from = diff_path_from.substr(1);
    }

    std::stringstream ss;
    for(i = 0; i < diff_path_to.size(); i++)
    {
        if(diff_path_to[i] == MT_PathSeparator)
        {
            ss << "../";
        }
    }
    ss << diff_path_from;

    return ss.str();
    
}

bool MT_PathHasFileExtension(const char* path, const char* extension)
{
    std::string ext(extension);
    std::string actual = MT_GetFileExtension(std::string(path));

    return (actual == ext);
}

std::string MT_EnsurePathHasExtension(const std::string& path,
                                      const std::string& extension)
{
    std::string _extension = extension;
    
    if(extension.size() == 0)
    {
        return path;
    }

    if(extension[0] == '.')
    {
        _extension = extension.substr(1);
    }
    
    if(MT_PathHasFileExtension(path.c_str(), _extension.c_str()))
    {
        return path;
    }
    else
    {
        std::string result = path + std::string(".") + _extension;
        return result;
    }
}

bool MT_FileIsAvailable(const char* name, const char* method)
{
  FILE* tmp_file = fopen(name, method);
  if(tmp_file)
  {
    fclose(tmp_file);
    return true;
  }
  else
  {
    return false;
  }
}

void MT_mkdir(const char* dir_name, unsigned int mode)
{
    struct stat st;
    if(stat(dir_name, &st) == 0)
    {
        return;
    }
    else
    {
        mkdir(dir_name, mode);
    }
}

int MT_RMDIR(const char* filename)
{
    std::string cmd;
#ifdef _WIN32
    cmd = "rmdir /s /q ";
#else
    cmd = "rm -rf ";
#endif

    cmd += filename;

    return system(cmd.c_str()); 
}
