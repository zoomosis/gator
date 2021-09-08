#ifdef __EMX__
#include <sys/emx.h>
#include <io.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <time.h>
#include "state.h"
#include "ftnpkt.h"
#include "prseaddr.h"
#include "gtrfido.h"
#include "gtrmask.h"
#include "gtrdmn.h"
#include "gtrlog.h"
#include "gtrpkt.h"

#define MAX_PKT_HDR  sizeof(raw_fido_pkt_hdr_t2)

void clear_pkt_hdr(pkt_hdr_t * h)
{
    h->state = STATE_UNKNOWN;
    h->type = 0;
    h->ptype = NULL;
    h->prodCode = 0;
    h->prodRevMaj = 0;
    h->prodRevMin = 0;
    h->origAddr = NULL;
    h->destAddr = NULL;
    h->password = NULL;

    h->tm.tm_sec = 0;
    h->tm.tm_min = 0;
    h->tm.tm_hour = 0;
    h->tm.tm_mday = 1;
    h->tm.tm_mon = 0;
    h->tm.tm_year = 95;
    h->tm.tm_isdst = -1;

    if (mktime(&h->tm) == -1)
    {
        h->tm.tm_wday = 7;
    }
}

unsigned short get_pkt_type(raw_fido_pkt_hdr_t2 * raw_p_t2, pkt_hdr_t * h)
{
    h->type = (unsigned short)((raw_p_t2->packetType[1] << 8) | raw_p_t2->packetType[0]);
    return h->type;
}

int is_pkt_type_2pt2(raw_fido_pkt_hdr_t2pt2 * raw_p_t2pt2, pkt_hdr_t * h)
{
    char nullchk[8];
    unsigned short sub_ver;
    unsigned short origZone, origNet, origNode, origPoint;
    unsigned short destZone, destNet, destNode, destPoint;
    char *origDomain, *destDomain;
    char tmpstr[9];

    tmpstr[8] = '\0';
    memset(nullchk, '\0', 8);
    sub_ver = (unsigned short)((raw_p_t2pt2->pktSubVer[1] << 8) | raw_p_t2pt2->pktSubVer[0]);

    if (sub_ver == 2 && memcmp(raw_p_t2pt2->fill, nullchk, 8) == 0)
    {
        /* it's a Type 2.2 packet */

        h->prodCode = raw_p_t2pt2->prodCode;
        h->prodRevMaj = raw_p_t2pt2->prodRev;

        origZone = (unsigned short)((raw_p_t2pt2->origZone[1] << 8) | raw_p_t2pt2->origZone[0]);
        origNet = (unsigned short)((raw_p_t2pt2->origNet[1] << 8) | raw_p_t2pt2->origNet[0]);
        origNode = (unsigned short)((raw_p_t2pt2->origNode[1] << 8) | raw_p_t2pt2->origNode[0]);
        origPoint = (unsigned short)((raw_p_t2pt2->origPoint[1] << 8) | raw_p_t2pt2->origPoint[0]);
        memcpy(tmpstr, raw_p_t2pt2->origDomain, 8);
        origDomain = tmpstr;
        h->origAddr = strdup(show_address(origZone, origNet, origNode, origPoint, origDomain));

        destZone = (unsigned short)((raw_p_t2pt2->destZone[1] << 8) | raw_p_t2pt2->destZone[0]);
        destNet = (unsigned short)((raw_p_t2pt2->destNet[1] << 8) | raw_p_t2pt2->destNet[0]);
        destNode = (unsigned short)((raw_p_t2pt2->destNode[1] << 8) | raw_p_t2pt2->destNode[0]);
        destPoint = (unsigned short)((raw_p_t2pt2->destPoint[1] << 8) | raw_p_t2pt2->destPoint[0]);
        memcpy(tmpstr, raw_p_t2pt2->destDomain, 8);
        destDomain = tmpstr;
        h->destAddr = strdup(show_address(destZone, destNet, destNode, destPoint, destDomain));

        memcpy(tmpstr, raw_p_t2pt2->password, 8);
        h->password = strdup(tmpstr);

        return 1;
    }
    else
    {
        return 0;
    }
}

