//
//  main.cpp
//  minisql
//
//  Created by 邓永辉 on 14/11/3.
//  Copyright (c) 2014年 邓永辉. All rights reserved.
//

#include <iostream>
#include "API.h"
using namespace std;

int main()
{
    cout << "Hello, World!\n";
    return 0;
    API api;
    vector<Condition> conditionVector;
    api.recordShow("aa", conditionVector);
    string a = "";
    a = "sdf";
    cout << a;
    
}
