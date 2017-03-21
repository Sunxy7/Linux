#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/epoll.h>

#define SIZE 1024
typedef struct epbuf
{
	int fd;
	char buf[SIZE];
}epbuf_t,*epbuf_p;

static epbuf_p alloc_epbuf(int fd)
{
	epbuf_p ptr = (epbuf_p)malloc(sizeof(epbuf_t));
	if (ptr == NULL)
	{
		perror("malloc");
		exit(0);
	}
	ptr->fd =fd;
	return ptr;
}
static void del_epbuf(epbuf_p ptr)
{
	if (ptr != NULL)
	{
		free(ptr);
	}
}

int start_up(const char * ip,int port)
{
	int sock =socket(AF_INET,SOCK_STREAM,0);
	if (sock <0)
	{
		perror("socket");
		exit(1);
	}
	
	struct sockaddr_in local;
	local.sin_family =AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr =inet_addr( ip);
	if (bind(sock,(struct sockaddr *)&local ,sizeof(local))<0)
	{
		perror("bind");
		exit(2);
	}
	if (listen(sock,5)<0)
	{
		perror("listen");
		exit(3);
	}
	return sock;
}

int main(int argc,char * argv[])
{
	if (argc != 3)
	{
		printf("Usage:%s local_ip  local_port \n",argv[1]);
		exit(4);
	}
	int epollfd =epoll_create(256);
	if (epollfd < 0)
	{
		perror("epoll_create");
		exit(5);
	}
	struct epoll_event ev;
	struct epoll_event ready_evs[32];
	int max_evs =32;
	int nfds,conn_sock,listen_sock;
	ev.data.ptr =alloc_epbuf(listen_sock);
	ev.events =EPOLLIN|EPOLLET;
	
	struct sockaddr_in peer;
	socklen_t len =sizeof(peer);
	int timeout =5000;
	if (epoll_ctl(epollfd,EPOLL_CTL_ADD,listen_sock,&ev)<0)
	{
		perror("epoll_ctl");
		exit(6);
	}
	nfds =epoll_wait(epollfd,ready_evs,max_evs,timeout);
	if (nfds ==0)
	{
		printf("timeout\n");
	}
	else if (nfds <0)
	{
		perror("epoll_wait");		
	}
	else
	{
		int j=0;
		int fd =((epbuf_p)ready_evs[j].data.ptr)->fd;
		for(;j<nfds;j++)
		{
			if((fd == listen_sock)&&(ready_evs[j].events & EPOLLIN))
			{
				conn_sock =accept(listen_sock,(struct sockaddr *)&peer,&len);
				if (conn_sock <0 )
				{
		

					perror("accept");
					exit(7);
				}
				ev.events =EPOLLIN;
				ready_evs[j].data.ptr = alloc_epbuf(conn_sock);
			
				epoll_ctl(epollfd,EPOLL_CTL_ADD,conn_sock,&ev);
			}
			else if (fd != listen_sock && (ready_evs[j].events &EPOLLIN))
			{
			
				char *buf = ((epbuf_p)(ready_evs[j].data.ptr))->buf;
				ssize_t s =read( fd,buf ,sizeof(buf)-1);
				if (s <0)
				{
					perror("read");
				
					return 2;
				}
				else if(s>0)
				{
			
					buf[s]=0;
					printf(" %s \n",buf);
					ev.events =EPOLLOUT;
					epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);

				}
				else
				{
					del_epbuf(ready_evs[j].data.ptr);
					ready_evs[j].data.ptr =NULL;
					epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,NULL);
				
					close(fd);
			
				}

			}
			else if(fd != listen_sock && (ready_evs[j].events & EPOLLOUT))
			{
	
				const char * smg ="HTTP/1.0 200\r\n\r\n<html><h1>hello world!</h1></html>\n";
				write(fd,smg,sizeof(smg));
				del_epbuf(ready_evs[j].data.ptr);
				ready_evs[j].data.ptr =NULL;
				epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,NULL);
			
				close(fd);

			}
			else
			{

			}
				
		}
	}
	return 0;
}
