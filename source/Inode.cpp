#include "Inode.h"

Inode::Inode()
{
	/* ���Inode�����е����� */
	// this->Clean(); 
	/* ȥ��this->Clean();�����ɣ�
	 * Inode::Clean()�ض�����IAlloc()������·���DiskInode��ԭ�����ݣ�
	 * �����ļ���Ϣ��Clean()�����в�Ӧ�����i_dev, i_number, i_flag, i_count,
	 * ���������ڴ�Inode����DiskInode�����ľ��ļ���Ϣ����Inode�๹�캯����Ҫ
	 * �����ʼ��Ϊ��Чֵ��
	 */

	 /* ��Inode����ĳ�Ա������ʼ��Ϊ��Чֵ */
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