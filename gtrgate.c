#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "slist.h"
#include "dlist.h"
#include "state.h"
#include "msgtxt.h"
#include "gtrmask.h"
#include "gtrpmsg.h"
#include "gtrgate.h"
#include "gtrlog.h"
#include "gtrpkt.h"
#include "gator.h"

struct
{
    char *pkt_orig, *pkt_dest;
    char *area_tag;
    int is_gating_msg;
}
cur_gate;

typedef struct
{
    char *area_tag;
    unsigned long gated, sent;
}
area_stat_t;

dlist_t *dl_msgtxt;
slist_t *sl_area_stat;
unsigned long txt_lines;

char *get0(FILE * fp);

int is_pkt_addr_for_gate(char *pkt_orig, char *pkt_dest)
{
    slistnode_t *node1, *node2, *node3;
    echo_gate_t *echo_gate;
    addr_mask_5d_t *addr_mask_5d;
    int match_orig, match_dest;

    /* traverse echo_gate */

    node1 = TraverseSListFirst(sw.sl_echo_gate);

    while (node1)
    {
        echo_gate = GetSListElement(node1);

        /* traverse in_pkt_orig */

        node2 = TraverseSListFirst(echo_gate->in_pkt_orig);

        while (node2)
        {
            addr_mask_5d = GetSListElement(node2);
            match_orig = cmp_addr_mask_5d(pkt_orig, addr_mask_5d);

            if (match_orig)
            {
                /* matched pkt_orig address with an in_pkt_orig address mask */

                /* traverse in_pkt_dest */

                node3 = TraverseSListFirst(echo_gate->in_pkt_dest);

                while (node3)
                {
                    addr_mask_5d = GetSListElement(node3);
                    match_dest = cmp_addr_mask_5d(pkt_dest, addr_mask_5d);

                    if (match_dest)
                    {
                        /* matched pkt_dest address with an in_pkt_dest address mask */

                        /* matched both! */

                        return 1;
                    }

                    node3 = TraverseSListNext(node3);
                }
            }

            node2 = TraverseSListNext(node2);
        }

        node1 = TraverseSListNext(node1);
    }

    /* fell through; no match */

    return 0;
}

int process_pkd_msg_hdr(FILE * stream, int *state,
                        unsigned long *proc_msg_count, pkd_msg_hdr_t * hdr)
{
    *state = read_pkd_msg_hdr(stream, hdr);

    switch (*state)
    {
    case STATE_OK:
        return 0;

    case STATE_UNKNOWN:
        log_printf("! Bad packet: fatal error reading packed message header (msg=%ld, ofs=%lx hex)",
          (*proc_msg_count) + 1, ftell(stream));
        return 1;

    case STATE_BAD:
        log_printf("! Bad packet: unknown packed message header type (msg=%ld, ofs=%lx hex)",
          (*proc_msg_count) + 1, ftell(stream));
        return 1;

    default:
        log_printf("! Bad packet: unhandled internal exception in process_message() (state=%d, msg=%ld, ofs=%lx hex)",
          *state, (*proc_msg_count) + 1, ftell(stream));
        return 1;
    }
}

int is_message_for_gate(char *pkt_orig, char *pkt_dest, char *area_tag)
{
    slistnode_t *node1, *node2, *node3;
    echo_gate_t *echo_gate;
    addr_mask_5d_t *addr_mask_5d;
    int match_orig, match_dest, match_area_tag;

    /* traverse echo_gate */

    node1 = TraverseSListFirst(sw.sl_echo_gate);

    while (node1)
    {
        echo_gate = GetSListElement(node1);

        /* traverse in_pkt_orig */

        node2 = TraverseSListFirst(echo_gate->in_pkt_orig);

        while (node2)
        {
            addr_mask_5d = GetSListElement(node2);
            match_orig = cmp_addr_mask_5d(pkt_orig, addr_mask_5d);

            if (match_orig)
            {
                /* matched pkt_orig address with an in_pkt_orig address mask */

                /* traverse in_pkt_dest */

                node3 = TraverseSListFirst(echo_gate->in_pkt_dest);

                while (node3)
                {
                    addr_mask_5d = GetSListElement(node3);
                    match_dest = cmp_addr_mask_5d(pkt_dest, addr_mask_5d);
                    match_area_tag = (stricmp(area_tag, echo_gate->in_name) == 0);

                    if (match_dest && match_area_tag)
                    {
                        /* matched pkt_dest address with an in_pkt_dest address mask */

                        /* matched inbound areatag with echo_gate->in_name */

                        /* matched all! */

                        return 1;
                    }

                    node3 = TraverseSListNext(node3);
                }
            }

            node2 = TraverseSListNext(node2);
        }

        node1 = TraverseSListNext(node1);
    }

    /* fell through; no match */

    return 0;
}

