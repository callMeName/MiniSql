//
//  RecordManager.cpp
//  minisql
//
//  Created by 邓永辉 on 14/11/5.
//  Copyright (c) 2014年 邓永辉. All rights reserved.
//

#include <iostream>
#include "RecordManager.h"
#include <cstring>
#include "API.h"

/**
 *
 * create a table
 * @param tableName: name of table
 */
int RecordManager::tableCreate(string tableName)
{
    string tableFileName = tableFileNameGet(tableName);
    
    FILE *fp;
    fp = fopen(tableFileName.c_str(), "w+");
    if (fp == NULL)
    {
        return 0;
    }
    fclose(fp);
    return 1;
}

/**
 *
 * drop a table
 * @param tableName: name of table
 */
int RecordManager::tableDrop(string tableName)
{
    string tableFileName = tableFileNameGet(tableName);
    bm.delete_fileNode(tableFileName.c_str());
    if (remove(tableFileName.c_str()))
    {
        return 0;
    }
    return 1;
}

/**
 *
 * create a index
 * @param indexName: name of index
 */
int RecordManager::indexCreate(string indexName)
{
    string indexFileName = indexFileNameGet(indexName);
    
    FILE *fp;
    fp = fopen(indexFileName.c_str(), "w+");
    if (fp == NULL)
    {
        return 0;
    }
    fclose(fp);
    return 1;
}

/**
 *
 * drop a index
 * @param indexName: name of index
 */
int RecordManager::indexDrop(string indexName)
{
    string indexFileName = indexFileNameGet(indexName);
    bm.delete_fileNode(indexFileName.c_str());
    if (remove(indexFileName.c_str()))
    {
        return 0;
    }
    return 1;
}

/**
 *
 * insert a record to table
 * @param tableName: name of table
 * @param record: value of record
 * @param recordSize: size of the record
 * @return the position of block in the file(-1 represent error)
 */
