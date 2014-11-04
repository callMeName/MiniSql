//
//  Table.h
//  minisql
//
//  Created by 邓永辉 on 14/11/4.
//  Copyright (c) 2014年 邓永辉. All rights reserved.
//

#ifndef minisql_Table_h
#define minisql_Table_h
class Table
{
    string name;            //table的名称
    string primaryKeyName;  //主键名称
    int recordNum;          //总记录数
    int attributeNum;       //属性数
    int recordSize;         //单条记录大小
};

#endif