int is_pkt_type_2p(raw_fido_pkt_hdr_t2p * raw_p_t2p, pkt_hdr_t * h)
{
    unsigned short origZone, origNet, origNode, origPoint;
    unsigned short destZone, destNet, destNode, destPoint;
    char *origDomain, *destDomain;
    char tmpstr[9];

    tmpstr[8] = '\0';

    if (raw_p_t2p->capWord[0] == raw_p_t2p->capValid[1] && raw_p_t2p->capWord[1] == raw_p_t2p->capValid[0])
    {
        /* it's a Type 2+ packet */

        h->prodCode = (unsigned short)((raw_p_t2p->prodCodeHi << 8) | raw_p_t2p->prodCodeLo);
        h->prodRevMaj = raw_p_t2p->prodRevMaj;
        h->prodRevMin = raw_p_t2p->prodRevMin;

        h->tm.tm_year = (raw_p_t2p->year[1] << 8) | raw_p_t2p->year[0];
        h->tm.tm_mon = ((raw_p_t2p->month[1] << 8) | raw_p_t2p->month[0]);
        h->tm.tm_mday = (raw_p_t2p->day[1] << 8) | raw_p_t2p->day[0];
        h->tm.tm_hour = (raw_p_t2p->hour[1] << 8) | raw_p_t2p->hour[0];
        h->tm.tm_min = (raw_p_t2p->minute[1] << 8) | raw_p_t2p->minute[0];
        h->tm.tm_sec = (raw_p_t2p->second[1] << 8) | raw_p_t2p->second[0];

        if (mktime(&h->tm) == -1)
	{
            h->tm.tm_wday = 7;
	}

        origZone = (unsigned short)((raw_p_t2p->origZone[1] << 8) | raw_p_t2p->origZone[0]);
        origNet = (unsigned short)((raw_p_t2p->origNet[1] << 8) | raw_p_t2p->origNet[0]);
        origNode = (unsigned short)((raw_p_t2p->origNode[1] << 8) | raw_p_t2p->origNode[0]);
        origPoint = (unsigned short)((raw_p_t2p->origPoint[1] << 8) | raw_p_t2p->origPoint[0]);
        origDomain = resolve_domain(origZone, origNet, origNode, origPoint);
        h->origAddr = strdup(show_address(origZone, origNet, origNode, origPoint, origDomain));

        destZone = (unsigned short)((raw_p_t2p->destZone[1] << 8) | raw_p_t2p->destZone[0]);
        destNet = (unsigned short)((raw_p_t2p->destNet[1] << 8) | raw_p_t2p->destNet[0]);
        destNode = (unsigned short)((raw_p_t2p->destNode[1] << 8) | raw_p_t2p->destNode[0]);
        destPoint = (unsigned short)((raw_p_t2p->destPoint[1] << 8) | raw_p_t2p->destPoint[0]);
        destDomain = resolve_domain(destZone, destNet, destNode, destPoint);
	h->destAddr = strdup(show_address(destZone, destNet, destNode, destPoint, destDomain));

        memcpy(tmpstr, raw_p_t2p->password, 8);
        h->password = strdup(tmpstr);

        return 1;
    }
    else
    {
        return 0;
    }
}

