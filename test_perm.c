#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "radixsrt_vec.h"


int check_sorted(int n, uint32_t *v)
{
	int rc = 0;

	for (int i = 1; i < n; i++) {
		if (v[i] < v[i-1])
			rc = 1;
	}
	return rc;
}

long sum_vector(int n, uint32_t *v)
{
	long sum = 0;
	for (int i = 0; i < n; i++)
		sum += v[i];
	return sum;
}

int main(int argc, char *argv[])
{
	FILE *in;
	int n;
	int i, pos;
	uint32_t *v, *r, *pin, *pout;

	if (argc != 2) {
		printf("Usage: %s <inputfile>\n", argv[0]);
		return 1;
	}
	in = fopen(argv[1], "r");
	if (in == NULL) {
		printf("Could not open file %s\n", argv[1]);
		return 1;
	}

	fscanf(in, "%d\n", &n);
	v = (unsigned int *)malloc(sizeof(int) * n);
	r = (unsigned int *)malloc(sizeof(int) * n);
	pin = (unsigned int *)malloc(sizeof(int) * n);
	pout = (unsigned int *)malloc(sizeof(int) * n);
	for (i = 0; i < n; i++) {
		fscanf(in, "%d\n", &v[i]);
	}
	fclose(in);
	
#ifdef DEBUG
	for (i = 0; i < n; i++)
		printf("%x ", v[i]);
	printf("\n");
#endif
	if (pin != NULL)
		for (i = 0; i < n; i++)
			pin[i] = i;
	
	radixsrt_vec(n, v, r, 8, pin, pout);

#ifdef DEBUG
	printf("permutation:\n");
	for (i = 0; i < n; i++)
		printf("%x ", pin[i]);
	printf("\n");
#endif
	return 0;
}
