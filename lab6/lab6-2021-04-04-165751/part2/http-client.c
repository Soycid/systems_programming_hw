/*
 * http-client.c
 */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

static void die(const char *s) { perror(s); exit(1); }

void closeFiles(FILE *a, FILE *b){ fclose(a); fclose(b);}

int main(int argc, char **argv)
{
    //argument check
	char buf[8192]; //only buffer used 
	if (argc != 4) {
        fprintf(stderr, "usage: %s <URL> <server-port> <path>\n",argv[0]);
        exit(1);
    }

	//define serverName and hosent struct
	struct hostent *he;	
	char *serverName = argv[1];

	// get server ip from server name
	if ((he = gethostbyname(serverName)) == NULL) {
		die("gethostbyname failed");
	}
	const char *ip = inet_ntoa(*(struct in_addr *)he->h_addr);
    unsigned short port = atoi(argv[2]);

    // Create a socket for TCP connection
    int sock; // socket descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("socket failed");

    // Construct a server address structure
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr)); // must zero out the structure
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port        = htons(port); // must be in network byte order

    // Establish a TCP connection to the server
    if (connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
		die("connect failed");
	}

    // Construct HTTP request and send() 
    sprintf(buf, "GET %s HTTP/1.0\r\nHost: %s:%s \r\n\r\n", argv[3],argv[1],argv[2]);
	size_t len = strlen(buf);
	if (send(sock,buf,len,0) != len){
		die("send failed");
	}

	//open socket for reading
 	FILE *fsocket = fdopen(sock, "rb");
		
	//get filename from path
	char *filename = strrchr(argv[3],'/');
	filename++;
	if(*filename == '\0'){
		fclose(fsocket);
		die("invalid filename");
	}
	FILE *out = fopen(filename,"wb");
	int r;
	int h = 1;
	len = sizeof(buf);

	//process HTTP header
	if(fgets(buf,len,fsocket)!=NULL){	
		if(strncmp(buf+9,"200",3)!=0){
			h = 0;}
	}

	//ignore subsequent headers
	while((fgets(buf,len,fsocket)!=NULL) && h 
			&&strcmp(buf, "\r\n")!=0);
	
	//process content
    while((r = fread(buf,1,len,fsocket)) > 0 && h) {
    	if(fwrite(buf, 1, r, out) != r){
       		closeFiles(out,fsocket);
			die("fwrite error");}
    }

	//error check socket
	if(ferror(fsocket)){
		closeFiles(out,fsocket);	
		die("bad socket");
	}
	
	// Clean-up
	closeFiles(out,fsocket); 
    return 0;
}
