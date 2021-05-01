# File System Utility

This project implements utilities that perform operations on a file system similar to Microsoft's FAT file system.

## Part I

Here, this program displays information about the file system. The program reads the file system super block and use the information in the super block to read in the FAT.

The program can be invoked as follows:
```
./diskinfo disk1_empty.img
```
Sample output:
```
Super block information:
Block size: 512
Block count: 15360
FAT starts: 1
FAT blocks: 120
Root directory start: 121
Root directory blocks: 4
FAT information:
Free Blocks: 15235
Reserved Blocks: 121
Allocated Blocks: 4
```
## Part II

This program displays the contents of the root directory in the file system.

The program can be invoked as follows:

```
./disklist disk1.img
```
For example:
```
F     91      readme.txt    2016/03/02    12:20:19
F     56      bar.txt       2016/03/02    12:20:43
F     25600   file3.txt     2016/03/02    12:20:51
F     51200   file4.txt     2016/03/02    12:20:55
F     38512   ls_mac        2016/03/02    12:21:42
```

## Part III

This part is about copying a file from the file system to the current directory in Unix. If the specified file is not found in the root directory of the file system, the program outputs the message “File not found.” on a single line and exits.

The program can be invoked as follows:
```
./diskget disk1.img foo.txt
```

## Part IV

This part is about copying a file from the current Unix directory into the file system. If the specified file is not found, you should output the message “File not found.” on a single line and exit.

The program can be invoked as follows:
```
./diskput disk1.img foo.txt
```
