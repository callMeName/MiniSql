//
//  Attribute.h
//  minisql
//
//  Created by 邓永辉 on 14/11/4.
//  Copyright (c) 2014年 邓永辉. All rights reserved.
//

#ifndef minisql_Attribute_h
#define minisql_Attribute_h

#include <string>
#include <iostream>
using namespace std;

class Attribute
{
public:
    string name;
    int type;                  //the type of the attribute,-1 represents float, 0 represents int, other positive integer represents char and the value is the number of char)
    bool ifUnique;
    string index;         // default value is "", representing no index
    Attribute(string n, int t, bool i);
    
public:
    int static const TYPE_FLOAT = -1;
    int static const TYPE_INT = 0;
    string indexNameGet(){return index;}
    
    void print()
    {
        cout <<  "name: " << name << ";type: " << type << ";ifUnique: " << ifUnique << ";index: " << index << endl;
    }
};

#endif
