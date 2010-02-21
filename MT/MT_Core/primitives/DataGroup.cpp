/*
 *  DataGroup.cpp
 *
 *  Created by Daniel Swain on 10/21/09.
 *
 */

#include "DataGroup.h"

#include <algorithm>              /* for transform */
#include "MT/MT_Core/support/mathsupport.h"  /* for MT_MAX/MT_MIN */

#define VOID_POINTER_TO_VECTOR_CAST(of_type, void_pointer, index)       \
    ((std::vector<of_type> *)void_pointer)->at(index);

#define GET_VOID_POINTER_TO_VECTOR_CAST(name, of_type, void_pointer)    \
    std::vector<of_type>* name = (std::vector<of_type> *)void_pointer;
  
MT_DataElement::MT_DataElement(const string& name, 
                         void* const p, 
                         MT_TYPE t,
                         bool isreadonly,
                         double min_val,
                         double max_val)
  : pdata(p), m_Type(t), DialogIgnore(false)
{
  
    Name = name;
    ReadOnly = isreadonly;
  
    switch(m_Type)
    {
    case MT_TYPE_BOOL:
        Min_val = 0;
        Max_val = 1;
        break;
    case MT_TYPE_INT:
        Min_val = MT_MAX(min_val, MT_min_int);
        Max_val = MT_MIN(max_val, MT_max_int);
        break;
    case MT_TYPE_UINT:
        Min_val = MT_MAX(min_val, 0);
        Max_val = MT_MIN(max_val, MT_max_uint);
        break;
    case MT_TYPE_LONG:
        Min_val = MT_MAX(min_val, MT_min_long);
        Max_val = MT_MIN(max_val, MT_max_long);
        break;
    case MT_TYPE_CHAR:
        Min_val = MT_MAX(min_val, MT_min_char);
        Max_val = MT_MIN(max_val, MT_max_char);
        break;
    case MT_TYPE_UCHAR:
        Min_val = MT_MAX(min_val, 0);
        Max_val = MT_MIN(max_val, MT_max_uchar);
        break;
    case MT_TYPE_STRING:
        Min_val = 0;
        Max_val = 1;
        break;
    case MT_TYPE_FLOAT:
        Min_val = MT_MAX(min_val, MT_min_float);
        Max_val = MT_MIN(max_val, MT_max_float);
        break;
    case MT_TYPE_DOUBLE:
        Min_val = min_val;
        Max_val = max_val;
        break;
    case MT_TYPE_COLOR:
        Min_val = 0;
        Max_val = 1;
        break;
    case MT_TYPE_CHOICE:
        Min_val = 0;
        Max_val = ((MT_Choice *)pdata)->GetMaxValue();
        break;
    case MT_TYPE_ERR:
    default:
        fprintf(stderr, "Data Element Error:  Unknown or error type.\n");
        break;
    }
  
    ClampValue();
  
}

MT_DataElement::MT_DataElement()
  : pdata(NULL), m_Type(MT_TYPE_ERR)
{
    ReadOnly = true;
    Name = "error";
}

double MT_DataElement::GetNumericValue() const
{
  
    switch(m_Type)
    {
    case MT_TYPE_BOOL:
        return (double) *(bool *)pdata;  break;
    case MT_TYPE_INT:
        return (double) *(int *)pdata;  break;
    case MT_TYPE_UINT:
        return (double) *(unsigned int *)pdata;  break;
    case MT_TYPE_LONG:
        return (double) *(long *)pdata;  break;
    case MT_TYPE_CHAR:
        return (double) *(char *)pdata;  break;
    case MT_TYPE_UCHAR:
        return (double) *(unsigned char *)pdata;  break;
    case MT_TYPE_STRING:
    {
        float val = 0;
        string sval = *(string *)pdata;
        sscanf(sval.c_str(), "%f", &val);
        return val;
        break;
    }
    case MT_TYPE_FLOAT:
        return (double) *(float *)pdata;  break;
    case MT_TYPE_DOUBLE:
        return (double) *(double *)pdata;  break;
    case MT_TYPE_COLOR:
        return 0; break;  /* doesn't make sense to ask for this */
    case MT_TYPE_CHOICE:
        return (double) ((MT_Choice *)pdata)->GetIntValue(); break;
    case MT_TYPE_ERR:
    default:
        fprintf(stderr, "Data Element Error:  Unknown or error type.\n");
        return 0;
        break;
    }
  
}

