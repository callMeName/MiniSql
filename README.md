#Minisql
A very simple local relational database implementation.

Just for learning the details of a database.

----
##Commands
To be easy, the command of Minisql is a little different from the classic sql command. Only support int \ float \ char(n).

All index is for single-attribute and single-value. Minisql will create index on the primary key automatically. Users can create index on unique attribute. 

* 		create table tableName(
			attributeName type,
			attributeName type,
			attributeName type,
			...
			primary key (attributeName)
		);

*		drop table tableName;
*		create index indexName on tableName(attributeName);
*		drop index indexName;
*		select * from tableName;
		select * from tableName where condition1;
		select * from tableName where condition1 and condition2;
		condition represents that attribute op value, where op is > <> = >= <=.
*		insert into tableName values(value1, value2);
*		delete * from tableName;
		delete * from tableName where condition;
*		quit;
*		execfile file;  # exec the sql queries in the file.

----
##software architecture
There are 6 modules in this system.

* Interpreter. This module is responsible for interaction with users, processing and checking the input and then constructing the inner data structure for next processing.

* API. This module produces interface for Interpreter module. Then it calls the interfaces of Catalog Manager, Record Manager, Index Manager and return the result to the Interpreter module.

* Catalog Manager. This module manages all information of the database such as the information of tables, index and attributes.

* Record Manager. This module manages the data files. The data file consists of multiple data blocks whose size equals to the buffer block.

* Index Manager. This module manages the B+ tree index and produces interfaces for other modules.

* Buffer Manager. This module manages the buffer areas. To improve the efficiency of I/O, the buffer areas transfer blocks with the file system. The block size if 4KB.
	
	






