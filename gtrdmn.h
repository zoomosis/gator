#ifndef __GTRDMN_H__
#define __GTRDMN_H__

typedef struct
{
    char *name;
    addr_mask_4d_t addr_mask_4d;
}
domain_t;

typedef struct
{
    int domain, zone, net, node, point;
}
addr_match_t;

int is_domain_equal(char *domain1, char *domain2);

char *resolve_domain(unsigned short zone, unsigned short net,
                     unsigned short node, unsigned short point);

#endif
