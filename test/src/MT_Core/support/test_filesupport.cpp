#include <string>
#include <iostream>
#include <string.h>

#include "MT_Test.h"

#include "MT/MT_Core/support/filesupport.h"

bool g_show_only_errors = true;

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

void CALCULATE_RELATIVE_PATH_TEST(const char* from_path,
                                  const char* to_path,
                                  const char* expected,
                                  int* p_in_status)
{
    std::string f(from_path);
    std::string t(to_path);
    std::string e(expected);

    std::string r = MT_CalculateRelativePath(f, t);

    if(!g_show_only_errors)
    {
        std::cout << "    From " << f << " to " << t << ": "
                  << r << " (expected " << e << ")" << std::endl;
    }

    if(r.compare(e) != 0)
    {
        std::cerr << "    With from path " << f
                  << " to path " << t
                  << " gave " << r
                  << " instead of " << e << std::endl;
        *p_in_status = MT_TEST_ERROR;
    }
}

void ENSURE_PATH_HAS_EXTENSION_TEST(const char* path,
                                    const char* ext,
                                    const char* expected,
                                    int* p_in_status)
{
    std::string p(path);
    std::string ex(ext);
    std::string e(expected);

    std::string r = MT_EnsurePathHasExtension(p, ex);

    if(!g_show_only_errors)
    {
        std::cout << "    Path " << p << " with extension " << ex << ": "
                  << r << " (expected " << e << ")" << std::endl;
    }

    if(r.compare(e) != 0)
    {
        std::cerr << "    With path " << p
                  << " and extension " << ex
                  << " gave " << r
                  << " instead of " << e << std::endl;
        *p_in_status = MT_TEST_ERROR;
    }
}

void PATH_IS_ABSOLUTE_TEST(const char* test_path,
                           bool expected,
                           int* p_in_status)
{

    bool r = MT_PathIsAbsolute(test_path);

    if(!g_show_only_errors)
    {
        std::cout << "    With path " << test_path << " got " << r
                  << ", exptected " << expected << std::endl;
    }

    if(r != expected)
    {
        std::cerr << "    With path " << test_path
                  << " got " << r
                  << " instead of " << expected << std::endl;
        *p_in_status = MT_TEST_ERROR;
    }
}

void FILE_IS_AVAILABLE_TEST(const char* filename,
                            const char* access,
                            bool expected,
                            int* p_in_status)
{
    bool r = MT_FileIsAvailable(filename, access);

    if(!g_show_only_errors)
    {
        std::cout << "    With filename " << filename << " got " << r
                  << ", expected " << expected << std::endl;
    }

    if(r != expected)
    {
        std::cerr << "    With filename " << filename << " got " << r
                  << " instead of " << expected << std::endl;
        *p_in_status = MT_TEST_ERROR;
    }
    
}

int main(int argc, char** argv)
{
    int status = MT_TEST_SUCCESS;

    if(argc > 1)
    {
        g_show_only_errors = false;
    }

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

    MT_TEST_START("MT_PathIsAbsolute");
    PATH_IS_ABSOLUTE_TEST("c:/abcd/def", true, &status);
    PATH_IS_ABSOLUTE_TEST("c:\\abcd/def", true, &status);
    PATH_IS_ABSOLUTE_TEST("c:\abcd/def", false, &status);    
    PATH_IS_ABSOLUTE_TEST("c\\abcd/def", false, &status);
    PATH_IS_ABSOLUTE_TEST("cd:\bcd/def", false, &status);        
    PATH_IS_ABSOLUTE_TEST("/abcd", true, &status);
    PATH_IS_ABSOLUTE_TEST("\\def", false, &status);
    PATH_IS_ABSOLUTE_TEST("def/ghi", false, &status);    
    
    MT_TEST_START("MT_CalculateRelativePath");
    CALCULATE_RELATIVE_PATH_TEST("abc/def/ghi", "abc/def/ggg/", "../ghi", &status);
    CALCULATE_RELATIVE_PATH_TEST("abc/def/ghi", "abc/def/ggg", "ghi", &status);
    CALCULATE_RELATIVE_PATH_TEST("/some/absolute/path/with/file.txt", "/some/absolute/path/basefile.txt", "with/file.txt", &status);
    CALCULATE_RELATIVE_PATH_TEST("/some/absolute/path/with/file.txt", "/some/absolute/path/with.txt", "with/file.txt", &status);
    CALCULATE_RELATIVE_PATH_TEST("completely", "different", "completely", &status);
    CALCULATE_RELATIVE_PATH_TEST("a", "b/f.foo", "../a", &status);
    CALCULATE_RELATIVE_PATH_TEST("b/f.foo", "a", "b/f.foo", &status);
    CALCULATE_RELATIVE_PATH_TEST("b/f.foo", "/foo/bar", "b/f.foo", &status);

    MT_TEST_START("MT_EnsurePathHasExtension");
    ENSURE_PATH_HAS_EXTENSION_TEST("foo.bar", "bar", "foo.bar", &status);
    ENSURE_PATH_HAS_EXTENSION_TEST("foo", "bar", "foo.bar", &status);
    ENSURE_PATH_HAS_EXTENSION_TEST("foo", ".bar", "foo.bar", &status);        
    ENSURE_PATH_HAS_EXTENSION_TEST("foo.asdf", "bar", "foo.asdf.bar", &status);

    MT_TEST_START("MT_FileIsAvailable");
    FILE_IS_AVAILABLE_TEST("foo.bar", "r", false, &status);
    FILE_IS_AVAILABLE_TEST("foo.bar", "w", true, &status);
    FILE_IS_AVAILABLE_TEST("foo.bar", "r", true, &status);        
    
    return status;

}
