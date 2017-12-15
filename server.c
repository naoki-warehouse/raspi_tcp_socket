#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
	int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;
	int sock;

	sock0 = socket(AF_INET,SOCK_STREAM,0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(10000);
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock0,(struct sockaddr *)&addr,sizeof(addr));

	listen(sock0,5);

	len = sizeof(client);
	sock = accept(sock0, (struct sockaddr *)&client, &len);

	write(sock,"HELLO",5);

	close(sock);

	close(sock0);

	return 0;
}
