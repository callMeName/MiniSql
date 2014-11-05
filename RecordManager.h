#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H
//#include "stdafx.h"
#include "Condition.h"
#include "Attribute.h"
#include <string>
#include <vector>
using namespace std;
class RecordManager{
public:
	RecordManager(){}
    
    //返回 总记录数
    //输出 table的所有记录
    //输入 table的名称
	int recordShow(string tableName);
    
    //返回 记录数
    //输出 table的符合条件的记录
    //输入 table的名称，条件列表
	int recordShow(string tableName, vector<Condition> conditionVector);

    //插入 record
    //输入 table的名称,和插入的这条record
	void recordInsert(string tableName, char *record);

    //返回 记录数
    //删除 一个表的条件的所有记录
    //输入table的名
	int recordDelete(string tableName);
    
    //返回 记录数
    //删除 一个表的条件的符合条件记录
    //输入 table的名称, 和一个条件的列表
	int recordDelete(string tableName, vector<Condition> conditionVector);
    
    //删除一个table,输入talbe的名称
    int tableDrop(string tableName);
    
    //删除一个table
    //输入talbe的名称
    int indexDrop(string indexName);
    
    //创建一个新的index
    //输入index的名称, talbe的名称 和talbe中属性的名称
    void indexCreate(string indexName, string tableName, string attributeName);
    
    //创建一个新的table
    //输入talbe的名称, 属性的一个列表 和talbe中的主键的名字
    int tableCreate(string tableName, vector<Attribute> attributeVector, string primaryKeyName);

private:
	int fullFillCond(string type,char * value,string scond,string operater);
	
};
#endif