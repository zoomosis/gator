#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "statlog.h"
#include "ctlparse.h"
#include "slist.h"
#include "gtrlog.h"
#include "gtrmask.h"
#include "gtrdmn.h"
#include "gtrutil.h"
#include "gator.h"

#define BLK_NONE      0
#define BLK_ECHOGATE  1
#define BLK_NODE      2

static int cfg_block = BLK_NONE;
static node_mgr_t *node_mgr;
static echo_gate_t *echo_gate;

static char *pGatorCfgArgs[] =
{
    "Inbound",
    "Outbound",
    "LogFile",
    "LogFormat",
    "DomainMatch",
    "Begin",
    "End",

    /* Begin EchoGate */

    "ImportName",
    "InPktOrig",
    "InPktDest",
    "ExportName",
    "ExportPktOrig",
    "ExportPktDest",
    "Origin",
    "OriginAddr",
    "ExportSeenBy",
    "ExportPath",
    /* End EchoGate */

    "App",

    /* Begin Node */
    "Address",
    "PktType",
    "Password",
    "ExportPkt",
    /* End Node */

    NULL
};

#define CTL_INBOUND       	1
#define CTL_OUTBOUND      	2
#define CTL_LOG_FILE      	3
#define CTL_LOG_FORMAT    	4
#define CTL_DOMAIN_MATCH  	5
#define CTL_BLOCK_BEGIN   	6
#define CTL_BLOCK_END           7

/* Begin EchoGate */

#define CTL_EG_IMPORT_NAME      8
#define CTL_EG_IN_PKT_ORIG  	9
#define CTL_EG_IN_PKT_DEST  	10
#define CTL_EG_EXPORT_NAME  	11
#define CTL_EG_EXPORT_PKT_ORIG  12
#define CTL_EG_EXPORT_PKT_DEST	13
#define CTL_EG_ORIGIN		14
#define CTL_EG_ORIGIN_ADDR	15
#define CTL_EG_EXPORT_SEEN_BY	16
#define CTL_EG_EXPORT_PATH	17

/* End EchoGate */

#define CTL_APP                 18

/* Begin Node */

#define CTL_N_ADDRESS           19
#define CTL_N_PKT_TYPE     	20
#define CTL_N_PASSWORD     	21
#define CTL_N_EXPORT_PKT        22

/* End Node */

int parse_log_format(char *fmt)
{
    switch (*fmt)
    {
    case 'O':                  /* Opus */
    case 'B':                  /* BinkleyTerm */
    case 'S':                  /* Squish */
        return LOG_STY_OPUS;
    case 'F':                  /* FrontDoor */
        return LOG_STY_FRODO;
    case 'D':                  /* D'Bridge */
        return LOG_STY_DBRG;
    default:
        return LOG_STY_OPUS;
    }
}

void cfg_inbound(char *path)
{
    slistnode_t *node;
    char tmpstr[250];
    char *inbound_path;
    strcpy(tmpstr, path);
    add_backslash(tmpstr);
    inbound_path = strdup(tmpstr);
    node = CreateSListNode(inbound_path);
    AddSListNode(sw.sl_inbound, node);
}

void cfg_domain_match(char *val)
{
    slistnode_t *node;
    domain_t *domain;
    char *name, *p;
    addr_mask_4d_t addr_mask_4d;

    p = strtok(val, " \t");
    name = strdup(p);

    p = strtok(NULL, " \t");

    if (p == NULL)
    {
        log_printf("!> Error! The DomainMatch command requires both an");
        log_printf("!> address resolution domain and address mask, eg.");
        log_printf("!> `DomainMatch fidonet 3:All/All.All'");
        free(name);
        clean_exit(EXIT_FAILURE);
    }
    else
    {
        while (p != NULL)
        {
            domain = malloc(sizeof(*domain));
            domain->name = strdup(name);
            parse_addr_mask_4d(p, &addr_mask_4d);
            domain->addr_mask_4d = addr_mask_4d;
            node = CreateSListNode(domain);
            AddSListNode(sw.sl_domain_match, node);
            p = strtok(NULL, " \t");
        }
    }

    free(name);
}

int cfg_eg_import_name(char *val)
{
    if (cfg_block != BLK_ECHOGATE)
    {
        return 0;
    }
    
    echo_gate->in_name = strdup(val);
    return 1;
}

int cfg_eg_in_pkt_orig(char *val)
{
    slistnode_t *node;
    addr_mask_5d_t *addr_mask_5d;
    char *p;

    if (cfg_block != BLK_ECHOGATE)
    {
        return 0;
    }
    
    p = strtok(val, " \t");

    while (p != NULL)
    {
        addr_mask_5d = malloc(sizeof(*addr_mask_5d));
        parse_addr_mask_5d(p, addr_mask_5d);
        node = CreateSListNode(addr_mask_5d);
        AddSListNode(echo_gate->in_pkt_orig, node);
        p = strtok(NULL, " \t");
    }

    return 1;
}

