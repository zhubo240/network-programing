

#include "Server.h"
#include "FileHelper.h"

extern struct sockaddr_in server_addr;
extern FileHelper *p_client_file_helper, *p_server_file_helper;

Server::Server() {

	this->server_address = server_addr;

	//创建服务器端套接字--IPv4协议，面向连接通信，TCP协议
	if ((server_socketfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("server::socket");
		exit(1);
	}

	//将套接字绑定到服务器的网络地址上
	if (bind(server_socketfd, (struct sockaddr *) &(server_address),
			sizeof(struct sockaddr)) < 0) {
		perror("server::bind error");
		exit(1);
	}
	//监听连接请求--监听队列长度为5
	//不会阻塞
	listen(server_socketfd, 5);
	printf("server::start to listen clients' connects request.\n");

	//创建一个epoll的句柄，size用来告诉内核这个监听的数目一共有多大
	epollfd = epoll_create(256); //生成用于处理accept的epoll专用的文件描述符

	//把socket设置为非阻塞方式
	//setnonblocking(listenfd);

	//epoll
	//设置与要处理的事件相关的文件描述符
	epoll_event.data.fd = server_socketfd;

	//设置要处理的事件类型
	epoll_event.events = EPOLLIN | EPOLLET;

	//注册epoll事件,第三个参数？？
	epoll_ctl(epollfd, EPOLL_CTL_ADD, server_socketfd, &epoll_event);
}

void* Server::run(void* empty) {
	unsigned sin_size = sizeof(struct sockaddr_in);
	int client_socketfd;
	struct sockaddr_in client_address;


	while (1) {

		int nfds = epoll_wait(epollfd, events, 20, 500);

		//处理所发生的所有事件
		for (int i = 0; i < nfds; ++i) {
			//如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。
			if (events[i].data.fd == server_socketfd) {

				int new_clientfd = accept(server_socketfd,
						(sockaddr *) &client_address, &sin_size);
				if (new_clientfd < 0) {
					perror("server::new_clientfd < 0");
					exit(1);
				}

				//setnonblocking(new_clientfd);
				char *str = inet_ntoa(client_address.sin_addr);

				cout << "server::accept a connection from " << str << endl;

				//设置用于读操作的文件描述符
				epoll_event.data.fd = new_clientfd;

				//设置用于注册的读操作事件
				epoll_event.events = EPOLLIN | EPOLLET;

				//注册ev
				epoll_ctl(epollfd, EPOLL_CTL_ADD, new_clientfd, &epoll_event); /* 添加 */
			}
			//如果是已经连接的用户，并且收到数据，那么进行读入。
			else if (events[i].events & EPOLLIN) {
				int sockfd, n;
				char buf[100];

				if ((sockfd = events[i].data.fd) < 0){
					cout << "server::get illegal fd during read" << endl;
					continue;
				}

				//recv is zero
				if ((n = recv(sockfd, buf, sizeof(buf), 0)) < 0) {
					cout << "server::error , during read from socketfd, maybe socket is closed" << endl;
					close(sockfd);
					events[i].data.fd = -1;
					continue;
				} else if (n == 0) { //读入的数据为空
					cout << "server::recv return 0, during read from socketfd" << endl;
					continue;
				}
				buf[n] = '\0';

				//cout << "server:: buf actually recv n is " << n <<endl;

				//cout << "server::get msg   \"" + string(buf) + "\"   from fd ["<< sockfd <<"]" << endl;
				n = response_client(buf, sockfd);
				//printf("server::response %s successfully, size is %d\n", buf, n);
				
				memset(buf, 0, sizeof(buf)); /* clear the buffer */

			} else if (events[i].events & EPOLLOUT) { // 如果有数据发送
				int client_socketfd;
				int n;

				//char* p_data = (char*) events[i].data.ptr;
				client_socketfd = events[i].data.fd;

				if ((n = send(client_socketfd, "hi", strlen("hi"), 0))
						< 0) {
					cout << "server::send error" << endl;
					close(client_socketfd);
					events[i].data.fd = -1;
				} else if (n == 0) { //读入的数据为空
					cout << "server::error , during write to socketfd, maybe socket is closed" << endl;
					close(client_socketfd);
					events[i].data.fd = -1;
				} else {
					printf("server::Server have replied   \"%s\"  to fd [%d] successfully!\n", "hi", client_socketfd );
					//设置用于读操作的文件描述符
					epoll_event.data.fd = client_socketfd;

					//设置用于注册的【【读】】操作事件
					epoll_event.events = EPOLLIN | EPOLLET;

					//修改sockfd上要处理的事件为EPOLIN
					epoll_ctl(epollfd, EPOLL_CTL_MOD, client_socketfd,
							&epoll_event);
				}
			}
		} //(over)处理所发生的所有事件
	}

	return NULL;
}

void Server::parse_client_request(char* request, int &begin, int &length){
	sscanf(request, "%d$%d$", &begin, &length);
}

int Server::response_client(char* request, int client_socketfd){
	int begin, length;
	parse_client_request(request, begin, length);
	
	char buffer[BLOCK_BUF_SIZE] = {0}; 	
	
	int readedNum = p_server_file_helper->read_block(begin, length, buffer, sizeof(buffer));
	
	
	if(readedNum == -1){
		perror("server::server read file error\n");
		exit(1);
	}


	int n;
	if ((n = send(client_socketfd, buffer, readedNum, 0)) < 0) {
		perror("server::send error");
		close(client_socketfd);
		return -1;
	} else if (n == 0) { //
		cout << "server::socket closed" << endl;
		close(client_socketfd);
		return -1;
	}
	//cout << "server::server send " << n <<endl;
	return readedNum;
}

Server::~Server() {

}

