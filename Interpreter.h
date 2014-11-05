/************************些文件包括Interpreter类里面的函数实现****************************/
/************************作者:郭周东  时间：2009年11月************************************/

#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <string>
#include <vector>
using namespace std;
class Interpreter{
public:
	int firstKey;		//第一关键字。
	int secondKey;		//第二关键字。
	string fileName;	//操作对象。


	int condNum;		//where后条件的个数。
	int logic;		//两个条件之间的逻辑连接符。
	string operater1;	//条件一中的关系运算符。
	string operater2;	//条件二中的关系运算符。
	string col1;		//where关键字后第一个条件中涉及的列名。
	string col2;		//where关键字后第二个条件中涉及的列名。 
	string  condition1;	//where关键字后第一个条件中逻辑运算符后的常量。
	string  condition2;	//where关键字后第二个条件中逻辑运算符后的常量。

	vector<string> insertValue;  	//insert语句中values后括号中的各个值。（存在一个vector变量中）

	
	vector<string> type;		//create语句建表时的列属性，按顺序存在一个vector变量中
	vector<string> col;		//create语句建表时的列名。
	vector<int> uniq;		//create语句建表时的列名。
	string primKey;			//create语句建表时定义的该表的primary key

	string tableName;
	string colName;

	int interpreter(string s);	

	string getWord(string s, int *st);
	
	Interpreter(){}
	~Interpreter(){}
};

#endif