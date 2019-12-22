#include <stdio.h>

int sum(int[]);

int main(){
	int my_num[] = {1, 2, 3};
	printf("The total is : %d", sum(my_num));
}

int sum(int x[])
