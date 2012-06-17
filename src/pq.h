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

#ifndef PQ_H
#define PQ_H 1

struct unf_mem;

struct pq_class 
  {
    struct pq *(*create) (struct unf_mem *mem, int n);
    void (*discard) (struct pq *pq);
    int (*extract_min) (struct pq *pq);
    void (*decrease_key) (struct pq *pq, int vertex, double key);
    int (*count) (const struct pq *pq);
    double (*key) (const struct pq *pq, int vertex);
    void (*verify) (const struct pq *pq);
    void (*dump) (const struct pq *pq);
  };

#endif /* pq.h */
