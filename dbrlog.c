/*
 *  dbrlog.c  D'Bridge-style BBS logging routines
 *  Written by Andrew Clarke in 1994 & 1995 and released to the public domain.
 */

#include <stdio.h>
#include <stdarg.h>
#include <share.h>
#include <time.h>
#include "dbrlog.h"

int DbrLogPutLog(char *filename, char *str)
{
    FILE *fp;
    struct tm *time_now;
    time_t secs_now;
    char date[20];

    if ((fp = _fsopen(filename, "at", SH_DENYRW)) == NULL)
    {
        return 0;
    }
    
    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);
    strftime(date, 20, "%m/%d/%y %H:%M", time_now);
    fprintf(fp, "%s  %s\n", date, str);
    fclose(fp);
    return 1;
}

int DbrLogPrintf(char *filename, const char *format, ...)
{
    va_list args;
    char buffer[512];
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    return DbrLogPutLog(filename, buffer);
}
