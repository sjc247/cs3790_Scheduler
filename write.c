#include <stdio.h>


int main(int argc, char* argv[]){

	const char* msg = argv[1];

	while(1){
		fprintf(stderr, msg);
		fprintf(stderr, " ");
		sleep(1);
	}
}
