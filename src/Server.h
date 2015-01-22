

#ifndef NETWORK_PROGRAMMING_SERVER_H_
#define NETWORK_PROGRAMMING_SERVER_H_

#include "commom.h"

class Server {
public:
	Server();
	virtual ~Server();
	void* run(void*);
	void parse_client_request(char* request, int &begin, int &length);
	int response_client(char* request, int client_socketfd);

private:
	int server_socketfd;
	struct sockaddr_in server_address;   //server_address
	struct epoll_event epoll_event, events[20];
	int epollfd;
};

#endif /* NETWORK_PROGRAMMING_SERVER_H_ */
