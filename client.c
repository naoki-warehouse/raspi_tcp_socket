#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
	struct sockaddr_in server;
	int sock;
	char buf[32];
	int n;

	sock = socket(AF_INET,SOCK_STREAM,0);

	server.sin_family = AF_INET;
	server.sin_port = htons(10000);

	inet_pton(AF_INET,"127.0.0.1",&server.sin_addr.s_addr);

	connect(sock,(struct sockaddr *)&server,sizeof(server));

	memset(buf,0,sizeof(buf));
	n = read(sock,buf,sizeof(buf));

	printf("%d,%s\n",n,buf);

	close(sock);

	return 0;
}
