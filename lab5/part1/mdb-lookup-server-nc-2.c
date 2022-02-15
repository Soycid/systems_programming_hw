
/*
 * mdb-lookup-server-nc-2.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

static void die(const char *s)
{
    perror(s);
    exit(1);
}
void findDeadChildren(){ //lol
	pid_t child = waitpid((pid_t)-1, NULL, WNOHANG); 
	while (child > 0){
		fprintf(stderr, "> process terminated: [pid=%d]\n", (int) child);
		child = waitpid((pid_t)-1, NULL, WNOHANG); 
	 }
	printf("port number: ");
	
}

int main(int argc, char **argv)
{
	    
	printf("port number: ");
	
	char str[64]; //buffer / port size
	
	while(fgets(str,64,stdin) != NULL){
		//error check
		if(str[0] == '\n'){
			findDeadChildren();
			continue;
		}
		if(atoi(str)<10000 || atoi(str) > 64000){ //from jae ANN
			die("invalid port");
        }
		pid_t pid;
		pid = fork();
		  if (pid < 0) {
			  die("fork failed");
		  } else if (pid == 0) {
		  // child process
			  fprintf(stderr, "[pid=%d] ", (int)getpid());
			  fprintf(stderr,"mdb-lookup-server started on port %s", str);
			  execl("./mdb-lookup-server-nc.sh", "mdb-lookup-server-nc.sh", str, (char *)0);
			  die("execl failed");
		  } else {
		  // parent process
		  sleep(1); //sleeps 1 so it doesn't conflict with child 
		  findDeadChildren();
		}
	}
    return 0;
}