string MT_DataElement::GetStringValue() const
{
  
    char cresult[100];
    string sresult;
  
    switch(m_Type)
    {
    case MT_TYPE_BOOL:
        sprintf(cresult, "%s", *(bool *)pdata ? "true" : "false"); break;
    case MT_TYPE_INT:
        sprintf(cresult, "%d", *(int *)pdata); break;
    case MT_TYPE_UINT:
        sprintf(cresult, "%d", *(unsigned int *)pdata); break;
    case MT_TYPE_LONG:
        sprintf(cresult, "%ld", *(long *)pdata); break;
    case MT_TYPE_CHAR:
        sprintf(cresult, "%c", *(char *)pdata); break;
    case MT_TYPE_UCHAR:
        sprintf(cresult, "%c", *(unsigned char *)pdata); break;
    case MT_TYPE_STRING:
        return *(string *)pdata; break;
    case MT_TYPE_FLOAT:
        sprintf(cresult, "%f", *(float *)pdata); break;
    case MT_TYPE_DOUBLE:
        sprintf(cresult, "%f", *(double *)pdata); break;
    case MT_TYPE_COLOR:
        sprintf(cresult, "%s", ((MT_Color *)pdata)->GetHexString()); break;
    case MT_TYPE_CHOICE:
        sprintf(cresult, "%s", ((MT_Choice *)pdata)->GetStringValue()); break;
    case MT_TYPE_ERR:
    default:
        sprintf(cresult, "Data Element Error:  Unknown or error type.\n");
        break;
    }
    sresult = cresult;
    return sresult;
  
}

void MT_DataElement::ClampValue()
{
  
    /* clamping these types makes no sense */
    if(m_Type == MT_TYPE_STRING || m_Type == MT_TYPE_BOOL || m_Type == MT_TYPE_CHOICE || m_Type == MT_TYPE_COLOR)
    {
        return;
    }
  
    switch(m_Type)
    {
    case MT_TYPE_INT:
        if(*(int *)pdata > Max_val)
        {
            *(int *)pdata = (int) Max_val;
        }
        if(*(int *)pdata < Min_val)
        {
            *(int *)pdata = (int) Min_val;
        }
        break;
    case MT_TYPE_UINT:
        if(*(unsigned int *)pdata > Max_val)
        {
            *(unsigned int *)pdata = (unsigned int) Max_val;
        }
        if(*(unsigned int *)pdata < Min_val)
        {
            *(unsigned int *)pdata = (unsigned int) Min_val;
        }
        break;
    case MT_TYPE_LONG:
        if(*(long *)pdata > Max_val)
        {
            *(long *)pdata = (long) Max_val;
        }
        if(*(long *)pdata < Min_val)
        {
            *(long *)pdata = (long) Min_val;
        }
        break;
    case MT_TYPE_CHAR:
        if(*(char *)pdata > Max_val)
        {
            *(char *)pdata = (char) Max_val;
        }
        if(*(char *)pdata < Min_val)
        {
            *(char *)pdata = (char) Min_val;
        }
        break;
    case MT_TYPE_UCHAR:
        if(*(unsigned char *)pdata > Max_val)
        {
            *(unsigned char *)pdata = (unsigned char) Max_val;
        }
        if(*(unsigned char *)pdata < Min_val)
        {
            *(unsigned char *)pdata = (unsigned char) Min_val;
        }
        break;
    case MT_TYPE_FLOAT:
        if(*(float *)pdata > Max_val)
        {
            *(float *)pdata = (float) Max_val;
        }
        if(*(float *)pdata < Min_val)
        {
            *(float *)pdata = (float) Min_val;
        }
        break;
    case MT_TYPE_DOUBLE:
        if(*(double *)pdata > Max_val)
        {
            *(double *)pdata = (double) Max_val;
        }
        if(*(double *)pdata < Min_val)
        {
            *(double *)pdata = (double) Min_val;
        }
        break;
    case MT_TYPE_STRING:  /* these should have been handled above */
    case MT_TYPE_BOOL:
    case MT_TYPE_CHOICE:
    case MT_TYPE_COLOR:
    case MT_TYPE_ERR:
    default:
        fprintf(stderr, "MT_DataElement Error:  Unknown or error type.\n"); break;
    }
}

