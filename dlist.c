/*
*  dlist.c  Doubly linked-list management functions
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
*  15-Apr-95 SVP
*  Fixed a bug (silly really) in DropListNode() where the whole list
*  following a dropped node would fall off with the node. Don't ask
*  me how I didn't ever see this one - I found it while constructing
*  BUILDPM.EXE (spent hours wondering why the segments kept disappearing
*  from the list!).
*
*  Fixed a bug in IsListEmpty() (pointed out by Andrew Clarke) where the
*  function would return true if the list contained elements, rather than
*  the other way around.
*
*  27-Jul-95 SVP
*  Change from LIST.* to DLIST.* before writing SLIST.*
*
*  07-Aug-95 ADC
*  Renamed functions:
*    TraverseDListStart() --> TraverseDListFirst()
*    TraverseDListEnd()   --> TraverseDListLast()
*    TraverseDListLast()  --> TraverseDListPrevious()
*
*  Fixed bug/typo in TraverseDListLast().
*/

#include <stdlib.h>
#include "dlist.h"

/*----------------------------------------------------------------------*/
/*    CreateLinkedDList();                                              */
/*                                                                      */
/*    Create and initialise a linked list                               */
/*----------------------------------------------------------------------*/

dlist_t *CreateLinkedDList(void)
{
    dlist_t *p_list;

    if ((p_list = malloc(sizeof(dlist_t))) != NULL)
    {
        p_list->L_first = p_list->L_last = NULL;
        p_list->L_elements = 0L;
    }
    return p_list;
}

/*----------------------------------------------------------------------*/
/*    DestroyLinkedDList();                                             */
/*                                                                      */
/*    Destroy a linked list                                             */
/*----------------------------------------------------------------------*/

void DestroyLinkedDList(dlist_t * p_list)
{
    dlistnode_t *p_node;

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
/*    GetDListElement();                                                */
/*                                                                      */
/*    Return the data held in a list node.                              */
/*----------------------------------------------------------------------*/

void *GetDListElement(dlistnode_t * p)
{
    if (p)
        return p->L_element;
    else
        return NULL;
}

/*----------------------------------------------------------------------*/
/*    SetDListElement();                                                */
/*                                                                      */
/*    Set the data held in a list node.                                 */
/*----------------------------------------------------------------------*/

void SetDListElement(dlistnode_t * p, void *d)
{
    if (p)
        p->L_element = d;
}

/*----------------------------------------------------------------------*/
/*    AddDListNode();                                                   */
/*                                                                      */
/*    Add a node to a list.                                             */
/*----------------------------------------------------------------------*/

void AddDListNode(dlist_t * p_list, dlistnode_t * p_node)
{
    if (p_list)
    {
        if ((p_node->L_prev = p_list->L_last) != NULL)
            p_list->L_last->L_next = p_node;
        p_node->L_next = NULL;
        if (!p_list->L_first)
            p_list->L_first = p_node;
        p_list->L_last = p_node;
        ++p_list->L_elements;
    }
}

/*----------------------------------------------------------------------*/
/*    DropDListNode();                                                  */
/*                                                                      */
/*    Drop a node from a list.                                          */
/*----------------------------------------------------------------------*/

void DropDListNode(dlist_t * p_list, dlistnode_t * p_node)
{
    dlistnode_t *poldnext = p_node->L_next; /* SVP 15-Apr-95 bug fix */

    if (p_list)
    {
        if (p_list->L_first == p_node)
            p_list->L_first = p_node->L_next;
        if (p_list->L_last == p_node)
            p_list->L_last = p_node->L_prev;
        if (p_node->L_next)
        {
            (p_node->L_next)->L_prev = p_node->L_prev;
            p_node->L_next = NULL;
        }
        if (p_node->L_prev)
        {
            (p_node->L_prev)->L_next = poldnext; /* SVP 15-Apr-95 bug fix */
            p_node->L_prev = NULL;
        }
        --p_list->L_elements;
    }
}

/*----------------------------------------------------------------------*/
/*    CreateDListNode();                                                */
/*                                                                      */
/*    Create a node element and return a pointer to it.                 */
/*----------------------------------------------------------------------*/

dlistnode_t *CreateDListNode(void *p_element)
{
    dlistnode_t *p_new;

    if ((p_new = malloc(sizeof(dlistnode_t))) != NULL)
    {
        p_new->L_next = p_new->L_prev = NULL;
        p_new->L_element = p_element;
    }
    return p_new;
}

/*----------------------------------------------------------------------*/
/*    DeleteDListNode();                                                */
/*                                                                      */
/*    Delete a node element.                                            */
/*----------------------------------------------------------------------*/

void DeleteDListNode(dlistnode_t * p)
{
    if (p)
        free(p);
}

/*----------------------------------------------------------------------*/
/*    CompareDListNodes();                                              */
/*                                                                      */
/*    Compare 2 elements in the list.                                   */
/*----------------------------------------------------------------------*/

int CompareDListNodes(dlistnode_t * p1, dlistnode_t * p2,
                      int (*cfunc) (void *, void *))
{
    return cfunc(p1->L_element, p2->L_element);
}

/*----------------------------------------------------------------------*/
/*    SwapDListNodes();                                                 */
/*                                                                      */
/*    Swap 2 elements in the list.                                      */
/*----------------------------------------------------------------------*/

void SwapDListNodes(dlistnode_t * p1, dlistnode_t * p2)
{
    dlistnode_t *temp;

    if (p1 && p2)
    {
        temp = p1->L_next;
        p1->L_next = p2->L_next;
        p2->L_next = temp;
        temp = p1->L_prev;
        p1->L_prev = p2->L_prev;
        p2->L_prev = temp;
    }
}

/*----------------------------------------------------------------------*/
/*    General list traversal functions.                                 */
/*----------------------------------------------------------------------*/

dlistnode_t *TraverseDListFirst(dlist_t * p_list)
{
    if (p_list)
        return p_list->L_first;
    else
        return NULL;
}

dlistnode_t *TraverseDListLast(dlist_t * p_list)
{
    if (p_list)
        return p_list->L_last;  /* ADC 07-Aug-95 bug fix */
    else
        return NULL;
}

dlistnode_t *TraverseDListPrevious(dlistnode_t * p_node)
{
    if (p_node)
        return p_node->L_prev;
    else
        return NULL;
}

dlistnode_t *TraverseDListNext(dlistnode_t * p_node)
{
    if (p_node)
        return p_node->L_next;
    else
        return NULL;
}

/*----------------------------------------------------------------------*/
/*    IsDListEmpty();                                                   */
/*                                                                      */
/*    Return true (non-0) if list is empty                              */
/*----------------------------------------------------------------------*/

int IsDListEmpty(dlist_t * p_list)
{
    return (p_list->L_elements ? 0 : 1); /* ADC & SVP 15-Apr-95 bug fix */
}

/*----------------------------------------------------------------------*/
/*    SearchDList();                                                    */
/*                                                                      */
/*    Search a list for an element, and return a pointer to the node    */
/*    containing the element, else return NULL.                         */
/*----------------------------------------------------------------------*/

dlistnode_t *SearchDList(dlist_t * p_list, void *p_element,
                         int (*cfunc) (void *, void *))
{
    dlistnode_t *p_node;

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
/*    TotalDListNodes();                                                */
/*                                                                      */
/*    Return total count of all nodes in the list.                      */
/*----------------------------------------------------------------------*/

unsigned long TotalDListNodes(dlist_t * p_list)
{
    if (p_list)
        return p_list->L_elements;
    else
        return 0;
}