int is_pkt_type_2ptN(raw_fido_pkt_hdr_t2ptN * raw_p_t2ptN, pkt_hdr_t * h)
{
    unsigned short origZone, origNet, origNode, origPoint;
    unsigned short destZone, destNet, destNode, destPoint;
    char *origDomain, *destDomain;
    char tmpstr[9];

    tmpstr[8] = '\0';

    if (raw_p_t2ptN->capWord[0] == raw_p_t2ptN->capValid[0] && raw_p_t2ptN->capWord[1] == raw_p_t2ptN->capValid[1])
    {
        /* it's a Type 2.N packet */

        h->prodCode = (unsigned short)((raw_p_t2ptN->prodCodeHi << 8) | raw_p_t2ptN->prodCodeLo);
        h->prodRevMaj = raw_p_t2ptN->prodRevMaj;
        h->prodRevMin = raw_p_t2ptN->prodRevMin;

        h->tm.tm_year = (raw_p_t2ptN->year[1] << 8) | raw_p_t2ptN->year[0];
        h->tm.tm_mon = ((raw_p_t2ptN->month[1] << 8) | raw_p_t2ptN->month[0]);
        h->tm.tm_mday = (raw_p_t2ptN->day[1] << 8) | raw_p_t2ptN->day[0];
        h->tm.tm_hour = (raw_p_t2ptN->hour[1] << 8) | raw_p_t2ptN->hour[0];
        h->tm.tm_min = (raw_p_t2ptN->minute[1] << 8) | raw_p_t2ptN->minute[0];
        h->tm.tm_sec = (raw_p_t2ptN->second[1] << 8) | raw_p_t2ptN->second[0];

        if (mktime(&h->tm) == -1)
	{
            h->tm.tm_wday = 7;
	}
	
        origZone = (unsigned short)((raw_p_t2ptN->origZone[1] << 8) | raw_p_t2ptN->origZone[0]);
        origNet = (unsigned short)((raw_p_t2ptN->origNet[1] << 8) | raw_p_t2ptN->origNet[0]);
        origNode = (unsigned short)((raw_p_t2ptN->origNode[1] << 8) | raw_p_t2ptN->origNode[0]);
        origPoint = (unsigned short)((raw_p_t2ptN->origPoint[1] << 8) | raw_p_t2ptN->origPoint[0]);
        origDomain = resolve_domain(origZone, origNet, origNode, origPoint);
        h->origAddr = strdup(show_address(origZone, origNet, origNode, origPoint, origDomain));

        destZone = (unsigned short)((raw_p_t2ptN->destZone[1] << 8) | raw_p_t2ptN->destZone[0]);
        destNet = (unsigned short)((raw_p_t2ptN->destNet[1] << 8) | raw_p_t2ptN->destNet[0]);
        destNode = (unsigned short)((raw_p_t2ptN->destNode[1] << 8) | raw_p_t2ptN->destNode[0]);
        destPoint = (unsigned short)((raw_p_t2ptN->destPoint[1] << 8) | raw_p_t2ptN->destPoint[0]);
        destDomain = resolve_domain(destZone, destNet, destNode, destPoint);
        h->destAddr = strdup(show_address(destZone, destNet, destNode, destPoint, destDomain));

        memcpy(tmpstr, raw_p_t2ptN->password, 8);
        h->password = strdup(tmpstr);

        return 1;
    }
    else
    {
        return 0;
    }
}

void set_pkt_type_2(raw_fido_pkt_hdr_t2 * raw_p_t2, pkt_hdr_t * h)
{
    unsigned short origZone, origNet, origNode;
    unsigned short destZone, destNet, destNode;
    char *origDomain, *destDomain;
    char tmpstr[9];

    tmpstr[8] = '\0';

    /* it's a Type 2 (StoneAge) packet */

    h->prodCode = raw_p_t2->prodCode;

    h->tm.tm_year = (raw_p_t2->year[1] << 8) | raw_p_t2->year[0];
    h->tm.tm_mon = ((raw_p_t2->month[1] << 8) | raw_p_t2->month[0]);
    h->tm.tm_mday = (raw_p_t2->day[1] << 8) | raw_p_t2->day[0];
    h->tm.tm_hour = (raw_p_t2->hour[1] << 8) | raw_p_t2->hour[0];
    h->tm.tm_min = (raw_p_t2->minute[1] << 8) | raw_p_t2->minute[0];
    h->tm.tm_sec = (raw_p_t2->second[1] << 8) | raw_p_t2->second[0];

    if (mktime(&h->tm) == -1)
        h->tm.tm_wday = 7;

    origZone = (unsigned short)((raw_p_t2->origZone[1] << 8) | raw_p_t2->origZone[0]);
    origNet = (unsigned short)((raw_p_t2->origNet[1] << 8) | raw_p_t2->origNet[0]);
    origNode = (unsigned short)((raw_p_t2->origNode[1] << 8) | raw_p_t2->origNode[0]);
    origDomain = resolve_domain(origZone, origNet, origNode, 0);
    h->origAddr = strdup(show_address(origZone, origNet, origNode, 0, origDomain));

    destZone = (unsigned short)((raw_p_t2->destZone[1] << 8) | raw_p_t2->destZone[0]);
    destNet = (unsigned short)((raw_p_t2->destNet[1] << 8) | raw_p_t2->destNet[0]);
    destNode = (unsigned short)((raw_p_t2->destNode[1] << 8) | raw_p_t2->destNode[0]);
    destDomain = resolve_domain(destZone, destNet, destNode, 0);
    h->destAddr = strdup(show_address(destZone, destNet, destNode, 0, destDomain));

    memcpy(tmpstr, raw_p_t2->password, 8);
    h->password = strdup(tmpstr);
}

