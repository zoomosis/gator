#include <stdio.h>
#include <string.h>
#include "slist.h"
#include "gtrmask.h"
#include "gtrdmn.h"
#include "gator.h"

int is_domain_equal(char *domain1, char *domain2)
{
    return (strnicmp(domain1, domain2, 8) == 0);
}

char *resolve_domain(unsigned short zone, unsigned short net,
                     unsigned short node, unsigned short point)
{
    slistnode_t *nodep;
    domain_t *domain;
    addr_mask_4d_t addr_mask_4d;
    addr_match_t is_matched;

    nodep = TraverseSListFirst(sw.sl_domain_match);

    while (nodep)
    {
        domain = GetSListElement(nodep);
        addr_mask_4d = domain->addr_mask_4d;

        is_matched.zone = (addr_mask_4d.all_zones || zone == addr_mask_4d.zone);
        is_matched.net = (addr_mask_4d.all_nets || net == addr_mask_4d.net);
        is_matched.node = (addr_mask_4d.all_nodes || node == addr_mask_4d.node);
        is_matched.point = (addr_mask_4d.all_points || point == addr_mask_4d.point);

        if (is_matched.zone && is_matched.net && is_matched.node && is_matched.point)
        {
            /* the domain was resolved! */
            return domain->name;
        }

        nodep = TraverseSListNext(nodep);
    }

    /* fell through; no domain resolution possible (address will be 4D) */

    return NULL;
}
