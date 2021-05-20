#include "DiskInode.h"

DiskInode::DiskInode()
{
	d_mode = 0;						/* ״̬�ı�־λ�������enum INodeFlag */
	d_nlink = 0;					/* �ļ���������������ļ���Ŀ¼���в�ͬ·���������� */
	d_uid = 0;						/* �ļ������ߵ��û���ʶ�� */
	d_gid = 0;						/* �ļ������ߵ����ʶ�� */
	d_size = 0;						/* �ļ���С���ֽ�Ϊ��λ */
	for (int i = 0; i < 10; ++i)
	{
		d_addr[i] = 0;				/* �����ļ��߼���ź�������ת���Ļ��������� */
	}	
	d_atime = 0;					/* ������ʱ�� */
	d_mtime = 0;					/* ����޸�ʱ�� */
}

DiskInode::~DiskInode()
{

}