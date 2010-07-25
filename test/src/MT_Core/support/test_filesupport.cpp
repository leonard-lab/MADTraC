#include <string>
#include <string.h>

#include "MT_Test.h"

#include "MT/MT_Core/support/filesupport.h"

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

int main(int argc, char** argv)
{
    int status = MT_TEST_SUCCESS;

    /**************************************************/
    MT_TEST_START("filesupport");

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

    CHECK_FILE_EXTENSION_TEST("/some/path/file.txt", "txt", true, &status);
    CHECK_FILE_EXTENSION_TEST("/some/path/file.txt", "snot", false, &status);
    CHECK_FILE_EXTENSION_TEST("movie.avi", "avi", true, &status);
    CHECK_FILE_EXTENSION_TEST("someavi", "avi", false, &status);

    return status;

}
