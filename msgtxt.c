#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msgtxt.h"

int ParseMsgTxt(int (*AppMsgTxtProc) (char *, int, long), FILE * fp)
{
    char szInBuf[MTXT_BUFSIZE + 1];
    size_t buflen;
    long ofs, len;
    int IsEOT, fAppReturn;
    char *pEndBuf, *str, *pCR, *p;

    pEndBuf = (char *) szInBuf + MTXT_BUFSIZE;
    *pEndBuf = '\0';

    /* is file open? */

    if (!fp)
    {
        if (!AppMsgTxtProc(NULL, MTXT_FILENOTOPEN, 0))
	{
            return 0;
	}
    }

    str = NULL;

    while (!feof(fp))
    {
        p = szInBuf;
        ofs = ftell(fp);
        buflen = fread(szInBuf, 1, MTXT_BUFSIZE, fp);
        len = strlen(szInBuf);
        IsEOT = ((size_t) len < buflen);

        while (p < pEndBuf)
	{
            pCR = strchr(p, '\13');

            if (pCR)
	    {
                *pCR = '\0';
	    }

            /* If str is NULL then it's a new paragraph, otherwise str is */
            /* not NULL so we append the new text to the old paragraph. */

            if (!str)
	    {
                /* new paragraph; strdup() does the work for us */

                if ((str = strdup(p)) == NULL)
		{
                    /* ran out of memory */
                    if (!AppMsgTxtProc(NULL, MTXT_NOMEM, ftell(fp)))
		    {
                        return 0;
		    }
                }
            }
	    else
	    {
                /* expand buffer to accept new string */
                if ((str = realloc(str, strlen(str) + strlen(p) + 1)) == NULL)
		{
                    /* ran out of memory */
                    if (!AppMsgTxtProc(NULL, MTXT_NOMEM, ftell(fp)))
		    {
                        return 0;
		    }
                }
		else
		{
                    /* got the space - so put the string in it */
                    strcat(str, p);
                }
            }

            if (pCR)
	    {
                fAppReturn = AppMsgTxtProc(str, MTXT_TEXT, ftell(fp));
                free(str);

                if (!fAppReturn)
		{
                    return 1;
		}
                str = NULL;
                p = pCR + 1;
            }
	    else
	    {
                p = pEndBuf;
            }
        }

        if (IsEOT)
	{
            if (str)
	    {
                fAppReturn = AppMsgTxtProc(str, MTXT_TEXT, ftell(fp));
                free(str);

                if (!fAppReturn)
		{
                    return 1;
		}

                str = NULL;
            }

            fseek(fp, ofs + len + 1, SEEK_SET);

            if (!AppMsgTxtProc(NULL, MTXT_ENDOFTEXT, ftell(fp)))
	    {
                return 1;
	    }
        }
    }

    AppMsgTxtProc(NULL, MTXT_ENDOFFILE, ftell(fp));
    return 0;
}

#ifdef TEST_MSGTXT

/* test code */

/* TestParserProc returns 1 to tell ParseMsgTxt() to continue, or returns 0 */
/* for ParseMsgTxt() to stop */

static int TestParserProc(char *str, int condition, long offset)
{
    switch (condition) {
    case MTXT_FILENOTOPEN:
        printf("MTXT_FILENOTOPEN: File not open!\n");
        return 0;

    case MTXT_ENDOFFILE:
        printf("MTXT_ENDOFFILE: End-of-file encountered (ofs=%ld).\n", offset);
        return 0;

    case MTXT_ENDOFTEXT:
        printf("MTXT_ENDOFTEXT: End-of-text encountered (ofs=%ld).\n", offset);
        return 1;

    case MTXT_NOMEM:
        printf("MTXT_NOMEM: Out of memory! (ofs=%ld)\n", offset);
        return 0;

    case MTXT_TEXT:
        /* puts(str); */
        return 1;

    default:
        printf("Error! Unhandled parser condition %d (ofs=%ld)!\n", condition, offset);
        return 0;
    }
}

int main(void)
{
    FILE *fp;

    /* test with Ezycom messagebase */

    fp = fopen("msgt005.bbs", "rb");

    if (fp == NULL)
    {
        perror("msgtxt: Cannot open input file");
        return 1;
    }

    ParseMsgTxt(TestParserProc, fp);
    fclose(fp);

    return 0;
}

#endif
