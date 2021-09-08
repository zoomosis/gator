#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "slist.h"
#include "gator.h"
#include "gtrutil.h"
#include "gtrlog.h"

void is_null(void *ptr)
{
    if (ptr == NULL)
    {
        log_printf("!Null pointer detected");
        clean_exit(EXIT_FAILURE);
    }
}

void show_version(void)
{
    puts("Gator version " VERSION);
    puts("Written by Andrew Clarke in 1994 & 1995 and released to the public domain.");
}

void show_info(void)
{
    puts("Build date: " __DATE__ ", " __TIME__);
}

void add_backslash(char *path)
{
    /* append a trailing '\' character */

    if (path[strlen(path) - 1] != '\\')
    {
        strcat(path, "\\");
    }
}

/* void cvt_tm_to_dos(struct tm *t, unsigned short *dos_date, unsigned short *dos_time); */

void cvt_dos_to_tm(const unsigned short dos_date, const unsigned short dos_time, struct tm *t)
{
    t->tm_year = (dos_date >> 9) + 80; /* years since 1980 */
    t->tm_mon = ((dos_date >> 5) & 0x0F) - 1; /* months since January */
    t->tm_mday = dos_date & 0x1F;
    t->tm_hour = (dos_time >> 11) & 0x1F;
    t->tm_min = (dos_time >> 5) & 0x3F;
    t->tm_sec = (dos_time & 0x1F) * 2;
    t->tm_isdst = -1;

    if (mktime(t) == -1)
    {
        t->tm_wday = 7;
    }
}

char *strmonth(char *dest, struct tm *t)
{
    strncpy(dest, asctime(t) + 4, 3);
    dest[3] = '\0';
    return dest;
}

char *strdow(char *dest, struct tm *t)
{
    strncpy(dest, asctime(t), 3);
    dest[3] = '\0';
    return dest;
}

long filesize(FILE * stream)
{
    long curpos, length;
    curpos = ftell(stream);
    fseek(stream, 0L, SEEK_END);
    length = ftell(stream);
    fseek(stream, curpos, SEEK_SET);
    return length;
}