int read_pkt_hdr(FILE * stream, pkt_hdr_t * h)
{
    unsigned char raw_hdr[MAX_PKT_HDR];
    raw_fido_pkt_hdr_t2 *raw_p_t2;
    raw_fido_pkt_hdr_t2p *raw_p_t2p;
    raw_fido_pkt_hdr_t2pt2 *raw_p_t2pt2;
    raw_fido_pkt_hdr_t2ptN *raw_p_t2ptN;

    raw_p_t2 = (raw_fido_pkt_hdr_t2 *) raw_hdr;
    raw_p_t2p = (raw_fido_pkt_hdr_t2p *) raw_hdr;
    raw_p_t2pt2 = (raw_fido_pkt_hdr_t2pt2 *) raw_hdr;
    raw_p_t2ptN = (raw_fido_pkt_hdr_t2ptN *) raw_hdr;

    memset(raw_hdr, '\0', sizeof(raw_hdr));
    fread(raw_hdr, 1, sizeof(raw_hdr), stream);
    clear_pkt_hdr(h);

    /* obtain packet type */

    get_pkt_type(raw_p_t2, h);

    switch (h->type)
    {
    case 2:
        if (is_pkt_type_2pt2(raw_p_t2pt2, h))
        {
            /* it's a Type 2.2 packet */
            h->state = STATE_OK;
            h->ptype = strdup("2.2");
        }
        else if (is_pkt_type_2p(raw_p_t2p, h))
        {
            /* it's a Type 2+ packet */
            h->state = STATE_OK;
            h->ptype = strdup("2+");
        }
        else if (is_pkt_type_2ptN(raw_p_t2ptN, h))
        {
            /* it's a Type 2.N packet */
            h->state = STATE_OK;
            h->ptype = strdup("2.N");
        }
        else
        {
            /* must be a Type 2 (StoneAge) packet */
            set_pkt_type_2(raw_p_t2, h);
            h->state = STATE_OK;
            h->ptype = strdup("StoneAge");
        }
        return h->state;

    default:
        /* it's an alien packet type! */
        h->state = STATE_BAD;
        h->ptype = strdup("Unknown");
        return h->state;
    }
}