void incr_area_stats(char *area_tag, unsigned long gated,
                     unsigned long sent)
{
    slistnode_t *node;
    area_stat_t *area_stat;

    /* traverse area_stat */

    node = TraverseSListFirst(sl_area_stat);

    while (node)
    {
        area_stat = GetSListElement(node);

        if (stricmp(area_tag, area_stat->area_tag) == 0)
        {
            area_stat->gated += gated;
            area_stat->sent += sent;
            return;             /* get out now */
        }
        node = TraverseSListNext(node);
    }

    /* fell through; areatag was not in area_stat list, so add it */

    area_stat = malloc(sizeof(*area_stat));
    area_stat->area_tag = strdup(area_tag);
    area_stat->gated = gated;
    area_stat->sent = sent;
    node = CreateSListNode(area_stat);
    AddSListNode(sl_area_stat, node);
}

void show_area_stats(void)
{
    slistnode_t *node;
    area_stat_t *area_stat;

    /* traverse area_stat */

    node = TraverseSListFirst(sl_area_stat);

    while (node)
    {
        area_stat = GetSListElement(node);
        log_printf(":   %-24s (Gated=%04ld  Sent=%04ld)",
          area_stat->area_tag, area_stat->gated, area_stat->sent);
        node = TraverseSListNext(node);
    }
}

char *unique_msgid(void)
{
    static long starter;
    static char serialno[10];

    if (starter == 0)
    {
        starter = (long)time(NULL) << 8;
    }
    
    sprintf(serialno, "%08lx", starter++);
    return serialno;
}

void invalidate_msg_txt(void)
{
    dlistnode_t *node;
    char *text;
    int done_origin, done_tearline;

    done_origin = 0;
    done_tearline = 0;

    /* traverse backwards through message body */

    node = TraverseDListLast(dl_msgtxt);

    while (node)
    {
        text = GetDListElement(node);

        if (!done_origin && strstr(text, " * Origin: ") == text)
        {
            /* invalidate the old origin */
            *(text + 1) = '+';
            done_origin = 1;
        }

        if (!done_tearline && strstr(text, "---") == text)
        {
            /* invalidate the old tearline */
            *(text + 1) = '+';
            done_tearline = 1;
        }

        node = TraverseDListPrevious(node);
    }
}

int write_msg_txt(FILE * stream, echo_gate_t * echo_gate)
{
    dlistnode_t *node;
    char *text;
    char *out_name;

    /* write the new (gated) areatag to the export packet */
    fputs("AREA:", stream);
    out_name = strdup(echo_gate->out_name);
    strupr(out_name);           /* uppercase it! */
    fputs(out_name, stream);
    free(out_name);
    fputc('\r', stream);

    /* generate MSGID */
    fprintf(stream, "\01MSGID: %s %s", echo_gate->out_origin_addr, unique_msgid());

    /* invalidate existing message text kludges */
    invalidate_msg_txt();

    /* traverse forwards through message body */

    node = TraverseDListFirst(dl_msgtxt);

    while (node)
    {
        text = GetDListElement(node);

        if (strstr(text, "\01PATH: ") == text)
        {
            /* change existing PATH to GPTH */
            strncpy(text + 1, "GPTH", 4);
            fputc('\r', stream);
            fputs(text, stream);
        }
        else if (*text != '\01' && strstr(text, "SEEN-BY: ") == NULL)
        {
            /* it's normal text */
            fputc('\r', stream);
            fputs(text, stream);
        }
        node = TraverseDListNext(node);
    }

    fputc('\r', stream);

    /* write the gateway's tear line */

    fprintf(stream, "--- %s\r", "Gator " VERSION);

    /* write the gateway's origin line */

    fprintf(stream, " * Origin: %s (%s)\r", echo_gate->out_origin_txt, echo_gate->out_origin_addr);

    /* write the gateway's SEEN-BY line */
    if (echo_gate->out_seenby)
    {
        fprintf(stream, "SEEN-BY: %s\r", echo_gate->out_seenby);
    }
    
    /* write the gateway's PATH line */

    if (echo_gate->out_path)
    {
        fprintf(stream, "\01PATH: %s\r", echo_gate->out_path);
    }
    
    fputc('\0', stream);
    fflush(stream);

    return STATE_OK;
}

