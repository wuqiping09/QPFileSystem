#include "File.h"

File::File()
{
	this->f_count = 0;
	this->f_flag = 0;
	this->f_offset = 0;
	this->f_inode = nullptr;
}

File::~File()
{
	
}