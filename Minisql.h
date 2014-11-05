//
//  Minisql.h
//  Minisql
//
//  Created by xuyuhao on 14/11/2.
//  Copyright (c) 2014年 xuyuhao. All rights reserved.
//

#ifndef Minisql_Minisql_h
#define Minisql_Minisql_h

struct blockNode
{
    int offsetNum; // the offset number in the block list
    bool pin;  // the flag that this block is locked
    bool dirty; // the flag that this block is dirty, which needs to written back to the disk later
    bool reference; // the LRU replacement flag
    int using_size; // the byte size that the block have used
    char *address; // the content address
    blockNode * preBlock;
    blockNode * nextBlock;
    char* fileName; // the file which the block node belongs to
};

struct fileNode
{
    char *fileName;
    bool pin; // the flag that this file is locked
    blockNode *blockHead;
    fileNode * nextFile;
    fileNode * preFile;
};

#define MAX_FILE_NUM 40
#define MAX_BLOCK_NUM 100
#define BLOCK_SIZE 8192
#define MAX_FILE_NAME 30

#endif
