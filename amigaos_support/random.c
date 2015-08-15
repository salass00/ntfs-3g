#include <stdlib.h>

#ifndef __AROS__
void srandom(unsigned seed) {
	srand(seed);
}

long random(void) {
	return rand();
}
#endif