void export_new_msg(pkd_msg_hdr_t * h, echo_gate_t * echo_gate,
                    node_mgr_t * node_mgr)
{
    pkt_hdr_t exp_h;
    pkd_msg_hdr_t exp_msg_h;
    char *override_fn;
    FILE *fp;
    int state;

    clear_pkt_hdr(&exp_h);
    exp_h.origAddr = echo_gate->out_pkt_orig;
    exp_h.destAddr = node_mgr->address;
    exp_h.password = node_mgr->password;
    override_fn = node_mgr->pkt_fn;

    fp = open_or_create_pkt(&exp_h, sw.outbound_path, override_fn);

    if (fp)
    {
        /* export message header and text */
        /* memcpy(exp_msg_h, *h, sizeof(*h)); */
        exp_msg_h = *h;
        exp_msg_h.origAddr = echo_gate->out_pkt_orig;
        exp_msg_h.destAddr = node_mgr->address;
        state = write_pkd_msg_hdr(fp, &exp_msg_h, 1);
        (void)state;
        state = write_msg_txt(fp, echo_gate);
        write_pkt_term(fp);
        fclose(fp);
    }
    else
    {
        /* unable to create an export packet */
    }
}

void gate_msg(pkd_msg_hdr_t * h, echo_gate_t * echo_gate,
              unsigned long *sent_msg_count)
{
    slistnode_t *node1, *node2;
    addr_mask_5d_t *addr_mask_5d;
    node_mgr_t *node_mgr;
    unsigned long sent_count;

    sent_count = 0;

    /*
     *  a message-export is pending for at least one destination, so let's
     *  find out which destination addresses
     */

    /* traverse echo_gate->out_pkt_dest */

    node1 = TraverseSListFirst(echo_gate->out_pkt_dest);

    while (node1)
    {
        addr_mask_5d = GetSListElement(node1);

        /*
	 *  now we must compare addr_mask_5d with all addresses in the
         *  node configuration, and send this message to the addresses
         *  that match
	 */

        /* traverse sw.sl_node */

        node2 = TraverseSListFirst(sw.sl_node_mgr);

        while (node2)
        {
            node_mgr = GetSListElement(node2);

            if (cmp_addr_mask_5d(node_mgr->address, addr_mask_5d))
            {
                /* matched address mask with node manager address */
                /* send a new gated message to this address */

                export_new_msg(h, echo_gate, node_mgr);
                (*sent_msg_count)++;
                sent_count++;
            }

            node2 = TraverseSListNext(node2);
        }

        node1 = TraverseSListNext(node1);
    }

    incr_area_stats(echo_gate->out_name, 1L, sent_count);
}

void send_msg_to_gates(pkd_msg_hdr_t * h, unsigned long *sent_msg_count)
{
    slistnode_t *node;
    echo_gate_t *echo_gate;

    /* message pending for at least one gateway, so lets find out which gates */

    /* traverse echo_gate */

    node = TraverseSListFirst(sw.sl_echo_gate);

    while (node)
    {
        echo_gate = GetSListElement(node);

        if (stricmp(cur_gate.area_tag, echo_gate->in_name) == 0)
        {
            /* matched a gate's areatag with the one that is pending */

            /* send message to gate */

            gate_msg(h, echo_gate, sent_msg_count);
        }

        node = TraverseSListNext(node);
    }
}

char spinner(void)
{
    static char state;
    char spin[4] = { '\\', '|', '/', '-' };

    if (state++ == 3)
    {
        state = 0;
    }

    return spin[state];
}

void add_para_to_msg_txt(char *str)
{
    dlistnode_t *node;
    char *paragraph;

    txt_lines++;

    if (txt_lines == 1)
    {
        /* first line in msgtxt, check AREA: line */

        if (strstr(str, "AREA:") == str)
        {
            /* echomail message */

            printf("%c    %-25s %5s\r", spinner(), str + strlen("AREA:"), "");

            if (is_message_for_gate(cur_gate.pkt_orig, cur_gate.pkt_dest, str + strlen("AREA:")))
            {
                cur_gate.is_gating_msg = 1;
                cur_gate.area_tag = strdup(str + strlen("AREA:"));
                printf("%c    %-25s %5s\r", spinner(), str + strlen("AREA:"), "Gated");
            }
        }
        else
        {
            /* netmail message */

            printf("%c    %-25s %5s\r", spinner(), "Netmail", "");
        }
    }
    else if (cur_gate.is_gating_msg)
    {
        paragraph = strdup(str);
        node = CreateDListNode(paragraph);
        AddDListNode(dl_msgtxt, node);
    }
}

