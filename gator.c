/*
 *  gator.c - Gator; FidoNet conference mail gateway.
 *  Written by Andrew Clarke in 1994 & 1995 and released to the public domain.
*/

#ifdef __EMX__
#include <sys/emx.h>
#include <io.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <errno.h>
#include <time.h>
#include "slist.h"
#include "statlog.h"
#include "state.h"
#include "unused.h"
#include "getopts.h"
#include "gtrcmd.h"
#include "gtrhlp.h"
#include "gtrutil.h"
#include "gtrlog.h"
#include "gtrmask.h"
#include "gtrdmn.h"
#include "gtrpkt.h"
#include "gtrfido.h"
#include "gtrgate.h"
#include "gator.h"

struct sw_t sw;
FILE *pkt_fp;

static unsigned long tot_pkt_size;
static unsigned long tot_proc_msg_count, tot_gated_msg_count, tot_sent_msg_count;
static clock_t tot_start, tot_stop;
static unsigned long tot_secs_elapsed;

int main(int argc, char *argv[])
{
    set_defaults();
    get_cmd_line(argc, argv);
    show_version();

    if (sw.info)
    {
        show_info();
        return EXIT_FAILURE;
    }

    if (sw.help)
    {
        do_help();
        return EXIT_FAILURE;
    }

    get_config();
    check_config();
    process_inbound();
    clean_exit(EXIT_SUCCESS);

    return EXIT_SUCCESS;
}

void clean_exit(int status)
{
    free_lists();
    log_close();
    exit(status);
}

void set_defaults(void)
{
    sw.help = 0;
    sw.info = 0;
    strcpy(sw.cfg_fn, "GATOR.CFG");
    *(sw.log_fn) = '\0';
    sw.log_fmt = LOG_STY_OPUS;
    sw.log_is_open = 0;
    sw.outbound_path = NULL;
    sw.sl_inbound = CreateLinkedSList();
    sw.sl_domain_match = CreateLinkedSList();
    sw.sl_echo_gate = CreateLinkedSList();
    sw.sl_node_mgr = CreateLinkedSList();
}

void free_lists(void)
{
    slistnode_t *node;
    char *path;
    domain_t *domain;

    node = TraverseSListFirst(sw.sl_inbound);

    while (node)
    {
        path = GetSListElement(node);
        free(path);
        node = TraverseSListNext(node);
    }

    DestroyLinkedSList(sw.sl_inbound);

    node = TraverseSListFirst(sw.sl_domain_match);

    while (node)
    {
        domain = GetSListElement(node);
        free(domain->name);
        free(domain);
        node = TraverseSListNext(node);
    }

    DestroyLinkedSList(sw.sl_domain_match);
}

void check_config(void)
{
    if (!sw.log_is_open)
    {
        log_printf("-Warning, no log file being generated");
    }
    
    if (IsSListEmpty(sw.sl_inbound))
    {
        log_printf("!Gator requires at least one inbound path to be defined!");
        clean_exit(EXIT_FAILURE);
    }

    if (sw.outbound_path == NULL)
    {
        log_printf("!Gator requires an outbound path to be defined!");
        clean_exit(EXIT_FAILURE);
    }

    if (IsSListEmpty(sw.sl_domain_match))
    {
        log_printf("!> Gator requires at least one address resolution");
        log_printf("!> (domain and address mask) setting, eg.");
        log_printf("!> `DomainMatch fidonet 3:All/All.All'");
        clean_exit(EXIT_FAILURE);
    }
}

void rename_pkt_to_bad(char *path, char *filename)
{
    char tmpstr[300];
    char oldname[250], newname[250];
    char *pext;

    strcpy(oldname, path);
    strcat(oldname, filename);
    strcpy(newname, oldname);
    pext = strrchr(newname, '.');

    if (pext)
    {
        *pext = '\0';
    }
    
    strcat(newname, ".bad");
    log_printf("! Bad packet: renaming to %s", newname);

    if (rename(oldname, newname) != 0)
    {
        sprintf(tmpstr, "! Bad packet: unable to rename packet: %s", strerror(errno));
        *(tmpstr + strlen(tmpstr) - 1) = '\0';
        log_printf(tmpstr);
    }
}

void show_proc_stats(unsigned long secs_elapsed, unsigned long proc_msg_count,
		     unsigned long gated_msg_count, unsigned long sent_msg_count,
		     unsigned long pkt_size, int total)
{
    unsigned long proc_msgs_per_sec, kbytes_per_sec;
    unsigned long gated_msgs_per_sec, sent_msgs_per_sec;

    if (secs_elapsed == 0)
    {
        proc_msgs_per_sec = 0L;
        kbytes_per_sec = 0L;
        gated_msgs_per_sec = 0L;
        sent_msgs_per_sec = 0L;
    }
    else
    {
        proc_msgs_per_sec = (proc_msg_count / secs_elapsed);
        kbytes_per_sec = ((pkt_size / secs_elapsed) / 1024);
        gated_msgs_per_sec = (gated_msg_count / secs_elapsed);
        sent_msgs_per_sec = (sent_msg_count / secs_elapsed);
    }

    log_printf(":%sProcessed=%ld (%ld/s, %ldK/s), Gated=%ld (%ld/s), Exported=%ld (%ld/s)",
      (total ? "Total " : " "), proc_msg_count, proc_msgs_per_sec,
      kbytes_per_sec, gated_msg_count, gated_msgs_per_sec,
      sent_msg_count, sent_msgs_per_sec);
}

