#include<stdio.h>
#include<poll.h>

#define SIZE 4
int mypoll()
{
	struct pollfd fds[SIZE]; 
	fds[0].fd =0;
	fds[0].events = POLLIN;
	fds[0].revents =0;
	int timeout =3000;
	char buf[1024];

	switch(poll(fds,SIZE,timeout))
	{
		case 0:
			printf("timeout \n");
			break;
		case -1:
			perror("poll");
			break;
		default:
			{
				if(fds[0].revents != 0 &&(fds[0].events && POLLIN))
				{
					printf("write is ready!\n");
				}
			}
			break;
	}
	return 0;
}

int main()
{
	mypoll();
	return 0;
}
