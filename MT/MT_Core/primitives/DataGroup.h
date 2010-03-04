#ifndef DATAGROUP_H
#define DATAGROUP_H

/** @addtogroup MT_Core
 * @{
 */

/** @file
 *  DataGroup.h
 *
 *  @brief Helper classes for keeping track of user-modifiable data.
 *
 *
 *  MT_VectorDataGroup is an extension of MT_DataGroup, except that it
 *  holds std::vector containers of the same data types.
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
/** Indicates that data can be read or written. */
#define MT_DATA_READWRITE false
/** Indicates that data is read-only. */
#define MT_DATA_READONLY true

/** Types available for MT_DataGroup and MT_VectorDataGroup */
typedef enum
{
    MT_TYPE_ERR,       /**< Indicates an error. */
    MT_TYPE_BOOL,      /**< Boolean data. */
    MT_TYPE_INT,       /**< Signed integer. */
    MT_TYPE_UINT,      /**< Unsigned integer. */
    MT_TYPE_LONG,      /**< Signed long integer. */
    MT_TYPE_CHAR,      /**< Signed character. */
    MT_TYPE_UCHAR,     /**< Unsigned character. */
    MT_TYPE_STRING,    /**< std::string */
    MT_TYPE_FLOAT,     /**< Float */
    MT_TYPE_DOUBLE,    /**< Double */
    MT_TYPE_COLOR,     /**< MT_Color */
    MT_TYPE_CHOICE     /**< MT_Choice */
} MT_TYPE; 

/* MT_DataElement is the basic storage unit for MT_DataGroup and the
 * base for the standard data unit of MT_VectorDataGroup.  It's not
 * documented in the dox because the user should not need to access
 * it.  There are no public access functions in the containers. */
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

/* See note for MT_DataElement. */
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

/** @class MT_DataGroup
 * 
 *  @brief Generic data container.
 *  
 *  MT_DataGroup was designed as a generic pointer container class so
 *  that pieces of data could be linked across multiple components of
 *  an application in a simple-to-implement way.  This is accomplished
 *  by storing a void pointer to the data along with a descriptor of
 *  the type of data contained.  There are generic input and output
 *  routines so that the data can be represented and modified in
 *  various ways.  Because of the way it's implemented, it cannot
 *  store arbitrary data types - only those whos "rules" are
 *  programmed into this module.  The types available are the ones
 *  defined in the enum MT_TYPE.
 *
 *  @see MT_TYPE
 *  @see MT_VectorDataGroup
 *  @see ReadDataGroupFromXML
 *  @see WriteDataGroupToXML
 *  @see MT_DataGroupDialog
 *  
 */
class MT_DataGroup
{
protected:
    /* Data Elements */
    std::vector<MT_DataElement> m_vDataElements;
    
    /* group properties */
    string m_sName;
    
public:
    /** The ctor takes a name for the group which is used for display
     * and file i/o purposes. */
    MT_DataGroup(const string name);
    virtual ~MT_DataGroup(){};

    /** Function to initialize data - intended for subclasses.  Does
     * nothing here. */
    virtual void InitializeData(){};
    
