/*********************************************************************/
/*                                                                   */
/*  This Program Written by Paul Edwards, 3:711/934@fidonet.         */
/*  Released to the Public Domain                                    */
/*                                                                   */
/*********************************************************************/
/*********************************************************************/
/*                                                                   */
/*  prseaddr - parse a Fidonet address                               */
/*                                                                   */
/*********************************************************************/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "prseaddr.h"

int prseaddr(char *buf, 
             char *domain, 
             int *hasDomain,
             int *zone,
             int *hasZone,
             int *net,
             int *hasNet,
             int *node,
             int *hasNode,
             int *point,
             int *hasPoint)
{
    char *oldpos;
    char *pos;
    char *pos2;
    int ch;

    oldpos = buf;
    while (isgraph(*oldpos))
    {
        oldpos++;
    }
    ch = *oldpos;
    *oldpos = '\0';
    *hasDomain = *hasZone = *hasNet = *hasNode = *hasPoint = 0;
    pos = strchr(buf, '@');
    if (pos != NULL)
    {
        strcpy(domain, pos + 1);
        *hasDomain = 1;
    }
    pos = strchr(buf, ':');
    if (pos != NULL)
    {
        *zone = atoi(buf);
        *hasZone = 1;
        pos++;
    }
    else
    {
        pos = buf;
    }
    pos2 = strchr(pos, '/');
    if (pos2 != NULL)
    {
       *net = atoi(pos);
       *hasNet = 1;
       pos = pos2 + 1;
    }
    else
    {
       pos = buf;
    }
    if (isdigit(*pos))
    {
        *node = atoi(pos);
        *hasNode = 1;
    }
    pos = strchr(pos, '.');
    if (pos != NULL)
    {
        *point = atoi(pos+1);
        *hasPoint = 1;
    }
    *oldpos = (char)ch;
    return (0);
}                   

