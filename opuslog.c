/*
*   opuslog.c  Opus-style BBS logging routines
*   (c) copyright 1994, 1995 Andrew Clarke
*/

#include <stdio.h>
#include <stdarg.h>
#include <share.h>
#include <time.h>
#include "opuslog.h"

int OpusLogInsertCR(char *filename)
{
    FILE *fp;

    if ((fp = _fsopen(filename, "at", SH_DENYRW)) == NULL)
    {
        return 0;
    }
    
    fputc('\n', fp);
    fclose(fp);
    return 1;
}

int OpusLogSPutLog(char *filename, char *pProgId, char *c, char *str)
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
    strftime(date, 20, "%d %b %H:%M:%S", time_now);
    fprintf(fp, "%c %s %s %s\n", *c, date, pProgId, str);
    fclose(fp);
    return 1;
}

int OpusLogPutLog(char *filename, char *pProgId, char *str)
{
    return OpusLogSPutLog(filename, pProgId, ">", str);
}

int OpusLogPrintf(char *filename, char *pProgId, const char *format, ...)
{
    va_list args;
    char buffer[512];
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    return OpusLogSPutLog(filename, pProgId, ">", buffer);
}

int OpusLogOpen(char *filename, char *pProgId, char *pProgName)
{
    char buffer[128];

    if (!OpusLogInsertCR(filename))
    {
        return 0;
    }
    
    sprintf(buffer, "begin, %s", pProgName);
    return OpusLogSPutLog(filename, pProgId, "+", buffer);
}

int OpusLogClose(char *filename, char *pProgId, char *pProgName)
{
    char buffer[128];
    sprintf(buffer, "end, %s", pProgName);
    return OpusLogSPutLog(filename, pProgId, "+", buffer);
}
