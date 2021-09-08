#ifndef __GTRUTIL_H__
#define __GTRUTIL_H__

void show_version(void);
void show_info(void);
void add_backslash(char *path);
void cvt_dos_to_tm(const unsigned short dos_date, const unsigned short dos_time, struct tm *t);
char *strmonth(char *dest, struct tm *t);
char *strdow(char *dest, struct tm *t);
long filesize(FILE * stream);
void is_null(void *ptr);

#endif
