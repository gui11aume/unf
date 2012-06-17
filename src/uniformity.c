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
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "uniformity.h"

/* Tests for uniformity of |p|, a set of |n| points in |d| dimensions.
   |p| is an array of |n * d| doubles, where the first point is stored
   in the first |d| elements, the second in the next |d| elements, and
   so on.
   The contents in |user_options|, if non-|NULL|, are used to control
   the testing procedure.
   Returns a confidence level between 0 and 1, 1 indicating that the
   set is perfectly uniform and 0 indicating that the set is perfectly
   clustered.  Returns |-1| in the event of a memory allocation
   error. */
double 
unf_test (const struct unf_options *user_options, 
	  const double *p, int n, int d) 
{
  struct unf_options options;   /* Local copy of options. */
  double *r;                    /* Stores |p| plus generated points. */

  assert (p != NULL && n > 0 && d > 0);

  {
    static const struct unf_options default_options = 
      {
	&unf_mem_malloc, 
	&unf_rng_system, 
	&unf_set_rectangular, 
	&unf_mst_prim_binary,
      };

    if (user_options == NULL) 
      options = default_options;
    else 
      {
	options = *user_options;
	if (options.unf_mem == NULL) 
	  options.unf_mem = default_options.unf_mem;
	if (options.unf_rng == NULL) 
	  options.unf_rng = default_options.unf_rng;
	if (options.unf_set == NULL) 
	  options.unf_set = default_options.unf_set;
	if (options.unf_mst == NULL) 
	  options.unf_mst = default_options.unf_mst;
      }
  }

  r = options.unf_mem->unf_alloc (sizeof *r * ((2 + n * 2) * d));
  if (r == NULL)
    return -1.0;

  {
    double *const tmp = r + (n * 2) * d;
    struct unf_a_set *s;
    int i;

    s = options.unf_set->unf_create (options.unf_mem, p, n, d);
    if (s == NULL) 
      {
	options.unf_mem->unf_free (r);
	return -1.0;
      }

    memcpy (r, p, sizeof *r * n * d);
    for (i = 0; i < n; ) 
      {
	double *y = r + (n + i) * d;
	options.unf_set->unf_random (s, options.unf_rng, y);
	if (unf_inside_hull (p, n, d, y, tmp))
	  i++;
      }

    options.unf_set->unf_discard (s);
  }

  {
    int c, t;
    int okay;

    okay = unf_run_mst (options.unf_mem, options.unf_mst, &c, &t, r, n, d);
    options.unf_mem->unf_free (r);
    return okay ? unf_calc_results (c, t, n) : -1;
  }
}

/* Calculates vector |n_star_est| at point |y| within points |p|.
   |tmp| is used for temporary storage.  Its contents are destroyed.
   There are |d| elements in each of |n_star|, |tmp|, and |y|, and |n * 
   d| elements in |p|. */
static void 
calc_n_star_est (double *n_star_est, double *tmp,
		 const double *p, int n, int d, const double *y) 
{
  int i;

  assert (n_star_est != NULL && tmp != NULL && p != NULL
	  && n > 0 && d > 0 && y != NULL);

  for (i = 0; i < d; i++)
    n_star_est[i] = 0.0;

  for (i = 0; i < n; i++) 
    {
      double length = 0.0;
      double factor;
      int j;

      for (j = 0; j < d; j++) 
	{
	  double diff = tmp[j] = p[j] - y[j];
	  length += diff * diff;
	}
      factor = 1.0 / pow (length, d + 1.0);

      for (j = 0; j < d; j++)
	n_star_est[j] += tmp[j] * factor;

      p += d;
    }

  for (i = 0; i < d; i++)
    n_star_est[i] /= n;
}

/* Returns nonzero only if point |y| is within the (approximate) convex
   hull of the |n| points in array |p|.
   Each point has |d| dimensions, and |tmp| must point to a modifiable
   scratch array with room for |2 * d| doubles. */
int 
unf_inside_hull (const double *p, int n, int d, const double *y, 
		 double *tmp) 
{
  double *n_star_est;
  int i;

  assert (p != NULL && n > 0 && d > 0 && y != NULL && tmp != NULL);

  n_star_est = tmp + d;
  calc_n_star_est (n_star_est, tmp, p, n, d, y);

  for (i = 0; i < n; i++) 
    {
      double dot;
      int j;

      dot = 0.0;
      for (j = 0; j < d; j++)
	dot += (*p++ - y[j]) * n_star_est[j];

      if (dot <= 0.0)
	return 1;
    }

  return 0;
}
/* Runs the MST-based test for uniformity on the |2 * n| points in 
   |r|, of |d| dimensions each.
   The first |n| of the points should be the points to test, the last
   |n| the randomly generated points within the approximate convex
   hull of those points.
   |*c| and |*t| receive on output the C and T statistics.
   Returns nonzero if successful, zero if a memory allocation error 
   occurred. */
int 
unf_run_mst (struct unf_mem *mem_class, struct unf_mst *mst_class,
	     int *c, int *t, const double *r, int n, int d) 
{
  unf_mst_result *mst;
  int i;

  assert (mem_class != NULL && mst_class != NULL && c != NULL && t != NULL
	  && r != NULL && n > 0 && d > 0);

  mst = mst_class->unf_run (mem_class, r, 2 * n, d);
  if (mst == NULL)
    return 0;

  *t = 0;
  for (i = 0; i < 2 * n - 1; i++)
    if ((mst[i][0] < n) != (mst[i][1] < n))
      (*t)++;

  *c = 0;
  for (i = 0; i < 2 * n - 1; i++) 
    {
      int j;

      for (j = i + 1; j < 2 * n - 1; j++)
	if (mst[i][0] == mst[j][0] 
	    || mst[i][0] == mst[j][1]
	    || mst[i][1] == mst[j][0] 
	    || mst[i][1] == mst[j][1])
	  (*c)++;
    }

  mem_class->unf_free (mst);
  return 1;
}

/* Returns the significance of normal variate |x|. */
static double 
normal_sig (double x) 
{
  const double a1 = 0.4361836;
  const double a2 = -0.1201676;
  const double a3 = 0.9372980;
  const double p = 0.33267;
  const double pi = 3.14159265358979323846;

  double y = fabs (x);
  double t = 1.0 / (1.0 + p * y);
  double t2 = t * t;
  double t3 = t2 * t;
  double z = 1.0 / sqrt (2.0 * pi) / exp (0.5 * y * y);
  double s = z * (a1 * t + a2 * t2 + a3 * t3);

  return x >= 0 ? 1.0 - s : s;
}

/* Returns uniformity significance level between 0 and 1.
   |c| and |t| are the values returned by |unf_run_mst()|, |n| the 
   size of the test set. */
double 
unf_calc_results (int c, int t, int n) 
{
  double mean = n;
  double var = ((n / (2.0 * n - 1.0))
		* (n - 1.0 - (c - 2.0 * n + 2.0) / (2.0 * n - 3.0)));
  double z = (t - mean) / sqrt (var);

  return normal_sig (z);
}

/* Initializes |options| to the defaults, as an alternative to using
   |unf_defaults| as an initializer. */
void 
unf_init_options (struct unf_options *options) 
{
  assert (options != NULL);

  options->unf_mem = NULL;
  options->unf_rng = NULL;
  options->unf_set = NULL;
  options->unf_mst = NULL;
}
