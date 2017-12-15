#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(){
	struct sockaddr_in server;
	int sock;
	char buf[32];
	int n = 1;

	sock = socket(AF_INET,SOCK_STREAM,0);

	server.sin_family = AF_INET;
	server.sin_port = htons(10000);

	inet_pton(AF_INET,"127.0.0.1",&server.sin_addr.s_addr);

	connect(sock,(struct sockaddr *)&server,sizeof(server));

	memset(buf,0,sizeof(buf));
	for(int i=0;i<1000;i++){
		write(sock,&i,sizeof(int));
		usleep(1e5);
	}
	close(sock);

	return 0;
}
