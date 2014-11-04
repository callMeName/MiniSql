#ifndef API_H
#define API_H
#include "RecordManager.h"
#include <string>
#include <vector>
using namespace std;
class IndexManager;
class RecordManager;
class API{
public:
	RecordManager rm;
	API(){}
	~API(){}
	void dropTable(string tableName);
	void dropIndex(string indexName);

	void createIndex(string fileName,string tableName,string colName);
	void createTable(string tableName,vector<string> col,vector<string> type,vector<int> uniq,string primKey);

	void printRecord(string tableName);
	void printRecord(string tableName,string colName1,string cond1,string operater1);
	void printRecord(string tableName,string colName1,string cond1,string operater1,
		string colName2,string cond2,string operater2,int logic);

	void insertRecord(string tableName,vector<string> v);


	int insertIndexItem(string fileName,string colName,string value,int block,int index);
	int getIndexItem(string fileName,string colName,string value,int * block,int * index);

	void deleteValue(string tableName);
	void deleteValue(string tableName,string colName1,string cond1,string operater1);
	void deleteValue(string tableName,string colName1,string cond1,string operater1,
		string colName2,string cond2,string operater2,int logic);	

	int getRecordNum(string tableName);
	int calcuteLenth(string tableName);
	int calcuteLenth2(string type);
	vector<string> getCollName(string tableName);
	vector<string> getCollType(string tableName);

};

struct int_t{
	int value;
};
struct float_t{
	float value;
};
#endif