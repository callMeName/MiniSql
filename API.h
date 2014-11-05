#ifndef API_H
#define API_H
#include "RecordManager.h"
#include "IndexManager.h"
#include "CatalogManager.h"

#include "Attribute.h"
#include "Condition.h"
#include <string>
#include <vector>
using namespace std;
class API{
public:
	RecordManager rm;
	API(){}
	~API(){}
    
    
	void tableDrop(string tableName);
    
    void indexDrop(string indexName);
    
  
	void indexCreate(string indexName, string tableName, string attributeName);
  
    //创建一个新的table
    //输入talbe的名称, 属性的一个列表 和talbe中的主键的名字
	void tableCreate(string tableName, vector<Attribute> attributeVector, string primaryKeyName);

    //输出 一个表的所有的记录 和 总记录数
    //输入table的名称
	void recordShow(string tableName);
    
    //输出 一个表的条件的记录 和 记录数
    //输入table的名称, 和一个条件的列表
	void recordShow(string tableName, vector<Condition> conditionVector);

    //插入record
    //输入 table的名称,和插入的这条record
	void recordInsert(string tableName,vector<string> recordContent);

    //删除 一个表的条件的所有记录
    //输出记录数
    //输入table的名
	void recordDelete(string tableName);
    
    //删除 一个表的条件的符合条件记录
    //输出记录数
    //输入table的名称, 和一个条件的列表
	void recordDelete(string tableName, vector<Condition> conditionVector);

    //输出 table的记录数
    //输入 table的名称
	int recordNumGet(string tableName);
    
    //输出 table每条记录的长度
    //输入 table的名称
	int recordSizeGet(string tableName);
    
    //输出 type每条记录的长度
    //输入 type的名称
	int typeSizeGet(int type);
    
    //输出 table的属性名称列表
    //输入 table的名称
	vector<string> attributeNameGet(string tableName);
    
    //输出 table的属性类型列表
    //输入 table的名称
	vector<string> attributeTypeGet(string tableName);
    
    //输出 table的属性列表
    //输入 table的名称
    vector<Attribute> attributeGet(string tableName);

    //插入 二叉树的一个节点（待修改）
    //输入 索引名称,value
    void indexValueInsert(string indexName, string value);
    
    //输出 二叉树的一个节点（待修改）
    //输入 索引名称,value
    int indexBlockGet(string indexName, string value);
    
};

struct int_t{
	int value;
};

struct float_t{
	float value;
};
#endif