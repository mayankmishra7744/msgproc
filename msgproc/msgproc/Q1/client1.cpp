#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>

#define PORT 5000
#define MAXLINE 100

int main(int argc,char *argv[])
{
	FILE *fp;
	char buffer[100];
	int b,i,sockfd, n;
	struct sockaddr_in servaddr;
	if(argc!=2)
	{
		printf("Usage: %s <File name>\n",argv[0]);
		exit(0);
	}
	fp=fopen(argv[1],"r");
	if(fp==NULL)
	{
		printf("File open error.\n");
		exit(0);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	servaddr.sin_family = AF_INET;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("\n Error : Connect Failed \n");
		exit(0);
	}
	sendto(sockfd, "START", MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr));
	while((fgets(buffer,100,fp))) {
		sendto(sockfd,buffer, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr));
	}
	sendto(sockfd, "STOP", MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr));
	close(sockfd);
	fclose(fp);
}

