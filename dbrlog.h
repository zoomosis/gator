/*
 *  dbrlog.h  D'Bridge-style BBS logging routines
 *  Written by Andrew Clarke in 1994 & 1995 and released to the public domain.
 */

#ifndef __DBRLOG_H__
#define __DBRLOG_H__

int DbrLogPutLog(char *filename, char *str);
int DbrLogPrintf(char *filename, const char *format, ...);

#endif
