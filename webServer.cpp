#include "task.h"  
#include "thread_pool.h"  
  
int main(int argc, char *argv[])  
{  
    if(argc != 2)  
        {  
        printf("usage : %s port\n", argv[0]);  
        return 1;  
        }  
  
        int sockfd, connfd;  
        struct sockaddr_in servaddr, client;  //sockaddr_in把port和addr 分开储存在两个变量中  sockaddr在头文件#include <sys/socket.h>中定义，sockaddr的缺陷是：sa_data把目标地址和端口信息混在一起了，
        int port = atoi(argv[1]);  //获取端口  
        //设置服务端的sockaddr_in  
        servaddr.sin_family = AF_INET;  //选择 AF_INET 的目的就是使用 IPv4 进行通信。address family 就是protocal family就是协议族PF_INET
        servaddr.sin_port = htons(port);  
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  
        //创建socket  
        sockfd = socket(AF_INET, SOCK_STREAM, 0);  //Socket()调用返回一个整型socket描述符，你可以在后面的调用使用它。SOCK_STREAM (流套接口)SOCK_DGRAM (数据报套接字)
         
		 
		 /*
		 调用Socket函数时，socket执行体将建立一个Socket，实际上"建立一个Socket"意味着为一个Socket数据结构分配存储空间。 Socket执行体为你管理描述符表。两个网络程序之间的一个网络连接包括五种信息：通信协议、本地协议地址、本地主机端口、远端主机地址和远端协议端口。
该函数如果调用成功就返回新创建的套接字的描述符，如果失败就返回INVALID_SOCKET。套接字描述符是一个整数类型的值。每个进程的进程空间里都有一个套接字描述符表，该表中存放着套接字描述符和套接字数据结构的对应关系。该表中有一个字段存放新创建的套接字的描述符，另一个字段存放套接字数据结构的地址，因此根据套接字描述符就可以找到其对应的套接字数据结构。每个进程在自己的进程空间里都有一个套接字描述符表但是套接字数据结构都是在操作系统的内核缓冲里。
		 
		 
		 
		 
		 
		 SOCK_STREAM (流套接口)

1、不保留任何消息的边界

      举一个例子：本地主机通过两次独立的write(2)调用向远程主机发送数据，第一次本地进程写入25字节的数据，并通过套接口发送到远程进程，第二次再写入30字节的数据发往远程进程，总共55字节的数据，而远程进程从套接口接收数据时，将消息作为一个完整的单元来接收，或是通过若干次独立的读操作来将数据取走，即接受端并不知道这55字节的数据是分25字节和30字节两次来发送的。

2、有序性  可以保证接受的数据字节与发送是顺序完全一致（意味着通信之前必须建立一个连接）

3、无错性   可以保证接受的数据在接收端被无错的接受。如果有错误发生，在尝试完所有的错误恢复措施后仍无法消除错误，流套接口就会报告错误。所进行的错误恢复措施尝试是完全自动的，不需编程者的指导。





SOCK_DGRAM (数据报套接字)

特征：

1、分组在发送后，可能无序地到达接收端

2、分组可能丢失。如果发生丢失，不会采取任何补救的措施，而且接受端也不必知道有分租丢失。

3、数据报分组有尺寸大小的限制，如果超出限制，在某些路由器和节点上就无法传送。

4、分组是在不建立连接的情况下被发送到远程进程的。



所以如果你需要传输的数据是准确的，建议采用TCP，也就是   SOCK_STREAM
如果你传输的是视频音频等数据，丢几个包也无所谓的，可以采用UDP，也就是  SOCK_DGRAM*/


	   if(sockfd < 0)  
        {  
        printf("socket error\n");  
        return 1;  
        }  
        //绑定  
        int ret = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));  
        if(ret < 0)  //0成功，-1不成功
        {  
        printf("bind error\n");  
        return 1;  
        }  
        //监听  
        ret = listen(sockfd, 10);  
        if(ret < 0)  
        {  
        printf("listen error\n");  
        return 1;  
        }  
        //创建线程池  
        threadpool<task> pool(20);  
        pool.start();  //线程池开始运行  
  
        while(1)  
        {  
            //printf("new connection\n");  
        socklen_t len = sizeof(client);  
        //接受连接  
        connfd = accept(sockfd, (struct sockaddr *)&client, &len);  //accept函数接受一个客户端请求后会返回一个新的SOCKFD值，当有不同的客户端同时有不同请求时，会返回不同的SOCKFD的值。这个不同的值和建立SOCKET 时生成的SOCKFD还是不同的。服务器与客户端之间的通信就是在这些不同的SOCKFD上进行的。
        
		/*
		accept函数返回值成功时返回非负值，失败时返回-1

accept函数接受一个客户端请求后会返回一个新的SOCKFD值，当有不同的客户端同时有不同请求时，会返回不同的SOCKFD的值。这个不同的值和建立SOCKET 时生成的SOCKFD还是不同的。服务器与客户端之间的通信就是在这些不同的SOCKFD上进行的。

在服务器端，socket()返回的套接字用于监听（listen）和接受（accept）客户端的连接请求。这个套接字不能用于与客户端之间发送和接收数据。accept()接受一个客户端的连接请求，并返回一个新的套接字。所谓“新的”就是说这个套接字与socket()返回的用于监听和接受客户端的连接请求的套接字不是同一个套接字。与本次接受的客户端的通信是通过在这个新的套接字上发送和接收数据来完成的。

再次调用accept()可以接受下一个客户端的连接请求，并再次返回一个新的套接字（与socket()返回的套接字、之前accept()返回的套接字都不同的新的套接字）。这个新的套接字用于与这次接受的客户端之间的通信。假设一共有3个客户端连接到服务器端。那么在服务器端就一共有4个套接字：第1个是socket()返回的、用于监听的套接字；其余3个是分别调用3次accept()返回的不同的套接字。如果已经有客户端连接到服务器端，不再需要监听和接受更多的客户端连接的时候，可以关闭由socket()返回的套接字，而不会影响与客户端之间的通信。当某个客户端断开连接、或者是与某个客户端的通信完成之后，服务器端需要关闭用于与该客户端通信的套接字。*/

		
	//fd是file descriptor的缩写，文件描述符  那么在操作这些所谓的文件的时候，我们不可能没操作一次就要找一次名字吧，这样会耗费大量的时间和效率。咱们可以每一个文件操作一个索引，这样，要操作文件的时候，我们直接找到索引就可以对其进行操作了。我们将这个索引叫做文件描述符（file descriptor），简称fd，在系统里面是一个非负的整数。每打开或创建一个文件，内核就会向进程返回一个fd，第一个打开文件是0,第二个是1,依次递增。

		
		task *ta = new task(connfd);  
        //向线程池添加任务  
        pool.append_task(ta);  
        }  
        return 0;  
}  