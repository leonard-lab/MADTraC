/*
 *  cChoice.cpp
 *
 *  Created by Daniel Swain on 11/23/09.
 *
 */

#include "Choice.h"

MT_Choice::MT_Choice()
{
    m_vsNames.resize(0);
    m_iValue = 0;
}

MT_Choice::MT_Choice(const std::vector<string>& names, unsigned int value)
{
    m_vsNames.resize(0);
    SetNames(names);
    SetIntValue(value);
}

MT_Choice::MT_Choice(const char* names[], unsigned int num_names, unsigned int value)
{
    m_vsNames.resize(0);
    SetNames(names, num_names);
    SetIntValue(value);
}

void MT_Choice::PushName(const string& newname)
{
    m_vsNames.push_back(newname);
}

void MT_Choice::PushName(const char* newname)
{
    PushName(string(newname));
}

void MT_Choice::SetNames(const std::vector<string>& names)
{
    m_vsNames = names;
}

void MT_Choice::SetNames(const char* names[], unsigned int num_names)
{
    for(unsigned int i = 0; i < num_names; i++)
    {
        PushName(names[i]);
    }
}

unsigned int MT_Choice::GetMaxValue() const
{
    return (m_vsNames.size() - 1);
}

unsigned int MT_Choice::GetIntValue() const
{
    return m_iValue;
}

unsigned int MT_Choice::SetIntValue(unsigned int i)
{
    if(i >= GetMaxValue())
    {
        m_iValue = GetMaxValue();
    }
    else
    {
        m_iValue = i;
    }
    return m_iValue;
}

const char* MT_Choice::GetStringValue() const
{
    return GetName(m_iValue);
}

const char* MT_Choice::GetName(unsigned int i) const
{
    if(m_vsNames.size() == 0)
    {
        return "Error.";
    }
    if(i >= GetMaxValue())
    {
        i = GetMaxValue();
    }
    return m_vsNames[i].c_str();
}

const char* MT_Choice::SetStringValue(const char* val)
{
    string sval(val);
    for(unsigned int i = 0; i < m_vsNames.size(); i++)
    {
        if(val == m_vsNames[i])
        {
            SetIntValue(i);
            return GetStringValue();
        }
    }
    return "Error";
}
