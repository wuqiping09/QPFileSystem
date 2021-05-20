#pragma once
/*
 * ��������ڵ�(DiskINode)�Ķ���
 * ���Inodeλ���ļ��洢�豸�ϵ�
 * ���Inode���С�ÿ���ļ���Ψһ��Ӧ
 * �����Inode���������Ǽ�¼�˸��ļ�
 * ��Ӧ�Ŀ�����Ϣ��
 * ���Inode������ֶκ��ڴ�Inode���ֶ�
 * ���Ӧ�����INode���󳤶�Ϊ64�ֽڣ�
 * ÿ�����̿���Դ��512/64 = 8�����Inode
 */
class DiskInode
{
public:
	DiskInode();
	~DiskInode();

	/* i_flag�б�־λ */
	enum INodeFlag
	{
		ILOCK = 0x1,		/* �����ڵ����� */
		IUPD = 0x2,			/* �ڴ�inode���޸Ĺ�����Ҫ������Ӧ���inode */
		IACC = 0x4,			/* �ڴ�inode�����ʹ�����Ҫ�޸����һ�η���ʱ�� */
		IMOUNT = 0x8,		/* �ڴ�inode���ڹ������ļ�ϵͳ */
		IWANT = 0x10,		/* �н������ڵȴ����ڴ�inode����������ILOCK��־ʱ��Ҫ�������ֽ��� */
		ITEXT = 0x20,		/* �ڴ�inode��Ӧ����ͼ������Ķ� */
		IDIR = 0x40			/* �Ƿ���Ŀ¼�ļ� */
	};

	unsigned int d_mode;	/* ״̬�ı�־λ�������enum INodeFlag */
	int	d_nlink;			/* �ļ���������������ļ���Ŀ¼���в�ͬ·���������� */

	short d_uid;			/* �ļ������ߵ��û���ʶ�� */
	short d_gid;			/* �ļ������ߵ����ʶ�� */

	int	d_size;				/* �ļ���С���ֽ�Ϊ��λ */
	int	d_addr[10];			/* �����ļ��߼���ú�������ת���Ļ��������� */

	int	d_atime;			/* ������ʱ�� */
	int	d_mtime;			/* ����޸�ʱ�� */
};