#include <stdio.h>

enum
{
    MT_TEST_ERROR_UNKNOWN = -1,
    MT_TEST_SUCCESS,
    MT_TEST_ERROR_GENERIC,
    MT_TEST_ERROR_HIGHEST
};

#define MT_TEST_ERROR MT_TEST_ERROR_GENERIC

#define MT_TEST_START(message)                                          \
    fprintf(stdout,                                                     \
    "############################################################\n");  \
    fprintf(stdout, "Starting Test of ");                               \
    fprintf(stdout, message);                                           \
    fprintf(stdout, "\n");

#define MT_TEST_ERROR_MESSAGE(message)                                  \
    fprintf(stderr, "  - Error: ");                                     \
    fprintf(stderr, message);                                           \
    fprintf(stderr, "\n");
