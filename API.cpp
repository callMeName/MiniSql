//
//  API.cpp
//  minisql
//
//  Created by 邓永辉 on 14/11/5.
//  Copyright (c) 2014年 邓永辉. All rights reserved.
//

#include "API.h"

#define UNKNOWN_FILE 8
#define TABLE_FILE 9
#define INDEX_FILE 10

RecordManager rm;
CatalogManager cm;

/**
 *
 * drop a table
 * @param tableName: name of table
 */
void API::tableDrop(string tableName)
{
    if(cm.findFile(tableName) != TABLE_FILE)
    {
        cout << "There is no table " << tableName << endl;
        return;
    }
    
    if(rm.tableDrop(tableName))
        cout << "Drop table " << tableName << " successfully" << endl;
}

/**
 *
 * drop a index
 * @param indexName: name of index
 */
void API::indexDrop(string indexName)
{
    if (cm.findFile(indexName) != INDEX_FILE)
    {
        cout << "There is no index " << indexName << endl;
        return;
    }
    if (rm.indexDrop(indexName))
        cout << "Drop index " << indexName << " successfully" << endl;
}

//创建一个新的index
//输入index的名称, talbe的名称 和talbe中属性的名称
void indexCreate(string indexName, string tableName, string attributeName)
{
    if (cm.findFile(indexName) != INDEX_FILE)
    {
        cout << "There is index " << indexName << "already" << endl;
        return;
    }
    
    cm.addIndex(indexName, tableName, attributeName);
    cout << "Drop index " << indexName << " successfully" << endl;
}