/*
 *  fdlog.c  FrontDoor-style BBS logging routines
 *  Written by Andrew Clarke in 1994 & 1995 and released to the public domain.
 */

#include <stdio.h>
#include <stdarg.h>
#include <share.h>
#include <time.h>
#include "fdlog.h"

int FrodoLogInsertCR(char *filename)
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

int FrodoLogSPutLog(char *filename, char *c, char *str)
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
    strftime(date, 20, "%H:%M:%S", time_now);
    fprintf(fp, "%c %s  %s\n", *c, date, str);
    fclose(fp);
    return 1;
}

int FrodoLogPutLog(char *filename, char *str)
{
    return FrodoLogSPutLog(filename, ">", str);
}

int FrodoLogPrintf(char *filename, const char *format, ...)
{
    va_list args;
    char buffer[512];
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    return FrodoLogSPutLog(filename, ">", buffer);
}

int FrodoLogOpen(char *filename, char *pProgName)
{
    FILE *fp;
    struct tm *time_now;
    time_t secs_now;
    char date[20];

    if (!FrodoLogInsertCR(filename))
    {
        return 0;
    }
    
    if ((fp = _fsopen(filename, "at", SH_DENYRW)) == NULL)
    {
        return 0;
    }

    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);
    strftime(date, 20, "%a %d %b %y", time_now);
    fprintf(fp, "----------  %s, %s\n", date, pProgName);
    fclose(fp);
    return 1;
}
