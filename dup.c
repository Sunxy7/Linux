#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
int main()
{
	umask(0);
	int oldfd =open("test",O_RDWR|O_CREAT,0644);
	if (oldfd <0)
	{
		perror("open");
		return 1;
	}

	dup2(oldfd,1);
	close(oldfd);
	printf("change write ");
	return 0;
}
//int main()
//{
//	umask(0);
//	int fd =open("test",O_RDWR|O_CREAT,0644);
//	//打开文件test，若没有则创建后打开
//	if (fd <0)
//	{
//		perror("open");
//		return 1;
//	}
//	close(1);//关闭标准输出
//	dup(fd);// 修改文件描述符fd为1
//	close(fd);
//	printf("write to test\n");
//
//
//	return 0;
//}
