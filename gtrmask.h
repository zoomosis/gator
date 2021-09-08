#ifndef __GTRMASK_H__
#define __GTRMASK_H__

typedef struct
{
    unsigned short zone, net, node, point;
    int all_zones, all_nets, all_nodes, all_points;
}
addr_mask_4d_t;

typedef struct
{
    char *domain;
    unsigned short zone, net, node, point;
    int all_domains, all_zones, all_nets, all_nodes, all_points;
}
addr_mask_5d_t;

int parse_addr_mask_4d(char *mask, addr_mask_4d_t * addr_mask_4d);
int parse_addr_mask_5d(char *mask, addr_mask_5d_t * addr_mask_5d);
char *addr_mask_4d_to_str(addr_mask_4d_t * addr_mask_4d);
char *addr_mask_5d_to_str(addr_mask_5d_t * addr_mask_5d);
int cmp_addr_mask_4d(char *address, addr_mask_4d_t * addr_mask_4d);
int cmp_addr_mask_5d(char *address, addr_mask_5d_t * addr_mask_5d);

#endif
