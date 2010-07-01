#ifndef WXSUPPORT_H
#define WXSUPPORT_H

/** @file
 *  wxSupport.h
 *
 *  Various support functions used in wx apps.  
 *
 *  Created by Daniel Swain on 1/5/09.
 *
 */

/* Standard WX include block */
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
/* End standard WX include block */

#include <vector>
#include <string>
using namespace std;

const bool MT_CONFIRM_OVERWRITE = true;
const bool MT_NO_CONFIRM_OVERWRITE = false;

/** Function to Validate a wxTextCtrl's string to a numeric value.  Automatically updates 
    text string and maintains insertion point.  See MT_validate_number for actual validation algorithm. */
void MT_ValidateTextCtrlNumeric(wxTextCtrl* TextBox);

/** Same as MT_ValidateTextCtrlNumeric(wxTextCtrl* TextBox) except for integers. */
void MT_ValidateTextCtrlInteger(wxTextCtrl* TextBox);

/** @val const bool MT_WITH_DECIMAL
 * Supply to MT_validate_number to allow a decimal point. */
const bool MT_WITH_DECIMAL = true;
/** @val const bool MT_NO_DECIMAL
 * Supply to MT_validate_number to disallow a decimal point. */
const bool MT_NO_DECIMAL = false;
/** Numerical string validation algorithm - version that returns a wxString.  Accounts for negative sign 
    and single decimal point.  No engineering notation yet, though this shouldn't be too hard to implement.
    @param InText Text to be validated
    @param with_decimal Determines whether a decimal point is allowed 
    (MT_WITH_DECIMAL) or not (MT_NO_DECIMAL) */
wxString MT_validate_number(const wxString& InText, bool with_decmial = MT_WITH_DECIMAL);

/** Function to convert an unsigned int to a bit string wxString - i.e. 42 -> "00101010".  Default
    number of bits = 8. */
wxString MT_UIntToBitString(unsigned int bits, unsigned int Nbits = 8);

/** Function to remove all instances of wxString "stringtoremove" from wxArrayString stringarray.
    No error if the string is not found.  Useful for managing combo box choice lists. 
    Returns true if the string was removed (i.e. stringarray did contain stringtoremove).*/
bool MT_SafeRemoveString(wxArrayString* stringarray, const wxString& stringtoremove);

/** Function to add wxString "stringtoadd" to wxArrayString "stringarray", but only if it is
    not already contained in stringarray.  Adds at end, does not remove pre-existing duplicates. 
    Returns true if the string is added (i.e. stringarray does not already contain stringtoadd).*/
bool MT_UniqueAddString(wxArrayString* stringarray, const wxString& stringtoadd);

/** Function to limit the value in a wxTextCtrl to within double limits min_val and max_val.  Sets the text 
    in the wxTextCtrl to the limited value.  NOTE that if the text control's string is not a valid floating 
    point number, this returns zero. */
double MT_ClampTextCtrlFloat(wxTextCtrl* TextBox, double min_val, double max_val, const char* format = "%lf");

/** Same as MT_ClampTextCtrlFloat but for an Int value. */
int MT_ClampTextCtrlInt(wxTextCtrl* TextBox, int min_val, int max_val, const char* format = "%d");

/** Function to get an existing file from a file selection dialog.  Returns wxID_OK if the user
 * selected a file and pressed OK.
 *
 * @param parent Parent window.  Usually pass "this".
 * @param StartDirectory The directory to start selection in. 
 * @param Prompt Prompt to display, e.g. "Select a file..."
 * @param FiletypeFilter File types to enable, e.g. "Movie files (*.avi; *.mov;)|*.avi;*.mov;"
 * @param Path Pointer to the string where we should store the full path of the file selected.
 * @param Directory Pointer to the string where we should store the directory of the 
 *              selected file. */
int MT_OpenFileDialog(wxWindow* parent,
                   const wxString& StartDirectory,
                   const wxString& Prompt,
                   const wxString& FiletypeFilter,
                   wxString* Path,
                   wxString* Directory);

/** Function to get a possibly new file from a file selection dialog.  Returns wxID_OK if 
 * the user selected a file and pressed OK.
 *
 * @param parent Parent window.  Usually pass "this".
 * @param StartDirectory The directory to start selection in. 
 * @param Prompt Prompt to display, e.g. "Select a file..."
 * @param FiletypeFilter File types to enable, e.g. "Movie files (*.avi; *.mov;)|*.avi;*.mov;"
 * @param Path Pointer to the string where we should store the full path of the file selected.
 * @param Directory Pointer to the string where we should store the directory of the 
 *              selected file. 
 * @param confirm_overwrite Pass MT_CONFIRM_OVERWRITE (true) if the dialog should prompt
 *              when the selected file exists.  Omit or pass MT_NO_CONFIRM_OVERWRITE
 *              (false, default value) not to do this. */
int MT_SaveFileDialog(wxWindow* parent,
                   const wxString& StartDirectory,
                   const wxString& Prompt,
                   const wxString& FiletypeFilter,
                   wxString* Path,
                   wxString* Directory,
                   bool confirm_overwrite = MT_NO_CONFIRM_OVERWRITE);

int MT_SelectDirectoryDialog(wxWindow* parent,
                          const wxString& StartDirectory,
                          const wxString& Prompt,
                          wxString* Directory);
    
/** Function to show a generic error dialog */
void MT_ShowErrorDialog(wxWindow* parent, const wxString& message);
void MT_ShowMessageDialog(wxWindow* parent, const wxString& message);

/** Function to convert an STL vector of strings to a wxArrayString */
wxArrayString MT_StringVectorToWxArrayString(std::vector<string> strings);

/** Replace spaces in a string with another character */
wxString MT_ReplaceSpaces(wxString string_to_fix, const char* replace_char = "_");

/** Replace a given character in a string with a space */
wxString MT_CharToSpace(wxString string_to_fix, const char* replace_char);

/** Function to get a resource file:
    - on OS X this searches the bundle,
    - on other platforms this searches the application directory */
wxString MT_GetApplicationResourcePath(wxString filename, wxString subdir);

/** Function to read a text file into a wxString.  Returns an error message (in the string) if
    there was an error reading from the file. */
wxString MT_wxStringFromTextFile(wxString filename);

/** Function to get a wxSize that will fit on the screen.  Divides DesiredSize by 2 until 
    it is smaller than the screen, then returns the result. */
wxSize MT_FitSizeToScreen(const wxSize& DesiredSize);

void MT_GetAbsolutePath(const wxString& input, wxString* abs_path, wxString* abs_dir = NULL); 

bool MT_isValidFilePath(const wxString& path, const wxArrayString& acceptable_formats = wxArrayString());

wxString MT_StringToWxString(const std::string& instring);

#endif // WXSUPPORT_H
