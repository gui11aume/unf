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
   PARTICULAR PURPOSE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "uniformity.h"

const char *pgm_name = "mst-test";

int
main (int argc, char *argv[])
{
  double *data;
  unf_mst_result *mst;
  int i;

  int n = 100;
  struct unf_mst *mst_alg = &unf_mst_prim_binary;
  unsigned seed = time_seed ();
  
  if (argc <= 1)
    {
      printf ("usage: %s N [MST] [SEED], where:\n"
	      " N is the number of points in the test set\n"
	      " MST is the minimum spanning tree algorithm, one of:\n"
	      "    prim-bin    Prim's algorithm with binary heap (default)\n"
	      "    prim-fib    Prim's algorithm with Fibonacci heap\n"
	      " SEED is the random seed (time-based default)\n",
	      argc > 0 && argv[0] != NULL ? argv[0] : "test");
      return EXIT_SUCCESS;
    }

  if (argc > 1)
    n = atoi (argv[1]);
  if (argc > 2)
    {
      if (!strcmp (argv[2], "prim-bin"))
	mst_alg = &unf_mst_prim_binary;
      else if (!strcmp (argv[2], "prim-fib"))
	mst_alg = &unf_mst_prim_fibonacci;
      else
	{
	  fprintf (stderr, "unknown MST algorithm `%s'\n", argv[2]);
	  return EXIT_FAILURE;
	}
    }
  if (argc > 3)
    seed = atoi (argv[3]);
  srand (seed);

  data = malloc (sizeof *data * 2 * n);
  if (data == NULL)
    {
      fprintf (stderr, "memory allocation error\n");
      return EXIT_FAILURE;
    }

  printf ("newgraph\n");
  printf ("xaxis nodraw\n");
  printf ("yaxis nodraw\n");

  for (i = 0; i < n; i++)
    {
      double x = data[i * 2] = (double) rand () / RAND_MAX;
      double y = data[i * 2 + 1] = (double) rand () / RAND_MAX;

      printf ("newcurve marktype circle pts %g %g\n", x, y);
    }

  mst = mst_alg->unf_run (&unf_mem_malloc, data, n, 2);
  if (mst == NULL)
    {
      fprintf (stderr, "virtual memory exhausted\n");
      return EXIT_FAILURE;
    }
  
  for (i = 0; i < n - 1; i++)
    {
      double x0 = data[mst[i][0] * 2];
      double y0 = data[mst[i][0] * 2 + 1];
      double x1 = data[mst[i][1] * 2];
      double y1 = data[mst[i][1] * 2 + 1];
	
      printf ("newline pts %g %g %g %g\n", x0, y0, x1, y1);
    }

  free (data);
  free (mst);

  return EXIT_SUCCESS;
}
