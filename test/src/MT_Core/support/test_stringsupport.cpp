#include <string>
#include <string.h>

#include "MT_Test.h"

#include "MT/MT_Core/support/stringsupport.h"

void FORMAT_STRING_TEST(std::string formatter,
                       MT_FORMAT_STRING_TYPE expect_format,
                       int expect_start,
                       int expect_stop,
                       int* p_in_status)
{

    MT_FORMAT_STRING_TYPE fs_type = MT_FORMAT_STRING_INVALID;
    int p_startof = -1;
    int p_endof = -1;

    fs_type = MT_hasFormatString(formatter.c_str(), &p_startof, &p_endof);

    if(fs_type != expect_format                    
       || p_startof != expect_start
       || p_endof != expect_stop)
    {
        *p_in_status = MT_TEST_ERROR;
        MT_TEST_ERROR_MESSAGE("MT_hasFormatString Error");                    
        fprintf(stderr, "    + With formatter %s gave fs_type %d, "           
                        "p_startof %d, p_endof %d\n",                         
                        formatter.c_str(), fs_type, p_startof, p_endof);      
        fprintf(stderr,
                "    + When expecting fs_type %d, "
                "p_startof %d, p_endof %d\n",
                expect_format, expect_start, expect_stop);
    }                                                                         
    
}

void IS_DIGIT_TEST(char c, bool expected_result, int* p_in_status)
{
    bool result = MT_isdigit(c);
    if(result != expected_result)
    {
        *p_in_status = MT_TEST_ERROR;
        fprintf(stderr,
                "    With char '%c' got '%s' when expecting '%s'\n",
                c, result ? "true" : "false",
                expected_result ? "true" : "false");
    }
}

int main(int argc, char** argv)
{
    int status = MT_TEST_SUCCESS;

    /**************************************************/
    MT_TEST_START("MT_hasFormatString");

    FORMAT_STRING_TEST("%03d", MT_FORMAT_STRING_INT, 0, 3, &status);
    FORMAT_STRING_TEST("Hello %s", MT_FORMAT_STRING_STRING, 6, 7, &status);
    FORMAT_STRING_TEST("frame%05d", MT_FORMAT_STRING_INT, 5, 8, &status);
    FORMAT_STRING_TEST("%lc", MT_FORMAT_STRING_CHAR, 0, 2, &status);
    FORMAT_STRING_TEST("% a stray percent", MT_FORMAT_STRING_INVALID, -1, -1, &status);
    FORMAT_STRING_TEST("a percent at the end %", MT_FORMAT_STRING_INVALID, -1, -1, &status);

    /**************************************************/    
    MT_TEST_START("MT_isdigit");

    IS_DIGIT_TEST('0', true, &status);
    IS_DIGIT_TEST('1', true, &status);
    IS_DIGIT_TEST('2', true, &status);
    IS_DIGIT_TEST('3', true, &status);
    IS_DIGIT_TEST('4', true, &status);
    IS_DIGIT_TEST('5', true, &status);
    IS_DIGIT_TEST('6', true, &status);
    IS_DIGIT_TEST('7', true, &status);
    IS_DIGIT_TEST('8', true, &status);
    IS_DIGIT_TEST('9', true, &status);
    IS_DIGIT_TEST('0'-1, false, &status);
    IS_DIGIT_TEST('9'+1, false, &status);
    IS_DIGIT_TEST('a', false, &status);
    IS_DIGIT_TEST('!', false, &status);        
    
    return status;
}