    /* functions to add data */
    /** Add a boolean value to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     */
    virtual void AddBool(const string name, bool* ptr, bool readonly = MT_DATA_READWRITE);
    /** Add a signed integer value to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     * @param min_val Minimum value to allow when setting value.
     * @param max_val Maximum value to allow when setting value.
     */
    virtual void AddInt(const string name, int* ptr, bool readonly = MT_DATA_READWRITE, int min_val = MT_min_int, int max_val = MT_max_int);
    /** Add an unsigned integer value to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     * @param min_val Minimum value to allow when setting value.
     * @param max_val Maximum value to allow when setting value.
     */
    virtual void AddUInt(const string name, unsigned int* ptr, bool readonly = MT_DATA_READWRITE, int min_val = 0, int max_val = MT_max_uint);
    /** Add a signed long integer value to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     * @param min_val Minimum value to allow when setting value.
     * @param max_val Maximum value to allow when setting value.
     */
    virtual void AddLong(const string name, long* ptr, bool readonly = MT_DATA_READWRITE, long min_val = MT_min_long, long max_val = MT_max_long);
    /** Add a signed character value to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     * @param min_val Minimum value to allow when setting value.
     * @param max_val Maximum value to allow when setting value.
     */
    virtual void AddChar(const string name, char* ptr, bool readonly = MT_DATA_READWRITE, char min_val = MT_min_char, char max_val = MT_max_char);
    /** Add an unsigned character value to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     * @param min_val Minimum value to allow when setting value.
     * @param max_val Maximum value to allow when setting value.
     */
    virtual void AddUChar(const string name, unsigned char* ptr, bool readonly = MT_DATA_READWRITE, unsigned char min_val = 0, unsigned char max_val = MT_max_uchar);
    /** Add a std::string to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     */
    virtual void AddString(const string name, string* ptr, bool readonly = MT_DATA_READWRITE);
    /** Add a float value to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     * @param min_val Minimum value to allow when setting value.
     * @param max_val Maximum value to allow when setting value.
     */
    virtual void AddFloat(const string name, float* ptr, bool readonly = MT_DATA_READWRITE, float min_val = MT_min_float, float max_val = MT_max_float);
    /** Add a double value to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     * @param min_val Minimum value to allow when setting value.
     * @param max_val Maximum value to allow when setting value.
     */
    virtual void AddDouble(const string name, double* ptr, bool readonly = MT_DATA_READWRITE, double min_val = MT_min_double, double max_val = MT_max_double);
    /** Add an MT_Color to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     */    
    virtual void AddColor(const string name, MT_Color* ptr, bool readonly = MT_DATA_READWRITE);
    /** Add an MT_Choice to the group.
     * @param name Display name
     * @param ptr Pointer to the data
     * @param readonly Flag whether the data is read-only (false by default)
     */  
    virtual void AddChoice(const string name, MT_Choice* ptr, bool readonly = MT_DATA_READWRITE);
    
    /* read-only accessor functions */
    /** Retrieve the value of every element as a vector of strings.
     * @see GetStringValue */
    virtual std::vector<string> GetStringValueArray() const;
    /** Retrieve the value of the i'th element as a string.  For
     * numeric values this is accomplished via sprintf with the
     * appropriate precision.  For bools the value is "true" or
     * "false".  For MT_Color the result is the HexString for the
     * color.  For MT_Choice the value is the string associated with
     * the currently selected choice. */
    virtual string GetStringValue(unsigned int i) const;
    /** Get a vector of the names of all elements. */
    virtual std::vector<string> GetNameStringArray() const;
    /** Get the name of the i'th element. */
    string GetNameString(unsigned int i) const;
    /** Get the name of the group. */
    string GetGroupName() const {return m_sName;};
    /** Get the number of elements currently in the group (i.e. the
     * number of times Add* was called. */
    unsigned int GetGroupSize() const {return m_vDataElements.size();};
    /** Get the MT_TYPE of the i'th element. */
    MT_TYPE GetDataType(unsigned int i) const;
    /** Get whether or not the i'th element is read-only. */
    bool GetReadOnly(unsigned int i) const;
    
    /** Force the value of the i'th element into the range that was
     * given during Add. */
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
    /** Set the numeric value of the i'th element, if possible.  For
     * standard numeric types this works as you'd expect using
     * standard casting rules.  For bool
     * the value is set to true if val > 0.  For strings the value is
     * the result of sprintf using "%f".  For MT_Choice the value is
     * set by index of the choice.  No action is taken for MT_Color.
     * The value is clamped according to the given limits.  The
     * returned value is the value after the operation is complete,
     * so if the value is clamped this reflects the clamped value or
     * if the variable is read-only this reflects the existing value
     * that was NOT overwritten.  Zero is returned if the index is out
     * of range.
     * @see SetStringValue */
    double SetDoubleValue(unsigned int i, double val);
    /** Similar to SetDoubleValue, except that the value is set
     * according to the provided string.  For numeric types a scanf
     * method is used.  For strings the operation is
     * straightforward.  For MT_Color the string is assumed to be a
     * hex value.  For MT_Choice a corresponding choice is searched
     * for.  For bools a string representing a number > 0 is taken as
     * true, or the (case-insensitive) strings "true" and "false" are
     * searched for.  As in SetDoubleValue, the returned string
     * represents the value after the operation.  "error" is returned
     * if the index is out of range.
     * @see SetDoubleValue */
    string SetStringValue(unsigned int i, string val);

