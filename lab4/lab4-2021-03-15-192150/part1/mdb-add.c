#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdb.h"

int main(int argc, char **argv)
{
	//read file	
	if (argc != 2) {
		fprintf(stderr, "%s\n", "usage: mdb-add <file_name>");
		exit(1);
	}
	char *filename = argv[1];
	FILE *fp = fopen(filename, "ab");
	if (fp == NULL) {
		perror(filename);
		exit(1);
	}

	//input
	char name[1024];
	char msg[1024];
	printf("name please (will truncate to 15 chars): ");
	fgets(name,1024,stdin);
	printf("msg please (will truncate to 23 chars): ");
	fgets(msg,1024,stdin);
	
	//processing
	name[15] = 0;
	msg[23] = 0;
	name[strcspn(name, "\n")] = 0;
	msg[strcspn(msg, "\n")] = 0; 
	struct MdbRec* m = malloc(40);
	strncpy(m->name,name,16);
	strncpy(m->msg,msg,24);
	//appending
	fwrite(m,40,1,fp);
	
	//get line #
	int n = ftell(fp)/40; 
	printf(" %d: {%s} said {%s}",n,m->name,m->msg);
	
	//garbage collection
	free(m);
	//file error
	printf("\n");
	if (ferror(fp)) {
		perror(filename);
		exit(1);
	}
	fclose(fp);
	return 0;
}