int write_pkt_hdr(FILE * stream, pkt_hdr_t * h)
{
    char szdomain[50], tmpstr[250];
    int zone, net, node, point;
    int hasDomain, hasZone, hasNet, hasNode, hasPoint;
    unsigned char raw_hdr[MAX_PKT_HDR];
    raw_fido_pkt_hdr_t2p *raw_p_t2p;
    struct tm *time_now;
    time_t secs_now;

    /* assume Type 2+ for now */
    raw_p_t2p = (raw_fido_pkt_hdr_t2p *) raw_hdr;

    memset(raw_hdr, '\0', sizeof(raw_hdr));

    strcpy(tmpstr, h->origAddr);
    prseaddr(tmpstr, szdomain, &hasDomain, &zone, &hasZone, &net, &hasNet, &node, &hasNode, &point, &hasPoint);

    if (!hasZone)
    {
        zone = 0;
    }
    
    if (!hasNet)
    {
        net = 0;
    }
    
    if (!hasNode)
    {
        node = 0;
    }
    
    if (!hasPoint)
    {
        point = 0;
    }
    
    if (!hasDomain)
    {
        *(szdomain) = '\0';
    }

    raw_p_t2p->origZone[0] = (unsigned char)(zone & 0xff);
    raw_p_t2p->origZone[1] = (unsigned char)((zone >> 8) & 0xff);
    raw_p_t2p->QMOrigZone[0] = (unsigned char)(zone & 0xff);
    raw_p_t2p->QMOrigZone[1] = (unsigned char)((zone >> 8) & 0xff);
    raw_p_t2p->origNode[0] = (unsigned char)(node & 0xff);
    raw_p_t2p->origNode[1] = (unsigned char)((node >> 8) & 0xff);
    raw_p_t2p->origNet[0] = (unsigned char)(net & 0xff);
    raw_p_t2p->origNet[1] = (unsigned char)((net >> 8) & 0xff);
    raw_p_t2p->origPoint[0] = (unsigned char)(point & 0xff);
    raw_p_t2p->origPoint[1] = (unsigned char)((point >> 8) & 0xff);

    strcpy(tmpstr, h->destAddr);
    prseaddr(tmpstr, szdomain, &hasDomain, &zone, &hasZone, &net, &hasNet, &node, &hasNode, &point, &hasPoint);

    if (!hasZone)
    {
        zone = 0;
    }
    
    if (!hasNet)
    {
        net = 0;
    }
    
    if (!hasNode)
    {
        node = 0;
    }
    
    if (!hasPoint)
    {
        point = 0;
    }
    
    if (!hasDomain)
    {
        *(szdomain) = '\0';
    }

    raw_p_t2p->destZone[0] = (unsigned char)(zone & 0xff);
    raw_p_t2p->destZone[1] = (unsigned char)((zone >> 8) & 0xff);
    raw_p_t2p->QMDestZone[0] = (unsigned char)(zone & 0xff);
    raw_p_t2p->QMDestZone[1] = (unsigned char)((zone >> 8) & 0xff);
    raw_p_t2p->destNode[0] = (unsigned char)(node & 0xff);
    raw_p_t2p->destNode[1] = (unsigned char)((node >> 8) & 0xff);
    raw_p_t2p->destNet[0] = (unsigned char)(net & 0xff);
    raw_p_t2p->destNet[1] = (unsigned char)((net >> 8) & 0xff);
    raw_p_t2p->destPoint[0] = (unsigned char)(point & 0xff);
    raw_p_t2p->destPoint[1] = (unsigned char)((point >> 8) & 0xff);

    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);
    h->tm = *time_now;

    raw_p_t2p->year[0] = (unsigned char)((h->tm.tm_year + 1900) & 0xff);
    raw_p_t2p->year[1] = (unsigned char)(((h->tm.tm_year + 1900) >> 8) & 0xff);
    raw_p_t2p->month[0] = (unsigned char)(h->tm.tm_mon & 0xff);
    raw_p_t2p->month[1] = (unsigned char)((h->tm.tm_mon >> 8) & 0xff);
    raw_p_t2p->day[0] = (unsigned char)(h->tm.tm_mday & 0xff);
    raw_p_t2p->day[1] = (unsigned char)((h->tm.tm_mday >> 8) & 0xff);
    raw_p_t2p->hour[0] = (unsigned char)(h->tm.tm_hour & 0xff);
    raw_p_t2p->hour[1] = (unsigned char)((h->tm.tm_hour >> 8) & 0xff);
    raw_p_t2p->minute[0] = (unsigned char)(h->tm.tm_min & 0xff);
    raw_p_t2p->minute[1] = (unsigned char)((h->tm.tm_min >> 8) & 0xff);
    raw_p_t2p->second[0] = (unsigned char)(h->tm.tm_sec & 0xff);
    raw_p_t2p->second[1] = (unsigned char)((h->tm.tm_sec >> 8) & 0xff);

    raw_p_t2p->packetType[0] = (unsigned char)(2 & 0xff);
    raw_p_t2p->packetType[1] = (unsigned char)((2 >> 8) & 0xff);

    if (h->password)
    {
        strncpy((char *)raw_p_t2p->password, h->password, 8);
    }

    raw_p_t2p->prodRevMaj = 1;
    raw_p_t2p->prodRevMin = 10;

    raw_p_t2p->capValid[0] = (unsigned char)((0x0001 >> 8) & 0xff);
    raw_p_t2p->capValid[1] = (unsigned char)(0x0001 & 0xff);
    raw_p_t2p->capWord[0] = raw_p_t2p->capValid[1];
    raw_p_t2p->capWord[1] = raw_p_t2p->capValid[0];

    fwrite(raw_hdr, 1, sizeof(raw_hdr), stream);

    return STATE_OK;
}

void write_pkt_term(FILE * stream)
{
    char pktterm[2];
    memset(pktterm, '\0', sizeof(pktterm));
    fwrite(pktterm, 1, sizeof(pktterm), stream);
}

int is_addr_equal(char *addr1, char *addr2)
{
    return (stricmp(addr1, addr2) == 0);
}

int is_pwd_equal(char *pwd1, char *pwd2)
{
    return (strnicmp(pwd1, pwd2, 8) == 0);
}

char *unique_dec_pkt_name(void)
{
    /* decimal-style packet names; similar to EzyMail */
    static char filename[10];
    struct tm *time_now;
    static time_t secs_now;

    if (secs_now == 0)
    {
        tzset();
        time(&secs_now);
    }

    time_now = localtime(&secs_now);
    strftime(filename, sizeof(filename), "%d%H%M%S", time_now);
    secs_now++;

    return filename;
}

char *unique_hex_pkt_name(void)
{
    /* hexadecimal-style packet names; similar to Squish */
    static long starter;
    static char filename[10];

    if (starter == 0)
    {
        starter = (long)time(NULL);
    }
    
    sprintf(filename, "%08lx", starter++);
    return filename;
}

char *unique_hex_pkt_name2(void)
{
    /*
     *  hexadecimal-style packet names; similar to Squish but quite not the
     *  same as unique_hex_pkt_name() ;-)
     */

    static long starter;
    static char filename[20];

    if (starter == 0)
    {
        starter = (long)time(NULL) << 8;
    }
    
    sprintf(filename, "%08lx", starter++);
    return filename;
}

