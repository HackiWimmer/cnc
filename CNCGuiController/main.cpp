#include <stdio.h>
#include <inttypes.h>
#include <math.h>  
#include <limits.h>

int main(int argc, char **argv) {
	
	
	printf("sizeof(int)          = %d\n", (int)sizeof(int));
	printf("sizeof(unsignet int) = %d\n", (int)sizeof(unsigned int));
	
	printf("min int:           %d\n", INT_MIN);
	printf("max int:           %d\n", INT_MAX);
	printf("max unsigned int:  %u\n", UINT_MAX);
	
	
	printf("max unsigned int:  %9.0lf\n", pow(2, 32) -1);
	
	
	return 0;
}
