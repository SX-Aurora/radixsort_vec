/*
 * Radix sort and counting sort functions vectorized for SX-Aurora TSUBASA.
 *
 * The core of radix sort is the counting sort function that vectorizes pretty
 * decently on SX-Aurora TSUBASA and performs with ~11-15 GOPS on one core.
 * It uses techniques described in the paper
 * "Radix Sort For Vector Multiprocessors" by Marco Zagha and Guy Blelloch,
 * Proceedings of the 1991 ACM/IEEE conference on Supercomputing, Pages 712-721
 * The techniques used here are somewhat simpler than in the publication because
 * of features of the SX-Aurora that simplify vectorization and scanning.
 *
 * Copyright (c) 2018 Erich Focht <efocht@gmail.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <stdio.h>
#include "radixsrt_vec.h"

/*
 * @brief Vectorized count sort with permutation.
 *
 * Function arguments:
 * @param n length of sort vector
 * @param v input values vector, unsigned int
 * @param r result values vector, unsigned int
 * @param bits number of bits for count sort, define the "digit"
 * @param pos position of the "digit", 0 means leftmost
 * @param permin permutation input vector, if NULL, no permutation computed
 * @param permout permutation output vector
 *
 * @author Erich Focht
 */
void countsrt_vec(int n, uint32_t *v, uint32_t *r, int bits, int pos,
		  uint32_t *permin, uint32_t *permout)
{
	int i, j, k, m, digit, sum, val;
	int nb = 1 << bits;
	int shift = bits * pos;
	unsigned int mask = (nb - 1) << shift;
	int stride = n / VLEN;
	int bucket[nb][VLEN];

	if (stride * VLEN != n)
		stride++;
	stride = stride > 0 ? stride : 1;

	for (i = 0; i < nb * VLEN; i++)
		((int *)bucket)[i] = 0;

	/* histogram keys */
	for (i = 0; i < stride; i++) {
#pragma _NEC ivdep
		for (j = i, k=0;
		     j < MIN(i + VLEN * stride, n);
		     j += stride, k++) {
			digit = (v[j] & mask) >> shift;
			bucket[digit][k] += 1;
		}
	}

#ifdef DEBUG
	printf("after histogram\n");
	for (i = 0; i < VLEN; i++) {
		printf("p%02d: ", i);
		for (j = 0; j < nb; j++) {
			printf("%2d ", bucket[j][i]);
		}
		printf("\n");
	}
	printf("----\n");
#endif	
	
	/* scan buckets */
	sum = 0;
	for (i = 0; i < nb * VLEN; i++) {
		val = ((int *)bucket)[i];
		((int *)bucket)[i] = sum;
		sum += val;
	}

#ifdef DEBUG
	printf("matrix after scan\n");
	for (i = 0; i < VLEN; i++) {
		printf("p%02d: ", i);
		for (j = 0; j < nb; j++) {
			printf("%2d ", bucket[j][i]);
		}
		printf("\n");
	}
	printf("====\n");
#endif

	/* rank and permute */
	for (i = 0; i < stride; i++) {
		for (j = i, k = 0;
		     j < MIN(i + VLEN * stride, n);
		     j += stride, k++) {
#pragma _NEC ivdep
			digit = (v[j] & mask) >> shift ;
			m = bucket[digit][k];
			r[m] = v[j];
			if (permin != NULL)
				permout[m] = permin[j];
			bucket[digit][k] = m + 1;
		}
	}
}

/*
 * @brief Vectorized radix sort
 *
 * Function arguments:
 * @param n length of sort vector, permutation vectors, etc
 * @param v input values vector, unsigned int, contains sorted array when function returns
 * @param r temporary vector, unsigned int, needed for count sort
 * @param bits number of bits for count sort, define the "digit"
 * @param perm permutation input vector, if NULL, no permutation computed. Contains result when function returns
 * @param ptmp temporary vector used for permutations
 *
 */
void radixsrt_vec(int n, uint32_t *v, uint32_t *r, int bits,
		  uint32_t *perm, uint32_t *ptmp)
{
	uint32_t *tmp;

	for (int pos = 0; pos < (sizeof(int) * 8) / bits; pos++) {
		countsrt_vec(n, v, r, bits, pos, perm, ptmp);
#ifdef DEBUG		
		for (int i = 0; i < n; i++)
			printf("%x ", r[i]);
		printf("\n");
#endif
		tmp = v;
		v = r;
		r = tmp;
		if (perm != NULL) {
			tmp = perm;
			perm = ptmp;
			ptmp = tmp;
		}
	}
}

