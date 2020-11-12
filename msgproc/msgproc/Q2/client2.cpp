#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>

#define PORT 5000
#define MAXLINE 100
struct record {
	char rec[MAXLINE];
	struct record *next;
};
struct record *start;
void load(char *);
void add(struct record *);
void delList(struct record *);
void load(char *file) {
	char buffer[MAXLINE];
	FILE *fp=fopen(file,"r");
	if(fp==NULL)
	{
		printf("File open error.\n");
		exit(0);
	}
	while((fgets(buffer,MAXLINE,fp))) {
		struct record *N=(struct record *) malloc(sizeof(struct record));
		N->next=NULL;
		strcpy(N->rec,buffer);
		add(N);
	}
	fclose(fp);
}

void add(struct record *N) {
	struct record *p;
	if(start==NULL)
		start=N;
	else
	{
		for(p=start;p->next;p=p->next);
		p->next=N;
	}
}

void delList(struct record *S)
{
	struct record *p=S->next;
	while(S!=NULL)
	{
		free(S);
		S=p;
		if(p)
			p=p->next;
	}
	start=NULL;
}


int main(int argc,char *argv[])
{

	char buffer[MAXLINE];
	int b,i,sockfd, n,sendStop=0,sendRestart=1;
	struct record *p;
	struct sockaddr_in servaddr;
	start=NULL;
	if(argc!=2)
	{
		printf("Usage: %s <File name>\n",argv[0]);
		exit(0);
	}
	load(argv[1]);

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
	p=start;
	while(p) {
		bzero(buffer, sizeof(buffer));
		if(sendStop==0 && sendRestart==1)
			sendto(sockfd,p->rec, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr));
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
		if(strncmp(buffer,"WAIT",4)==0) {
			sendStop=1;
			sendRestart=0;
		}
		else if(strncmp(buffer,"RESTART",7)==0) {
			sendStop=0;
			sendRestart=1;
			p=p->next;
		}

	}
	sendto(sockfd, "STOP", MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr));

	close(sockfd);
	delList(start);
}


