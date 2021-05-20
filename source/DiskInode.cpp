#include "DiskInode.h"

DiskInode::DiskInode()
{
	d_mode = 0;						/* 状态的标志位，定义见enum INodeFlag */
	d_nlink = 0;					/* 文件联结计数，即该文件在目录树中不同路径名的数量 */
	d_uid = 0;						/* 文件所有者的用户标识数 */
	d_gid = 0;						/* 文件所有者的组标识数 */
	d_size = 0;						/* 文件大小，字节为单位 */
	for (int i = 0; i < 10; ++i)
	{
		d_addr[i] = 0;				/* 用于文件逻辑块号和物理块号转换的基本索引表 */
	}	
	d_atime = 0;					/* 最后访问时间 */
	d_mtime = 0;					/* 最后修改时间 */
}

DiskInode::~DiskInode()
{

}