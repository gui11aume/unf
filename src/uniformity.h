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

#ifndef UNIFORMITY_H
#define UNIFORMITY_H

#include <stddef.h>

/* Options. */

/* A set of options for |unf_test()|. */
struct unf_options 
  {
    struct unf_mem *unf_mem;    /* Memory allocator. */
    struct unf_rng *unf_rng;    /* Random number generator. */
    struct unf_set *unf_set;    /* Point set. */
    struct unf_mst *unf_mst;    /* Minimum spanning tree. */
  };

/* Default options. */
#define UNF_DEFAULTS {NULL, NULL, NULL, NULL}
void unf_init_options (struct unf_options *);

/* Provided class implementations. */
extern struct unf_mem unf_mem_malloc;
extern struct unf_set unf_set_rectangular;
extern struct unf_rng unf_rng_mt;
extern struct unf_rng unf_rng_system;
extern struct unf_mst unf_mst_prim_binary;
extern struct unf_mst unf_mst_prim_fibonacci;

typedef int unf_mst_result[2];

struct unf_mst 
  {
    unf_mst_result *(*unf_run) (struct unf_mem *, 
				const double *, int, int);
  };

/* Memory allocator class. */
struct unf_mem 
  {
    void *(*unf_alloc) (size_t);
    void (*unf_free) (void *);
  };

struct unf_rng 
  {
    void (*unf_seed) (unsigned long);
    double (*unf_get) (void);
  };

/* Point set. */
struct unf_set 
  {
    struct unf_a_set *(*unf_create) (struct unf_mem *, 
				     const double *, int n, int d);
    void (*unf_discard) (struct unf_a_set *);
    void (*unf_random) (const struct unf_a_set *, 
			struct unf_rng *, double *);
  };
/* High-level routine. */
double unf_test (const struct unf_options *, const double *, int, int);

/* Low-level routines. */
int unf_inside_hull (const double *, int, int, const double *, double *);
int unf_run_mst (struct unf_mem *, struct unf_mst *, 
		 int *, int *, const double *, int, int);
double unf_calc_results (int, int, int);


#endif /* uniformity.h */
