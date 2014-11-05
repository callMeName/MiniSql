/****************************此文件包括MiniSQl中CatalogManager类的定义 **********************/
/****************************作者：陈矫彦   时间：2009年10月*******************************/

#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H
// #include "stdafx.h"
#include <string>
#include <vector>
using namespace std;
class CatalogManager{
public:
	struct Index
	{
		string indexName;              //存放索引名
		string searchTable;            //存放索引对应的表名
		string searchKey;              //存放索引对应的关键属性
	};
	struct Table
	{
		string tableName;               //表名
		int recordNum;                  //存放记录数目
		int collNum;                    //存入字段数目
		int recordLen;                  //存入记录长度
		string primKey;                 //存放主键
		vector<string> collName;        //存放对应的字段名
		vector<string> collType;        //存放对应的字段类型
		vector<int> collUniq;           //存放对应的字段是否惟一
		vector<string> collIndex;       //存放对应字段上的索引信息
	};
	 
	int tableNum;                       //表数目
	int indexNum;                       //索引数目
	vector<Table> tv;                   //表列表
	vector<Index> iv;                   //索引列表


	CatalogManager();
	~CatalogManager(){};

   
	int addTable(string tableName,vector<string> col,vector<string> type,vector<int> uniq);
	int addTable(string tableName,vector<string> col,vector<string> type,vector<int> uniq,string primKey);
	
	//用来创建索引时调用，向字典中加入新创建索引的信息
	int addIndex(string indexName,string tableName,string colName);
	

	//在字典文件中的记录数加recordNum，用于insert values
	int insertRecord(string tableName,int recordNum);
	
	//删除一个表的字典信息
	int dropTable(string tableName);

	//删除一个索引的字典信息
	int dropIndex(string index);

	//在字典文件中删除记录,第一个重载函数删除全部记录将recordNum设为0，
	//第二个重载函数删除deleteNum个记录，将deleteNum减掉
	int deleteValue(string tableName);
	int deleteValue(string tableName,int deleteNum);

	//查找文件信息，是否为索引或者表，如果后者，则包括表长度和记录数
	 void getFileInfo(string fileName,int * n);
	
	//查找文件，返回文件类型或者是否有这个文件
	int findFile(string fileName);

	//判断在对应表对应字段上是否有索引
	int existIndex(string tableName,string col);

	//将内存中的字典信息存入文件
	int writeBack();

	//三个重载函数用于查找记录的长度
	int calcuteLenth(string tableName);
	int calcuteLenth(vector<string> v);
	int calcuteLenth2(string type);
	
	//返回表的记录数目
	int getRecordNum(string tableName);

	//返回字段名列表
	vector<string> getCollName(string tableName);

	//返回字段类型列表
	vector<string> getCollType(string tableName);

	//返回字段是否惟一列表
	vector<int> getCollUniq(string tableName);

	//返回字段索引列表
	vector<string> getCollIndex(string tableName);
};
#endif