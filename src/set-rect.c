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
#include "uniformity.h"

/* Hyper-rectangular point set. */
struct unf_a_set 
  {
    struct unf_mem mem; /* Memory allocator. */

    int d;              /* Number of dimensions. */
    double *min;        /* Minimum values for each of |d| dimensions. */
    double *max;        /* Maximum values for each of |d| dimensions. */
  };
struct unf_a_set *
set_create (struct unf_mem *mem, const double *p, int n, int d) 
{
  struct unf_a_set *set;
  int i;

  assert (p != NULL && n > 0 && d > 0);

  /* Memory allocation. */
  set = mem->unf_alloc (sizeof *set);
  if (set == NULL)
    return NULL;
  set->mem = *mem;

  set->d = d;
  set->min = mem->unf_alloc (sizeof *set->min * d * 2);
  if (set->min == NULL) 
    {
      mem->unf_free (set->min);
      mem->unf_free (set);
      return NULL;
    }
  set->max = set->min + d;

  /* Construct hyper-rectangle. */
  for (i = 0; i < d; i++)
    set->min[i] = set->max[i] = *p++;

  for (i = 1; i < n; i++) 
    {
      int j;

      for (j = 0; j < d; j++) 
	{
	  if (*p < set->min[j])
	    set->min[j] = *p;
	  else if (*p > set->max[j])
	    set->max[j] = *p;

	  p++;
	}
    }

  return set;
}

void 
set_discard (struct unf_a_set *set) 
{
  assert (set != NULL);

  set->mem.unf_free (set->min);
  set->mem.unf_free (set);
}
void 
set_random (const struct unf_a_set *set, struct unf_rng *rng, double *v) 
{
  int i;

  for (i = 0; i < set->d; i++)
    v[i] = set->min[i] + rng->unf_get () * (set->max[i] - set->min[i]);
}
struct unf_set unf_set_rectangular = 
  {
    set_create, 
    set_discard, 
    set_random,
  };
