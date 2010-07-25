#include "filesupport.h"

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

std::string MT_StringVectorToString(const std::vector<std::string>& input,
                                    const std::string& sep)
{
    std::string all_lines;
    for(unsigned int i = 0; i < input.size(); i++)
    {
        all_lines += input[i];
        all_lines += sep;
    }
    return all_lines;
}

std::vector<std::string> MT_SplitString(const std::string& input,
                                        const std::string& split_on)
{
    std::vector<std::string> result;
    result.resize(0);

    if(!split_on.size())
    {
        return result;
    }

    size_t found, foundp = 0;

    found = input.find(split_on);
    while(found != std::string::npos)
    {
        result.push_back(input.substr(foundp, found - foundp));
        foundp = found + split_on.size();
        found = input.find(split_on,
                           foundp + 1);
    }
    result.push_back(input.substr(foundp, std::string::npos));
    return result;
}
