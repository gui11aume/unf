/* Produced by texiweb from report.w on 2001/04/27 at 01:37. */

/* Copyright (c) 2001 Ben Pfaff <pfaffben@msu.edu>.  All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

     - Redistributions of source code must retain the above copyright
       notice, this list of conditions, and the following disclaimer.

     - Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions, and the following disclaimer in
       the documentation and/or other materials provided with the
       distribution.

   There is NO WARRANTY, not even for MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE. */

#include <assert.h>
#include <float.h>
#include <stdio.h>
#include "pq.h"
#include "uniformity.h"

/* Fibonacci heap priority queue. */
struct pq 
  {
    int n;                      /* Number of items in heap. */
    int m;                      /* Maximum number of items in heap. */

    struct unf_mem *mem;        /* Memory allocator. */
    struct node *min;           /* Minimum node. */
    struct node **index;        /* Index by vertex number to get node.  */
    struct node **rank;         /* Used during extract_min(). */
    struct node *prealloc;      /* Memory allocated for nodes. */
  };

/* Fibonacci heap node. */
struct node 
  {
    int vertex;                 /* Vertex value. */
    double key;                 /* Key value. */

    int rank;                   /* Number of direct children. */
    int mark;                   /* 1=marked, 0=unmarked. */

    struct node *parent;        /* Parent node, |NULL| if a root. */
    struct node *child;         /* One of our children, |NULL| if terminal. */
    struct node *next, *prev;   /* Next and previous in circular list. */
  };

static void insert (struct pq *pq, int vertex, double key);
static void detach (struct node *node);
static void add_root (struct pq *pq, struct node *root);
static void add_child (struct node *parent, struct node *child);

static struct pq *
pq_create (struct unf_mem *mem, int n) 
{
  struct pq *pq;
  int i;

  assert (n >= 0);

  pq = mem->unf_alloc (sizeof *pq);
  if (pq == NULL)
    return NULL;

  pq->mem = mem;
  pq->n = 0;
  pq->m = n;
  pq->min = NULL;
  pq->index = pq->mem->unf_alloc (sizeof *pq->index * n);
  pq->rank = pq->mem->unf_alloc (sizeof *pq->rank * n);
  pq->prealloc = pq->mem->unf_alloc (sizeof *pq->prealloc * n);
  if (pq->index == NULL || pq->prealloc == NULL) 
    {
      mem->unf_free (pq);
      return NULL;
    }

  for (i = 0; i < n; i++) 
    {
      pq->index[i] = NULL;
      pq->rank[i] = NULL;
    }

  for (i = 0; i < n; i++)
    insert (pq, i, DBL_MAX);

  return pq;
}

static void 
insert (struct pq *pq, int vertex, double key) 
{
  struct node *node;

  assert (pq != NULL);
  assert (vertex >= 0 && vertex < pq->m);
  assert (pq->index[vertex] == NULL);

  pq->index[vertex] = node = pq->prealloc + pq->n++;

  node->vertex = vertex;
  node->key = key;
  node->rank = 1;
  node->mark = 0;
  node->parent = node->child = NULL;
  add_root (pq, node);
  if (node->key < pq->min->key)
    pq->min = node;
}

static void 
pq_discard (struct pq *pq) 
{
  assert (pq != NULL);

  pq->mem->unf_free (pq->index);
  pq->mem->unf_free (pq->prealloc);
  pq->mem->unf_free (pq->rank);
  pq->mem->unf_free (pq);
}

static int 
pq_extract_min (struct pq *pq) 
{
  struct node *min;

  assert (pq != NULL);
  assert (pq->n > 0);

  /* Grab the minimum value and remove it from the heap. */
  min = pq->min;
  pq->index[min->vertex] = NULL;
  pq->n--;
  pq->min = min->next;
  if (pq->min == min)
    pq->min = NULL;
  detach (min);

  /* Add all of the children of the former minimum as roots. */
  for (;;) 
    {
      struct node *child = min->child;
      if (child == NULL)
	break;

      detach (child);
      add_root (pq, child);
    }

  /* If the heap is now empty, return early. */
  if (pq->min == NULL)
    return min->vertex;

  /* Go through all the root nodes and put them into |rank[]|,
     linking together the ones with equal |rank| fields. */
  {
    struct node *w = pq->min;

    do 
      {
	struct node *x = w;
	w = w->next;
	if (w == pq->min)
	  w = NULL;

	for (;;) 
	  {
	    struct node *y = pq->rank[x->rank];
	    if (y == NULL || y == x)
	      break;
	    pq->rank[x->rank] = NULL;

	    if (x->key > y->key) 
	      {
		struct node *t = x;
		x = y;
		y = t;
	      }
	    if (pq->min == y)
	      pq->min = y->next;
	    detach (y);
	    add_child (x, y);
	    y->mark = 0;
	  }
	pq->rank[x->rank] = x;
      } 
    while (w != NULL);
  }

  /* Go through the root nodes and unset the corresponding |rank[]| 
     elements.
     Pick the next |pq->min| while we're at it. */
  {
    struct node *x;

    x = pq->min;
    for (;;) 
      {
	pq->rank[x->rank] = NULL;
	x = x->next;
	if (x == pq->min)
	  break;

	if (x->key < pq->min->key)
	  pq->min = x;
      }
  }

  return min->vertex;
}

