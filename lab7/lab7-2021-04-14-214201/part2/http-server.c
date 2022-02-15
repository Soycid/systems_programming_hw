/*
 * http-server.c
 */
//TODO: fix loop free, /tng/ bug, use one socket for mdb 
#include <netdb.h>
#include <ctype.h>
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

int mdblookup(int clntsock, char* q, char* mdbHost, int mdbPort,int sock);

//from jae's note
int isFile(char *filename){
	struct stat s;
	stat(filename,&s); 
	return S_ISREG(s.st_mode); //macro instruction from Stack Exchange
}


int main(int argc, char **argv)
{
	// Control c handling + usage 
	if (argc != 5) {
    	fprintf(stderr,"usage: %s <server_port> "
				"<web_root> <mdb-lookup-host> <mdb-lookup-port>\n", argv[0]);
        exit(1);
    }
    unsigned short port = atoi(argv[1]);
	unsigned short mdbPort = atoi(argv[4]);
	char * mdbHost = argv[3];
    
	// Create a listening socket (also called server socket) 
	int sock = 0;
    int servsock;
    if ((servsock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("socket failed");

    // Construct local address structure
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // any network interface
    servaddr.sin_port = htons(port);

    // Bind to the local address
    if (bind(servsock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        die("bind failed");

    // Start listening for incoming connections
	if (listen(servsock, 5 /* queue size for connection requests */ ) < 0)
		die("listen failed");
    
	int r;
    char buf[1024];
	int mdb  = 0;	
    int clntsock;
    socklen_t clntlen;
    struct sockaddr_in clntaddr;
	char *filename;

    while (1) {
		if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
			die("signal() failed");


		filename = malloc(125);
		strcpy(filename,argv[2]);
		int GET = 0;
        clntlen = sizeof(clntaddr); // initialize the in-out parameter

        if ((clntsock = accept(servsock,
                        (struct sockaddr *) &clntaddr, &clntlen)) < 0)
            die("accept failed");

		FILE *fd;
		if ((fd = fdopen(clntsock,"r")) == NULL){
			fclose(fd);
			free(filename);
			continue;	
		}
	
		// Request Processing
		char duf[4096];	
		while(fgets(duf,sizeof(duf),fd)){
			if(!isspace(duf[0])){

				
				char *token_separators = "\t \r\n"; // tab, space, new line
				char *method = strtok(duf, token_separators);
				char *requestURI = strtok(NULL, token_separators);
				char *httpVersion = strtok(NULL, token_separators);

				if(strncmp(method,"GET",3) == 0){
					GET = 1;
					strtok(duf,"\n");
					char *c = (inet_ntoa(clntaddr.sin_addr));
					fprintf(stderr, "%s \"%s %s %s\" ",c,method,requestURI,httpVersion);
					//fprintf(stderr, "200 OK\n");
   					if(strncmp(requestURI,"/mdb-lookup",11) == 0){
						sock = mdblookup(clntsock,requestURI+16,mdbHost,mdbPort,sock);
						mdb = 1;				
					}else{
						mdb = 0;
					}
					if(strcmp(httpVersion,"HTTP/1.1")!=0 &&	strcmp(httpVersion,"HTTP/1.0")!=0){
						GET = 0;
					}
					if(requestURI[strlen(requestURI)-1] == '/'){
						strcat(filename,requestURI);
						strcat(filename,"/index.html");
					}else{
						strcat(filename,requestURI);
					}	
				}
			}else{
				break;
			}
		}

		if(mdb){
			fclose(fd);
			free(filename);
			continue;
		}else{
			if(sock != 0){close(sock);}
			sock = 0;
		}		
		FILE *fp;

		//ERROR HANDLING
		if(!GET){
			fprintf(stderr, " 501 Not Implemented\n");
			send(clntsock,"HTTP/1.0 501 Not Implemented\r\n\r\n501 Not Implemented\n",45,0);
			fclose(fd);
			free(filename);
			continue;
		}	

		if ((fp = fopen(filename,"rb"))){
			if(isFile(filename)){
				fprintf(stderr, " 200 OK\n");}
			else{
				fprintf(stderr, " 403 Forbidden\n");
				send(clntsock,"HTTP/1.0 403 Forbidden\r\n\r\n403 Forbidden\n",45,0);
				fclose(fd);
				free(filename);
				continue;
			}
		}else{
			//404 ERROR
			fprintf(stderr, " 404 Not Found\n");
			send(clntsock,"HTTP/1.0 404 Not Found\r\n\r\n404 Not Found\n",41,0);
			fclose(fd);
			free(filename);
			continue;
		}
		sprintf(buf , "HTTP/1.0 200 OK\r\n\r\n");
		send(clntsock,buf,strlen(buf),0);
		
		while ((r = fread(buf, 1,sizeof(buf), fp)) > 0) {
            if (send(clntsock, buf, r, 0) != r) {
                fprintf(stderr, "ERR: send failed\n");
                break;
            }
        }
        if (r < 0) {
            fprintf(stderr, "ERR: recv failed\n");
        }

        // Client closed the connection (r==0) or there was an error
        // Either way, close the client socket and go back to accept()
		fclose(fd);
		fclose(fp);
		free(filename);
		close(clntsock);
	}
		close(clntsock);
	
}














/* MDB LOOKUP STARTS HERE*/
const char *form =
"HTTP/1.1 200 OK\r\n\r\n"
"<html><body>\n"
"<h1>mdb-lookup</h1>\n"
"<p>\n"
"<form method=GET action=/mdb-lookup>\n"
"lookup: <input type=text name=key>\n"
"<input type=submit>\n"
"</form>\n"
"<p>\n"
"<p><table border>\n"
;

const char *footer =
"</table>"
"</body></html>";
FILE *fsocket;

int mdblookup(int clntsock, char* q, char* mdbHost, int mdbPort, int sock){
	//based off tcp-echo-client code
	send(clntsock,form,strlen(form),0);
	//fprintf(stderr,"query %s\n",q);

	struct hostent *he;
 
      // get server ip from server name
      if ((he = gethostbyname(mdbHost)) == NULL) {
        die("gethostbyname failed");
		}


	const char *ip = inet_ntoa(*(struct in_addr *)he->h_addr);
    unsigned short port = mdbPort;

    // Create a socket for TCP connection
    if(sock == 0){
			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				die("socket failed");
			// Construct a server address structure
			
			struct sockaddr_in servaddr;
			memset(&servaddr, 0, sizeof(servaddr)); // must zero out the structure
			servaddr.sin_family      = AF_INET;
			servaddr.sin_addr.s_addr = inet_addr(ip);
			servaddr.sin_port        = htons(port); // must be in network byte order

			// Establish a TCP connection to the server

			if (connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
				die("connect failed");
			fsocket = fdopen(sock,"rb");
			}
	char buf[100];
	
	send(sock,q,strlen(q),0);
	send(sock,"\n",1,0);
	int i = 1;
	while(fgets(buf,sizeof(buf),fsocket)!=NULL){
		if(strcmp("\n",buf) == 0){
			break;
		}
		char mdbstr[128];
		if(i%2){
			sprintf(mdbstr,"<tr><td> %s\n",buf);}
		else{
			sprintf(mdbstr,"<tr><td bgcolor=yellow> %s\n",buf);}
		//fprintf(stderr, "%s", mdbstr);
		send(clntsock,mdbstr,strlen(mdbstr),0);
		i++;
	}
	send(clntsock,footer,strlen(footer),0);
    fprintf(stderr, " 200 OK\n");
    // Clean-up
	//fclose(fsocket);
	//fflush(fsocket);
    close(clntsock);
	//close(sock);
	
	return sock;
}	
