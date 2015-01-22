
#include "FileHelper.h"

FileHelper::FileHelper() {
	this->p_file = NULL;
	this->size = 0;
}


void FileHelper::create_file(string file_name, int size){
	if(this->p_file != NULL) return;
	
	
	this->p_file = fopen(file_name.c_str(), "rb+");
	fseek(p_file, 0, SEEK_END);
	this->size = size=ftell(this->p_file);
	
	fseek(this->p_file, 0, SEEK_SET);
	cout << size << " " << p_file << endl;
}

int FileHelper::read_block(int begin, int block_size, char* buffer, int buffer_size){
	if(buffer_size < block_size){ 
		perror("file::buffer size is less than block size\n");
		printf("file::buf is %d , block is %d\n", buffer_size, block_size);		
		fflush(stdout);
		return -1;
	}

	fseek(this->p_file, begin, SEEK_SET);
	
	int n = fread(buffer, block_size, 1, this->p_file);
	
	//cout<<"file::read begin is " << begin <<" lenght is "<< block_size << " " << n <<endl;
	
	return n*block_size;
}

int FileHelper::write_block(int begin, int block_size, char* buffer, int buffer_size){
	if(buffer_size < block_size) {
		cout << "File:: erorr, block_size is " << block_size << "buffer size is " << buffer_size <<endl;
		return -1;
	}
	
	fseek(this->p_file, begin, SEEK_SET);
	
	int n = fwrite(buffer, block_size, 1, this->p_file);

	//cout<<"file::write begin is " << begin <<" length is "<< block_size*n << " " <<endl;

	return n*block_size;
}

int FileHelper::compare_with(FILE *p_file){
	return 1;
}

void FileHelper::close(){
	fclose(this->p_file);
	this->p_file = NULL;
}

FileHelper::~FileHelper() {
	// TODO Auto-generated destructor stub
}

