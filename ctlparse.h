/*
 *  ctlparse.h  Configuration control file parser
 *  Based on "cfgparse" source written by Scott Pitcher, 1992, 1993.
 *  Written by Andrew Clarke in 1994 & 1995 and released to the public domain.
 */

#ifndef __CTLPARSE_H__
#define __CTLPARSE_H__

/* config file parser call constants */

#define CTL_NOIGNORECASE  0
#define CTL_IGNORECASE    1

/* config file parser return constants */

#define CTL_NOFILE        -1
#define CTL_READFAILURE   -2
#define CTL_ENDOFFILE     -3
#define CTL_LINETOOLONG   -4
#define CTL_MISSINGPARAM  -5
#define CTL_ILLEGALARG    -6
#define CTL_INVCASEFLAG   -7

void parse_ctl_file(void (*ctl_proc) (char *arg, char *val, int status, int line),
		    char *pArgs[], FILE * fp, int case_flag);

#endif

/* __CTLPARSE_H__ */
