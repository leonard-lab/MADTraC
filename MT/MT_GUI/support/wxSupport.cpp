/*
 *  wxSupport.cpp
 *
 *  Created by Daniel Swain on 1/5/09.
 *
 */

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h> /* used to find application support paths */
#endif

#include "wxSupport.h"

#include <wx/filename.h>

void MT_ValidateTextCtrlNumeric(wxTextCtrl* TextBox)
{
  
    unsigned int InsPoint = TextBox->GetInsertionPoint();
    wxString Text = TextBox->GetValue();
    wxString ValidatedText = MT_validate_number(Text);
  
    if(ValidatedText.Len() < Text.Len())
    {
        InsPoint--;
    }
  
    TextBox->SetValue(ValidatedText);
    TextBox->SetInsertionPoint(InsPoint);
}

void MT_ValidateTextCtrlInteger(wxTextCtrl* TextBox)
{
  
    unsigned int InsPoint = TextBox->GetInsertionPoint();
    wxString Text = TextBox->GetValue();
    wxString ValidatedText = MT_validate_number(Text, MT_NO_DECIMAL);
  
    if(ValidatedText.Len() < Text.Len())
    {
        InsPoint--;
    }
  
    TextBox->SetValue(ValidatedText);
    TextBox->SetInsertionPoint(InsPoint);
}

wxString MT_validate_number(const wxString& InText, bool with_decimal)
{
    wxString OutText;
  
    unsigned int i; int dec = 0;
  
    for (i = 0; i < InText.Len(); i++) {
        if (i == 0 && InText[i] == '-') { OutText += InText[i]; }
        if (isdigit(InText[i])) {OutText += InText[i];}
        if (with_decimal && InText[i] == '.' && !dec) {OutText += InText[i]; dec++;}
    }
  
    return OutText;
}

wxString MT_UIntToBitString(unsigned int bits, unsigned int Nbits)
{
    wxString OutText;
  
    for(unsigned int i = 0; i < Nbits; i++)
    {
        if(bits & (1 << i))
        {
            OutText = "1" + OutText;
        }
        else
        {
            OutText = "0" + OutText;
        }
    }
  
    return OutText;
}

bool MT_SafeRemoveString(wxArrayString* stringarray, const wxString& stringtoremove)
{
  
    bool stringremoved = false;

    for(unsigned int i = 0; i < stringarray->GetCount(); i++)
    {
        if(stringarray->Item(i) == stringtoremove)
        {
            stringarray->RemoveAt(i);
            stringremoved = true;
        }
    }

    return stringremoved;
  
}

bool MT_UniqueAddString(wxArrayString* stringarray, const wxString& stringtoadd)
{
  
    for(unsigned int i = 0; i < stringarray->GetCount(); i++)
    {
        if(stringarray->Item(i) == stringtoadd)
        {
            return false;  // found -> done so leave and say new string was not added
        }
    }
  
    stringarray->Add(stringtoadd);
    return true;  // new string added
  
}

double MT_ClampTextCtrlFloat(wxTextCtrl* TextBox, double min_val, double max_val, const char* format)
{
  
    double value = 0; 
    wxString ctrlstring = TextBox->GetValue();
  
    // ToDouble returns false if the string is not a number
    if(!ctrlstring.ToDouble(&value))
    {
        return 0;  // note - this could be dangerous, but usually we have valid input up front
    }
  
    if(value > max_val)
    {
        ctrlstring.Printf(format,max_val);
        TextBox->SetValue(ctrlstring);
        return max_val;
    }
  
    if(value < min_val)
    {
        ctrlstring.Printf(format,min_val);
        TextBox->SetValue(ctrlstring);
        return min_val;
    }
  
    return value;
}


int MT_ClampTextCtrlInt(wxTextCtrl* TextBox, int min_val, int max_val, const char* format)
{
  
    double value = 0;  // no wxString.ToFloat, only ToDouble, convert later..
    // this also allows us to round float input
    wxString ctrlstring = TextBox->GetValue();
  
    // ToDouble returns false if the string is not a number
    if(!ctrlstring.ToDouble(&value))
    {
        return 0;  // note - this could be dangerous, but usually we have valid input up front
    }
  
    int value_int = (int)( floor(value + 0.5));
  
    if(value > max_val)
    {
        ctrlstring.Printf(format,max_val);
        TextBox->SetValue(ctrlstring);
        return max_val;
    }
  
    if(value < min_val)
    {
        ctrlstring.Printf(format,min_val);
        TextBox->SetValue(ctrlstring);
        return min_val;
    }
  
    // if not clamped, make sure we get an int
    ctrlstring.Printf(format, value_int);
    TextBox->SetValue(ctrlstring);  
    return (float) value;
}

int MT_OpenFileDialog(wxWindow* parent,
                   const wxString& StartDirectory,
                   const wxString& Prompt,
                   const wxString& FiletypeFilter,
                   wxString* Path,
                   wxString* Directory)
{
    wxFileDialog* dlg = new wxFileDialog(parent, 
                                         Prompt,
                                         StartDirectory,
                                         wxEmptyString,
                                         FiletypeFilter,
                                         wxFD_FILE_MUST_EXIST);
    int result = dlg->ShowModal();
  
    if(result == wxID_OK)
    {
        *Directory = dlg->GetDirectory();
        *Path = dlg->GetPath();
    }
  
    dlg->Destroy();
    return result;
  
}