double MT_DataElement::SetDoubleValue(double val)
{
    if(m_Type == MT_TYPE_COLOR)  /* doesn't make sense to ask for this */
    {
        return 0;
    }
  
    /* pre-clamp if necessary - prevents numerical issues */
    if(m_Type != MT_TYPE_BOOL && m_Type != MT_TYPE_STRING)
    {
        if(val > Max_val)
        {
            val = Max_val;
        }
        if(val < Min_val)
        {
            val = Min_val;
        }
    }

    switch(m_Type)
    {
    case MT_TYPE_BOOL:
        if(!ReadOnly)
        {
            *(bool *)pdata = (val > 0);
        }
        return (double) *(bool *)pdata;
        break;
    case MT_TYPE_INT:
        if(!ReadOnly)
        {
            *(int *)pdata = (int) val;
        }
        return (double) *(int *)pdata;
        break;
    case MT_TYPE_UINT:
        if(!ReadOnly)
        {
            *(unsigned int *)pdata = (unsigned int) val;
        }
        return (double) *(unsigned int *)pdata;
        break;
    case MT_TYPE_LONG:
        if(!ReadOnly)
        {
            *(long *)pdata = (long) val;
        }
        return (double) *(long *)pdata;
        break;
    case MT_TYPE_CHAR:
        if(!ReadOnly)
        {
            *(char *)pdata = (char) val;
        }
        return (double) *(char *)pdata;
        break;
    case MT_TYPE_UCHAR:
        if(!ReadOnly)
        {
            *(unsigned char *)pdata = (unsigned char) val;
        }
        return (double) *(unsigned char *)pdata;
        break;
    case MT_TYPE_STRING:
        if(!ReadOnly)
        {
            char cval[100];
            sprintf(cval, "%f", val);
            *(string *)pdata = (string) cval;
        }
        return val;
        break;
    case MT_TYPE_FLOAT:
        if(!ReadOnly)
        {
            *(float *)pdata = (float) val;
        }
        return (double) *(float *)pdata;
        break;
    case MT_TYPE_DOUBLE:
        if(!ReadOnly)
        {
            *(double *)pdata = (double) val;
        }
        return (double) *(double *)pdata;
        break;
    case MT_TYPE_CHOICE:
        if(!ReadOnly)
        {
            ((MT_Choice *)pdata)->SetIntValue((unsigned int) val);
        }
        return (double) ((MT_Choice *)pdata)->GetIntValue();
        break;
    case MT_TYPE_ERR:
    default:
        return 0;
        break;
    }
  
}

string MT_DataElement::SetStringValue(string sval)
{
    if(m_Type == MT_TYPE_CHOICE)  /* not implemented */
    {
        if(!ReadOnly)
        {
            const char* r = ((MT_Choice *)pdata)->SetStringValue(sval.c_str());
            return string(r);
        }
    }
  
    if(m_Type == MT_TYPE_STRING)
    {
        if(!ReadOnly)
        {
            *(string *)pdata = sval;
        }
        return *(string *)pdata;
    }
  
    if(m_Type == MT_TYPE_COLOR)
    {
        ((MT_Color *)pdata)->SetHexValue(sval.c_str());
        return ((MT_Color *)pdata)->GetHexString();
    }
  
    /* convert to upper case */
    std::transform(sval.begin(), sval.end(), sval.begin(), ::toupper);
    double v;
  
    if(m_Type == MT_TYPE_BOOL)
    {
        if(!ReadOnly)
        {
            if(sscanf(sval.c_str(), "%lf", &v))
            {
                *(bool *)pdata = (v > 0);
            }
            else if(sval.find("TRUE")!=string::npos)
            {
                *(bool *)pdata = true;
            }
            else if(sval.find("FALSE")!=string::npos)
            {
                *(bool *)pdata = false;
            }
        }
        char cresult[100];
        sprintf(cresult, "%s", *(bool *)pdata ? "true" : "false");
        sval = cresult;
        return sval;
    }
  
    if(sscanf(sval.c_str(), "%lf", &v))
    {
        SetDoubleValue(v);
    }
  
    return GetStringValue();
  
}

MT_VectorDataElement::MT_VectorDataElement(const string& name, 
                                     void* const p, 
                                     MT_TYPE t, 
                                     double min_val, 
                                     double max_val)
  : MT_DataElement(name, p, t, MT_DATA_READONLY, min_val, max_val)
{
}

