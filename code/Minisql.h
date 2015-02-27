//
//  Minisql.h
//  Minisql
//
//  Created by xuyuhao on 14/11/2.
//  Copyright (c) 2014年 xuyuhao. All rights reserved.
//

#ifndef Minisql_Minisql_h
#define Minisql_Minisql_h
#include <stdio.h>
#include <time.h>
#include <iostream>
using namespace std;

struct blockNode
{
    int offsetNum; // the offset number in the block list
    bool pin;  // the flag that this block is locked
    bool ifbottom; // flag that this is the end of the file node
    char* fileName; // the file which the block node belongs to
    friend class BufferManager;
    
private:
    char *address; // the content address
    blockNode * preBlock;
    blockNode * nextBlock;
    bool reference; // the LRU replacement flag
    bool dirty; // the flag that this block is dirty, which needs to written back to the disk later
    size_t using_size; // the byte size that the block have used. The total size of the block is BLOCK_SIZE . This value is stored in the block head.

};

struct fileNode
{
    char *fileName;
    bool pin; // the flag that this file is locked
    blockNode *blockHead;
    fileNode * nextFile;
    fileNode * preFile;
};

extern clock_t start;
extern void print();

#define MAX_FILE_NUM 40
#define MAX_BLOCK_NUM 300
#define MAX_FILE_NAME 100

#endif
