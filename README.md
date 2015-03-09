#Minisql
A very simple local relational database implementation. A small sql server.

Just for learning the details of a database.

----
##Commands
To be easy, the command of Minisql is a little different from the classic sql command. Only support int \ float \ char(n).

All index is for single-attribute and single-value. Minisql will create index on the primary key automatically. Users can create index on unique attribute. 


		create table tableName(
			attributeName type,
			attributeName type,
			attributeName type,
			...
			primary key (attributeName)
		);

		drop table tableName;

		create index indexName on tableName(attributeName);
		
		drop index indexName;
		
		select * from tableName;
		select * from tableName where condition1;
		select * from tableName where condition1 and condition2;
		condition represents that attribute op value, where op is > <> = >= <=.
		
		insert into tableName values(value1, value2);
		
		delete * from tableName;
		delete * from tableName where condition;
		
		quit;
		
		execfile file;  # exec the sql queries in the file.

----
The details of implementation is [here](http://www.xuyuhao.com/?p=96).
	
	