    /** Searches for an element with a name matching the input and
     * returns the index of that element.  If none is found, the size
     * of the group is returned. */
    int GetIndexByName(string name) const;
    /** Uses GetIndexByName to find the index of an element and then
     * calls SetStringValue.  "error" is returned if the name is not
     * found.
     * @see GetIndexByName
     * @see SetStringValue */
    string SetStringValueByName(string name, string sval);

    /** Returns a void pointer to the actual data.  Use with caution! */
    void* GetPointer(unsigned int i) const;

    /** Returns the dialog ignore flag.  The flag is false by
     *  default.  This flag is intended to indicate whether a value
     *  should be displayed in an MT_DataGroupDialog, but could really
     *  be used for any purpose (as long as you don't want to use the
     *  dialog).
     *  @see MT_DataGroupDialog
     *  @see SetDialogIgnore */
    bool GetDialogIgnore(unsigned int i) const;
    /** Set the value of the dialog ignore flag.  The flag is false
     * by default.
     * @see MT_DataGroupDialog
     * @see GetDialogIgnore */
    void SetDialogIgnore(unsigned int i, bool value = true);
    
};

/** @class MT_VectorDataGroup
 *
 * @brief Vector version of MT_DataGroup.
 *
 * This is an adapted version of MT_DataGroup used to store
 * std::vector containers of the corresponding data types.  For
 * further documentation see the MT_DataGroup documentation.
 *
 * Take care here.  These are not the most robust operations.  They
 * work well where they're implemented in MT, but I'm not sure how
 * robust they are. 
 * 
 * Note MT_Color and MT_Choice are not implemented for vectors and
 * that the implementations here do not take clamping values or
 * read-only flags.  There's conceivably no reason this couldn't be
 * done, it's just not done *yet*.  Feel free :)
 *
 * @see MT_DataGroup
 */
class MT_VectorDataGroup : public MT_DataGroup
{
protected:
    std::vector<MT_VectorDataElement> m_vDataElements;

public:
    /** The ctor takes a name for the group. */
    MT_VectorDataGroup(const string name) : MT_DataGroup(name){};
    virtual ~MT_VectorDataGroup(){};

    /** See MT_DataGroup::AddBool */
    void AddBool(const string name, std::vector<bool>* ptr);
    /** See MT_DataGroup::AddInt */
    void AddInt(const string name, std::vector<int>* ptr);
    /** See MT_DataGroup::AddUInt */
    void AddUInt(const string name, std::vector<unsigned int>* ptr);
    /** See MT_DataGroup::AddLong */
    void AddLong(const string name, std::vector<long>* ptr);
    /** See MT_DataGroup::AddChar */
    void AddChar(const string name, std::vector<char>* ptr);
    /** See MT_DataGroup::AddUChar */
    void AddUChar(const string name, std::vector<unsigned char>* ptr);
    /** See MT_DataGroup::AddString */
    void AddString(const string name, std::vector<string>* ptr);
    /** See MT_DataGroup::AddFloat */
    void AddFloat(const string name, std::vector<float>* ptr);
    /** See MT_DataGroup::AddDouble */
    void AddDouble(const string name, std::vector<double>* ptr);

    /** Calls MT_DataGroup::GetStringValue for the array_index'th
     * vector in the group and the data_index'th element of that
     * vector.  Returns "error" if unsuccessful. */
    string GetStringValue(unsigned int array_index, unsigned int data_index) const;

    /** Builds an array of the results of GetStringValue for each
     * item in each vector.  Assumes that each vector has the same
     * length as the first one.  Shorter vectors will be padded with
     * "error"s.  The index of the output can be mapped as
     * (vector_index, element_index) -> (vector_index*GetGroupSize()
     * + element_index)
     * @see GetStringValue */
    std::vector<string> GetStringValueArray() const;
    /** Returns an array of strings containing the name of each
     * element. */
    std::vector<string> GetNameStringArray() const;

    /** Returns "error" as this operation should be undefined. */
    string GetStringValue(unsigned int i) const {return "error";};

};

typedef MT_VectorDataGroup MT_DataReport;

/** @} */

#endif // DATAGROUP_H
