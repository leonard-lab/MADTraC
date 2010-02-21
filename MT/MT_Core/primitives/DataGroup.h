#ifndef DATAGROUP_H
#define DATAGROUP_H

/*
 *  DataGroup.h
 *
 *  Created by Daniel Swain on 10/21/09.
 *
 */

#include <vector>
#include <string>
using namespace std;

#include "MT/MT_Core/support/mathsupport.h"  /* for limits */
#include "MT/MT_Core/primitives/Color.h"
#include "MT/MT_Core/primitives/Choice.h"


/* readable flags for readonly */
#define MT_DATA_READWRITE false
#define MT_DATA_READONLY true

typedef enum
{
    MT_TYPE_ERR,      // returned in case of error
    MT_TYPE_BOOL,
    MT_TYPE_INT,
    MT_TYPE_UINT,
    MT_TYPE_LONG,
    MT_TYPE_CHAR,
    MT_TYPE_UCHAR,
    MT_TYPE_STRING,
    MT_TYPE_FLOAT,
    MT_TYPE_DOUBLE,
    MT_TYPE_COLOR,
    MT_TYPE_CHOICE
} MT_TYPE;

class MT_DataElement
{
protected:
    void* pdata;
    double Min_val;
    double Max_val;
    MT_TYPE m_Type;
    bool ReadOnly;
    bool DialogIgnore;
public:
    string Name;
    
    MT_DataElement(const string& name, 
                void* const p, 
                MT_TYPE t,
                bool isreadonly = MT_DATA_READWRITE,
                double min_val = MT_min_double,
                double max_val = MT_max_double);
    
    MT_DataElement();
    virtual ~MT_DataElement(){}; 
    
    virtual double GetNumericValue() const;
    virtual string GetStringValue() const;
    
    virtual void ClampValue();
    
    virtual double SetDoubleValue(double val);
    virtual string SetStringValue(string sval);
    
    bool GetReadOnly() const {return ReadOnly;};
    
    MT_TYPE GetType() const {return m_Type;};
    
    void* GetPointer() const {return pdata;};

    bool GetDialogIgnore() const {return DialogIgnore;};
    void SetDialogIgnore(bool value = true){DialogIgnore = value;};
};

class MT_VectorDataGroup;

class MT_VectorDataElement : public MT_DataElement
{
private:
public:
    MT_VectorDataElement(const string& name, 
                      void* const p, 
                      MT_TYPE t, 
                      double min_val = MT_min_double, 
                      double max_val = MT_max_double);
    MT_VectorDataElement();
    virtual ~MT_VectorDataElement(){};
    
    double GetNumericValue(unsigned int i) const;
    string GetStringValue(unsigned int i) const;
    
    unsigned int GetVectorSize() const;
    
    /* should do nothing */
    void ClampValue(){};
    
    /* should return an error */
    double GetNumericValue() const {return -1;};
    string GetStringValue() const {return "error";};
    double SetDoubleValue(double val){return -1;};
    string SetStringValue(string sval){return "error";}; 

};

class MT_DataGroup
{
protected:
    /* Data Elements */
    std::vector<MT_DataElement> m_vDataElements;
    
    /* group properties */
    string m_sName;
    
public:
    /* ctor resizes arrays to zero */
    MT_DataGroup(const string name);
    virtual ~MT_DataGroup(){};
    
    /* Function to initialize data - can
       get overridden by subclasses */
    virtual void InitializeData(){};
    
    /* functions to add data */
    virtual void AddBool(const string name, bool* ptr, bool readonly = MT_DATA_READWRITE);
    virtual void AddInt(const string name, int* ptr, bool readonly = MT_DATA_READWRITE, int min_val = MT_min_int, int max_val = MT_max_int);
    virtual void AddUInt(const string name, unsigned int* ptr, bool readonly = MT_DATA_READWRITE, int min_val = 0, int max_val = MT_max_uint);
    virtual void AddLong(const string name, long* ptr, bool readonly = MT_DATA_READWRITE, long min_val = MT_min_long, long max_val = MT_max_long);
    virtual void AddChar(const string name, char* ptr, bool readonly = MT_DATA_READWRITE, char min_val = MT_min_char, char max_val = MT_max_char);
    virtual void AddUChar(const string name, unsigned char* ptr, bool readonly = MT_DATA_READWRITE, unsigned char min_val = 0, unsigned char max_val = MT_max_uchar);
    virtual void AddString(const string name, string* ptr, bool readonly = MT_DATA_READWRITE);
    virtual void AddFloat(const string name, float* ptr, bool readonly = MT_DATA_READWRITE, float min_val = MT_min_float, float max_val = MT_max_float);
    virtual void AddDouble(const string name, double* ptr, bool readonly = MT_DATA_READWRITE, double min_val = MT_min_double, double max_val = MT_max_double);    
    virtual void AddColor(const string name, MT_Color* ptr, bool readonly = MT_DATA_READWRITE);
    virtual void AddChoice(const string name, MT_Choice* ptr, bool readonly = MT_DATA_READWRITE);
    
    /* read-only accessor functions */
    virtual std::vector<string> GetStringValueArray() const;
    virtual string GetStringValue(unsigned int i) const;
    virtual std::vector<string> GetNameStringArray() const;
    string GetNameString(unsigned int i) const;
    string GetGroupName() const {return m_sName;};
    unsigned int GetGroupSize() const {return m_vDataElements.size();};
    MT_TYPE GetDataType(unsigned int i) const;
    bool GetReadOnly(unsigned int i) const;
    
    /* function to force values into range */
    void ClampValue(unsigned int i);
    
    /* Value setting functions
     * NOTE these are indexed in the overall index set
     * i.e. 0 to GetGroupSize()-1, so it's up to you
     * to make sure the value types are OK... 
     * NOTE only double and int functions are provided,
     * since this should be casting safe..
     * NOTE these functions have return values - they
     * will return the outgoing value of the variable -
     * i.e. if the value is clamped or read-only */
    double SetDoubleValue(unsigned int i, double val);
    string SetStringValue(unsigned int i, string val);
    
    int GetIndexByName(string name) const;
    string SetStringValueByName(string name, string sval);
    
    void* GetPointer(unsigned int i) const;

    bool GetDialogIgnore(unsigned int i) const;
    void SetDialogIgnore(unsigned int i, bool value = true);
    
};

class MT_VectorDataGroup : public MT_DataGroup
{
protected:
    std::vector<MT_VectorDataElement> m_vDataElements;

public:
MT_VectorDataGroup(const string name) : MT_DataGroup(name){};
    virtual ~MT_VectorDataGroup(){};
    
    void AddBool(const string name, std::vector<bool>* ptr);
    void AddInt(const string name, std::vector<int>* ptr);
    void AddUInt(const string name, std::vector<unsigned int>* ptr);
    void AddLong(const string name, std::vector<long>* ptr);
    void AddChar(const string name, std::vector<char>* ptr);
    void AddUChar(const string name, std::vector<unsigned char>* ptr);
    void AddString(const string name, std::vector<string>* ptr);
    void AddFloat(const string name, std::vector<float>* ptr);
    void AddDouble(const string name, std::vector<double>* ptr);
    
    string GetStringValue(unsigned int array_index, unsigned int data_index) const;
    
    std::vector<string> GetStringValueArray() const;
    std::vector<string> GetNameStringArray() const;

    string GetStringValue(unsigned int i) const {return "error";};

};

typedef MT_VectorDataGroup MT_DataReport;

#endif // DATAGROUP_H
