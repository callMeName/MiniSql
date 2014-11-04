#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H
//#include "stdafx.h"
#include "Condition.h"
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

private:
	int fullFillCond(string type,char * value,string scond,string operater);
	
};
#endif