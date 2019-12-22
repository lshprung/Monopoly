#include <stdio.h>

int main(){
	printf("\033[0;31m");
	printf("Red\n");
	printf("\033[1;31m");
	printf("Bold Red\n");
	printf("\033[0;32m");
	printf("Green\n");
	printf("\033[1;32m");
	printf("Bold Green\n");
	printf("\033[0;30m");
	printf("\033[47m");
	printf("???\n");

	return 0;
}
