#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H

#include <string>
#include <vector>
#include "Attribute.h"
#include "BufferManager.h"
#include "IndexInfo.h"
using namespace std;
class CatalogManager {
public:
    BufferManager bm;
    CatalogManager();
    virtual ~CatalogManager();
    int addIndex(string indexName,string tableName,string attributeName,int type);
    int revokeIndexOnAttribute(string tableName,string AttributeName,string indexName);
    int findTable(string tableName);
    int findIndex(string indexName);
    int dropTable(string tableName);
    int dropIndex(string index);
    int deleteValue(string tableName, int deleteNum);// delete the number of record
    int insertRecord(string tableName, int recordNum);  // increment the number of record
    int getRecordNum(string tableName);
    int indexNameListGet(string tableName, vector<string>* indexNameVector);
    int getAllIndex(vector<IndexInfo> * indexs);
    int setIndexOnAttribute(string tableName,string AttributeName,string indexName);
    int addTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName ,int primaryKeyLocation );
    int getIndexType(string indexName);
    int attributeGet(string tableName, vector<Attribute>* attributeVector);
    int calcuteLenth(string tableName);
    int calcuteLenth2(int type);
    void recordStringGet(string tableName, vector<string>* recordContent, char* recordResult);
};




#endif