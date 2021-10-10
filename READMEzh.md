# 基于Unix v6++的文件系统  
本文件系统为二级文件系统，磁盘名为"myDisk.img"，是宿主机中的一个大文件。本系统宿主机的操作系统为Windows10。  

## 编译说明  
使用MSVC进行编译。在命令行窗口中进入source文件夹，执行nmake。  
```
>>>cd source
>>>nmake
```
而后会自动生成所有的.obj文件和.exe文件。  
使用nmake clean清楚所有.obj文件和.exe文件。  
```
>>>nmake clean
```

## 操作说明  
所有操作均通过命令行进行。  
启动main.exe文件，进入命令行模式。  
1. format  
格式化myDisk.img。  
参数：无。  
例：  
```
>>>format
```
---
2. mkdir < dir >  
新建目录文件。  
参数：
- dir---目录文件路径  
例：在home目录下新建texts目录文件。  
```
>>>mkdir /home/texts
```
---
3. create < filename >  
新建文件。  
参数：
- filename---文件名（包括路径）  
例：在etc目录下新建1.txt文件。  
```
>>>create /etc/1.txt
```
---
4. rm < filename >  
删除文件或目录文件。  
参数：
- filename---文件名（包括路径）或路径。  
例：删除etc目录下的1.txt文件和dev目录。   
```
>>>rm /etc/1.txt
>>>rm /dev
```
---
5. read < filename >  
读文件或目录文件。  
参数：
- filename---文件名（包括路径）或路径。  
例：读etc目录下的1.txt文件和根目录。  
```
>>>read /etc/1.txt
>>>read /
```
---
6. write < filename > < str >  
写文件。此处为延迟写。  
参数：
- filename---文件名（包括路径）。  
- str---需要写入的字符串（不能包含空格）。  
例：向etc目录下的1.txt文件中写入"abc"。  
```
>>>write /etc/1.txt abc
```
---
7. load < filename >  
将myDisk.img中的文件或目录文件载入宿主机中。宿主机中的文件名为myDisk.img中的文件名（包括路径），其中‘/’用‘_’代替。    
参数:
- filename---文件在myDisk.img中的文件名（包括路径）。  
例：将myDisk.img中etc目录下的1.jpg文件载入宿主机中，文件名为_etc_1.jpg。  
```
>>>load /etc/1.jpg
```
---
8. store < filename_out > < filename_in >  
将宿主机中的文件存入myDisk.img中。  
参数：
- filename_out---宿主机中的文件名（包括路径）。
- filename_in---myDisk.img中的文件名（包括路径）。
例：将宿主机中的1.jpg文件存入myDisk.img中的etc目录下，名为1.jpg。
```
>>>store 1.jpg /etc/1.jpg
```
---
9. shutdown  
退出文件系统。无参数。  
例：  
```
>>>shutdown
```
