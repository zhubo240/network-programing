#ifndef CLIENT_H_
#define CLIENT_H_

#include "commom.h"
#include "FileHelper.h"


class Client {
public:
	Client();
	void run();
	void build_request(int begin, int length, char* buffer);
	void recv_block(char* recv_buf, int buf_size, int block_size);
	virtual ~Client();

	static int numClients;
	static int curClientNum;
	static int block_size;
	
private:

	FileHelper p_file_helper;
	int client_socketfd;
	struct sockaddr_in server_address;
	int clientNum;
	int begin_address;
	int length;
};

#endif /* CLIENT_H_ */
