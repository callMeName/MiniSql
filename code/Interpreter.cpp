#include "Interpreter.h"
#include "Condition.h"
#include "Attribute.h"
#include <string>
#include <string.h>
#include <iostream>
using namespace std;

class SyntaxException{};

int Interpreter::interpreter(string s)
{

	int tmp=0;
	string word;

	word = getWord(s, &tmp);
	if (strcmp(word.c_str(), "create") == 0)
	{
		word = getWord(s,&tmp);

		if (strcmp(word.c_str(), "table") == 0)
		{
			string primaryKey = "";
			string tableName = "";
			word = getWord(s,&tmp);
			if (!word.empty())			//create table tablename
				tableName = word;
			else
			{
				cout<<"Syntax Error for no table name"<<endl;
				return 0;
			}

			word = getWord(s,&tmp);
			if (word.empty() || strcmp(word.c_str(),"(") != 0)
			{
				cout<<"Error in syntax!"<<endl;
				return 0;
			}
			else				// deal with attribute list
			{
				word = getWord(s,&tmp);
				std::vector<Attribute> attributeVector;
				while (!word.empty() && strcmp(word.c_str(),"primary") != 0 && strcmp(word.c_str(),")") != 0)
				{
					string attributeName = word;
					int type = 0;
					bool ifUnique = false;
					// deal with the data type
					word = getWord(s,&tmp);
					if (strcmp(word.c_str(), "int") == 0)
						type = 0;
					else if(strcmp(word.c_str(), "float") == 0)
						type = -1;
					else if(strcmp(word.c_str(), "char") == 0)
					{
						word = getWord(s,&tmp);
						if(strcmp(word.c_str(),"("))
						{
							cout<<"Syntax Error: unknown data type"<<endl;
							return 0;
						}
						word = getWord(s,&tmp);
						istringstream convert(word);
						if ( !(convert >> type) )
						{
							cout<<"Syntax error : illegal number in char()"<<endl;
							return 0;
						}
						word = getWord(s,&tmp);
						if(strcmp(word.c_str(),")"))
						{
							cout<<"Syntax Error: unknown data type"<<endl;
							return 0;
						}
					}
					else
					{
						cout<<"Syntax Error: unknown or missing data type!"<<endl;
						return 0;
					}
					word = getWord(s,&tmp);
					if(strcmp(word.c_str(), "unique") == 0)
					{
						ifUnique = true;
                        word = getWord(s,&tmp);
					}
					Attribute attr(attributeName,type,ifUnique);
					attributeVector.push_back(attr);
					if(strcmp(word.c_str(), ",") != 0)
					{
						if(strcmp(word.c_str(), ")") != 0){
							cout<<"Syntax Error for ,!"<<endl;
							return 0;
						}
						else
							break;
					}

					word = getWord(s,&tmp);
				}
				int primaryKeyLocation = 0;
				if (strcmp(word.c_str(),"primary") == 0)	// deal with primary key
				{
					word = getWord(s,&tmp);
					if (strcmp(word.c_str(),"key") != 0)
					{
						cout<<"Error in syntax!"<<endl;
						return 0;
					}
					else
					{
						word = getWord(s,&tmp);
						if (strcmp(word.c_str(),"(") == 0)
						{
							word = getWord(s,&tmp);
							primaryKey = word;
							int i = 0;
							for( i= 0;i<attributeVector.size();i++)
							{
								if(primaryKey==attributeVector[i].name)
								{
									attributeVector[i].ifUnique = true;
									break;
								}

							}
							if(i==attributeVector.size())
							{
								cout<<"Syntax Error: primaryKey does not exist in attributes "<<endl;
								return 0;
							}
							primaryKeyLocation = i;
							word = getWord(s,&tmp);
							if (strcmp(word.c_str(),")") != 0)
							{
								cout<<"Error in syntax!"<<endl;
								return 0;
							}
						}
						else
						{
							cout<<"Error in syntax!"<<endl;
							return 0;
						}
						word = getWord(s,&tmp);
						if (strcmp(word.c_str(),")") != 0)
						{
							cout<<"Error in syntax!"<<endl;
							return 0;
						}
					}
				}
				else if (word.empty())
				{
					cout<<"Syntax Error: ')' absent!"<<endl;
					return 0;
				}

				ap->tableCreate(tableName,&attributeVector,primaryKey,primaryKeyLocation);
				return 1;
			}
		}
		else if (strcmp(word.c_str(), "index") == 0)
		{
			string indexName = "";
			string tableName = "";
			string attributeName = "";
			word = getWord(s,&tmp);
			if (!word.empty())			//create index indexname
				indexName = word;
			else
			{
				cout<<"Error in syntax!"<<endl;
				return 0;
			}

			word = getWord(s,&tmp);
			try {
				if (strcmp(word.c_str(),"on") != 0)
					throw SyntaxException();
				word = getWord(s,&tmp);
				if (word.empty())
					throw SyntaxException();
				tableName = word;
				word = getWord(s,&tmp);
				if (strcmp(word.c_str(),"(") != 0)
					throw SyntaxException();
				word = getWord(s,&tmp);
				if (word.empty())
					throw SyntaxException();
				attributeName = word;
				word = getWord(s,&tmp);
				if (strcmp(word.c_str(),")") != 0)
					throw SyntaxException();
				ap->indexCreate(indexName,tableName,attributeName);
				return 1;
			} catch(SyntaxException&) {
				cout<<"Syntax Error!"<<endl;
				return 0;
			}
		}
		else
		{
			cout<<"Syntax Error for "<<word<<endl;
			return 0;
		}
		return 0;
	}



	else if(strcmp(word.c_str(), "select")==0)
	{
		vector<string> attrSelected;
		string tableName = "";
		word = getWord(s,&tmp);
		if (strcmp(word.c_str(), "*") != 0)	// only accept select *
		{
			while(strcmp(word.c_str(), "from") != 0)
			{
				attrSelected.push_back(word);
				word = getWord(s,&tmp);
			}
		}
		else
		{
			word = getWord(s,&tmp);
		}
		if (strcmp(word.c_str(), "from") != 0)
		{
			cout<<"Error in syntax!"<<endl;
			return 0;
		}

		word = getWord(s,&tmp);
		if (!word.empty())
			tableName = word;
		else
		{
			cout<<"Error in syntax!"<<endl;
			return 0;
		}

		// condition extricate
		word = getWord(s,&tmp);
		if (word.empty())	// without condition
		{
			if(attrSelected.size()==0){
				ap->recordShow(tableName);
			}
			else
				ap->recordShow(tableName,&attrSelected);
			return 1;
		}
		else if (strcmp(word.c_str(),"where") == 0)
		{
			string attributeName = "";
			string value = "";
			int operate = Condition::OPERATOR_EQUAL;
			std::vector<Condition> conditionVector;
			word = getWord(s,&tmp);		//col1
			while(1){
				try {
					if(word.empty())
						throw SyntaxException();
					attributeName = word ;
					word = getWord(s,&tmp);
					if (strcmp(word.c_str(),"<=") == 0)
						operate = Condition::OPERATOR_LESS_EQUAL;
					else if (strcmp(word.c_str(),">=") == 0)
						operate = Condition::OPERATOR_MORE_EQUAL;
					else if (strcmp(word.c_str(),"<") == 0)
						operate = Condition::OPERATOR_LESS;
					else if (strcmp(word.c_str(),">") == 0)
						operate = Condition::OPERATOR_MORE;
					else if (strcmp(word.c_str(),"=") == 0)
						operate = Condition::OPERATOR_EQUAL;
					else if (strcmp(word.c_str(),"<>") == 0)
						operate = Condition::OPERATOR_NOT_EQUAL;
					else
						throw SyntaxException();
					word = getWord(s,&tmp);
					if(word.empty()) // no condition
						throw SyntaxException();
					value = word;
					Condition c(attributeName,value,operate);
					conditionVector.push_back(c);
					word = getWord(s,&tmp);
					if(word.empty()) // no condition
						break;
					if (strcmp(word.c_str(),"and") != 0)
						throw SyntaxException();
					word = getWord(s,&tmp);
				} 	catch (SyntaxException&) {
					cout<<"Syntax Error!"<<endl;
					return 0;
				}
			}
			if(attrSelected.size()==0)
				ap->recordShow(tableName,NULL,&conditionVector);
			else
				ap->recordShow(tableName, &attrSelected,&conditionVector);
			
			return 1;
		}
	}



	else if (strcmp(word.c_str(), "drop")==0)
	{
		word = getWord(s,&tmp);

		if (strcmp(word.c_str(), "table") == 0)
		{
			word = getWord(s,&tmp);
			if (!word.empty())
			{
				ap->tableDrop(word);
				return 1;
			}
			else
			{
				cout<<"Error in syntax!"<<endl;
				return 1;
			}
		}
		else if (strcmp(word.c_str(), "index") == 0)
		{
			word = getWord(s,&tmp);
			if (!word.empty())
			{
				ap->indexDrop(word);
				return 1;
			}
			else
			{
				cout<<"Error in syntax!"<<endl;
				return 1;
			}
		}
		else
		{
			cout<<"Error in syntax!"<<endl;
			return 0;
		}
	}


	else if (strcmp(word.c_str(), "delete")==0)
	{
		string tableName = "";
		word = getWord(s,&tmp);
		if (strcmp(word.c_str(), "from") != 0)
		{
			cout<<"Error in syntax!"<<endl;
			return 0;
		}

		word = getWord(s,&tmp);
		if (!word.empty())
			tableName = word;
		else
		{
			cout<<"Error in syntax!"<<endl;
			return 0;
		}

		// condition extricate
		word = getWord(s,&tmp);
		if (word.empty())	// without condition
		{
			ap->recordDelete(tableName);
			return 1;
		}
		else if (strcmp(word.c_str(),"where") == 0)
		{
			string attributeName = "";
			string value = "";
			int operate = Condition::OPERATOR_EQUAL;
			std::vector<Condition> conditionVector;
			word = getWord(s,&tmp);		//col1
			while(1){
				try {
					if(word.empty())
						throw SyntaxException();
					attributeName = word ;
					word = getWord(s,&tmp);
					if (strcmp(word.c_str(),"<=") == 0)
						operate = Condition::OPERATOR_LESS_EQUAL;
					else if (strcmp(word.c_str(),">=") == 0)
						operate = Condition::OPERATOR_MORE_EQUAL;
					else if (strcmp(word.c_str(),"<") == 0)
						operate = Condition::OPERATOR_LESS;
					else if (strcmp(word.c_str(),">") == 0)
						operate = Condition::OPERATOR_MORE;
					else if (strcmp(word.c_str(),"=") == 0)
						operate = Condition::OPERATOR_EQUAL;
					else if (strcmp(word.c_str(),"<>") == 0)
						operate = Condition::OPERATOR_NOT_EQUAL;
					else
						throw SyntaxException();
					word = getWord(s,&tmp);
					if(word.empty()) // no condition
						throw SyntaxException();
					value = word;
					word = getWord(s,&tmp);
					Condition c(attributeName,value,operate);
					conditionVector.push_back(c);
					if(word.empty()) // no condition
						break;
					if (strcmp(word.c_str(),"and") != 0)
						throw SyntaxException();
					word = getWord(s,&tmp);
				} 	catch (SyntaxException&) {
					cout<<"Syntax Error!"<<endl;
					return 0;
				}
			}
			ap->recordDelete(tableName, &conditionVector);
			return 1;
		}
	}



	else if (strcmp(word.c_str(), "insert") == 0)
	{
		string tableName = "";
		std::vector<string> valueVector;
		word = getWord(s,&tmp);
		try {
			if (strcmp(word.c_str(),"into") != 0)
				throw SyntaxException();
			word = getWord(s,&tmp);
			if (word.empty())
				throw SyntaxException();
			tableName = word;
			word = getWord(s,&tmp);
			if (strcmp(word.c_str(),"values") != 0)
				throw SyntaxException();
			word = getWord(s,&tmp);
			if (strcmp(word.c_str(),"(") != 0)
				throw SyntaxException();
			word = getWord(s,&tmp);
			while (!word.empty() && strcmp(word.c_str(),")") != 0)
			{
				valueVector.push_back(word);
				word = getWord(s,&tmp);
				if (strcmp(word.c_str(),",") == 0)  // bug here
					word = getWord(s,&tmp);
			}
			if (strcmp(word.c_str(),")") != 0)
				throw SyntaxException();
		} catch (SyntaxException&){
			cout<<"Syntax Error!"<<endl;
			return 0;
		}
		ap->recordInsert(tableName,&valueVector);
		return 1;
	}


	else if (strcmp(word.c_str(), "quit") == 0)
	{ return 587;}

	else if (strcmp(word.c_str(), "commit") == 0)
	{ return 1;}
	else if (strcmp(word.c_str(), "execfile") == 0)
	{
		fileName = getWord(s,&tmp);
		cout<<"try to open "<<fileName<<"..."<<endl;
		return 2;
	}
	else
	{
		if(word != "")
			cout<<"Error, command "<<word<<" not found"<<endl;
		return 0;
	}
	return 0;


}



