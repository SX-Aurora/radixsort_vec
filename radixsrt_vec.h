#ifndef INC_RADIXSRT_VEC
#define INC_RADIXSRT_VEC

#include <stdint.h>

//#define DEBUG
#define VLEN 256
#define MIN(a,b) ((a) < (b) ? (a) : (b))

void countsrt_vec(int n, uint32_t *v, uint32_t *r, int bits, int pos,
		  uint32_t *permin, uint32_t *permout);

void radixsrt_vec(int n, uint32_t *v, uint32_t *r, int bits,
		  uint32_t *perm, uint32_t *ptmp);

#endif /* INC_RADIXSRT_VEC */
