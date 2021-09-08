#include <stdio.h>
#include <string.h>
#include <time.h>
#include "state.h"
#include "ftnpkt.h"
#include "dtsplit.h"
#include "dtfido.h"
#include "prseaddr.h"
#include "gtrfido.h"
#include "gtrpmsg.h"

#define MAX_PKD_MSG_HDR  sizeof(raw_fido_pkd_msg_hdr_t2)

void clear_pkd_msg_hdr(pkd_msg_hdr_t * h)
{
    h->state = STATE_UNKNOWN;
    h->type = 0;
    h->attr = 0;
    h->origAddr = NULL;
    h->destAddr = NULL;
    h->from_user = NULL;
    h->to_user = NULL;
    h->subject = NULL;
    h->is_date_valid = 0;

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

unsigned short get_pkd_msg_type(raw_fido_pkd_msg_hdr_t2 * raw_p_t2,
                                pkd_msg_hdr_t * h)
{
    h->type = (unsigned short)((raw_p_t2->packetType[1] << 8) | raw_p_t2->packetType[0]);
    return h->type;
}

void set_pkd_msg_type_2(raw_fido_pkd_msg_hdr_t2 * raw_p_t2, pkd_msg_hdr_t * h)
{
    unsigned short origNet, origNode;
    unsigned short destNet, destNode;
    int year, mon, mday, hour, min, sec;

    h->attr = (unsigned short)((raw_p_t2->Attribute[1] << 8) | raw_p_t2->Attribute[0]);

    if (dt_split(raw_p_t2->DateTime, &year, &mon, &mday, &hour, &min, &sec) == GOOD_DATE)
    {
        h->is_date_valid = 1;
        h->tm.tm_sec = sec;
        h->tm.tm_min = min;
        h->tm.tm_hour = hour;
        h->tm.tm_mday = mday;
        h->tm.tm_mon = mon - 1;
        h->tm.tm_year = year - 1900;
        h->tm.tm_isdst = -1;

        if (mktime(&h->tm) == -1)
	{
            h->tm.tm_wday = 7;
	}
    }
    else
    {
        h->is_date_valid = 0;
    }

    origNet = (unsigned short)((raw_p_t2->origNet[1] << 8) | raw_p_t2->origNet[0]);
    origNode = (unsigned short)((raw_p_t2->origNode[1] << 8) | raw_p_t2->origNode[0]);
    h->origAddr = strdup(show_address(0, origNet, origNode, 0, NULL));

    destNet = (unsigned short)((raw_p_t2->destNet[1] << 8) | raw_p_t2->destNet[0]);
    destNode = (unsigned short)((raw_p_t2->destNode[1] << 8) | raw_p_t2->destNode[0]);
    h->destAddr = strdup(show_address(0, destNet, destNode, 0, NULL));
}

int read_pkd_msg_hdr(FILE * fp, pkd_msg_hdr_t * h)
{
    unsigned char raw_hdr[MAX_PKD_MSG_HDR];
    raw_fido_pkd_msg_hdr_t2 *raw_p_t2;

    raw_p_t2 = (raw_fido_pkd_msg_hdr_t2 *) raw_hdr;

    memset(raw_hdr, '\0', sizeof(raw_hdr));
    fread(raw_hdr, 1, sizeof(raw_hdr), fp);
    clear_pkd_msg_hdr(h);

    /* obtain packed-message type */
    get_pkd_msg_type(raw_p_t2, h);

    switch (h->type)
    {
    case 0:
        /* end-of-packet marker found */
        h->state = STATE_OK;
        return h->state;

    case 2:
        /* it's a Type 2 packed message */
        set_pkd_msg_type_2(raw_p_t2, h);
        h->state = STATE_OK;
        return h->state;

    default:
        /* it's an alien packed message type */
        h->state = STATE_BAD;
        return h->state;
    }
}

int write_pkd_msg_hdr(FILE * fp, pkd_msg_hdr_t * h, int write_fields)
{
    char szdomain[50], tmpstr[250];
    int zone, net, node, point;
    int hasDomain, hasZone, hasNet, hasNode, hasPoint;
    unsigned char raw_hdr[MAX_PKD_MSG_HDR];
    raw_fido_pkd_msg_hdr_t2 *raw_p_t2;

    raw_p_t2 = (raw_fido_pkd_msg_hdr_t2 *) raw_hdr;
    memset(raw_hdr, '\0', sizeof(raw_hdr));

    raw_p_t2->packetType[0] = (unsigned char)(2 & 0xff);
    raw_p_t2->packetType[1] = (unsigned char)((2 >> 8) & 0xff);

    strcpy(tmpstr, h->origAddr);
    prseaddr(tmpstr, szdomain, &hasDomain, &zone, &hasZone,
             &net, &hasNet, &node, &hasNode, &point, &hasPoint);

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

    raw_p_t2->origNode[0] = (unsigned char)(node & 0xff);
    raw_p_t2->origNode[1] = (unsigned char)((node >> 8) & 0xff);
    raw_p_t2->origNet[0] = (unsigned char)(net & 0xff);
    raw_p_t2->origNet[1] = (unsigned char)((net >> 8) & 0xff);

    strcpy(tmpstr, h->destAddr);
    prseaddr(tmpstr, szdomain, &hasDomain, &zone, &hasZone,
             &net, &hasNet, &node, &hasNode, &point, &hasPoint);

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

    raw_p_t2->destNode[0] = (unsigned char)(node & 0xff);
    raw_p_t2->destNode[1] = (unsigned char)((node >> 8) & 0xff);
    raw_p_t2->destNet[0] = (unsigned char)(net & 0xff);
    raw_p_t2->destNet[1] = (unsigned char)((net >> 8) & 0xff);

    raw_p_t2->Attribute[0] = (unsigned char)(h->attr & 0xff);
    raw_p_t2->Attribute[1] = (unsigned char)((h->attr >> 8) & 0xff);

    dateToFido(raw_p_t2->DateTime, 20, h->tm.tm_year, h->tm.tm_mon + 1,
               h->tm.tm_mday, h->tm.tm_hour, h->tm.tm_min, h->tm.tm_sec);

    fwrite(raw_hdr, 1, sizeof(raw_hdr), fp);

    if (write_fields)
    {
        fwrite(h->to_user, 1, strlen(h->to_user), fp);
        fputc('\0', fp);
        fwrite(h->from_user, 1, strlen(h->from_user), fp);
        fputc('\0', fp);
        fwrite(h->subject, 1, strlen(h->subject), fp);
        fputc('\0', fp);
    }

    return STATE_OK;
}
