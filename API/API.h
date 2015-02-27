#ifndef API_H
#define API_H

#include "Attribute.h"
#include "Condition.h"
#include "Minisql.h"
#include "IndexInfo.h"
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>

class CatalogManager;
class RecordManager;
class IndexManager;
class API{
public:
    RecordManager *rm;
    CatalogManager *cm;
    IndexManager *im;
	API(){}
	~API(){}
    
    
    void tableDrop(string tableName);
    void tableCreate(string tableName, vector<Attribute>* attributeVector, string primaryKeyName,int primaryKeyLocation);

    
    void indexDrop(string indexName);
	void indexCreate(string indexName, string tableName, string attributeName);
  
    void recordShow(string tableName, vector<string>* attributeNameVector = NULL);
	void recordShow(string tableName,  vector<string>* attributeNameVector, vector<Condition>* conditionVector);

	void recordInsert(string tableName,vector<string>* recordContent);

	void recordDelete(string tableName);
	void recordDelete(string tableName, vector<Condition>* conditionVector);

	int recordNumGet(string tableName);
	int recordSizeGet(string tableName);

	int typeSizeGet(int type);
    
    void allIndexAddressInfoGet(vector<IndexInfo> *indexNameVector);
    
    int attributeNameGet(string tableName, vector<string>* attributeNameVector);
	int attributeTypeGet(string tableName, vector<string>* attributeTypeVector);
    int attributeGet(string tableName, vector<Attribute>* attributeVector);

    void indexValueInsert(string indexName, string value, int blockOffset);
    
    void indexInsert(string indexName, char* value, int type, int blockOffset);
    void recordIndexDelete(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset);
    void recordIndexInsert(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset);
    
private:
    int tableExist(string tableName);
    int indexNameListGet(string tableName, vector<string>* indexNameVector);
    string primaryIndexNameGet(string tableName);
    void tableAttributePrint(vector<string>* name);
};

struct int_t{
	int value;
};

struct float_t{
	float value;
};
#endif