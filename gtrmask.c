#include <stdio.h>
#include <string.h>
#include "stristr.h"
#include "prseaddr.h"
#include "gtrmask.h"
#include "gtrdmn.h"

int parse_addr_mask_4d(char *mask, addr_mask_4d_t * addr_mask_4d)
{
    char szdomain[50], tmpstr[250];
    int zone, net, node, point;
    int hasDomain, hasZone, hasNet, hasNode, hasPoint;

    addr_mask_4d->all_zones = (stristr(mask, "*:") == mask || stristr(mask, "All:") == mask);
    addr_mask_4d->all_nets = (stristr(mask, "*/") != NULL || stristr(mask, "All/") != NULL);
    addr_mask_4d->all_nodes = (stristr(mask, "*.") != NULL || stristr(mask, "/All") != NULL);
    addr_mask_4d->all_points = (stristr(mask, ".*") != NULL || stristr(mask, ".All") != NULL);

    strcpy(tmpstr, mask);

    prseaddr(tmpstr, szdomain, &hasDomain, &zone, &hasZone,
	     &net, &hasNet, &node, &hasNode, &point, &hasPoint);

    if (hasZone)
    {
        addr_mask_4d->zone = (unsigned short)zone;
    }
    else
    {
        addr_mask_4d->zone = 0;
    }
    
    if (hasNet)
    {
        addr_mask_4d->net = (unsigned short)net;
    }
    else
    {
        addr_mask_4d->net = 0;
    }

    if (hasNode)
    {
        addr_mask_4d->node = (unsigned short)node;
    }
    else
    {
        addr_mask_4d->node = 0;
    }
    
    if (hasPoint)
    {
        addr_mask_4d->point = (unsigned short)point;
    }
    else
    {
        addr_mask_4d->point = 0;
    }

    return 0;
}

char *addr_mask_4d_to_str(addr_mask_4d_t * addr_mask_4d)
{
    static char mask[100];
    char tmpstr[25];

    *(mask) = '\0';

    if (addr_mask_4d->all_zones)
    {
        strcat(mask, "All:");
    }
    else
    {
        sprintf(tmpstr, "%hu:", addr_mask_4d->zone);
        strcat(mask, tmpstr);
    }

    if (addr_mask_4d->all_nets)
    {
        strcat(mask, "All/");
    }
    else
    {
        sprintf(tmpstr, "%hu/", addr_mask_4d->net);
        strcat(mask, tmpstr);
    }

    if (addr_mask_4d->all_nodes)
    {
        strcat(mask, "All");
    }
    else
    {
        sprintf(tmpstr, "%hu", addr_mask_4d->node);
        strcat(mask, tmpstr);
    }

    if (addr_mask_4d->all_points)
    {
        strcat(mask, ".All");
    }
    else
    {
        sprintf(tmpstr, ".%hu", addr_mask_4d->point);
        strcat(mask, tmpstr);
    }

    return mask;
}

int parse_addr_mask_5d(char *mask, addr_mask_5d_t * addr_mask_5d)
{
    char szdomain[50], tmpstr[250];
    int zone, net, node, point;
    int hasDomain, hasZone, hasNet, hasNode, hasPoint;

    addr_mask_5d->all_zones = (stristr(mask, "*:") == mask || stristr(mask, "All:") == mask);
    addr_mask_5d->all_nets = (stristr(mask, "*/") != NULL || stristr(mask, "All/") != NULL);
    addr_mask_5d->all_nodes = (stristr(mask, "*.") != NULL || stristr(mask, "/All") != NULL);
    addr_mask_5d->all_points = (stristr(mask, ".*") != NULL || stristr(mask, ".All") != NULL);
    addr_mask_5d->all_domains = (stristr(mask, "@*") != NULL || stristr(mask, "@All") != NULL);

    strcpy(tmpstr, mask);

    prseaddr(tmpstr, szdomain, &hasDomain, &zone, &hasZone,
             &net, &hasNet, &node, &hasNode, &point, &hasPoint);

    if (!addr_mask_5d->all_domains && hasDomain)
    {
        addr_mask_5d->domain = strdup(szdomain);
    }
    else
    {
        addr_mask_5d->domain = NULL;
    }

    if (hasZone)
    {
        addr_mask_5d->zone = (unsigned short)zone;
    }
    else
    {
        addr_mask_5d->zone = 0;
    }

    if (hasNet)
    {
        addr_mask_5d->net = (unsigned short)net;
    }
    else
    {
        addr_mask_5d->net = 0;
    }

    if (hasNode)
    {
        addr_mask_5d->node = (unsigned short)node;
    }
    else
    {
        addr_mask_5d->node = 0;
    }

    if (hasPoint)
    {
        addr_mask_5d->point = (unsigned short)point;
    }
    else
    {
        addr_mask_5d->point = 0;
    }

    return 0;
}

