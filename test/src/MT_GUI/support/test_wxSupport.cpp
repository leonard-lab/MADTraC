#include "MT_Test.h"
#include "MT_wxTest.h"

#include "MT/MT_GUI/support/wxSupport.h"

bool g_ShowOnlyErrors = true;

void VALIDATE_NUMBER_TEST(const wxString& input,
                          bool with_decimal,
                          const wxString& expected,
                          int* p_in_status)
{
    wxString output = MT_validate_number(input, with_decimal);
    if(output != expected)
    {
        *p_in_status = MT_TEST_ERROR;
        fprintf(stderr, "    With input %s, got %s, expected %s\n",
                (const char*) input.mb_str(),
                (const char*) output.mb_str(),
                (const char*) expected.mb_str());
    }
    else if(!g_ShowOnlyErrors)
    {
        fprintf(stdout, "    Successful test: %s -> %s\n",
                (const char*) input.mb_str(),
                (const char*) output.mb_str());
    }
}

void UINT_TO_BIT_STRING_TEST(unsigned int input,
                             unsigned int nbits,
                             const wxString& expected,
                             int *p_in_status)
{
    wxString output = MT_UIntToBitString(input, nbits);

    if(output != expected)
    {
        *p_in_status = MT_TEST_ERROR;
        fprintf(stderr, "    With input %d, got %s, expected %s\n",
                input, (const char*) output.mb_str(),
                (const char*) expected.mb_str());
    }
    else if(!g_ShowOnlyErrors)
    {
        fprintf(stdout, "    Successful test: %d (%d bits) -> %s\n",
                input,
                nbits,
                (const char*) output.mb_str());
    }
}

void SAFE_REMOVE_STRING_TEST(wxArrayString* stringarray,
                             const wxString& stringtoremove,
                             bool expected_result,
                             const wxArrayString& expected_array,
                             int* p_in_status)
{
    bool result = MT_SafeRemoveString(stringarray, stringtoremove);

    if(*stringarray != expected_array
        || result != expected_result)
    {
        *p_in_status = MT_TEST_ERROR;
//        fprintf(stderr, "
    }
}

int main(int argc, char** argv)
{
    WX_CONSOLE_APP_INIT;

    if(argc > 1)
    {
        g_ShowOnlyErrors = false;
    }

    int status = MT_TEST_SUCCESS;

    MT_TEST_START("MT_validate_number");

    VALIDATE_NUMBER_TEST(wxT("1.234"),     true,  wxT("1.234"),  &status);
    VALIDATE_NUMBER_TEST(wxT("1.234"),     false, wxT("1234"),   &status);    
    VALIDATE_NUMBER_TEST(wxT("-1.234"),    true,  wxT("-1.234"), &status);
    VALIDATE_NUMBER_TEST(wxT("-1.234"),    false, wxT("-1234"),  &status);
    VALIDATE_NUMBER_TEST(wxT("1a3345"),    true,  wxT("13345"),  &status);
    VALIDATE_NUMBER_TEST(wxT("abc123"),    true,  wxT("123"),    &status);
    VALIDATE_NUMBER_TEST(wxT("-a1"),       true,  wxT("-1"),     &status);
    VALIDATE_NUMBER_TEST(wxT("a.a1"),      true,  wxT(".1"),     &status);
    VALIDATE_NUMBER_TEST(wxT(".-1"),       true,  wxT(".1"),     &status);
    VALIDATE_NUMBER_TEST(wxT("1."),        true,  wxT("1."),     &status);
    VALIDATE_NUMBER_TEST(wxT("."),         true,  wxT("."),      &status);
    VALIDATE_NUMBER_TEST(wxT("-"),         true,  wxT("-"),      &status);
    VALIDATE_NUMBER_TEST(wxT("-."),        true,  wxT("-."),     &status);
    VALIDATE_NUMBER_TEST(wxT(".-"),        true,  wxT("."),      &status);            
    VALIDATE_NUMBER_TEST(wxT("..."),       true,  wxT("."),      &status);
    VALIDATE_NUMBER_TEST(wxT("---"),       true,  wxT("-"),      &status);
    VALIDATE_NUMBER_TEST(wxT("---...---"), true,  wxT("-."),     &status);    
    VALIDATE_NUMBER_TEST(wxT("abc"),       true,  wxT(""),       &status);
    VALIDATE_NUMBER_TEST(wxT("-q1a.b3df"), true,  wxT("-1.3"),   &status);

    MT_TEST_START("MT_UIntToBitString");

    UINT_TO_BIT_STRING_TEST(0, 8, wxT("00000000"), &status);
    UINT_TO_BIT_STRING_TEST(0, 4, wxT("0000"), &status);
    UINT_TO_BIT_STRING_TEST(42, 8, wxT("00101010"), &status);
    UINT_TO_BIT_STRING_TEST((unsigned int) -1, 2, wxT("11"), &status);
    UINT_TO_BIT_STRING_TEST(3, 1, wxT("1"), &status);

    return status;
}
