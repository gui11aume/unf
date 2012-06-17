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

/* Heap movement operations. */
#define parent(i) ((i) / 2)     /* Get node |i|'s parent. */
#define left(i) ((i) * 2)       /* Get node |i|'s left child. */
#define right(i) (left (i) + 1) /* Get node |i|'s right child. */

/* Binary heap priority queue. */
struct pq 
  {
    struct unf_mem *mem; /* Memory allocator. */
    int n;		/* Number of items in tree. */
    int m;		/* Maximum number of items in tree. */

    /* Heap contents. */
    double *key;	/* Keys. */
    int *vertex;	/* Satellite information (vertex number). */

    /* Reverse index. */
    int *index;		/* Heap index by vertex number; 0 = not present. */
  };

static void pq_discard (struct pq *pq);

static struct pq *
pq_create (struct unf_mem *mem, int n) 
{
  struct pq *pq;
  int i;

  /* Allocate memory for heap itself. */
  assert (n >= 0);
  pq = mem->unf_alloc (sizeof *pq);
  if (pq == NULL)
    return NULL;

  /* Allocate memory for heap's members. */
  pq->mem = mem;
  pq->key = mem->unf_alloc (sizeof *pq->key * (n + 1));
  pq->vertex = mem->unf_alloc (sizeof *pq->vertex * (n + 1));
  pq->index = mem->unf_alloc (sizeof *pq->index * (n + 1));
  if (pq->key == NULL || pq->vertex == NULL || pq->index == NULL) 
    {
      pq_discard (pq);
      return NULL;
    }

  /* Initialize heap. */
  pq->n = pq->m = n;
  for (i = 1; i <= n; i++) 
    {
      pq->key[i] = DBL_MAX;
      pq->vertex[i] = i;
      pq->index[i] = i;
    }

  return pq;
}

static void 
pq_discard (struct pq *pq) 
{
  assert (pq != NULL);

  pq->mem->unf_free (pq->key);
  pq->mem->unf_free (pq->vertex);
  pq->mem->unf_free (pq->index);
  pq->mem->unf_free (pq);
}

static int
pq_extract_min (struct pq *pq)
{
  int min;

  assert (pq != NULL && pq->n > 0);

  min = pq->vertex[1];
  assert (min >= 1 && min <= pq->m);
  pq->index[min] = 0;

  pq->vertex[1] = pq->vertex[pq->n];
  pq->key[1] = pq->key[pq->n];
  pq->index[pq->vertex[1]] = 1;

  pq->n--;

  {
    int i = 1;

    for (;;) 
      {
	/* Find index of smallest of |i| or its children as |smallest|. */
	int l = left (i);
	int r = right (i);
	int smallest = i;
	if (l <= pq->n && pq->key[l] < pq->key[smallest])
	  smallest = l;
	if (r <= pq->n && pq->key[r] < pq->key[smallest])
	  smallest = r;
	if (smallest == i)
	  break;

	/* Swap nodes |i| and |smallest|. */
	{
	  int t_vertex = pq->vertex[i];
	  double t_key = pq->key[i];
	  pq->vertex[i] = pq->vertex[smallest];
	  pq->key[i] = pq->key[smallest];
	  pq->vertex[smallest] = t_vertex;
	  pq->key[smallest] = t_key;
	  pq->index[pq->vertex[i]] = i;
	  pq->index[pq->vertex[smallest]] = smallest;
	}

	i = smallest;
      }
  }


  return min - 1;
}
static void 
pq_decrease_key (struct pq *pq, int vertex, double key) 
{
  int i, p;

  assert (pq != NULL);
  assert (vertex >= 0 && vertex < pq->m);
  assert (pq->index[vertex + 1] != 0);
  assert (key <= pq->key[pq->index[vertex + 1]]);

  for (i = pq->index[vertex + 1]; i > 1; i = p) 
    {
      p = parent (i);
      if (pq->key[p] < key)
	break;

      pq->key[i] = pq->key[p];
      pq->vertex[i] = pq->vertex[p];
      pq->index[pq->vertex[i]] = i;
    }

  pq->key[i] = key;
  pq->vertex[i] = vertex + 1;
  pq->index[pq->vertex[i]] = i;
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
  int i;

  assert (pq != NULL && vertex >= 0 && vertex < pq->m);

  i = pq->index[vertex + 1];
  return i != 0 ? pq->key[i] : -DBL_MAX;
}

static void 
pq_verify (const struct pq *pq) 
{
  int i;
  int error = 0;

  assert (pq != NULL);
  assert (pq->m > 0);
  assert (pq->n <= pq->m);

  for (i = 2; i <= pq->n; i++) 
    {
      int parent = parent (i);
      if (pq->key[parent] > pq->key[i]) 
	{
	  printf ("Heap property for %d:%d violated: %g > %g\n",
		  parent, i, pq->key[parent], pq->key[i]);
	  error = 1;
	}
    }

  for (i = 1; i <= pq->n; i++)
    if (pq->index[i] != 0 && pq->vertex[pq->index[i]] != i) 
      {
	printf ("index for %d points to position %d, which contains %d\n",
		i, pq->index[i], pq->vertex[pq->index[i]]);
	error = 1;
      }

  assert (error == 0);
}

static void 
pq_dump (const struct pq *pq) 
{
  int i;

  for (i = 1; i <= pq->n; i++) 
    {
      printf ("%d", pq->vertex[i]);
      if (pq->key[i] != DBL_MAX)
	printf (":%g", pq->key[i]);
      putchar (' ');
    }
  putchar ('\n');
}

struct pq_class unf_pq_bin_heap = 
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
