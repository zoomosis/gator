/*
 *  fdlog.h  FrontDoor-style BBS logging routines
 *  Written by Andrew Clarke in 1994 & 1995 and released to the public domain.
 */

#ifndef __FDLOG_H__
#define __FDLOG_H__

int FrodoLogInsertCR(char *filename);
int FrodoLogSPutLog(char *filename, char *c, char *str);
int FrodoLogPutLog(char *filename, char *str);
int FrodoLogPrintf(char *filename, const char *format, ...);
int FrodoLogOpen(char *filename, char *pProgName);

#endif
