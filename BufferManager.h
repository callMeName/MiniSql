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


static int replaced_block = -1;


class BufferManager
{
    private:
        fileNode *fileHead;
        fileNode file_pool[MAX_FILE_NUM];
        blockNode block_pool[MAX_BLOCK_NUM];
        int total_block; // the number of block that have been used, which means the block is in the list.
        int total_file; // the number of file that have been used, which means the file is in the list.
        void init_block(blockNode & block);
        void init_file(fileNode & file);
        blockNode* getBlock(const char* fileName,blockNode* position,bool if_pin = false);
        void writtenBackToDiskAll();
        void writtenBackToDisk(const char* fileName,blockNode* block);
        void clean_dirty(blockNode &block);
    
    public:
        BufferManager();
        ~BufferManager();
        fileNode* getFile(const char* fileName,bool if_pin = false);
        void set_dirty(blockNode & block);
        void set_pin(blockNode & block,bool pin);
        void set_pin(fileNode & file,bool pin);
        blockNode* getNextBlock(const char* fileName,blockNode* block);
        blockNode* getBlockHead(const char* fileName,fileNode* file);
};

#endif /* defined(__Minisql__BufferManager__) */


