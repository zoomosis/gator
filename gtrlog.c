#include <stdio.h>
#include <stdarg.h>

#ifdef USECORELEFT
#if defined(__MSDOS__) && defined(__BORLANDC__)
#include <alloc.h>
#elif defined(_QC)
#include "coreleft.h"
#endif
#endif

#include "statlog.h"
#include "gtrlog.h"
#include "slist.h"
#include "gator.h"

void log_open(void)
{
    char str[512];

#ifdef USECORELEFT
    sprintf(str, "+begin, %s (mem=%ldK - main=%p)", "Gator " VERSION,
      (unsigned long) coreleft() / 1024, main);
#else
    sprintf(str, "+begin, %s (main=%p)", "Gator " VERSION, main);
#endif

    printf("%c %s\n", *str, str + 1);

    if (!sw.log_is_open)
    {
        sw.log_is_open = statlog_open(sw.log_fn, sw.log_fmt, "GATOR", "Gator " VERSION, str + 1);
    }
}

void log_close(void)
{
    char str[512];

#ifdef USECORELEFT
    sprintf(str, "+end, %s (mem=%ldK - main=%p)", "Gator " VERSION,
      (unsigned long) coreleft() / 1024, main);
#else
    sprintf(str, "+end, %s (main=%p)", "Gator " VERSION, main);
#endif

    printf("%c %s\n", *str, str + 1);

    if (sw.log_is_open)
    {
        statlog_close(sw.log_fn, sw.log_fmt, "GATOR", str + 1);
        sw.log_is_open = 0;
    }
}

int log_printf(const char *format, ...)
{
    va_list args;
    char str[512];

    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);
    printf("%c %s\n", *str, str + 1);

    if (sw.log_is_open)
    {
        return statlog_printf(sw.log_fn, sw.log_fmt, "GATOR", str);
    }

    return 1;
}
