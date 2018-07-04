#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#define PORT 6000
#define LISTENQ 20
#define BUFFSIZE 65536
#define FILE_NAME_MAX_SIZE 512

#endif /* COMMON_H_ */
using namespace std;

#include "common.h"

int main(int argc, char **argv[])
{
    //Create socket
    int sockfd,connfd;
    struct sockaddr_in svraddr,clientaddr;
    bzero(&svraddr,sizeof(svraddr));
    bzero(&clientaddr,sizeof(clientaddr));

    svraddr.sin_family=AF_INET;
    svraddr.sin_addr.s_addr=htonl(INADDR_ANY);
    svraddr.sin_port=htons(PORT);

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
    {
        perror("socket");
        exit(1);
    }

    //bind
    if(bind(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr))<0)
    {
        perror("bind");
        exit(1);
    }

    //listen
    if(listen(sockfd,LISTENQ)<0)
    {
        perror("listen");
        exit(1);
    }

    while(1)
    {
        socklen_t clientaddrlen=sizeof(clientaddr);

        //accept
        connfd=accept(sockfd,(struct sockaddr*)&clientaddr,&clientaddrlen);
        if(connfd<0)
        {
            perror("connect");
            exit(1);
        }

        //recv file imformation
        char buff[BUFFSIZE];

	char buff1[BUFFSIZE];

    char filename[FILE_NAME_MAX_SIZE];
	char imgname[FILE_NAME_MAX_SIZE];
        int count;
        bzero(buff,BUFFSIZE);
        count=recv(connfd,buff,BUFFSIZE,0);

	bzero(buff1,BUFFSIZE);
	strncpy(buff1,"recv filename finish.",strlen("recv filename finish."));
	int num=send(connfd,buff1,strlen(buff1),0);
	cout << "num=" << num << endl;


        cout<<"count="<<count<<endl<<endl;

      if(count<0)
        {
            perror("recv");
            exit(1);
        }
        strncpy(filename,buff,strlen(buff)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buff));
        printf("Preparing recv file : %s\n",filename);

	sprintf(imgname,"%s%s","/home/xin/E/Recv/",filename);
        //recv file
        FILE *fd=fopen(imgname,"wb+");

        if(NULL==fd)
        {
            perror("open");
            exit(1);
        }
        bzero(buff,BUFFSIZE);

        int length=0;
        while(length=recv(connfd,buff,BUFFSIZE,0))

        {
            if(length<0)
            {
                perror("recv");
                exit(1);
            }
            cout<<"length="<<length<<endl;
            int writelen=fwrite(buff,sizeof(char),length,fd);
            cout<<"writelen="<<writelen<<endl;
            if(writelen<length)
            {
                perror("write");
                exit(1);
            }
            bzero(buff,BUFFSIZE);
        }
        printf("Receieved file:%s finished!\n",filename);
        bzero(filename,FILE_NAME_MAX_SIZE);
        fclose(fd);
        close(connfd);
    }
    close(sockfd);
    return 0;
}
