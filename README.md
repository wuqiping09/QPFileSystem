# A File System based on Unix V6++  
This file system is a two-level file system. The name of the disk is "myDisk.img", which is a big file in the host machine. The operating system of the host machine is  Windows 10.  

## Compile Instruction  
Compile with MSVC.  
In command line window, enter source folder, and execute nmake.  
```
>>>cd source
>>>nmake
```
All .obj files and .exe files will be created.  
Delete all .obj files and .exe files with nmake clean.  
```
>>>nmake clean
```

## File System Instruction  
All operations should be executed by command line.  
Run main.exe to start command line mode.   
1. format  
Format "myDisk.img".  
No parameter.  
eg.  
```
>>>format
```
---
2. mkdir \<dir>  
Create a folder.  
parameters:
- dir---the path of the folder  
eg. Create folder "text" under "/home".  
```
>>>mkdir /home/texts
```
---
3. create \<filename>  
Create a new file.  
parameters:
- filename---the name of the file(include path)  
eg. Create file "1.txt" under "/etc".  
```
>>>create /etc/1.txt
```
---
4. rm \<filename>  
Delete a file or a folder.  
parameters:
- filename---the name of a file or a folder(include path)  
eg. Delete file "1.txt" under "/etc". Delete folder "dev" under "/".  
```
>>>rm /etc/1.txt
>>>rm /dev
```
---
5. read \<filename>  
Read the content of a file or the directory items of a folder.  
parameters:
- filename---the name of the file or the folder(include path)  
eg. Read the content of file "1.txt" under "/etc". Read the directory items of root folder.  
```
>>>read /etc/1.txt
>>>read /
```
---
6. write \<filename> \<str>  
Write(delay write) content into a file.  
parameters:
- filename---the name of the file(include path)  
- str---the content to write(no space contained)  
eg. Write "abc" into file "1.txt" under "/etc".  
```
>>>write /etc/1.txt abc
```
---
7. load \<filename>  
Load a file or the directory items of a folder from "myDisk.img" to the host machine. The name of the loaded file or folder will be the name(include path) in "myDisk.img" but replacing '/' with '\_'.  
parameters:
- filename---the name of the file or the folder(include path) in "myDisk.img"  
eg. Load file "1.jpg" under "/etc" in "myDisk.img" into the host machine. The name of the loaded file is "_etc_1.jpg".
```
>>>load /etc/1.jpg
```
---
8. store \<filename_out> \<filename_in>  
Store a file from the host machine to "myDisk.img".  
parameters:
- filename_out---the name of the file(include path) in the host machine
- filename_in---the name of the file(include path) in "myDisk.img"  
eg. Store the file "1.jpg" in the host machine into "myDisk.img" under "/etc". The name of the stored file is "1.jpg".
```
>>>store 1.jpg /etc/1.jpg
```
---
9. shutdown  
Exit the file system.  
No parameter.  
eg.  
```
>>>shutdown
```
