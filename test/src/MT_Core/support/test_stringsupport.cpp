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

void GET_FILE_EXTENSION_TEST(const std::string& path,
                             const std::string& expected_ext,
                             int* p_in_status)
{
    std::string ext = MT_GetFileExtension(path);
    if(strcmp(ext.c_str(), expected_ext.c_str()))
    {
        *p_in_status = MT_TEST_ERROR;
        fprintf(stderr,
                "    With path %s got extension %s when expecting %s\n",
                path.c_str(), ext.c_str(), expected_ext.c_str());
    }
}

void CHECK_FILE_EXTENSION_TEST(const char* path,
                               const char* check_ext,
                               bool expected_result,
                               int* p_in_status)
{
    bool result = MT_PathHasFileExtension(path, check_ext);
    if(result != expected_result)
    {
        *p_in_status = MT_TEST_ERROR;
        fprintf(stderr,
                "    With path %s got result '%s' when expecting '%s'\n",
                path, result ? "true" : "false",
                expected_result ? "true" : "false");
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
    MT_TEST_START("MT_GetFileExtension");

#ifndef _WIN32
    GET_FILE_EXTENSION_TEST("/some/path/some.txt", "txt", &status);
    GET_FILE_EXTENSION_TEST("/some/path/and_no_extension", "", &status);
    GET_FILE_EXTENSION_TEST("~/a/path/with.long_extension", "long_extension", &status);
    GET_FILE_EXTENSION_TEST("relpath/with.dot/and.dat", "dat", &status);
    GET_FILE_EXTENSION_TEST("relpath/with.dot/and_no_extension", "", &status);
    GET_FILE_EXTENSION_TEST("/some/path.dot/with.two/dots.txt", "txt", &status);
    GET_FILE_EXTENSION_TEST("file.avi", "avi", &status);
    GET_FILE_EXTENSION_TEST("file..ext", "ext", &status);
    GET_FILE_EXTENSION_TEST("path/file..ext", "ext", &status);
    GET_FILE_EXTENSION_TEST("path\\with\\wrong\\seps.txt", "txt", &status);
#else
    /* TODO */
#endif

    /**************************************************/    
    MT_TEST_START("MT_PathHasFileExtension");

    CHECK_FILE_EXTENSION_TEST("/some/path/file.txt", "txt", true, &status);
    CHECK_FILE_EXTENSION_TEST("/some/path/file.txt", "snot", false, &status);
    CHECK_FILE_EXTENSION_TEST("movie.avi", "avi", true, &status);
    CHECK_FILE_EXTENSION_TEST("someavi", "avi", false, &status);

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
