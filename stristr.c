/*
 *  stristr.c
 *
 *
 *  Finds the first occurrence of a substring in another string,
 *  with case insensitivity.
 *
 *  stristr scans s1 for the first occurrence of the substring s2,
 *  ignoring case.
 *
 *
 *  Return values:
 * 
 *  On success, strstr returns a pointer to the element in s1
 *  where s2 begins (points to s2 in s1).
 * 
 *  On error, if s2 does not occur in s1, strstr returns NULL.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

char *stristr(char *s1, char *s2)
{
    char *start, *pptr, *sptr;
    size_t slen, plen;

    for (start = s1, pptr = s2, slen = strlen(s1), plen = strlen(s2);
      slen >= plen; start++, slen--)
    {

        while (tolower(*start) != tolower(*s2))
        {
            start++;
            slen--;

            if (slen < plen)
	    {
                return NULL;
	    }
        }

        sptr = start;
        pptr = s2;

        while (tolower(*sptr) == tolower(*pptr))
        {
            sptr++;
            pptr++;

            if (*pptr == '\0')
	    {
                return start;
	    }
        }
    }

    return NULL;
}
