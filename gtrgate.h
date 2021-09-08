#ifndef __GTRGATE_H__
#define __GTRGATE_H__

int is_pkt_addr_for_gate(char *pkt_orig, char *pkt_dest);

int do_gating(char *pkt_orig, char *pkt_dest, FILE * stream,
              unsigned long *proc_msg_count,
              unsigned long *gated_msg_count,
              unsigned long *sent_msg_count);

#endif
