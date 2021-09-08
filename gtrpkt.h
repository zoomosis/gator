#ifndef __GTRPKT_H__
#define __GTRPKT_H__

typedef struct
{
    int state;
    unsigned short type;
    char *ptype;
    unsigned short prodCode;
    unsigned char prodRevMaj;
    unsigned char prodRevMin;
    struct tm tm;
    char *origAddr;
    char *destAddr;
    char *password;
}
pkt_hdr_t;

void clear_pkt_hdr(pkt_hdr_t * h);
int read_pkt_hdr(FILE * stream, pkt_hdr_t * h);
FILE *open_or_create_pkt(pkt_hdr_t * hdr, char *outbound_path, char *override_fn);
void write_pkt_term(FILE * stream);

#endif
