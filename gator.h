/*
 *  gator.h - Gator; FidoNet conference mail gateway.
 *  Written by Andrew Clarke in 1994 & 1995 and released to the public domain.
*/

#ifndef __GATOR_H__
#define __GATOR_H__

#define VERSION "1.1.a1"

struct sw_t
{
    int help;
    int info;
    char cfg_fn[250];
    char log_fn[250];
    char *outbound_path;
    slist_t *sl_inbound;
    slist_t *sl_domain_match;
    slist_t *sl_echo_gate;
    slist_t *sl_node_mgr;
    int log_fmt;
    int log_is_open;
};

/* gateway struct */

typedef struct
{
    /* inbound import settings */
    char *in_name;              /* imported conference name */
    slist_t *in_pkt_orig;       /* masklist; import from these addresses
                                   (PKT orig) */
    slist_t *in_pkt_dest;       /* masklist; process packets destined for
                                   these addresses (PKT dest) */

    /* outbound export settings */
    char *out_name;             /* exported conference name */
    char *out_pkt_orig;         /* export using this address (PKT orig) */
    slist_t *out_pkt_dest;      /* masklist(prjnote); export to these
                                   addresses (PKT dest) */
    char *out_origin_txt;       /* exported origin line text */
    char *out_origin_addr;      /* exported origin line address */
    char *out_seenby;           /* export this string as SEEN-BY */
    char *out_path;             /* export this string as PATH */
} echo_gate_t;

/* node manager struct */

typedef struct
{
    char *address;              /* destination node's address */
    char *pkt_type;             /* packet type to use when sending to this 
                                   node */
    char *pkt_fn;               /* packet filename to use when * sending
                                   to this node (overrides * outbound path 
                                   and packet filename * generator */
    char *password;             /* packet password to use when sending to
                                   this node */
} node_mgr_t;

extern struct sw_t sw;

int main(int argc, char *argv[]);
void free_lists(void);
void clean_exit(int status);
void set_defaults(void);
void get_config(void);
void check_config(void);
void process_inbound(void);
void process_packet(char *path, char *filename, unsigned short dos_date, 
		    unsigned short dos_time);

#endif
