//
//  Condition.h
//  minisql
//
//  Created by 邓永辉 on 14/11/4.
//  Copyright (c) 2014年 邓永辉. All rights reserved.
//

#ifndef minisql_Condition_h
#define minisql_Condition_h

class Condition
{
    string attributeName;   //属性名称
    string value;           //要比较的值
    int operate;            //比较类型
};

#endif
