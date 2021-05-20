#include "SuperBlock.h"

SuperBlock::SuperBlock()
{
	//初始化superblock，磁盘共1G，其中superblock占据0#和1#，外存inode占据2#到11#，其余12#到2097151#为数据区
	s_isize = 10;					/* 外存Inode区占用的盘块数 */
	s_fsize = 2097152;				/* 盘块总数 */
	s_nfree = 100;					/* 直接管理的空闲盘块数量 */
	/* 105#盘块为队首数据块，6#到104#数据块可用 */
	s_free[0] = 105;
	for (int i = 1; i < 100; ++i)	
	{
		s_free[i] = i + 5;
	}
	s_ninode = 73;					/* 直接管理的空闲外存Inode数量 */
	for (int i = 0; i < 73; ++i)	/* 7#到79#外存Inode */
	{
		s_inode[i] = i + 7;
	}
	s_flock = 0;					/* 封锁空闲盘块索引表标志 */
	s_ilock = 0;					/* 封锁空闲Inode表标志 */

	s_fmod = 1;						/* 内存中super block副本被修改标志，意味着需要更新外存对应的Super Block */
	s_ronly = 0;					/* 本文件系统只能读出 */
	s_time = 0;						/* 最近一次更新时间 */
	for (int i = 0; i < 47; ++i)	/* 填充使SuperBlock块大小等于1024字节，占据2个扇区 */
	{
		padding[i] = 0;
	}
}

SuperBlock::~SuperBlock()
{

}
