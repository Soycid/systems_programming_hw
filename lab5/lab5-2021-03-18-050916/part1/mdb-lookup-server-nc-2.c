
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


int main(int argc, char **argv)
{
    
	printf("port number: ");

	//var declarations
	char str[64];
	int num_p  = 0;
	int p[256] = {0}; //caps at 256 processes and inits to zeroes to prevent reading uninit bytes	
	
	while(scanf("%s",str)){ //loop
			//valid input check
			
			if(atoi(str)<1000 || atoi(str) > 6400){ //from jae ANN
				die("invalid port");
			}

			pid_t pid;
			pid = fork();
			//store child pid in p[]
			p[num_p] =  pid;
			num_p++;

			if (pid < 0) {
			die("fork failed");
			} else if (pid == 0) {
			// child process
			fprintf(stderr, "[pid=%d] ", (int)getpid());
			fprintf(stderr,"mdb-lookup-server started on port %s\n", str);

			execl("./mdb-lookup-server-nc.sh", "mdb-lookup-server-nc.sh", 
				str, (char *)0);
			die("execl failed");
			} else {
			// parent process
			sleep(1); //sleeps 1 so it doesn't conflict with 
				
			for (int i = 0;i<num_p+1;i++){
				pid_t child = waitpid(p[i], NULL, WNOHANG); 
				if(child > 0){
					fprintf(stderr, "> process terminated: [pid=%d]\n", p[i]);
					p[i] = (int)getpid(); // write own pid to array element

				}
			}

			fprintf(stderr,"port number: ");
			
			//fprintf(stderr, "[pid=%d] ", (int)pid);
			//fprintf(stderr, "mdb-lookup-server terminated\n");
			}
	}
	
	//garbage collection
	//free(str);
    return 0;
}

