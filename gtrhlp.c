#include <stdio.h>
#include "gtrhlp.h"

void do_usage(void)
{
    puts("Usage: gator [-?hi] [-c<config_file>] [-n<log_file>]");
}

void do_help(void)
{
    do_usage();

    puts(
      "\n"
      "   -c<config_file>       - Use <config_file> instead of gator.cfg\n"
      "   -n<log_file>          - Override the log file given in gator.cfg");
}
