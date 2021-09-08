#ifndef __MSGTXT_H__
#define __MSGTXT_H__

/* AppMsgTxtProc() return constants */

#define MTXT_TEXT          0
#define MTXT_FILENOTOPEN  -1
#define MTXT_ENDOFFILE    -2
#define MTXT_ENDOFTEXT    -3
#define MTXT_NOMEM        -4

/* input buffer size */

#define MTXT_BUFSIZE      512

/* function prototype */

int ParseMsgTxt(int (*AppMsgTxtProc) (char *, int, long), FILE * fp);

#endif
