#include "Buf.h"

Buf::Buf()
{
	b_flags = 0;			/* 缓存控制块标志位 */
	padding = 0;			/* 4字节填充，使得b_forw和b_back在Buf类中与Devtab类中的字段顺序能够一致，否则强制转换会出错。 */
	/* 缓存控制块队列勾连指针 */
	b_forw = nullptr;
	b_back = nullptr;
	av_forw = nullptr;
	av_back = nullptr;
	b_dev = -1;				/* 主、次设备号，其中高8位是主设备号，低8位是次设备号 */
	b_wcount = 0;			/* 需传送的字节数 */
	b_addr = nullptr;		/* 指向该缓存控制块所管理的缓冲区的首地址 */
	b_blkno = -1;			/* 磁盘逻辑块号 */
	b_error = 0;			/* I/O出错时信息 */
	b_resid = 0;			/* I/O出错时尚未传送的剩余字节数 */
}

Buf::~Buf()
{

}