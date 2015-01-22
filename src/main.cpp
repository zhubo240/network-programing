
#include "commom.h"
#include "Server.h"
#include "Client.h"
#include "FileHelper.h"

using namespace std;


pthread_t client;
struct sockaddr_in server_addr;   //server_addr
sem_t* syn = sem_open("/semsyn1", O_CREAT, 0644, 0);

FileHelper *p_client_file_helper, *p_server_file_helper;
char buffer[1024*1204*20]; 
void runCopy();
void transmitFile();

void* run_server(void *empty)
{ 
	Server* p_server = new Server();
	sem_post(syn);
   	p_server->run(NULL);
	return NULL;
}

void* run_client(void* empty)
{
	Client* p_client = new Client();
   	p_client->run();
	return NULL;
}

int main(int argc, char* argv[]) {
	// init network address
	memset(&(server_addr), 0xff, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8000);


	//init file helper
	p_client_file_helper = new FileHelper();
	p_server_file_helper = new FileHelper();
	p_server_file_helper->create_file(string("1g.file"), 0);
	p_client_file_helper->create_file(string("client_file"), 0);


	
	//server
	pthread_t server;
	pthread_create(&server, NULL, run_server, NULL);

	sem_wait(syn);

	time_t a,b;
	

	if(argc == 1){
		a = time(NULL);
		transmitFile();
		if(pthread_join(client, NULL) < 0){
			perror("thread join error");
		}
		b = time(NULL);

	}else{
		if(0 == strcmp("best", argv[1])){
			
			a = time(NULL);
			runCopy();
			b = time(NULL);
		}
	}
	
	
	cout << "time is :" << b -a <<endl;
	p_client_file_helper->close();
	p_server_file_helper->close();

	if(pthread_join(server, NULL) < 0){
		perror("thread join error");
	}

	return 0;
}

void runCopy(){
	int block_size = Client::block_size;
	int num_blocks = p_server_file_helper->size / block_size;
	int begin = 0;	
	cout << "block size is " << block_size <<endl;

	for(int block_num = 0; block_num < num_blocks; block_num++){
		p_server_file_helper->read_block(begin, block_size, buffer, sizeof(buffer));
		p_client_file_helper->write_block(begin, block_size, buffer, sizeof(buffer));
		begin += block_size;
	}
	
	int left = p_server_file_helper->size % block_size;
	if(left){
		p_server_file_helper->read_block(begin, left, buffer, sizeof(buffer));
		p_client_file_helper->write_block(begin, left, buffer, sizeof(buffer));
	}
}

void transmitFile(){
		for (int i = 0; i < Client::numClients; i++) {
			puts("create client");
			
			pthread_create(&client, NULL, run_client, NULL);
		}

		
}

