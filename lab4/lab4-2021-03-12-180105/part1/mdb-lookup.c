#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mylist.h"
#include "mdb.h"
//do not use add back!
int i = 0;
char search[1024];
static void printEntry(void *p){
	struct MdbRec* m = (struct MdbRec*) p;
	i++;
	if(strstr(m->name,search)||strstr(m->msg,search)){	
		printf(" %d: {%s} said {%s}\n",i, m->name, m->msg); 
	}
}
int main(int argc, char **argv)
{
	//read file	
	if (argc != 2) {
		fprintf(stderr, "%s\n", "usage: mdb-lookup <file_name>");
		exit(1);
	}
	char *filename = argv[1];
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		perror(filename);
		exit(1);
	}
	//init list
	char buf[1024];
	struct List chain;
	initList(&chain);
	//build list
	struct Node *last = chain.head;
	while (fread(buf, 40, 1, fp) == 1) {
		if (buf == NULL) {
			perror("can’t write to stdout");
			exit(1);
		}
		else{
			struct MdbRec *n = malloc(40);
					
			strcpy(n->name,buf);
			strcpy(n->msg,buf+16);	
			last = addAfter(&chain,last,n);
		}
	}

	//loop
	printf("lookup: ");
	while(fgets(search,1024,stdin) != NULL ){
		search[5] = 0;
		search[strcspn(search, "\n")] = 0; //line taken from stackoverflow (link in readme)
		i = 0;
		traverseList(&chain,&printEntry);
		fflush(stdout);	
		printf("\nlookup: ");
		if (fputs(buf, stdout) == EOF) {
			perror("can’t write to stdout");
			exit(1);
		}
		fflush(stdout);	
	
	}

	//garbage collect
	traverseList(&chain,&free);
	removeAllNodes(&chain);
	//file error
	printf("\n");
	if (ferror(fp)) {
		perror(filename);
		exit(1);
	}
	fclose(fp);
	return 0;
}
