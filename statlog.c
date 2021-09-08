#include <stdio.h>
#include <stdarg.h>
#include "opuslog.h"
#include "fdlog.h"
#include "dbrlog.h"
#include "statlog.h"

int statlog_open(char *filename, int style, char *prod_short, char *prod_long, char *str)
{
    switch (style)
    {
    case LOG_STY_OPUS:
        OpusLogInsertCR(filename);
        return OpusLogSPutLog(filename, prod_short, "+", str);

    case LOG_STY_FRODO:
        FrodoLogOpen(filename, prod_long);
        return FrodoLogSPutLog(filename, "+", str);

    case LOG_STY_DBRG:
        return DbrLogPutLog(filename, str);

    default:
        return -1;
    }
}

int statlog_close(char *filename, int style, char *prod_short, char *str)
{
    switch (style)
    {
    case LOG_STY_OPUS:
        return OpusLogSPutLog(filename, prod_short, "+", str);

    case LOG_STY_FRODO:
        return FrodoLogSPutLog(filename, "+", str);

    case LOG_STY_DBRG:
        return DbrLogPutLog(filename, str);

    default:
        return -1;
    }
}

int statlog_printf(char *filename, int style, char *prod_short, const char *format, ...)
{
    va_list args;
    char str[512];
    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);

    switch (style)
    {
    case LOG_STY_OPUS:
        return OpusLogSPutLog(filename, prod_short, str, str + 1);

    case LOG_STY_FRODO:
        return FrodoLogSPutLog(filename, str, str + 1);

    case LOG_STY_DBRG:
        return DbrLogPutLog(filename, str + 1);

    default:
        return -1;
    }
}
