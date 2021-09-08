#ifndef __GTRLOG_H__
#define __GTRLOG_H__

void log_open(void);
void log_close(void);
int log_printf(const char *format, ...);

#endif
