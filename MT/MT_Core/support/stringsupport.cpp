/*
 *  stringsupport.cpp
 *  MADTraC
 *
 *  Created by Daniel Swain on 6/7/10.
 *
 */

#include <string.h>
#include "stringsupport.h"

bool MT_isdigit(char c)
{
    /* check based on ascii value */
    return (c >= '0' && c <= '9'); 
}

bool MT_IsFloatChar(char c)
{
    return MT_isdigit(c) || (c == '+') || (c == '-')
        || (c == 'E') || (c == 'e') || (c == '.');
}

//------------------------------------------------------------
//    String analaysis functions
//------------------------------------------------------------

/* shorthand - makes the function return 0 if the character is past the end
 * of the string (assuming l = strlen(string)).  Mostly for readability. */
#define BAIL_IF_LAST_CHAR(ch) if(ch >= l) { return MT_FORMAT_STRING_INVALID; }

MT_FORMAT_STRING_TYPE MT_hasFormatString(const char* s, int* p_start, int* p_end)
{
    /* look for a %, p1 will be a pointer to the first appearance */
    const char* p1 = strchr(s, '%');
    int p = p1 - s; /* convert pointer to position */
    int l = strlen(s);

    /* a format specifier must begin with a % */
    if(!p1)
    {
        return MT_FORMAT_STRING_INVALID;
    }
    /* it also can't be the last position */
    if(p == l)
    {
        return MT_FORMAT_STRING_INVALID;
    }

    /* c will be the relative index from p (where the % is) */
    int c = 1;
    char n = s[p + c];

    /* the % can be followed by +, -, or a space to indicate
     * special formatting;  if this is true then we want to
     * move on to the next character */
    if(n == '-' || n == '+' || n == ' ')
    {
        c++;
        BAIL_IF_LAST_CHAR(p + c);
        n = s[p+c];
    }

    /* a digit indicates a width specifier, a . indicates moving on
     * to a precision specifier, and a * indicates a special width specifier */
    if( isdigit(n) || n == '.' || n == '*')
    {
        bool have_decimal = (n == '.');
        bool search = true;

        /* if this char is a *, the next one MUST either be
         * a . or moving on to the lenght / format specifier */
        if(n == '*')
        {
            BAIL_IF_LAST_CHAR(p+c);
            if(s[p + c] != '.')
            {
                search = false;
                c++;
            }
        }

        /* loop over characters until we find a non-digit, except
         * that we can allow a single decimal */
        while(1)
        {
            BAIL_IF_LAST_CHAR(p+c);
            n = s[p+c];

            /* numbers are OK, just keep going */
            if(!isdigit(n))
            {
                if(n == '.')
                {
                    /* we can only have one decimal */
                    if(have_decimal)
                    {
                        return MT_FORMAT_STRING_INVALID;
                    }
                    have_decimal = true;
                }
                else
                {   /* loop breaks here */
                    break;
                }
            }
            c++; /* next char */
        }
    }

    BAIL_IF_LAST_CHAR(p+c);
    n = s[p+c];

    if(n == '*')
    {
        /* if this is a * then the previous char can NOT have been
        * a number */
        if(isdigit(s[p + c - 1]))
        {
            return MT_FORMAT_STRING_INVALID;
        }
        c++;
    }

    BAIL_IF_LAST_CHAR(p+c);
    n = s[p+c];
    
    /* check if this is a length formatter */
    if(n == 'l' || n == 'L' || n == 'h')
    {
        c++; /* if it is, move to the next char */
    }

    BAIL_IF_LAST_CHAR(p+c);
    n = s[p+c];

    MT_FORMAT_STRING_TYPE ret_fmt;

    /* now finally check to see if this is a valid formatter */
    switch(s[p + c])
    {
        case 'c':
            ret_fmt = MT_FORMAT_STRING_CHAR;
            break;
        case 's':
            ret_fmt = MT_FORMAT_STRING_STRING;
            break;
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
        case 'p':
            ret_fmt = MT_FORMAT_STRING_INT;
            break;
        case 'f':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
            ret_fmt = MT_FORMAT_STRING_FLOAT;
            break;
        default:
            ret_fmt = MT_FORMAT_STRING_INVALID;
    }

    /* store the start and end positions if necessary */
    if(ret_fmt != MT_FORMAT_STRING_INVALID)
    {
        if(p_start)
        {
            *p_start = p;
        }
        if(p_end)
        {
            *p_end = p + c;
        }
    }

    return ret_fmt;

}

std::string MT_StringVectorToString(const std::vector<std::string>& input,
                                    const std::string& sep)
{
    std::string all_lines;
    for(unsigned int i = 0; i < input.size(); i++)
    {
        all_lines += input[i];
        all_lines += sep;
    }
    return all_lines;
}

std::vector<std::string> MT_SplitString(const std::string& input,
                                        const std::string& split_on)
{
    std::vector<std::string> result;
    result.resize(0);

    if(!split_on.size())
    {
        return result;
    }

    size_t found, foundp = 0;

    found = input.find(split_on);
    while(found != std::string::npos)
    {
        result.push_back(input.substr(foundp, found - foundp));
        foundp = found + split_on.size();
        found = input.find(split_on,
                           foundp + 1);
    }
    result.push_back(input.substr(foundp, std::string::npos));
    return result;
}

std::string MT_ReplaceCharWithSpaceInString(const std::string& input,
                                            const char replacee)
{
    std::string result = input;
    for(unsigned int i = 0; i < result.length(); i++)
    {
        if(result[i] == replacee)
        {
            result[i] = ' ';
        }
    }
    return result;

}


