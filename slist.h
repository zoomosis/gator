/*
*  slist.h  Singly linked-list management functions
*  (c) copyright 1995 Scott Pitcher, Andrew Clarke
*
*  Singly linked-list management structures and decls.
*/

#ifndef __SLIST_H__
#define __SLIST_H__

/* the singly linked-list data types */

struct slistnode_t
{
    struct slistnode_t *L_next; /* next node in list */
    void *L_element;            /* the data in this node */
};

typedef struct slistnode_t slistnode_t;

struct slist_t
{
    slistnode_t *L_first;
    slistnode_t *L_end;
    unsigned long L_elements;
};

typedef struct slist_t slist_t;

/* functions for dealing with lists */

slist_t *CreateLinkedSList(void);
void DestroyLinkedSList(slist_t * p_list);
void *GetSListElement(slistnode_t * p);
void SetSListElement(slistnode_t * p, void *d);
slistnode_t *CreateSListNode(void *p_element);
void DeleteSListNode(slistnode_t * p);

void AddSListNode(slist_t * p_list, slistnode_t * p_node);
void DropSListNode(slist_t * p_list, slistnode_t * p_node);
int CompareSListNodes(slistnode_t * p1, slistnode_t * p2,
                      int (*cfunc) (void *, void *));
int IsSListEmpty(slist_t * p_list);
slistnode_t *SearchSList(slist_t * p_list, void *p_element,
                         int (*cfunc) (void *, void *));
unsigned long TotalSListNodes(slist_t * p_list);

slistnode_t *TraverseSListFirst(slist_t * p_list);
slistnode_t *TraverseSListNext(slistnode_t * p_node);

#endif

/* __SLIST_H__ */
