#include <stdio.h>
#include <stdlib.h>
#include <omp.h> // OpenMP para paralelismo

#define N 5

int in[N], out[N];

int main(int argc, char *argv[]) {
  unsigned int i;
  int r = 0;
  // Paralelizar el patrón map
  #pragma omp parallel for
  for (i = 0; i < N; i++) {
    out[i] = in[i] * in[i];
  }
  // Paralelizar la reducción
  #pragma omp parallel for reduction(+:r)
  for (i = 0; i < N; i++) {
    r += out[i];
  }

  return r;
}