int RecordManager::recordInsert(string tableName,char* record, int recordSize)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    while (true)
    {
        if (btmp == NULL)
        {
            return -1;
        }
        if (bm.get_usingSize(*btmp) <= bm.getBlockSize() - recordSize)
        {
            
            char* addressBegin;
            addressBegin = bm.get_content(*btmp) + bm.get_usingSize(*btmp);
            memcpy(addressBegin, record, recordSize);
            bm.set_usingSize(*btmp, bm.get_usingSize(*btmp) + recordSize);
            bm.set_dirty(*btmp);
            return btmp->offsetNum;
        }
        else
        {
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

/**
 *
 * print all record of a table meet requirement
 * @param tableName: name of table
 * @param attributeNameVector: the attribute list
 * @param conditionVector: the conditions list
 * @return int: the number of the record meet requirements(-1 represent error)
 */
int RecordManager::recordAllShow(string tableName, vector<string>* attributeNameVector,  vector<Condition>* conditionVector)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true)
    {
        if (btmp == NULL)
        {
            return -1;
        }
        if (btmp->ifbottom)
        {
            int recordBlockNum = recordBlockShow(tableName,attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            return count;
        }
        else
        {
            int recordBlockNum = recordBlockShow(tableName, attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

/**
 *
 * print record of a table in a block
 * @param tableName: name of table
 * @param attributeNameVector: the attribute list
 * @param conditionVector: the conditions list
 * @param blockOffset: the block's offsetNum
 * @return int: the number of the record meet requirements in the block(-1 represent error)
 */
int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);
    if (block == NULL)
    {
        return -1;
    }
    else
    {
        return  recordBlockShow(tableName, attributeNameVector, conditionVector, block);
    }
}

/**
 *
 * print record of a table in a block
 * @param tableName: name of table
 * @param attributeNameVector: the attribute list
 * @param conditionVector: the conditions list
 * @param block: search record in the block
 * @return int: the number of the record meet requirements in the block(-1 represent error)
 */
int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, blockNode* block)
{
    
    //if block is null, return -1
    if (block == NULL)
    {
        return -1;
    }
    
    int count = 0;
    
    char* recordBegin = bm.get_content(*block);
    vector<Attribute> attributeVector;
    int recordSize = api->recordSizeGet(tableName);

    api->attributeGet(tableName, &attributeVector);
    char* blockBegin = bm.get_content(*block);
    size_t usingSize = bm.get_usingSize(*block);
    
    while (recordBegin - blockBegin  < usingSize)
    {
        //if the recordBegin point to a record
        
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
        {
            count ++;
            recordPrint(recordBegin, recordSize, &attributeVector, attributeNameVector);
            printf("\n");
        }
        
        recordBegin += recordSize;
    }
    
    return count;
}

/**
 *
 * find the number of all record of a table meet requirement
 * @param tableName: name of table
 * @param conditionVector: the conditions list
 * @return int: the number of the record meet requirements(-1 represent error)
 */
int RecordManager::recordAllFind(string tableName, vector<Condition>* conditionVector)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true)
    {
        if (btmp == NULL)
        {
            return -1;
        }
        if (btmp->ifbottom)
        {
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            return count;
        }
        else
        {
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

/**
 *
 * find the number of record of a table in a block
 * @param tableName: name of table
 * @param block: search record in the block
 * @param conditionVector: the conditions list
 * @return int: the number of the record meet requirements in the block(-1 represent error)
 */
int RecordManager::recordBlockFind(string tableName, vector<Condition>* conditionVector, blockNode* block)
{
    //if block is null, return -1
    if (block == NULL)
    {
        return -1;
    }
    int count = 0;
    
    char* recordBegin = bm.get_content(*block);
    vector<Attribute> attributeVector;
    int recordSize = api->recordSizeGet(tableName);
    
    api->attributeGet(tableName, &attributeVector);
    
    while (recordBegin - bm.get_content(*block)  < bm.get_usingSize(*block))
    {
        //if the recordBegin point to a record
        
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
        {
            count++;
        }
        
        recordBegin += recordSize;
        
    }
    
    return count;
}

/**
 *
 * delete all record of a table meet requirement
 * @param tableName: name of table
 * @param conditionVector: the conditions list
 * @return int: the number of the record meet requirements(-1 represent error)
 */
int RecordManager::recordAllDelete(string tableName, vector<Condition>* conditionVector)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);

    int count = 0;
    while (true)
    {
        if (btmp == NULL)
        {
            return -1;
        }
        if (btmp->ifbottom)
        {
            int recordBlockNum = recordBlockDelete(tableName, conditionVector, btmp);
            count += recordBlockNum;
            return count;
        }
        else
        {
            int recordBlockNum = recordBlockDelete(tableName, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

/**
 *
 * delete record of a table in a block
 * @param tableName: name of table
 * @param conditionVector: the conditions list
 * @param blockOffset: the block's offsetNum
 * @return int: the number of the record meet requirements in the block(-1 represent error)
 */
int RecordManager::recordBlockDelete(string tableName,  vector<Condition>* conditionVector, int blockOffset)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);
    if (block == NULL)
    {
        return -1;
    }
    else
    {
        return  recordBlockDelete(tableName, conditionVector, block);
    }
}

/**
 *
 * delete record of a table in a block
 * @param tableName: name of table
 * @param conditionVector: the conditions list
 * @param block: search record in the block
 * @return int: the number of the record meet requirements in the block(-1 represent error)
 */
int RecordManager::recordBlockDelete(string tableName,  vector<Condition>* conditionVector, blockNode* block)
{
    //if block is null, return -1
    if (block == NULL)
    {
        return -1;
    }
    int count = 0;
    
    char* recordBegin = bm.get_content(*block);
    vector<Attribute> attributeVector;
    int recordSize = api->recordSizeGet(tableName);
    
    api->attributeGet(tableName, &attributeVector);
    
    while (recordBegin - bm.get_content(*block) < bm.get_usingSize(*block))
    {
        //if the recordBegin point to a record
        
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
        {
            count ++;
            
            api->recordIndexDelete(recordBegin, recordSize, &attributeVector, block->offsetNum);
            int i = 0;
            for (i = 0; i + recordSize + recordBegin - bm.get_content(*block) < bm.get_usingSize(*block); i++)
            {
                recordBegin[i] = recordBegin[i + recordSize];
            }
            memset(recordBegin + i, 0, recordSize);
            bm.set_usingSize(*block, bm.get_usingSize(*block) - recordSize);
            bm.set_dirty(*block);
        }
        else
        {
            recordBegin += recordSize;
        }
    }
    
    return count;
}

/**
 *
 * insert the index of all record of the table
 * @param tableName: name of table
 * @param indexName: name of index
 * @return int: the number of the record meet requirements(-1 represent error)
 */
int RecordManager::indexRecordAllAlreadyInsert(string tableName,string indexName)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true)
    {
        if (btmp == NULL)
        {
            return -1;
        }
        if (btmp->ifbottom)
        {
            int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, btmp);
            count += recordBlockNum;
            return count;
        }
        else
        {
            int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}


/**
 *
 * insert the index of a record of a table in a block
 * @param tableName: name of table
 * @param indexName: name of index
 * @param block: search record in the block
 * @return int: the number of the record meet requirements in the block(-1 represent error)
 */
 int RecordManager::indexRecordBlockAlreadyInsert(string tableName,string indexName,  blockNode* block)
{
    //if block is null, return -1
    if (block == NULL)
    {
        return -1;
    }
    int count = 0;
    
    char* recordBegin = bm.get_content(*block);
    vector<Attribute> attributeVector;
    int recordSize = api->recordSizeGet(tableName);
    
    api->attributeGet(tableName, &attributeVector);
    
    int type;
    int typeSize;
    char * contentBegin;
    
    while (recordBegin - bm.get_content(*block)  < bm.get_usingSize(*block))
    {
        contentBegin = recordBegin;
        //if the recordBegin point to a record
        for (int i = 0; i < attributeVector.size(); i++)
        {
            type = attributeVector[i].type;
            typeSize = api->typeSizeGet(type);
            
            //find the index  of the record, and insert it to index tree
            if (attributeVector[i].index == indexName)
            {
                api->indexInsert(indexName, contentBegin, type, block->offsetNum);
                count++;
            }
            
            contentBegin += typeSize;
        }
        recordBegin += recordSize;
    }
    
    return count;
}

/**
 *
 * judge if the record meet the requirement
 * @param recordBegin: point to a record
 * @param recordSize: size of the record
 * @param attributeVector: the attribute list of the record
 * @param conditionVector: the conditions
 * @return bool: if the record fit the condition
 */
bool RecordManager::recordConditionFit(char* recordBegin,int recordSize, vector<Attribute>* attributeVector,vector<Condition>* conditionVector)
{
    if (conditionVector == NULL) {
        return true;
    }
    int type;
    string attributeName;
    int typeSize;
    char content[255];
    
    char *contentBegin = recordBegin;
    for(int i = 0; i < attributeVector->size(); i++)
    {
        type = (*attributeVector)[i].type;
        attributeName = (*attributeVector)[i].name;
        typeSize = api->typeSizeGet(type);
        
        //init content (when content is string , we can get a string easily)
        memset(content, 0, 255);
        memcpy(content, contentBegin, typeSize);
        for(int j = 0; j < (*conditionVector).size(); j++)
        {
            if ((*conditionVector)[j].attributeName == attributeName)
            {
                //if this attribute need to deal about the condition
                if(!contentConditionFit(content, type, &(*conditionVector)[j]))
                {
                    //if this record is not fit the conditon
                    return false;
                }
            }
        }

        contentBegin += typeSize;
    }
    return true;
}

/**
 *
 * print value of record
 * @param recordBegin: point to a record
 * @param recordSize: size of the record
 * @param attributeVector: the attribute list of the record
 * @param attributeVector: the name list of all attribute you want to print
 */
void RecordManager::recordPrint(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector)
{
    int type;
    string attributeName;
    int typeSize;
    char content[255];
    
    char *contentBegin = recordBegin;
    for(int i = 0; i < attributeVector->size(); i++)
    {
        type = (*attributeVector)[i].type;
        typeSize = api->typeSizeGet(type);
        
        //init content (when content is string , we can get a string easily)
        memset(content, 0, 255);
        
        memcpy(content, contentBegin, typeSize);

        for(int j = 0; j < (*attributeNameVector).size(); j++)
        {
            if ((*attributeNameVector)[j] == (*attributeVector)[i].name)
            {
                contentPrint(content, type);
                break;
            }
        }
        
        contentBegin += typeSize;
    }
}

/**
 *
 * print value of content
 * @param content: point to content
 * @param type: type of content
 */
void RecordManager::contentPrint(char * content, int type)
{
    if (type == Attribute::TYPE_INT)
    {
        //if the content is a int
        int tmp = *((int *) content);   //get content value by point
        printf("%d ", tmp);
    }
    else if (type == Attribute::TYPE_FLOAT)
    {
        //if the content is a float
        float tmp = *((float *) content);   //get content value by point
        printf("%f ", tmp);
    }
    else
    {
        //if the content is a string
        string tmp = content;
        printf("%s ", tmp.c_str());
    }

}

/**
 *
 * judge if the content meet the requirement
 * @param content: point to content
 * @param type: type of content
 * @param condition: condition
 * @return bool: the content if meet
 */
bool RecordManager::contentConditionFit(char* content,int type,Condition* condition)
{
    if (type == Attribute::TYPE_INT)
    {
        //if the content is a int
        int tmp = *((int *) content);   //get content value by point
        return condition->ifRight(tmp);
    }
    else if (type == Attribute::TYPE_FLOAT)
    {
        //if the content is a float
        float tmp = *((float *) content);   //get content value by point
        return condition->ifRight(tmp);
    }
    else
    {
        //if the content is a string
        return condition->ifRight(content);
    }
    return true;
}

/**
 *
 * get a index's file name
 * @param indexName: name of index
 */
string RecordManager::indexFileNameGet(string indexName)
{
    string tmp = "";
    return "INDEX_FILE_"+indexName;
}

/**
 *
 * get a table's file name
 * @param tableName: name of table
 */
string RecordManager::tableFileNameGet(string tableName)
{
    string tmp = "";
    return tmp + "TABLE_FILE_" + tableName;
}