char *addr_mask_5d_to_str(addr_mask_5d_t * addr_mask_5d)
{
    static char mask[100];
    char tmpstr[25];

    *(mask) = '\0';

    if (addr_mask_5d->all_zones)
    {
        strcat(mask, "All:");
    }
    else
    {
        sprintf(tmpstr, "%hu:", addr_mask_5d->zone);
        strcat(mask, tmpstr);
    }

    if (addr_mask_5d->all_nets)
    {
        strcat(mask, "All/");
    }
    else
    {
        sprintf(tmpstr, "%hu/", addr_mask_5d->net);
        strcat(mask, tmpstr);
    }

    if (addr_mask_5d->all_nodes)
    {
        strcat(mask, "All");
    }
    else
    {
        sprintf(tmpstr, "%hu", addr_mask_5d->node);
        strcat(mask, tmpstr);
    }

    if (addr_mask_5d->all_points)
    {
        strcat(mask, ".All");
    }
    else
    {
        sprintf(tmpstr, ".%hu", addr_mask_5d->point);
        strcat(mask, tmpstr);
    }

    if (addr_mask_5d->all_domains)
    {
        strcat(mask, "@All");
    }
    else
    {
        sprintf(tmpstr, "@%s", addr_mask_5d->domain);
        strcat(mask, tmpstr);
    }

    return mask;
}

int cmp_addr_mask_4d(char *address, addr_mask_4d_t * addr_mask_4d)
{
    char szdomain[50], tmpstr[250];
    int zone, net, node, point;
    int hasDomain, hasZone, hasNet, hasNode, hasPoint;
    addr_match_t is_matched;

    strcpy(tmpstr, address);
    prseaddr(tmpstr, szdomain, &hasDomain, &zone, &hasZone,
             &net, &hasNet, &node, &hasNode, &point, &hasPoint);

    if (!hasPoint)
    {
        hasPoint = 1;
        point = 0;
    }

    is_matched.zone = (addr_mask_4d->all_zones || (hasZone && (unsigned short)zone == addr_mask_4d->zone));
    is_matched.net = (addr_mask_4d->all_nets || (hasNet && (unsigned short)net == addr_mask_4d->net));
    is_matched.node = (addr_mask_4d->all_nodes || (hasNode && (unsigned short)node == addr_mask_4d->node));
    is_matched.point = (addr_mask_4d->all_points || (hasPoint && (unsigned short)point == addr_mask_4d->point));

    return (is_matched.zone && is_matched.net && is_matched.node && is_matched.point);
}

int cmp_addr_mask_5d(char *address, addr_mask_5d_t * addr_mask_5d)
{
    char szdomain[50], tmpstr[250];
    int zone, net, node, point;
    int hasDomain, hasZone, hasNet, hasNode, hasPoint;
    addr_match_t is_matched;

    strcpy(tmpstr, address);
    prseaddr(tmpstr, szdomain, &hasDomain, &zone, &hasZone,
             &net, &hasNet, &node, &hasNode, &point, &hasPoint);

    if (!hasPoint)
    {
        hasPoint = 1;
        point = 0;
    }

    is_matched.domain = (addr_mask_5d->all_domains || (addr_mask_5d->domain && hasDomain && is_domain_equal(szdomain, addr_mask_5d->domain)));
    is_matched.zone = (addr_mask_5d->all_zones || (hasZone && (unsigned short)zone == addr_mask_5d->zone));
    is_matched.net = (addr_mask_5d->all_nets || (hasNet && (unsigned short)net == addr_mask_5d->net));
    is_matched.node = (addr_mask_5d->all_nodes || (hasNode && (unsigned short)node == addr_mask_5d->node));
    is_matched.point = (addr_mask_5d->all_points || (hasPoint && (unsigned short)point == addr_mask_5d->point));

    return (is_matched.domain && is_matched.zone && is_matched.net && is_matched.node && is_matched.point);
}

#ifdef TEST_ADDRMASK

int main(void)
{
    /* test code: compare an address with an address mask */
    char addr[50], mask[50];
    addr_mask_5d_t  addr_mask_5d;
    printf("Enter an address : ");
    scanf("%40s", addr);
    fflush(stdin);
    printf("Enter a mask     : ");
    scanf("%40s", mask);
    fflush(stdin);
    parse_addr_mask_5d(mask, &addr_mask_5d);
    printf("= *** DEBUG: cmp_addr_mask_5d(`%s', `%s'[mask]: %d\n", addr, mask, cmp_addr_mask_5d(addr, addr_mask_5d));
    return 0;
}

#endif
