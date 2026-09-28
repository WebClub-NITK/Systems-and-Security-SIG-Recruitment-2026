# Operating Systems

## Introduction:
Operating Systems enable users and software to interact with the underlying hardware of computer systems. As an intermediary between the hardware and software, the operating system has to carry out process management, memory management, device management, storage management, and provide an application interface through which other software can interact with and use the hardware.

## Problem Statement(s): 
This project focuses on the low-level implementation of the filesystem and file management, specifically targeting the ext2 filesystem, which is popular on Linux.  
You are provided with custom-made ext2 filesystem images in binary format. Your task is to develop a comprehensive program in a programming language of your choice (e.g., C, C++, Python, etc.) that can parse, read, modify, and manage these filesystem images concurrently.  
Your program must be able to perform the following tasks:
- Read Core Structures: Parse the filesystem image to extract and display the contents of the superblock and block group descriptors.
- Traverse Directories: Starting from the root directory, your program must be able to recursively traverse all subdirectories, printing the complete layout of the filesystem.
- Read File Contents: For any given file in the filesystem, your program should be able to locate its data blocks and display its full contents.
- Update Existing Files: Implement functionality to append data to or overwrite the contents of existing files, handling block allocation as needed.
- [Bonus] Lock-Free Concurrent Access: Design and implement a mechanism that allows multiple threads or processes to safely access and read from the filesystem simultaneously without using traditional locks, ensuring data consistency.
- [Bonus] Minimal Block Locking Writes: For write operations (updates), implement a fine-grained locking strategy that locks only the minimal set of blocks or metadata structures required for the duration of the operation. This is to maximize parallelism and minimize contention between concurrent write operations.

## Files:
[disk1.img](./Artifacts/disk-backpup.img)

## Resources:
- [Ext2 - OSDev Wiki](https://wiki.osdev.org/Ext2)  
- [Design and Implementation of the Second Extended Filesystem](https://e2fsprogs.sourceforge.net/ext2intro.html)
- [ext2 - the third extended file system ext4](https://www.linux.org/docs/man5/ext2.html)
- [Extended file system - Wikipedia](https://en.wikipedia.org/wiki/Extended_file_system)

## Submission:
- Create a private GitHub repository and add mentors as collaborators.  
- Attach a README file explaining each of the steps taken to implement the solutions, along with screenshots and shell logs wherever necessary.  
- The README must have a section for the outputs of each disk file, and must explain which tasks were completed for that file.

## Mentor:
`Nishant A S` (+91 6360219728, github: @NishantAS)