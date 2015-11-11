/* deal with unix sockets demo program */
/* get lines from stdin, send lines to socket */
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
//#include "fdread.h"

int main(int argc, char *argv[]){
	int sock;
	int rc;
	char buffer[4096] = "h\n";
	
	struct sockaddr_un addr = {
		.sun_family = AF_UNIX,
		.sun_path = "/home/timothy/vlc.sock",
	};
	
	//open a unix socket
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	rc = connect(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
	if(rc != 0){
		perror("connect");
	}

	while(1){
		//io loop
		if(fdready(sock, FDOP_READ)){
			rc = recv(sock, buffer, 4096, 0);
		}
		if(fdready(sock, FDOP_WRITE)){
			rc = send(sock, buffer, 4096, 0);
		}
	};
	
	return 0;
}
