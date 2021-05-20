#pragma once
/*
 * 外存索引节点(DiskINode)的定义
 * 外存Inode位于文件存储设备上的
 * 外存Inode区中。每个文件有唯一对应
 * 的外存Inode，其作用是记录了该文件
 * 对应的控制信息。
 * 外存Inode中许多字段和内存Inode中字段
 * 相对应。外存INode对象长度为64字节，
 * 每个磁盘块可以存放512/64 = 8个外存Inode
 */
class DiskInode
{
public:
	DiskInode();
	~DiskInode();

	/* i_flag中标志位 */
	enum INodeFlag
	{
		ILOCK = 0x1,		/* 索引节点上锁 */
		IUPD = 0x2,			/* 内存inode被修改过，需要更新相应外存inode */
		IACC = 0x4,			/* 内存inode被访问过，需要修改最近一次访问时间 */
		IMOUNT = 0x8,		/* 内存inode用于挂载子文件系统 */
		IWANT = 0x10,		/* 有进程正在等待该内存inode被解锁，清ILOCK标志时，要唤醒这种进程 */
		ITEXT = 0x20,		/* 内存inode对应进程图像的正文段 */
		IDIR = 0x40			/* 是否是目录文件 */
	};

	unsigned int d_mode;	/* 状态的标志位，定义见enum INodeFlag */
	int	d_nlink;			/* 文件联结计数，即该文件在目录树中不同路径名的数量 */

	short d_uid;			/* 文件所有者的用户标识数 */
	short d_gid;			/* 文件所有者的组标识数 */

	int	d_size;				/* 文件大小，字节为单位 */
	int	d_addr[10];			/* 用于文件逻辑块好和物理块好转换的基本索引表 */

	int	d_atime;			/* 最后访问时间 */
	int	d_mtime;			/* 最后修改时间 */
};