int cfg_eg_in_pkt_dest(char *val)
{
    slistnode_t *node;
    addr_mask_5d_t *addr_mask_5d;
    char *p;

    if (cfg_block != BLK_ECHOGATE)
    {
        return 0;
    }
    
    p = strtok(val, " \t");

    while (p != NULL)
    {
        addr_mask_5d = malloc(sizeof(*addr_mask_5d));
        parse_addr_mask_5d(p, addr_mask_5d);
        node = CreateSListNode(addr_mask_5d);
        AddSListNode(echo_gate->in_pkt_dest, node);
        p = strtok(NULL, " \t");
    }

    return 1;
}

int cfg_eg_export_name(char *val)
{
    if (cfg_block != BLK_ECHOGATE)
    {
        return 0;
    }
    
    echo_gate->out_name = strdup(val);
    return 1;
}

int cfg_eg_export_pkt_orig(char *val)
{
    if (cfg_block != BLK_ECHOGATE)
    {
        return 0;
    }

    echo_gate->out_pkt_orig = strdup(val);
    return 1;
}

int cfg_eg_export_pkt_dest(char *val)
{
    slistnode_t *node;
    addr_mask_5d_t *addr_mask_5d;
    char *p;

    if (cfg_block != BLK_ECHOGATE)
    {
        return 0;
    }
    
    p = strtok(val, " \t");
    
    while (p != NULL)
    {
        addr_mask_5d = malloc(sizeof(*addr_mask_5d));
        parse_addr_mask_5d(p, addr_mask_5d);
        node = CreateSListNode(addr_mask_5d);
        AddSListNode(echo_gate->out_pkt_dest, node);
        p = strtok(NULL, " \t");
    }

    return 1;
}

int cfg_eg_origin(char *val)
{
    if (cfg_block != BLK_ECHOGATE)
    {
        return 0;
    }
    
    echo_gate->out_origin_txt = strdup(val);
    return 1;
}

int cfg_eg_origin_addr(char *val)
{
    if (cfg_block != BLK_ECHOGATE)
    {
        return 0;
    }
    
    echo_gate->out_origin_addr = strdup(val);
    return 1;
}

int cfg_eg_export_seen_by(char *val)
{
    if (cfg_block != BLK_ECHOGATE)
    {
        return 0;
    }
    
    echo_gate->out_seenby = strdup(val);
    return 1;
}

int cfg_eg_export_path(char *val)
{
    if (cfg_block != BLK_ECHOGATE)
    {
        return 0;
    }
    
    echo_gate->out_path = strdup(val);
    return 1;
}

int cfg_n_address(char *val)
{
    if (cfg_block != BLK_NODE)
    {
        return 0;
    }
    
    node_mgr->address = strdup(val);
    return 1;
}

int cfg_n_pkt_type(char *val)
{
    if (cfg_block != BLK_NODE)
    {
        return 0;
    }
    
    node_mgr->pkt_type = strdup(val);
    return 1;
}

int cfg_n_password(char *val)
{
    if (cfg_block != BLK_NODE)
    {
        return 0;
    }
    
    node_mgr->password = strdup(val);
    return 1;
}

int cfg_n_export_pkt(char *val)
{
    if (cfg_block != BLK_NODE)
    {
        return 0;
    }

    node_mgr->pkt_fn = strdup(val);
    return 1;
}

int cfg_block_begin(char *name)
{
    if (cfg_block != BLK_NONE)
    {
        log_printf("!Warning, `Begin %s' unexpected", name);
        return BLK_NONE;
    }

    if (stricmp(name, "EchoGate") == 0)
    {
        echo_gate = malloc(sizeof(*echo_gate));
        echo_gate->in_name = NULL;
        echo_gate->in_pkt_orig = CreateLinkedSList();
        echo_gate->in_pkt_dest = CreateLinkedSList();
        echo_gate->out_name = NULL;
        echo_gate->out_pkt_orig = NULL;
        echo_gate->out_pkt_dest = CreateLinkedSList();
        echo_gate->out_origin_txt = NULL;
        echo_gate->out_origin_addr = NULL;
        echo_gate->out_seenby = NULL;
        echo_gate->out_path = NULL;
        return BLK_ECHOGATE;
    }
    else if (stricmp(name, "Node") == 0)
    {
        node_mgr = malloc(sizeof(*node_mgr));
        node_mgr->address = NULL;
        node_mgr->pkt_type = NULL;
        node_mgr->pkt_fn = NULL;
        node_mgr->password = NULL;
        return BLK_NODE;
    }

    /* fell through; it's an alien block name! */
    log_printf("!Warning, `Begin %s': invalid block name", name);
    return BLK_NONE;
}

