#include "Inode.h"

Inode::Inode()
{
	/* 清空Inode对象中的数据 */
	// this->Clean(); 
	/* 去除this->Clean();的理由：
	 * Inode::Clean()特定用于IAlloc()中清空新分配DiskInode的原有数据，
	 * 即旧文件信息。Clean()函数中不应当清除i_dev, i_number, i_flag, i_count,
	 * 这是属于内存Inode而非DiskInode包含的旧文件信息，而Inode类构造函数需要
	 * 将其初始化为无效值。
	 */

	 /* 将Inode对象的成员变量初始化为无效值 */
	this->i_flag = 0;
	this->i_mode = 0;
	this->i_count = 0;
	this->i_nlink = 0;
	this->i_dev = -1;
	this->i_number = -1;
	this->i_uid = -1;
	this->i_gid = -1;
	this->i_size = 0;
	this->i_lastr = -1;
	for (int i = 0; i < 10; i++)
	{
		this->i_addr[i] = 0;
	}
}

Inode::~Inode()
{
	
}