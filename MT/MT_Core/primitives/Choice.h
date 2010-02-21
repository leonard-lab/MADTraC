#ifndef cChoice_H
#define cChoice_H

/*
 *  cChoice.h
 *
 *  Created by Daniel Swain on 11/23/09.
 *
 */

#include <string>
#include <vector>
using namespace std;

class MT_Choice
{
protected:
    std::vector<string> m_vsNames;
    unsigned int m_iValue;
    
public:
    MT_Choice();
    MT_Choice(const std::vector<string>& names, unsigned int value);
    MT_Choice(const char* names[], unsigned int num_names, unsigned int value);
    
    void PushName(const string& newname);
    void PushName(const char* newname);
    void SetNames(const std::vector<string>& names);
    void SetNames(const char* names[], unsigned int num_names);
    
    unsigned int GetMaxValue() const;
    unsigned int GetIntValue() const;
    unsigned int SetIntValue(unsigned int i);
    
    //string GetStringValue();
    const char* GetStringValue() const;
    //string GetName(unsigned int i);
    const char* GetName(unsigned int i) const;
    const char* SetStringValue(const char* val);
    
};

#endif // cChoice_H