int MT_SaveFileDialog(wxWindow* parent,
                   const wxString& StartDirectory,
                   const wxString& Prompt,
                   const wxString& FiletypeFilter,
                   wxString* Path,
                   wxString* Directory,
                   bool confirm_overwrite)
{
    long style = wxFD_SAVE;
    if(confirm_overwrite == MT_CONFIRM_OVERWRITE)
    {
        style = style | wxFD_OVERWRITE_PROMPT;
    }

    wxFileDialog* dlg = new wxFileDialog(parent, 
                                         Prompt,
                                         StartDirectory,
                                         wxEmptyString,
                                         FiletypeFilter,
                                         wxFD_SAVE);
    int result = dlg->ShowModal();
  
    if(result == wxID_OK)
    {
        *Directory = dlg->GetDirectory();
        *Path = dlg->GetPath();
    }
  
    dlg->Destroy();
    return result;
}

int MT_SelectDirectoryDialog(wxWindow* parent,
                          const wxString& StartDirectory,
                          const wxString& Prompt,
                          wxString* Directory)
{
    wxDirDialog* dlg = new wxDirDialog(parent,
                                       Prompt,
                                       StartDirectory,
                                       wxDD_DEFAULT_STYLE);
    int result = dlg->ShowModal();

    if(result == wxID_OK)
    {
        *Directory = dlg->GetPath();
    }
    dlg->Destroy();
    return result;
}

void MT_ShowErrorDialog(wxWindow* parent, const wxString& message)
{
  
    wxMessageDialog* dlg = new wxMessageDialog(parent,
                                               message,
                                               wxT("Error."),
                                               wxOK);
    dlg->ShowModal();
    dlg->Destroy();

}

void MT_ShowMessageDialog(wxWindow* parent, const wxString& message)
{
  
    wxMessageDialog* dlg = new wxMessageDialog(parent,
                                               message,
                                               wxTheApp->GetAppName(),
                                               wxOK);
    dlg->ShowModal();
    dlg->Destroy();

}

wxArrayString MT_StringVectorToWxArrayString(std::vector<string> strings)
{
    wxArrayString result;
    result.Empty();
  
    for(unsigned int i = 0; i < strings.size(); i++)
    {
        result.Add(strings[i]);
    }
  
    return result;
}

wxString MT_ReplaceSpaces(wxString string_to_fix, const char* replace_char)
{
    char* space_char = " ";
    wxString return_val = string_to_fix;
    return_val.Replace(space_char, replace_char);
    return return_val;
}

wxString MT_CharToSpace(wxString string_to_fix, const char* replace_char)
{
    char* space_char = " ";
    wxString return_val = string_to_fix;
    return_val.Replace(replace_char, space_char);
    return return_val;
}

wxString MT_GetApplicationResourcePath(wxString filename, wxString subdir)
{
#ifdef __APPLE__
    wxString extension = filename.AfterLast('.');
    wxString root = filename.BeforeLast('.');
  
#define MAX_PATH_LENGTH 512  /* could be set larger */
    UInt8 ui_path[MAX_PATH_LENGTH];
    ui_path[0] = '\0';
  
    CFURLRef ref = CFBundleCopyResourceURL(CFBundleGetMainBundle(),
                                           CFStringCreateWithCString(NULL, root.c_str(), kCFStringEncodingMacRoman),
                                           CFStringCreateWithCString(NULL, extension.c_str(), kCFStringEncodingMacRoman),
                                           CFStringCreateWithCString(NULL, subdir.c_str(), kCFStringEncodingMacRoman) );
    if(CFURLGetFileSystemRepresentation (ref,
                                         true,
                                         ui_path,
                                         MAX_PATH_LENGTH))
    {
        wxString result;
        unsigned int i = 0;
        while((i < MAX_PATH_LENGTH) && (ui_path[i] != '\0'))
        {
            result = result + (char) ui_path[i++];
        }    
        return result;
    }
    else
    {
        return wxT("");
    }
  
#else  // not apple
  
    return subdir + wxFileName::GetPathSeparator() + filename;
  
#endif
  
}

wxString MT_wxStringFromTextFile(wxString filename)
{
    FILE* fp = fopen(filename.c_str(),"r");
    if(!fp)
    {
        return wxT("Error opening or reading from ") + filename + wxT("\n");
    }
  
    wxString result = wxT("");
  
    char t;
    while(!feof(fp))
    {
        fscanf(fp, "%c", &t);
        result = result + t;
    }
  
    return result;
  
}

wxSize MT_FitSizeToScreen(const wxSize& DesiredSize)
{
    wxSize result = DesiredSize;
  
    /* we want to make sure the window is not larger than the screen */
    wxSize ScreenSize = wxSize(wxSystemSettings::GetMetric(wxSYS_SCREEN_X), 
                               wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
    
    /* if the desired size is bigger than the screen, halve it until it fits */
    while((result.x >=  ScreenSize.x) && (result.y >= ScreenSize.y))
    {
        result /= 2;
    }
  
    return result;
}

void MT_GetAbsolutePath(const wxString& input, wxString* abs_path, wxString* abs_dir)
{
    if(wxIsAbsolutePath(input))
    {
        *abs_path = input;
    }
    else
    {
        *abs_path = wxGetCwd() + input;
    }
    if(abs_dir)
    {
        *abs_dir = wxPathOnly(*abs_path);
    }
}

bool MT_isValidFilePath(const wxString& path, const wxArrayString& acceptable_formats)
{
    wxFileName _path(path);
    
    if(!_path.IsOk())
    {
        return false;
    }

    return true;
}
