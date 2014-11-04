//
//  BufferManager.h
//  Minisql
//
//  Created by xuyuhao on 14/11/1.
//  Copyright (c) 2014年 xuyuhao. All rights reserved.
//

#ifndef __Minisql__BufferManager__
#define __Minisql__BufferManager__
#include "Minisql.h"
#include <stdio.h>

struct blockNode
{
    int offsetNum;
    bool pin;
    bool dirty;
    bool reference;
    char *address;
    blockNode * nextBlock;
    blockNode * preBlock;
};

struct fileNode
{
    char *fileName;
    bool pin;
   // bool is_using;
    blockNode *blockHead;
    fileNode * nextFile;
    fileNode * preFile;
};
static int replaced_block = -1;

class BufferManager
{
    private:
        fileNode *fileHead;
        fileNode file_pool[MAX_FILE_NUM];
        blockNode block_pool[MAX_BLOCK_NUM];
        int total_block;
        int total_file;
        void init_block(blockNode &block);
        void init_file(fileNode &file);
    public:
        BufferManager();
        ~BufferManager();
        fileNode* getFile(const char* fileName,bool if_pin = false);
        blockNode* getBlock(const char* fileName,blockNode* position,bool if_pin = false);
        void writtenBackToDiskAll();
        void writtenBackToDisk(const char* fileName,blockNode* block);
        void set_dirty(blockNode &block);
        void clean_dirty(blockNode &block);
        void set_pin(blockNode &block,bool pin);
        void set_pin(fileNode &file,bool pin);
};

#endif /* defined(__Minisql__BufferManager__) */


