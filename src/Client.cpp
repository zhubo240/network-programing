#include "Client.h"
#include "FileHelper.h"

extern struct sockaddr_in server_addr;

extern FileHelper *p_client_file_helper, *p_server_file_helper;

Client::Client() {
	this->clientNum = curClientNum++;
	this->begin_address = (p_server_file_helper->size / numClients) * this->clientNum;
	this->length = p_server_file_helper->size / numClients;
	

	this->server_address = server_addr;

	/*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/
	if ((this->client_socketfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	/*将套接字绑定到服务器的网络地址上*/
	if (connect(this->client_socketfd,
			(struct sockaddr *) &(this->server_address),
			sizeof(struct sockaddr)) < 0) {
		perror("connect error");
		exit(1);
	}
}


int Client::numClients = 1;
int Client::curClientNum = 0;
int Client::block_size = 1024 * 1024 * 2;

void Client::run() {

	int num_blocks = this->length / block_size;
	
	char request[20] = {0};
	char recv_buf[BLOCK_BUF_SIZE] = {0};
	int n;
	int begin = 0;
	for(int block_num = 0; block_num < num_blocks; block_num++){
		memset(request, 0x00, sizeof(request));
		memset(recv_buf, 0x00, sizeof(recv_buf));
		
		build_request(begin, block_size, request);
		
		//printf("client::request is %s, block_num is %d\n", request, block_num);

		n = send(this->client_socketfd, request, strlen(request), 0);
		if(n <= 0){
			cout << "server:: socket is closed\n" << endl;
		}

		recv_block(recv_buf, sizeof(recv_buf), block_size);

		p_client_file_helper->write_block(begin, block_size, recv_buf, sizeof(recv_buf));
		begin += block_size;
		
		//cout << endl;
	}

	int left_block_size = p_server_file_helper->size % block_size;
	if( left_block_size > 0){
		memset(request, 0x00, sizeof(request));
		memset(recv_buf, 0x00, sizeof(recv_buf));

		build_request(begin, left_block_size, request);
		send(this->client_socketfd, request, strlen(request), 0);
		
		int n;
		//recv from server
		recv_block(recv_buf, sizeof(recv_buf), left_block_size);
		
		n = p_client_file_helper->write_block(begin, left_block_size, recv_buf, sizeof(recv_buf));
		//cout << endl;
	}
}

void Client::build_request(int begin, int length, char* request){
	int n = sprintf(request,"%d$%d$", begin, length);
	request[n] = '\0';
}

void Client::recv_block(char* recv_buf, int buf_size, int block_size){
		int accCount = 0;
		
		int n;
		//recv from server
		while(1){
			if ((n = recv(this->client_socketfd, recv_buf + accCount, buf_size - accCount, 0))
					<= 0) {
				cout << "client::server is closed\n" << endl;
			}
			//printf("client::recv %d from server\n", n);	
			
			accCount += n;

			if(accCount == block_size) break;
			if(accCount > block_size){
				perror("client::error in recv a block");
				exit(1);			
			}
		}
}

Client::~Client() {
	// TODO Auto-generated destructor stub
}