string Interpreter::getWord(string s, int *tmp)
{
	string word;
	int idx1,idx2;

	while ((s[*tmp] == ' ' || s[*tmp] == 10  || s[*tmp] == '\t') && s[*tmp] != 0)
	{
		(*tmp)++;
	}
	idx1 = *tmp;

	if (s[*tmp] == '(' || s[*tmp] == ',' || s[*tmp] == ')')
	{
		(*tmp)++;
		idx2 = *tmp;
		word = s.substr(idx1,idx2-idx1);
		return word;
	}
	else if (s[*tmp] == 39)
	{
		(*tmp)++;
		while (s[*tmp] != 39 && s[*tmp] !=0)
			(*tmp)++;
		if (s[*tmp] == 39)
		{
			idx1++;
			idx2 = *tmp;
			(*tmp)++;
			word = s.substr(idx1,idx2-idx1);
			return word;
		}
		else
		{
			word = "";
			return word;
		}
	}
	else
	{
		while (s[*tmp] != ' ' &&s[*tmp] != '(' && s[*tmp] != 10 && s[*tmp] != 0 && s[*tmp] != ')' && s[*tmp] != ',')
			(*tmp)++;
		idx2 = *tmp;
		if (idx1 != idx2)
			word = s.substr(idx1,idx2-idx1);
		else
			word = "";

		return word;
	}
}
