#ifndef __GTRPMSG_H__
#define __GTRPMSG_H__

typedef struct
{
    int state;
    unsigned short type;
    unsigned short attr;
    struct tm tm;
    int is_date_valid;
    char *origAddr;
    char *destAddr;
    char *from_user;
    char *to_user;
    char *subject;
}
pkd_msg_hdr_t;

void clear_pkd_msg_hdr(pkd_msg_hdr_t * h);
int read_pkd_msg_hdr(FILE * fp, pkd_msg_hdr_t * h);
int write_pkd_msg_hdr(FILE * fp, pkd_msg_hdr_t * h, int write_fields);

#endif