int cfg_block_end(char *name)
{
    char *blk_name;

    switch (cfg_block)
    {
    case BLK_NONE:
        log_printf("!Warning, `End %s' unexpected", name);
        return BLK_NONE;

    case BLK_ECHOGATE:
        blk_name = "EchoGate";

        if (stricmp(name, blk_name) == 0)
        {
            slistnode_t *nodep;
            nodep = CreateSListNode(echo_gate);
            AddSListNode(sw.sl_echo_gate, nodep);
            return BLK_NONE;
        }
        else
	{
            break;
	}

    case BLK_NODE:
        blk_name = "Node";

        if (stricmp(name, blk_name) == 0)
        {
            slistnode_t *nodep;
            nodep = CreateSListNode(node_mgr);
            AddSListNode(sw.sl_node_mgr, nodep);
            return BLK_NONE;
        }
        else
	{
            break;
	}
	
    default:
        /* fell through; it's an alien block name! */
        log_printf("!Warning, `End %s': invalid block name", name);
        return BLK_NONE;
    }

    /* fell through; unexpected block name! */

    log_printf("!Warning, `End %s' expected, got `End %s'", blk_name, name);
    return cfg_block;
}

void cfg_invalid_cmd(char *arg, int line)
{
    log_printf("!Invalid command in config file \"%s\" (line %d): \"%s\"", sw.cfg_fn, line, arg);
}

void cfg_proc(char *arg, char *val, int status, int line)
{
    char tmpstr[300];

    switch (status)
    {
    case CTL_NOFILE:
        log_printf("!Can't find config file \"%s\"", sw.cfg_fn);
        clean_exit(EXIT_FAILURE);
        break;

    case CTL_READFAILURE:
        sprintf(tmpstr, "!Error reading config file \"%s\" (line %d): %s", sw.cfg_fn, line, strerror(errno));
        *(tmpstr + strlen(tmpstr) - 1) = '\0';
        log_printf(tmpstr);
        clean_exit(EXIT_FAILURE);
        break;

    case CTL_ENDOFFILE:
        break;

    case CTL_LINETOOLONG:
        log_printf("!Error reading config file \"%s\" (line %d): %s", sw.cfg_fn, line, "line too long");
        clean_exit(EXIT_FAILURE);
        break;

    case CTL_MISSINGPARAM:
        log_printf("!Missing parameter in config file \"%s\" (line %d): \"%s\"", sw.cfg_fn, line, arg);
        clean_exit(EXIT_FAILURE);
        break;

    case CTL_ILLEGALARG:
        cfg_invalid_cmd(arg, line);
        break;

    case CTL_INVCASEFLAG:
        log_printf("!Error reading config file \"%s\" (line %d): %s", sw.cfg_fn, line, "invalid case_flag");
        clean_exit(EXIT_FAILURE);
        break;

    /* valid gator.cfg commands */

    case CTL_INBOUND:
        cfg_inbound(val);
        break;

    case CTL_OUTBOUND:
        strcpy(tmpstr, val);
        add_backslash(tmpstr);
        sw.outbound_path = strdup(tmpstr);
        break;

    case CTL_LOG_FILE:
        if (*(sw.log_fn) == '\0')
	{
            strcpy(sw.log_fn, val);
	}
        break;

    case CTL_LOG_FORMAT:
        sw.log_fmt = parse_log_format(val);
        break;

    case CTL_DOMAIN_MATCH:
        cfg_domain_match(val);
        break;

    case CTL_APP:
        break;

    case CTL_BLOCK_BEGIN:
        cfg_block = cfg_block_begin(val);
        break;

    case CTL_BLOCK_END:
        cfg_block = cfg_block_end(val);
        break;

    /* Begin EchoGate */
    case CTL_EG_IMPORT_NAME:
        if (!cfg_eg_import_name(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_EG_IN_PKT_ORIG:
        if (!cfg_eg_in_pkt_orig(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_EG_IN_PKT_DEST:
        if (!cfg_eg_in_pkt_dest(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_EG_EXPORT_NAME:
        if (!cfg_eg_export_name(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_EG_EXPORT_PKT_ORIG:
        if (!cfg_eg_export_pkt_orig(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_EG_EXPORT_PKT_DEST:
        if (!cfg_eg_export_pkt_dest(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_EG_ORIGIN:
        if (!cfg_eg_origin(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_EG_ORIGIN_ADDR:
        if (!cfg_eg_origin_addr(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_EG_EXPORT_SEEN_BY:
        if (!cfg_eg_export_seen_by(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_EG_EXPORT_PATH:
        if (!cfg_eg_export_path(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;
    /* End EchoGate */

    /* Begin Node */
    case CTL_N_ADDRESS:
        if (!cfg_n_address(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_N_PKT_TYPE:
        if (!cfg_n_pkt_type(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;

    case CTL_N_PASSWORD:
        if (!cfg_n_password(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;
	
    case CTL_N_EXPORT_PKT:
        if (!cfg_n_export_pkt(val))
	{
            cfg_invalid_cmd(arg, line);
	}
        break;
    /* End Node */

    default:
        log_printf("!Error reading config file \"%s\" (line %d): %s (status=%d)",
          sw.cfg_fn, line, "unhandled internal exception in cfg_proc()", status);
        clean_exit(EXIT_FAILURE);
        break;
    }
}

void get_config(void)
{
    FILE *fp;
    fp = fopen(sw.cfg_fn, "r");
    parse_ctl_file(cfg_proc, pGatorCfgArgs, fp, CTL_IGNORECASE);
    fclose(fp);
    putchar('\n');
    log_open();
}