FILE *open_or_create_pkt(pkt_hdr_t * hdr, char *outbound_path,
                         char *override_fn)
{
    int done;
#ifdef __EMX__
    struct _find ff;
#else
    struct find_t ff;
#endif
    char pkt_spec[250], pkt_fn[250], pktterm[2];
    char tmpstr[300];
    pkt_hdr_t hdr2;
    FILE *fp;
    long curpos;
    int state;

    memset(pktterm, '\0', sizeof(pktterm));
    clear_pkt_hdr(&hdr2);
    fp = NULL;

    /* first, check if packet exists and matches our criteria */

    if (!override_fn)
    {
        strcpy(pkt_spec, outbound_path);
        strcat(pkt_spec, "*.PKT");
    }
    else
    {
        strcpy(pkt_spec, override_fn);
    }

#ifdef __EMX__
    done = __findfirst(pkt_spec, _A_NORMAL, &ff);
#else
    done = _dos_findfirst(pkt_spec, _A_NORMAL, &ff);
#endif

    while (!done)
    {
        if (!fp)
        {
            if (!override_fn)
            {
                strcpy(pkt_fn, outbound_path);
                strcat(pkt_fn, ff.name);
            }
            else
            {
                strcpy(pkt_fn, override_fn);
            }

            fp = fopen(pkt_fn, "r+b");

            if (fp)
            {
                state = read_pkt_hdr(fp, &hdr2);

                if (state == STATE_OK)
                {
                    /* compare packet origin and dest addresses */
                    if (!is_addr_equal(hdr->origAddr, hdr2.origAddr) &&
			!is_addr_equal(hdr->destAddr, hdr2.destAddr))
                    {
                        /* packet is not what we wanted */
                        fclose(fp);
                        fp = NULL;
                    }

                    /* Note: if fp != NULL then we have the packet we want! */

                    if (hdr2.ptype)
		    {
                        free(hdr2.ptype);
		    }
		    
                    if (hdr2.origAddr)
		    {
                        free(hdr2.origAddr);
		    }
		    
                    if (hdr2.destAddr)
		    {
                        free(hdr2.destAddr);
		    }
		    
                    if (hdr2.password)
		    {
                        free(hdr2.password);
		    }
                }
                else
                {
                    log_printf("!Error! Unhandled internal exception in read_pkt_hdr() (state=%d)", state);
                }
            }
            else
            {
                sprintf(tmpstr, "!Error opening outbound packet \"%s\": %s", pkt_fn, strerror(errno));
                *(tmpstr + strlen(tmpstr) - 1) = '\0';
                log_printf(tmpstr);
            }
        }

#ifdef __EMX__
        done = __findnext(&ff);
#else
        done = _dos_findnext(&ff);
#endif
    }

    if (fp)
    {
        /* update an existing packet */

        /* seek to end of packet */
        fseek(fp, 0L, SEEK_END);

        /* seek 2 bytes backwards; behind terminator */
        curpos = ftell(fp);
        fseek(fp, curpos - sizeof(pktterm), SEEK_SET);
    }
    else
    {
        /* create a new packet */

        if (override_fn)
        {
            strcpy(pkt_fn, override_fn);
        }
        else
        {
            strcpy(pkt_fn, outbound_path);
            /* strcat(pkt_fn, unique_pkt_name()); */
            strcat(pkt_fn, unique_hex_pkt_name());
            /* strcat(pkt_fn, unique_hex_pkt_name2()); */
            strcat(pkt_fn, ".pkt");
        }

        fp = fopen(pkt_fn, "wb");

        if (fp)
        {
            state = write_pkt_hdr(fp, hdr);

            if (state == STATE_OK)
            {
                write_pkt_term(fp);
                /* seek 2 bytes backwards; behind terminator */
                curpos = ftell(fp);
                fseek(fp, curpos - sizeof(pktterm), SEEK_SET);
                log_printf(": Sending mail to %s", hdr->destAddr);
            }
            else
            {
                log_printf("!Error! Unhandled internal exception in read_pkt_hdr() (state=%d)", state);
            }
        }
        else
        {
            sprintf(tmpstr, "!Error creating outbound packet \"%s\": %s", pkt_fn, strerror(errno));
            *(tmpstr + strlen(tmpstr) - 1) = '\0';
            log_printf(tmpstr);
        }
    }

    return fp;
}
