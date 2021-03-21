#include <stdio.h>
//\f \n \r \t \v

int main(void){
	char s[] = "AAA\fBBB\fCCC\fDDD\fEEE\vFFF\n";
	printf("%s", s);
	return 0;
}