static int pkt_txt_proc(char *str, int status, long offset)
{
    switch (status)
    {
    case MTXT_FILENOTOPEN:
        log_printf("!Fatal error reading packet text (ofs=%lx hex): %s",
          offset, "File not open!");
        return 0;

    case MTXT_ENDOFFILE:
        log_printf("!Fatal error reading packet text (ofs=%lx hex): %s",
          offset, "Read failure or unexpected end-of-file encountered");
        return 0;

    case MTXT_ENDOFTEXT:
        return 0;

    case MTXT_NOMEM:
        log_printf("!Fatal error reading packet text (ofs=%lx hex): %s",
          offset, "Out of memory!");
        return 0;

    case MTXT_TEXT:
        add_para_to_msg_txt(str);
        return 1;

    default:
        log_printf
          ("!Fatal error reading packet text (ofs=%lx hex): %s (status=%d)",
          offset, "unhandled internal exception in pkt_txt_proc()", status);
        return 0;
    }
}

int process_pkd_msg_txt(FILE * stream)
{
    return ParseMsgTxt(pkt_txt_proc, stream);
}

void free_msg_txt(void)
{
    dlistnode_t *node;
    char *paragraph;

    node = TraverseDListFirst(dl_msgtxt);

    while (node)
    {
        paragraph = GetDListElement(node);
        free(paragraph);
        node = TraverseDListNext(node);
    }
}

void free_area_stats(void)
{
    slistnode_t *node;
    area_stat_t *area_stat;

    /* traverse area_stat */

    node = TraverseSListFirst(sl_area_stat);

    while (node)
    {
        area_stat = GetSListElement(node);
        free(area_stat->area_tag);
        free(area_stat);
        node = TraverseSListNext(node);
    }
}

int do_gating(char *pkt_orig, char *pkt_dest, FILE * stream,
              unsigned long *proc_msg_count,
              unsigned long *gated_msg_count,
              unsigned long *sent_msg_count)
{
    pkd_msg_hdr_t h;
    int done, state, is_msg_valid;

    *proc_msg_count = 0;
    *gated_msg_count = 0;
    *sent_msg_count = 0;

    sl_area_stat = CreateLinkedSList();

    done = 0;

    while (!done)
    {
        done = process_pkd_msg_hdr(stream, &state, proc_msg_count, &h);

        if (h.type == 0)
        {
            /* (success) end-of-packet marker found */
            break;
        }
        else
        {
            if (!h.is_date_valid)
	    {
                log_printf("- Warning, bad date detected (msg=%ld, ofs=%lx hex)",
                  (*proc_msg_count), ftell(stream));
	    }

            h.to_user = get0(stream);
            h.from_user = get0(stream);
            h.subject = get0(stream);

            cur_gate.pkt_orig = pkt_orig;
            cur_gate.pkt_dest = pkt_dest;
            cur_gate.area_tag = NULL;
            cur_gate.is_gating_msg = 0;

            dl_msgtxt = CreateLinkedDList();
            txt_lines = 0;
            is_msg_valid = process_pkd_msg_txt(stream);

            if (is_msg_valid)
            {
                (*proc_msg_count)++;

                if (cur_gate.is_gating_msg)
                {
                    send_msg_to_gates(&h, sent_msg_count);
                    (*gated_msg_count)++;
                }
            }
            else
            {
                done = 1;
            }

            free_msg_txt();
            DestroyLinkedDList(dl_msgtxt);

            if (cur_gate.area_tag)
	    {
                free(cur_gate.area_tag);
	    }
	    
            free(h.subject);
            free(h.from_user);
            free(h.to_user);

            if (h.destAddr)
	    {
                free(h.destAddr);
	    }
	    
            if (h.origAddr)
	    {
                free(h.origAddr);
	    }
        }
    }

    show_area_stats();
    free_area_stats();
    DestroyLinkedSList(sl_area_stat);

    return (state == STATE_OK && is_msg_valid);
}

char *get0(FILE * fp)
{
    char *str;
    long get0posi;
    char *get0ptr;

    str = malloc(73);
    memset(str, ' ', 73);
    get0posi = ftell(fp);
    fread(str, 1, 73, fp);
    get0ptr = memchr(str, '\0', 73);

    if (get0ptr)
    {
        get0posi += get0ptr - str + 1;
    }
    
    fseek(fp, get0posi, SEEK_SET);
    return str;
}