MT_VectorDataElement::MT_VectorDataElement()
  : MT_DataElement()
{
}

double MT_VectorDataElement::GetNumericValue(unsigned int i) const
{
  
    try
    {
        switch(m_Type)
        {
        case MT_TYPE_BOOL:
            return (double) VOID_POINTER_TO_VECTOR_CAST(bool, pdata, i);
            break;
        case MT_TYPE_INT:
            return (double) VOID_POINTER_TO_VECTOR_CAST(int, pdata, i);  break;
        case MT_TYPE_UINT:
            return (double) VOID_POINTER_TO_VECTOR_CAST(unsigned int, pdata, i);  break;
        case MT_TYPE_LONG:
            return (double) VOID_POINTER_TO_VECTOR_CAST(long, pdata, i);  break;
        case MT_TYPE_CHAR:
            return (double) VOID_POINTER_TO_VECTOR_CAST(char, pdata, i);  break;
        case MT_TYPE_UCHAR:
            return (double) VOID_POINTER_TO_VECTOR_CAST(unsigned char, pdata, i);  break;
        case MT_TYPE_STRING:
        {
            float val = 0;
            string sval = VOID_POINTER_TO_VECTOR_CAST(string, pdata, i);
            sscanf(sval.c_str(), "%f", &val);
            return val;
            break;
        }
        case MT_TYPE_FLOAT:
            return (double) VOID_POINTER_TO_VECTOR_CAST(float, pdata, i);  break;
        case MT_TYPE_DOUBLE:
            return (double) VOID_POINTER_TO_VECTOR_CAST(double, pdata, i);  break;
        default:
            return 0;
        }
    }
    catch(exception& e)
    {
        return 0;
    }
  
}

string MT_VectorDataElement::GetStringValue(unsigned int i) const
{
  
    char cresult[100];
    string sresult;
  
    if(m_Type == MT_TYPE_STRING)
    {
        try
        {
            return VOID_POINTER_TO_VECTOR_CAST(string, pdata, i);
        }
        catch(exception &e)
        {
            return "error";
        }
    }
  
    double v = GetNumericValue(i);
  
    switch(m_Type)
    {
    case MT_TYPE_BOOL:
        sprintf(cresult, "%s", (i > 0) ? "true" : "false"); break;
    case MT_TYPE_INT:
    case MT_TYPE_UINT:
    case MT_TYPE_LONG:
        sprintf(cresult, "%.0f", v); break;
    case MT_TYPE_CHAR:
    case MT_TYPE_UCHAR:
        sprintf(cresult, "%c", (char) v); break;
    case MT_TYPE_FLOAT:
    case MT_TYPE_DOUBLE:
        sprintf(cresult, "%f", v); break;
    default:
        sprintf(cresult, "%s", "Error."); break;
    }
    sresult = cresult;
    return sresult;
  
}

unsigned int MT_VectorDataElement::GetVectorSize() const
{
  
    try
    {
        switch(m_Type)
        {
        case MT_TYPE_BOOL:
        {
            GET_VOID_POINTER_TO_VECTOR_CAST(v, bool, pdata);
            return v->size();
            break;
        }
        case MT_TYPE_INT:
        {
            GET_VOID_POINTER_TO_VECTOR_CAST(v, int, pdata);
            return v->size();
            break;
        }
        case MT_TYPE_UINT:
        {
            GET_VOID_POINTER_TO_VECTOR_CAST(v, unsigned int, pdata);
            return v->size();
            break;
        }
        case MT_TYPE_LONG:
        {
            GET_VOID_POINTER_TO_VECTOR_CAST(v, long, pdata);
            return v->size();
            break;
        }
        case MT_TYPE_CHAR:
        {
            GET_VOID_POINTER_TO_VECTOR_CAST(v, char, pdata);
            return v->size();
            break;
        }
        case MT_TYPE_UCHAR:
        {
            GET_VOID_POINTER_TO_VECTOR_CAST(v, unsigned char, pdata);
            return v->size();
            break;
        }
        case MT_TYPE_STRING:
        {
            GET_VOID_POINTER_TO_VECTOR_CAST(v, string, pdata);
            return v->size();
            break;
        }
        case MT_TYPE_FLOAT:
        {
            GET_VOID_POINTER_TO_VECTOR_CAST(v, float, pdata);
            return v->size();
            break;
        }
        case MT_TYPE_DOUBLE:
        {
            GET_VOID_POINTER_TO_VECTOR_CAST(v, double, pdata);
            return v->size();
            break;
        }
        default:
            return 0;
            break;
        }
    }
    catch(exception& e)
    {
        return 0;
    }
}

