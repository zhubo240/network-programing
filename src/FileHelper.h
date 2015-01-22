/*
 * FileHelper.h
 *
 *  Created on: Jan 1, 2015
 *      Author: seed
 */

#ifndef FILEHELPER_H_
#define FILEHELPER_H_
#include "commom.h"


//no syn and mutex, because no need..
class FileHelper {
public:
	FILE* p_file;
	int size;

	FileHelper();
	//byte, mode is unrandom same ...
	void create_file(string file_name, int size/*, CREATE_MODE mode*/);

	//return the actual byte count, -1 is error
	int read_block(int begin, int size, char* buffer, int buffer_size);

	int write_block(int begin, int size, char* buffer, int buffer_size);

	int compare_with(FILE* file);

	void close();
	//int compress();

	virtual ~FileHelper();



};

#endif /* FILEHELPER_H_ */
