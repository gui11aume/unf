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

#include <stdlib.h>
#include "uniformity.h"

static void 
rng_seed (unsigned long seed) 
{
  srand (seed);
}

static double 
rng_get (void) 
{
  return (double) rand () / RAND_MAX;
}

struct unf_rng unf_rng_system = 
  {
    rng_seed, 
    rng_get
  };
