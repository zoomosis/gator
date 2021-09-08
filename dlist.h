/*
*  dlist.h  Doubly linked-list management functions
*  (c) copyright 1995 Scott Pitcher, Andrew Clarke
*
*  Doubly linked-list management structures and decls.
*/

#ifndef __DLIST_H__
#define __DLIST_H__

/* the doubly linked-list data types */

struct dlistnode_t
{
    struct dlistnode_t *L_prev; /* previous node in list */
    struct dlistnode_t *L_next; /* next node in list */
    void *L_element;            /* the data in this node */
};

typedef struct dlistnode_t dlistnode_t;

struct dlist_t
{
    dlistnode_t *L_first;
    dlistnode_t *L_last;
    unsigned long L_elements;
};

typedef struct dlist_t dlist_t;

/* functions for dealing with lists */

dlist_t *CreateLinkedDList(void);
void DestroyLinkedDList(dlist_t * p_list);
void *GetDListElement(dlistnode_t * p);
void SetDListElement(dlistnode_t * p, void *d);
dlistnode_t *CreateDListNode(void *p_element);
void DeleteDListNode(dlistnode_t * p);

void AddDListNode(dlist_t * p_list, dlistnode_t * p_node);
void DropDListNode(dlist_t * p_list, dlistnode_t * p_node);
int CompareDListNodes(dlistnode_t * p1, dlistnode_t * p2,
                      int (*cfunc) (void *, void *));
void SwapDListNodes(dlistnode_t * p1, dlistnode_t * p2);
int IsDListEmpty(dlist_t * p_list);
dlistnode_t *SearchDList(dlist_t * p_list, void *p_element,
                         int (*cfunc) (void *, void *));
unsigned long TotalDListNodes(dlist_t * p_list);

dlistnode_t *TraverseDListFirst(dlist_t * p_list);
dlistnode_t *TraverseDListLast(dlist_t * p_list);
dlistnode_t *TraverseDListPrevious(dlistnode_t * p_node);
dlistnode_t *TraverseDListNext(dlistnode_t * p_node);

#endif

/* __DLIST_H__ */
