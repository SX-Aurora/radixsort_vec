# radixsort_vec

Radix sort and counting sort functions vectorized for SX-Aurora TSUBASA.

The core of radix sort is the counting sort function that vectorizes pretty
decently on SX-Aurora TSUBASA and performs with ~11-15 GOPS on one core.

It uses techniques described in the paper
"Radix Sort For Vector Multiprocessors" by Marco Zagha and Guy Blelloch,
Proceedings of the 1991 ACM/IEEE conference on Supercomputing, Pages 712-721

The techniques used here are somewhat simpler than in the publication because
of features of the SX-Aurora that simplify vectorization and scanning.
