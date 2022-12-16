#include <stdio.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>

#include "scanner.h"
#include "parser.h"
#include "statSem.h"

int main(int argc, char **argv){

	const char* filename = argv[1];	   
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	accepting = true;
	lineNum = 1;
	if(filename!= NULL){
		fp = fopen(filename, "r");
		if (fp == NULL){
		printf("ERROR: File doesn't exist or is NULL\n");
		exit(EXIT_FAILURE);
		}
		while ((read = getline(&line, &len, fp)) != -1) {
			tokenizeString(formatLine(line));
			lineNum++;
		}

		fclose(fp);
		if (line)
		free(line);
		print();
		reverse();
		parser();
		statSem(filename);
	}
	else{
		char *string = NULL;
		size_t size = 0;
		ssize_t chars_read;

		chars_read = getline(&string, &size, stdin);

		if (chars_read < 0) {
			puts("ERROR: Could not read the keyboard input");
			free(string);
			return 1;
		}
		tokenizeString(formatLine(string));
		print();
	}

	return 0;
}

