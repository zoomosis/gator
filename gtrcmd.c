#include <stdio.h>
#include "getopts.h"
#include "gtrcmd.h"
#include "slist.h"
#include "gator.h"

opt_t opttable[] =
{
    {"?", OPTBOOL, &sw.help},
    {"h", OPTBOOL, &sw.help},
    {"i", OPTBOOL, &sw.info},
    {"c", OPTSTR, sw.cfg_fn},
    {"n", OPTSTR, sw.log_fn},
    {NULL, 0, NULL}
};

void get_cmd_line(int argc, char *argv[])
{
    /* int optup = */ getopts(argc, argv, opttable);
}
