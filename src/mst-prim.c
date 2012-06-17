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

/* Calculates and returns the distance between points |a| and |b|, 
   each in |d| dimensions. */
static double 
calc_distance (const double *a, const double *b, int d) 
{
  double sum = 0.0;

  assert (a != NULL && b != NULL && d >= 0);
  while (d--) 
    {
      double diff = *a++ - *b++;
      sum += diff * diff;
    }

  return sum;
}

/* Calculates the MST of the |n| |d|-dimensional points at |p|.
   Uses |mem| for memory allocation
   and an instance of |pq_class| as a priority queue. */
static unf_mst_result *
calc_mst (struct pq_class *pq_class,
	  struct unf_mem *mem, const double *p, int n, int d) 
{
  struct pq *pq;
  unf_mst_result *mst;
  int *edges;

  assert (pq_class != NULL && mem != NULL && p != NULL 
	  && n > 0 && d > 0);

  pq = pq_class->create (mem, n);
  mst = mem->unf_alloc (sizeof *mst * (n - 1));
  edges = mem->unf_alloc (sizeof *edges * n);
  if (pq == NULL || mst == NULL || edges == NULL) 
    {
      if (pq != NULL)
	pq_class->discard (pq);
      mem->unf_free (mst);
      mem->unf_free (edges);
      return NULL;
    }

  pq_class->decrease_key (pq, 0, 0);
  while (pq_class->count (pq) > 0) 
    {
      int u, v;

      u = pq_class->extract_min (pq);
      for (v = 0; v < n; v++)
	if (u != v) 
	  {
	    double key = pq_class->key (pq, v);

	    if (key != -DBL_MAX) 
	      {
		double cost = calc_distance (p + d * u, p + d * v, d);

		if (cost < key) 
		  {
		    edges[v] = u;
		    pq_class->decrease_key (pq, v, cost);
		  }
	      }
	  }
    }

  {
    int i;

    for (i = 1; i < n; i++) 
      {
	mst[i - 1][0] = i;
	mst[i - 1][1] = edges[i];
      }
  }
  pq_class->discard (pq);
  mem->unf_free (edges);

  return mst;
}

/* Prim's Algorithm MST with binary heap priority queue. */
static unf_mst_result *
calc_mst_prim_bin_heap (struct unf_mem *mem, const double *p, int n, int d)
{
  extern struct pq_class unf_pq_bin_heap;

  return calc_mst (&unf_pq_bin_heap, mem, p, n, d);
}

struct unf_mst unf_mst_prim_binary = 
  { 
    calc_mst_prim_bin_heap, 
  };

/* Prim's Algorithm MST with Fibonacci heap priority queue. */
static unf_mst_result *
calc_mst_prim_fib_heap (struct unf_mem *mem, const double *p, int n, int d)
{
  extern struct pq_class unf_pq_fib_heap;

  return calc_mst (&unf_pq_fib_heap, mem, p, n, d);
}

struct unf_mst unf_mst_prim_fibonacci = 
  { 
    calc_mst_prim_fib_heap, 
  };
