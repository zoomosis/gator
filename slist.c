/*
*  slist.c  Singly linked-list management functions
*  (c) copyright 1995 Scott Pitcher, Andrew Clarke
*
*
*  LEGAL NOTICE
*
*  This software was written by Scott Pitcher and Andrew Clarke. It is
*  presented without copyright, so anyone may use this code for whatever
*  reason they wish without restriction, as long as they understand that
*  the authors give no guarantee, warranty, or statement of suitability
*  or fitness for use of any kind.
*
*
*  HISTORY
*
*  27-Jul-95 SVP
*  First implemented
*
*  07-Aug-95 ADC
*  Fixed bug in CreateLinkedSList(); p_list->L_end was not being
*  initialised to NULL.
*
*  Renamed functions:
*    TraverseSListStart() --> TraverseSListFirst()
*/

#include <stdlib.h>
#include "slist.h"

/*----------------------------------------------------------------------*/
/*    CreateLinkedSList();                                              */
/*                                                                      */
/*    Create and initialise a linked list                               */
/*----------------------------------------------------------------------*/

slist_t *CreateLinkedSList(void)
{
    slist_t *p_list;

    if ((p_list = malloc(sizeof(slist_t))) != NULL)
    {
        p_list->L_first = p_list->L_end = NULL;
        p_list->L_elements = 0L;
    }
    return p_list;
}

/*----------------------------------------------------------------------*/
/*    DestroyLinkedSList();                                             */
/*                                                                      */
/*    Destroy a linked list                                             */
/*----------------------------------------------------------------------*/

void DestroyLinkedSList(slist_t * p_list)
{
    slistnode_t *p_node;

    if (p_list)
    {
        if (p_list->L_elements)
        {
            p_node = p_list->L_first;
            while (p_node)
            {
                p_list->L_first = p_node->L_next;
                free(p_node);
                p_node = p_list->L_first;
            }
        }
        free(p_list);
    }
}

/*----------------------------------------------------------------------*/
/*    GetSListElement();                                                */
/*                                                                      */
/*    Return the data held in a list node.                              */
/*----------------------------------------------------------------------*/

void *GetSListElement(slistnode_t * p)
{
    if (p)
        return p->L_element;
    else
        return NULL;
}

/*----------------------------------------------------------------------*/
/*    SetSListElement();                                                */
/*                                                                      */
/*    Set the data held in a list node.                                 */
/*----------------------------------------------------------------------*/

void SetSListElement(slistnode_t * p, void *d)
{
    if (p)
        p->L_element = d;
}

/*----------------------------------------------------------------------*/
/*    AddSListNode();                                                   */
/*                                                                      */
/*    Add a node to the end of a list.                                  */
/*----------------------------------------------------------------------*/

void AddSListNode(slist_t * p_list, slistnode_t * p_node)
{
    if (p_list)
    {
        if (p_list->L_end)
            p_list->L_end = p_list->L_end->L_next = p_node;
        else
            p_list->L_first = p_list->L_end = p_node;
        p_node->L_next = NULL;
        ++p_list->L_elements;
    }
}

/*----------------------------------------------------------------------*/
/*    DropSListNode();                                                  */
/*                                                                      */
/*    Drop a node from a list.                                          */
/*----------------------------------------------------------------------*/

void DropSListNode(slist_t * p_list, slistnode_t * p_node)
{
    slistnode_t *plast, *pthis;

    if (p_list)
    {
        plast = NULL;
        pthis = p_list->L_first;
        while (pthis)
        {
            if (pthis == p_node)
            {
                /* this is the node we are going to drop */
                if (plast)
                    plast->L_next = pthis->L_next; /* link last node to
                                                      next */
                else
                    p_list->L_first = pthis->L_next; /* else this was the
                                                        first node */
                if (p_list->L_end == pthis)
                    p_list->L_end = plast; /* if last then fix the end
                                              link */
                --p_list->L_elements;
                return;
            }
            plast = pthis;
            pthis = pthis->L_next;
        }
    }
}

/*----------------------------------------------------------------------*/
/*    CreateSListNode();                                                */
/*                                                                      */
/*    Create a node element and return a pointer to it.                 */
/*----------------------------------------------------------------------*/

slistnode_t *CreateSListNode(void *p_element)
{
    slistnode_t *p_new;

    if ((p_new = malloc(sizeof(slistnode_t))) != NULL)
    {
        p_new->L_next = NULL;
        p_new->L_element = p_element;
    }
    return p_new;
}

/*----------------------------------------------------------------------*/
/*    DeleteSListNode();                                                */
/*                                                                      */
/*    Delete a node element.                                            */
/*----------------------------------------------------------------------*/

void DeleteSListNode(slistnode_t * p)
{
    if (p)
        free(p);
}

/*----------------------------------------------------------------------*/
/*    CompareSListNodes();                                              */
/*                                                                      */
/*    Compare 2 elements in the list.                                   */
/*----------------------------------------------------------------------*/

int CompareSListNodes(slistnode_t * p1, slistnode_t * p2,
                      int (*cfunc) (void *, void *))
{
    return cfunc(p1->L_element, p2->L_element);
}

/*----------------------------------------------------------------------*/
/*    General list traversal functions.                                 */
/*----------------------------------------------------------------------*/

slistnode_t *TraverseSListFirst(slist_t * p_list)
{
    if (p_list)
        return p_list->L_first;
    else
        return NULL;
}

slistnode_t *TraverseSListNext(slistnode_t * p_node)
{
    if (p_node)
        return p_node->L_next;
    else
        return NULL;
}

/*----------------------------------------------------------------------*/
/*    IsSListEmpty();                                                   */
/*                                                                      */
/*    Return true (non-0) if list is empty                              */
/*----------------------------------------------------------------------*/

int IsSListEmpty(slist_t * p_list)
{
    return (p_list->L_elements ? 0 : 1); /* ADC & SVP 15-Apr-95 bug fix */
}

/*----------------------------------------------------------------------*/
/*    SearchSList();                                                    */
/*                                                                      */
/*    Search a list for an element, and return a pointer to the node    */
/*    containing the element, else return NULL.                         */
/*----------------------------------------------------------------------*/

slistnode_t *SearchSList(slist_t * p_list, void *p_element,
                         int (*cfunc) (void *, void *))
{
    slistnode_t *p_node;

    if (p_list && p_list->L_elements)
    {
        p_node = p_list->L_first;
        while (p_node)
        {
            if (!cfunc(p_node->L_element, p_element))
                return p_node;
            else
                p_node = p_node->L_next;
        }
    }
    return NULL;
}

/*----------------------------------------------------------------------*/
/*    TotalSListNodes();                                                */
/*                                                                      */
/*    Return total count of all nodes in the list.                      */
/*----------------------------------------------------------------------*/

unsigned long TotalSListNodes(slist_t * p_list)
{
    if (p_list)
        return p_list->L_elements;
    else
        return 0;
}