MT_DataGroup::MT_DataGroup(const string name)
{
    m_sName = name;
    m_vDataElements.resize(0);
}

void MT_DataGroup::AddBool(const string name, bool* ptr, bool readonly)
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_BOOL, readonly, 0, 1));
}

void MT_DataGroup::AddInt(const string name,
                       int* ptr,
                       bool readonly,
                       int min_val,
                       int max_val)
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_INT, readonly, min_val, max_val));
}

void MT_DataGroup::AddUInt(const string name, 
                        unsigned int* ptr, 
                        bool readonly, 
                        int min_val, 
                        int max_val)
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_UINT, readonly, min_val, max_val));
}

void MT_DataGroup::AddLong(const string name, 
                        long* ptr, 
                        bool readonly, 
                        long min_val, 
                        long max_val)
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_LONG, readonly, min_val, max_val));
}

void MT_DataGroup::AddChar(const string name, 
                        char* ptr, 
                        bool readonly, 
                        char min_val, 
                        char max_val)
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_CHAR, readonly, min_val, max_val));
}

void MT_DataGroup::AddUChar(const string name, 
                         unsigned char* ptr, 
                         bool readonly, 
                         unsigned char min_val, 
                         unsigned char max_val)
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_UCHAR, readonly, min_val, max_val));
}

void MT_DataGroup::AddString(const string name, 
                          string* ptr, 
                          bool readonly) 
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_STRING, readonly));
}

void MT_DataGroup::AddFloat(const string name, 
                         float* ptr, 
                         bool readonly, 
                         float min_val, 
                         float max_val)
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_FLOAT, readonly, min_val, max_val));
}

void MT_DataGroup::AddDouble(const string name, 
                          double* ptr, 
                          bool readonly, 
                          double min_val, 
                          double max_val)
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_DOUBLE, readonly, min_val, max_val));
}

void MT_DataGroup::AddColor(const string name,
                         MT_Color* ptr,
                         bool readonly)
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_COLOR, readonly));
}

void MT_DataGroup::AddChoice(const string name,
                          MT_Choice* ptr,
                          bool readonly)
{
    m_vDataElements.push_back(MT_DataElement(name, ptr, MT_TYPE_CHOICE, readonly));
}

std::vector<string> MT_DataGroup::GetStringValueArray() const
{
    std::vector<string> result;
    result.resize(0);
  
    for(unsigned int i = 0; i < GetGroupSize(); i++)
    {
        result.push_back(GetStringValue(i));
    }
    return result;
}

string MT_DataGroup::GetStringValue(unsigned int i) const
{
    if(i < 0 || i >= GetGroupSize())
    {
        return "error";
    }
    else
    {
        return m_vDataElements[i].GetStringValue();
    }
}

std::vector<string> MT_DataGroup::GetNameStringArray() const
{
  
    std::vector<string> result;
    result.resize(0);
  
    for(unsigned int i = 0; i < GetGroupSize(); i++)
    {
        result.push_back(GetNameString(i));
    }
    return result;
  
}

string MT_DataGroup::GetNameString(unsigned int i) const
{
    if(i < 0 || i >= GetGroupSize())
    {
        return "error";
    }
    else
    {
        return m_vDataElements[i].Name;
    }
}

MT_TYPE MT_DataGroup::GetDataType(unsigned int i) const
{
  
    if(i < 0 || i >= GetGroupSize())
    {
        return MT_TYPE_ERR;
    }
    else
    {
        return m_vDataElements[i].GetType();
    }
  
}

bool MT_DataGroup::GetReadOnly(unsigned int i) const
{
  
    if(i < 0 || i >= GetGroupSize())
    {
        return true;
    }
    else
    {
        return m_vDataElements[i].GetReadOnly();
    }
  
}

void MT_DataGroup::ClampValue(unsigned int i)
{
  
    if(i < 0 || i >= GetGroupSize())
    {
        return;
    }
    else
    {
        m_vDataElements[i].ClampValue();
    }
  
}

double MT_DataGroup::SetDoubleValue(unsigned int i, double val)
{ 
  
    if(i < 0 || i >= GetGroupSize())
    {
        return 0;
    }
    else
    {
        return m_vDataElements[i].SetDoubleValue(val);
    }
  
}