void process_packet(char *path, char *filename, unsigned short dos_date, 
		    unsigned short dos_time)
{
    char pkt_fn[250];
    struct tm tm;
    char szmonth[5], szdow[5];
    pkt_hdr_t hdr;
    int state, is_pkt_bad, is_pkt_for_gate;
    unsigned long pkt_size;
    unsigned long proc_msg_count, gated_msg_count, sent_msg_count;
    clock_t start, stop;
    unsigned long secs_elapsed;

    start = clock();

    strcpy(pkt_fn, path);
    strcat(pkt_fn, filename);
    pkt_fp = fopen(pkt_fn, "rb");

    is_pkt_bad = 0;

    state = read_pkt_hdr(pkt_fp, &hdr);

    switch (state)
    {
    case STATE_OK:
        cvt_dos_to_tm(dos_date, dos_time, &tm);
        strmonth(szmonth, &tm);
        strdow(szdow, &tm);
        pkt_size = filesize(pkt_fp);
        tot_pkt_size += pkt_size;

        log_printf("* %s (%ld bytes) %s %02d-%s-%02d, %02d:%02d:%02d, by %s %d.%02d",
          filename, pkt_size, szdow, tm.tm_mday, szmonth, tm.tm_year,
          tm.tm_hour, tm.tm_min, tm.tm_sec, show_product(hdr.prodCode),
          hdr.prodRevMaj, hdr.prodRevMin);

        log_printf("- Orig=%s, Dest=%s, Type=%s", hdr.origAddr, hdr.destAddr, hdr.ptype);
        is_pkt_for_gate = is_pkt_addr_for_gate(hdr.origAddr, hdr.destAddr);

        if (is_pkt_for_gate)
        {
            is_pkt_bad = !do_gating(hdr.origAddr, hdr.destAddr, pkt_fp, &proc_msg_count,
              &gated_msg_count, &sent_msg_count);
            tot_proc_msg_count += proc_msg_count;
            tot_gated_msg_count += gated_msg_count;
            tot_sent_msg_count += sent_msg_count;
        }
        else
        {
            log_printf("- Packet is not for a gateway; packet ignored");
        }
        break;

    case STATE_UNKNOWN:
        log_printf("! Bad packet: fatal error reading packet");
        is_pkt_bad = 1;
        break;

    case STATE_BAD:
        log_printf("! Bad packet: unknown packet header type");
        is_pkt_bad = 1;
        break;

    default:
        /* assume the packet is bad since it wasn't processed correctly */
        log_printf("! Bad packet: unhandled internal exception in process_packet() (state=%d)", state);
        is_pkt_bad = 1;
        break;
    }

    if (hdr.ptype)
    {
        free(hdr.ptype);
    }
    
    if (hdr.origAddr)
    {
        free(hdr.origAddr);
    }
    
    if (hdr.destAddr)
    {
        free(hdr.destAddr);
    }
    
    if (hdr.password)
    {
        free(hdr.password);
    }

    stop = clock();
    secs_elapsed = ((stop - start) / (long)CLOCKS_PER_SEC);

    if (is_pkt_for_gate)
    {
        show_proc_stats(secs_elapsed, proc_msg_count, gated_msg_count, sent_msg_count, pkt_size, 0);
    }

    fclose(pkt_fp);

    if (is_pkt_bad)
    {
        rename_pkt_to_bad(path, filename);
    }
}

void process_inbound(void)
{
    int done;
    slistnode_t *node;
    char *inbound_path;
    char pkt_spec[250];

#ifdef __EMX__
    struct _find ff;
#else
    struct find_t ff;
#endif

    tot_start = clock();

    node = TraverseSListFirst(sw.sl_inbound);

    while (node)
    {
        inbound_path = GetSListElement(node);
        log_printf("#Processing packets from %s (%s)", inbound_path,
                   "insecure");
        strcpy(pkt_spec, inbound_path);
        strcat(pkt_spec, "*.pkt");

#ifdef __EMX__
        done = __findfirst(pkt_spec, _A_NORMAL, &ff);
#else
        done = _dos_findfirst(pkt_spec, _A_NORMAL, &ff);
#endif
        while (!done)
        {
#ifdef __EMX__
            process_packet(inbound_path, ff.name, ff.date, ff.time);
            done = __findnext(&ff);
#else
            process_packet(inbound_path, ff.name, ff.wr_date, ff.wr_time);
            done = _dos_findnext(&ff);
#endif
        }

        node = TraverseSListNext(node);
    }

    tot_stop = clock();
    tot_secs_elapsed = ((tot_stop - tot_start) / (long)CLOCKS_PER_SEC);
    show_proc_stats(tot_secs_elapsed, tot_proc_msg_count,
      tot_gated_msg_count, tot_sent_msg_count, tot_pkt_size, 1);
}
