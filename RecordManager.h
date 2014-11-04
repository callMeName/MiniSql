#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H
//#include "stdafx.h"
#include <string>
#include <vector>
using namespace std;
class RecordManager{
public:
	RecordManager(){}
	int selectRecord(string tableName);
	int selectRecord(string tableName,string colName1,string cond1,string operater1);
	int selectRecord(string tableName,string colName1,string cond1,string operater1,
		string colName2,string cond2,string operater2,int logic);

	int insertRecord(string tableName,char * s);

	int deleteValue(string tableName);
	int deleteValue(string tableName, string colName1,string cond1,string operater1);
	int deleteValue(string tableName,string colName1,string cond1,string operater1,
		string colName2,string cond2,string operater2,int logic);

	int fullFillCond(string type,char * value,string scond,string operater);
	
};
#endif