
CC = cl

OBJS = 	Buf.obj \
		BufferManager.obj \
		DirectoryEntry.obj \
		DiskInode.obj \
		FileManager.obj \
		FileSystem.obj \
		Inode.obj \
		SuperBlock.obj

#-c -D_X86=1 -DWIN32 -D_DEBUG -D_CONSOLE

all: main.exe

main.exe: main.obj $(OBJS)
	link main.obj $(OBJS)

main.obj: main.cpp
	$(CC) -c main.cpp

Buf.obj: Buf.cpp
	$(CC) -c Buf.cpp  

BufferManager.obj: BufferManager.cpp
	$(CC) -c BufferManager.cpp

DirectoryEntry.obj: DirectoryEntry.cpp
	$(CC) -c DirectoryEntry.cpp
	
DiskInode.obj: DiskInode.cpp
	$(CC) -c DiskInode.cpp  

FileManager.obj: FileManager.cpp
	$(CC) -c FileManager.cpp

FileSystem.obj: FileSystem.cpp
	$(CC) -c FileSystem.cpp

Inode.obj: Inode.cpp
	$(CC) -c Inode.cpp
	
SuperBlock.obj: SuperBlock.cpp
	$(CC) -c SuperBlock.cpp

clean:
	del *.obj *.exe



