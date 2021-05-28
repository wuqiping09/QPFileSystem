#pragma once
#include "SuperBlock.h"
#include "Buf.h"
#include "BufferManager.h"
#include "DiskInode.h"
#include "Inode.h"
#include<string>
#include<vector>

/*
 * �ļ�ϵͳ��(FileSystem)�����ļ��洢�豸��
 * �ĸ���洢��Դ�����̿顢���INode�ķ��䡢
 * �ͷš�
 */
class FileSystem
{
public:
	//static const int NMOUNT = 5;										/* ϵͳ�����ڹ������ļ�ϵͳ��װ������� */

	static const int SUPER_BLOCK_SECTOR_NUMBER = 0;						/* ����SuperBlockλ�ڴ����ϵ������ţ�ռ��0��1���������� */

	static const int ROOTINO = 2;										/* �ļ�ϵͳ��Ŀ¼���Inode��� */

	static const int INODE_NUMBER_PER_SECTOR = 8;						/* ÿ�����̿���Դ��8�����Inode */
	static const int INODE_ZONE_START_SECTOR = 2;						/* ���Inode��λ�ڴ����ϵ���ʼ������ */
	static const int INODE_ZONE_SIZE = 10;								/* ���������Inode��ռ�ݵ������� */

	static const int DATA_ZONE_START_SECTOR = 12;						/* ����������ʼ������ */
	static const int DATA_ZONE_END_SECTOR = 2097140 - 1;				/* �������Ľ��������� */
	static const int DATA_ZONE_SIZE = 2097140 - DATA_ZONE_START_SECTOR;	/* ������ռ�ݵ��������� */

	FileSystem();
	~FileSystem();

	/*
	 * @comment ��ʼ����Ա����
	 */
	void Initialize();

	//���̸�ʽ��
	int format();

	int formatDiskInode();

	int formatDataBlock();

	//��myDisk.img��superblock�����ڴ�
	SuperBlock* loadSuperBlock(std::string disk_name);

	//���ڴ��е�superblock����myDisk.img
	int storeSuperBlock(std::string disk_name, SuperBlock* sb);

	void showSuperBlock();

	//����һ������DiskInode
	int iAlloc();

	//�ͷű��Ϊnumber��DiskInode
	void iFree(int number);

	//������д��̿�
	int alloc();

	//�ͷű��Ϊblkno�Ĵ��̿�
	void free(int blkno);

	//��ȡ�ļ���DiskInode��
	int getDiskInode(std::vector<std::string>& path);

	//����һ�����ݿ鵽�ڴ�
	void loadBlock(int block_no, int length, std::ifstream& infile, char* data);

	//����һ�����ݿ鵽�ļ�
	void loadBlock(int block_no, int length, std::ifstream& infile, std::ofstream& outfile, bool isDir);

	//����һ�����ݿ��������Ļ
	void loadBlock(int block_no, int length, std::ifstream& infile, bool is_dir);

	//���������������Ļ
	void displayBufferData(Buf* bp, int length, bool is_dir);

	//���ļ�д��һ�����ݿ�
	void storeBlock(int block_no, int length, std::fstream& inoutfile, std::ifstream& infile);

	//���ڴ�д��һ�����ݿ�
	void storeBlock(int block_no, int length, std::fstream& inoutfile, char* data);

	//��myDisk.img�����ļ�������
	int loadFile(int no_diskinode, std::string file_name);

	//���������ļ���myDisk.img
	int storeFile(std::ifstream& infile, int no_diskinode);

	//�½��ļ�
	int createFile(int no_diskinode, std::string file_name);

	//�½�Ŀ¼�ļ�
	int createDir(int no_diskinode, std::string dir_name);

	//ɾ���ļ�
	int removeFile(int no_diskinode, int no_fdiskinode);

	//��myDisk.img�����ļ�����ֱ�Ӵӻ�����ж���
	int readFile(int no_diskinode);

	//���ַ���д�뻺����ļ�
	int writeFile(int no_diskinode, std::string str);

	//���ӳ�д������д�ش���
	void updateFile();

	/*
	 * @comment �����ļ��洢�豸���豸��dev��ȡ
	 * ���ļ�ϵͳ��SuperBlock
	 */
	//SuperBlock* GetFS(short dev);

	/*
	 * @comment ��SuperBlock������ڴ渱�����µ�
	 * �洢�豸��SuperBlock��ȥ
	 */
	//void Update();

	/*
	 * @comment  �ڴ洢�豸dev�Ϸ���һ������
	 * ���INode��һ�����ڴ����µ��ļ���
	 */
	//Inode* IAlloc(short dev);

	/*
	 * @comment  �ͷŴ洢�豸dev�ϱ��Ϊnumber
	 * �����INode��һ������ɾ���ļ���
	 */
	//void IFree(short dev, int number);

	/*
	 * @comment �ڴ洢�豸dev�Ϸ�����д��̿�
	 */
	//Buf* Alloc(short dev);

	/*
	 * @comment �ͷŴ洢�豸dev�ϱ��Ϊblkno�Ĵ��̿�
	 */
	//void Free(short dev, int blkno);

	/*
	 * @comment �����ļ�ϵͳװ�������ָ��Inode��Ӧ��Mountװ���
	 */
	//Mount* GetMount(Inode* pInode);

private:
	/*
	 * @comment ����豸dev�ϱ��blkno�Ĵ��̿��Ƿ�����
	 * �����̿���
	 */
	//bool BadBlock(SuperBlock* spb, short dev, int blkno);

	/* Members */
public:
	//Mount m_Mount[NMOUNT];		/* �ļ�ϵͳװ����Mount[0]���ڸ��ļ�ϵͳ */

private:
	BufferManager* m_BufferManager;		/* FileSystem����Ҫ�������ģ��(BufferManager)�ṩ�Ľӿ� */
	int updlock;				/* Update()�����������ú�������ͬ���ڴ����SuperBlock�����Լ���
								���޸Ĺ����ڴ�Inode����һʱ��ֻ����һ�����̵��øú��� */
	DiskInode* disk_inodes[80];
};