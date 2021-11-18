#include "cuda.cuh"

__global__ void kernel() {
	
}
int wrapper(int n) {
	kernel<<<1, 1>>>();
	printf("Hello, world! (%d)\n", n);
	return n*n;
}