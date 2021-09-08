#ifndef __STATLOG_H__
#define __STATLOG_H__

#define LOG_STY_OPUS   1
#define LOG_STY_FRODO  2
#define LOG_STY_DBRG   3

int statlog_open(char *filename, int style, char *prod_short, char *prod_long, char *str);
int statlog_close(char *filename, int style, char *prod_short, char *str);
int statlog_printf(char *filename, int style, char *prod_short, const char *format, ...);

#endif

/* __STATLOG_H__ */
