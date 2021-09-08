/*
*   opuslog.h  Opus-style BBS logging routines
*   (c) copyright 1994, 1995 Andrew Clarke
*/

#ifndef __OPUSLOG_H__
#define __OPUSLOG_H__

int OpusLogInsertCR(char *filename);
int OpusLogSPutLog(char *filename, char *pProgId, char *c, char *str);
int OpusLogPutLog(char *filename, char *pProgId, char *str);
int OpusLogPrintf(char *filename, char *pProgId, const char *format, ...);
int OpusLogOpen(char *filename, char *pProgId, char *pProgName);
int OpusLogClose(char *filename, char *pProgId, char *pProgName);

#endif
