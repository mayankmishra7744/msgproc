#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include <string.h>
#include <stdlib.h>
#define PORT 5000
#define MAXLINE 1000
struct message {
	char mtype,side,ticker[80];
	unsigned long seqnum,size;
	double price;
	struct message *next;
};
struct message *start;
void dumptoFile();
void delList(struct message *S);
void parseBuffer(char *buf);
void insert(struct message *N);
void sigintHandler(int sig_num)
{
	signal(SIGINT, sigintHandler);
	dumptoFile();
	delList(start);
	exit(0);
}

int main()
{
	char buffer[100];
	int client=0,listenfd, len;
	struct sockaddr_in servaddr, cliaddr;
	signal(SIGINT, sigintHandler);
	start=NULL;
	puts("Press ctrl+c to end when all clients terminated.");
	bzero(&servaddr, sizeof(servaddr));
	listenfd = socket(AF_INET, SOCK_DGRAM, 0);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	servaddr.sin_family = AF_INET;

	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	while(1) {
		len = sizeof(cliaddr);
		bzero(buffer, sizeof(buffer));
		int n = recvfrom(listenfd, buffer, sizeof(buffer),
				0, (struct sockaddr*)&cliaddr,(socklen_t *) &len);
		buffer[n] = '\0';
		if(strncmp(buffer,"START",5)==0){
			client++;
		}
		else if(strncmp(buffer,"STOP",4)==0) {
			client--;
		}
		else 
			parseBuffer(buffer);
	}
	printf("Exiting\n");
}

void parseBuffer(char *buf) {
	int i,j;
	char temp[100];
	struct message *N=(struct message *) malloc(sizeof(struct message));
	N->next=NULL;
	i=0;
	N->mtype=buf[0];
	for(i=2,j=0;buf[i]!=':';i++,j++)
		temp[j]=buf[i];
	temp[j]=0;
	N->seqnum=strtoul(temp,NULL,10);
	N->side=buf[++i];
	for(i=i+1,j=0;buf[i]!=':';i++,j++)
		temp[j]=buf[i];
	temp[j]=0;
	N->size=strtoul(temp,NULL,10);
	for(i=i+1,j=0;buf[i]!=':';i++,j++)
		N->ticker[j]=buf[i];
	N->ticker[j]=0;
	for(i=i+1,j=0;buf[i]!=':';i++,j++)
		temp[j]=buf[i];
	temp[j]=0;
	N->price=strtod(temp,NULL);
	insert(N);
}
void insert(struct message *N) {
	struct message *p,*ptr;
	if(start==NULL)
		start=N;
	else
	{
		for(p=start;p;p=p->next)
		{
			if(N->seqnum==p->seqnum){
				printf("Duplicate SeqNumber %lu\n",N->seqnum);
				return;
			}
		}
		//Locate the nodes to insert new node in order to create sorted list
		for(p=ptr=start;p && N->seqnum>p->seqnum;ptr=p,p=p->next);
		if(p==start) {
			N->next=start;
			start=N;
		}
		else
		{
			ptr->next=N;
			N->next=p;
		}
	}
}

void delList(struct message *S)
{
	struct message *p=S->next;
	while(S!=NULL)
	{
		free(S);
		S=p;
		if(p)
			p=p->next;
	}
	start=NULL;
}

void dumptoFile() {
	FILE *fp1,*fp2;
	//dump.txt will contain all the data while Outfile.txt will contain sorted and unique SeqNumber as given in question
	fp1=fopen("dump.txt","w");
	fp2=fopen("Outfile.txt","w");
	struct message *p;
	for(p=start;p;p=p->next)
	{
		fprintf(fp1,"%c:%lu:%c:%lu:%s:%ld:\n",p->mtype,p->seqnum,p->side,p->size,p->ticker,p->price);
		fprintf(fp2,"%lu,",p->seqnum);
	}
	fclose(fp1);
	fclose(fp2);
}