string MT_DataGroup::SetStringValue(unsigned int i, string sval)
{
    if(i < 0 || i >= GetGroupSize())
    {
        return "error";
    }
    else
    {
        return m_vDataElements[i].SetStringValue(sval);
    }
  
}

int MT_DataGroup::GetIndexByName(string name) const
{
    for(unsigned int i = 0; i < GetGroupSize(); i++)
    {
        if(GetNameString(i) == name)
        {
            return i;
        }
    }
  
    return GetGroupSize();
}

string MT_DataGroup::SetStringValueByName(string name, string sval)
{
    return SetStringValue(GetIndexByName(name), sval);
}

void* MT_DataGroup::GetPointer(unsigned int i) const
{
    if(i < 0 || i >= GetGroupSize())
    {
        i = 0;
    }
    return m_vDataElements[i].GetPointer();
}

bool MT_DataGroup::GetDialogIgnore(unsigned int i) const
{
    if(i < 0 || i >= GetGroupSize())
    {
        i = 0;
    }
    return m_vDataElements[i].GetDialogIgnore();
}

void MT_DataGroup::SetDialogIgnore(unsigned int i, bool value)
{
    if(i < 0 || i >= GetGroupSize())
    {
        return;
    }
    m_vDataElements[i].SetDialogIgnore(value);
}

void MT_VectorDataGroup::AddBool(const string name, std::vector<bool>* ptr)
{
    m_vDataElements.push_back(MT_VectorDataElement(name, ptr, MT_TYPE_BOOL));
}
void MT_VectorDataGroup::AddInt(const string name, std::vector<int>* ptr)
{
    m_vDataElements.push_back(MT_VectorDataElement(name, ptr, MT_TYPE_INT));
}
void MT_VectorDataGroup::AddUInt(const string name, std::vector<unsigned int>* ptr)
{
    m_vDataElements.push_back(MT_VectorDataElement(name, ptr, MT_TYPE_UINT));
}
void MT_VectorDataGroup::AddLong(const string name, std::vector<long>* ptr)
{
    m_vDataElements.push_back(MT_VectorDataElement(name, ptr, MT_TYPE_LONG));
}
void MT_VectorDataGroup::AddChar(const string name, std::vector<char>* ptr)
{
    m_vDataElements.push_back(MT_VectorDataElement(name, ptr, MT_TYPE_CHAR));
}
void MT_VectorDataGroup::AddUChar(const string name, std::vector<unsigned char>* ptr)
{
    m_vDataElements.push_back(MT_VectorDataElement(name, ptr, MT_TYPE_UCHAR));
}
void MT_VectorDataGroup::AddString(const string name, std::vector<string>* ptr)
{
    m_vDataElements.push_back(MT_VectorDataElement(name, ptr, MT_TYPE_STRING));
}
void MT_VectorDataGroup::AddFloat(const string name, std::vector<float>* ptr)
{
    m_vDataElements.push_back(MT_VectorDataElement(name, ptr, MT_TYPE_FLOAT));
}
void MT_VectorDataGroup::AddDouble(const string name, std::vector<double>* ptr)
{
    m_vDataElements.push_back(MT_VectorDataElement(name, ptr, MT_TYPE_DOUBLE));
}

string MT_VectorDataGroup::GetStringValue(unsigned int array_index, unsigned int data_index) const
{
    if(array_index >= m_vDataElements.size())
    {
        return "error";
    }
  
    return m_vDataElements[array_index].GetStringValue(data_index);
}

std::vector<string> MT_VectorDataGroup::GetStringValueArray() const
{
    std::vector<string> result;
    result.resize(0);
  
    /* note we assume each vector has the same size, or at least that the
       first one is the longest */
    for(unsigned int i = 0; i < m_vDataElements[0].GetVectorSize(); i++)
    {
        for(unsigned int j = 0; j < m_vDataElements.size(); j++)
        {
            result.push_back(m_vDataElements[j].GetStringValue(i));
        }
    }
    return result;
}

std::vector<string> MT_VectorDataGroup::GetNameStringArray() const
{
    std::vector<string> result;
    result.resize(0);
  
    for(unsigned int i = 0; i < m_vDataElements.size(); i++)
    {
        result.push_back(m_vDataElements[i].Name);
    }
  
    return result;
}