/* Removes |node| from the heap, making it parentless. */
static void 
detach (struct node *node) 
{
  assert (node->next != NULL && node->prev != NULL);
  node->next->prev = node->prev;
  node->prev->next = node->next;

  if (node->parent != NULL) 
    {
      if (node->parent->child == node) 
	{
	  node->parent->child = node->next;
	  if (node->parent->child == node)
	    node->parent->child = NULL;
	}

      node->parent->rank--;
      node->parent = NULL;
    }

#ifndef NDEBUG
  node->next = node->prev = NULL;
#endif
}

/* Adds |root| as a root of |pq|.  
   |pq->min| is changed only if it is |NULL|,
   so the caller is responsible for updating it if it should decrease. */
static void 
add_root (struct pq *pq, struct node *root) 
{
  assert (root->parent == NULL);

  if (pq->min == NULL) 
    {
      pq->min = root;
      root->prev = root->next = root;
    } 
  else 
    {
      root->prev = pq->min;
      root->next = pq->min->next;
      pq->min->next->prev = root;
      pq->min->next = root;
    }
}

/* Adds |child| as a child of |parent|. */
static void 
add_child (struct node *parent, struct node *child) 
{
  assert (parent != NULL && child != NULL);
  assert (child->parent == NULL);

  child->parent = parent;
  if (parent->child == NULL) 
    {
      child->next = child->prev = child;
      parent->child = child;
    } 
  else 
    {
      child->prev = parent->child;
      child->next = parent->child->next;
      parent->child->next->prev = child;
      parent->child->next = child;
    }

  parent->rank++;
}

static void 
pq_decrease_key (struct pq *pq, int vertex, double key) 
{
  struct node *x, *y;

  assert (pq != NULL);
  assert (vertex >= 0 && vertex < pq->m);
  assert (pq->index[vertex] != NULL);
  assert (key <= pq->index[vertex]->key);

  /* Find the node in question. */
  x = pq->index[vertex];
  x->key = key;
  y = x->parent;

  /* Make cascading cuts as necessary. */
  if (y != NULL && x->key < y->key)
    for (;;) 
      {
	struct node *z;

	assert (x->parent != NULL);
	assert (x->parent == y);
	assert (pq->min != x);
	detach (x);
	add_root (pq, x);
	x->mark = 0;

	z = y->parent;
	if (z == NULL)
	  break;

	if (y->mark == 0) 
	  {
	    y->mark = 1;
	    break;
	  } 
	else 
	  {
	    x = y;
	    y = z;
	  }
      }

  /* Actually decrease the key's value. */
  if (key < pq->min->key)
    pq->min = pq->index[vertex];
}

static int 
pq_count (const struct pq *pq) 
{
  assert (pq != NULL);

  return pq->n;
}

static double 
pq_key (const struct pq *pq, int vertex) 
{
  struct node *node;

  assert (pq != NULL);
  assert (vertex >= 0 && vertex < pq->m);

  node = pq->index[vertex];
  return node != NULL ? node->key : -DBL_MAX;
}

static void 
verify_recursive (const struct pq *pq, const struct node *first, 
		  const struct node *parent, const struct node **tmp) 
{
  const struct node *iter;
  int count;

  if (first == NULL)
    return;

  /* Check that the forward links are circular. */
  iter = first;
  count = 0;
  do 
    {
      assert (iter->parent == parent);
      assert (count < pq->m);
      tmp[count++] = iter;
      iter = iter->next;
    } 
  while (iter != first);

  /* Check that the backward links are circular,
     and that they are the same ones as when we go forward. */
  iter = first;
  do 
    {
      iter = iter->prev;

      assert (count > 0);
      count--;
      assert (tmp[count] == iter);
    } 
  while (iter != first);
  assert (count == 0);

  /* Perform verification recursively on child nodes. */
  iter = first;
  do 
    {
      verify_recursive (pq, iter->child, iter, tmp);
      iter = iter->next;
    } 
  while (iter != first);
}

static void 
pq_verify (const struct pq *pq) 
{
  const struct node **tmp;

  assert (pq != NULL);
  assert (pq->n >= 0 && pq->n <= pq->m);

  tmp = pq->mem->unf_alloc (sizeof *tmp * pq->n);
  if (pq->n != 0 && tmp == NULL) 
    {
      printf ("virtual memory exhausted\n");
      abort ();
    }

  verify_recursive (pq, pq->min, NULL, tmp);

  pq->mem->unf_free (tmp);
}

static void 
dump_recursive (const struct node *p) 
{
  const struct node *q = p;

  putchar ('(');
  do 
    {
      if (q != p)
	fputs (", ", stdout);
      printf (q->key == DBL_MAX ? "+oo" : "%g", q->key);
      if (q->child != NULL)
	dump_recursive (q->child);

      q = q->next;
    } 
  while (q != p);
  putchar (')');
}

static void 
pq_dump (const struct pq *pq) 
{
  dump_recursive (pq->min);
}

struct pq_class unf_pq_fib_heap = 
  {
    pq_create, 
    pq_discard, 
    pq_extract_min, 
    pq_decrease_key,
    pq_count, 
    pq_key, 
    pq_verify, 
    pq_dump,
  };
