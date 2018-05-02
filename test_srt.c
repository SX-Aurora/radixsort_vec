#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void radixsrt_vec(int n, uint32_t *v, uint32_t *r, int bits,
		  uint32_t *perm, uint32_t *ptmp);

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
	for (i = 0; i < n; i++) {
		fscanf(in, "%d\n", &v[i]);
	}
	fclose(in);
	
#ifdef DEBUG
	for (i = 0; i < n; i++)
		printf("%x ", v[i]);
	printf("\n");
#endif
	
	radixsrt_vec(n, v, r, 8, NULL, NULL);

	return 0;
}
