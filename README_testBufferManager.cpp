//
//  main.cpp
//  Minisql
//
//  Created by xuyuhao on 14/10/29.
//  Copyright (c) 2014年 xuyuhao. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <string>
#include "Minisql.h"
#include "BufferManager.h"
using namespace std;

int main(){
    cout << "这里是BufferManager的示例程序，正式使用时请删除本程序！" << endl;
    BufferManager bm;
    const char* fileName = "Student";
    
    
    fileNode *ftmp =  bm.getFile(fileName); // 获得文件节点
    blockNode *btmp = bm.getBlockHead(ftmp ); //获得文件节点的首个block节点
    
    /*
     if you want to lock the block node , you must also lock the file node.
     bm.set_pin(*btmp, true );
     bm.set_pin(*ftmp, true );
     */
    
    cout << "第一个block中的using_size " << btmp -> using_size << endl;
    
    cout << "====begin===" << endl;
    int recordLen = 5000;
    while(1)
    {
        if(btmp->using_size <= BLOCK_SIZE - recordLen)
        {
            cout << "如果需要修改这个block，请使用set_dirty函数,如：bm.set_dirty(*btmp)" << endl;
            bm.set_dirty(*btmp);
            
            cout << "修改该block前, using_size:" << btmp->using_size << endl;
            for(int i = 0; i < recordLen; i ++)
                strcat(btmp->address+btmp->using_size,"a");
            
            cout << "修改之后，请更改该block的using_size，如：btmp->using_size += recordLen" << endl;
            btmp->using_size += recordLen;
            
            cout << "btmp -> fileName: " << btmp->fileName << endl;
            
            cout << "修改该block后, using_size" << btmp->using_size << endl;
            cout << "修改之后，btmp->address:" << btmp->address[1] << endl;
            break;
        }
        else
        {
            cout << "发现在上一个block中没有空余字节，获得下一个block，如：btmp = bm.getNextBlock(fileName, btmp)" << endl;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    
    cout << "====exit==="<< endl;
    
    return 0;
}