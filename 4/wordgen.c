#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <time.h>

void generate_word(int);

int main(int argc, char **argv){
	int length = 0, nc = 25, num = 0, i = 0; 
	length = argc < 1 || argv[1] == 0 ? INT_MAX : atoi(argv[1]);

	srand(time(NULL));

	while(i++ < length){
		do{
			num = rand() % nc; 
		}while(num < 3);
		generate_word(num); //num = number of characters in a word
	}

	//create a while loop and add 1 every time for the loop
	//in the while loop, determine how many characters are in the word
	//generate_word(# of characters)
		//output the word
}

void generate_word(int num){
	int i = 0, nl = 26; 
	char arr[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
	while(i++ < num){
		int val = rand() % nl;
		fprintf(stderr, "%c", arr[val]);
	}
	fprintf(stderr, "\n");
}