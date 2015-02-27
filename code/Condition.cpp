//
//  Condition.cpp
//  minisql
//
//  Created by 邓永辉 on 14/11/10.
//  Copyright (c) 2014年 邓永辉. All rights reserved.
//

#include "Condition.h"
bool Condition::ifRight(int content)
{
    stringstream ss;
    ss << value;
    int myContent;
    ss >> myContent;
    
    switch (operate)
    {
        case Condition::OPERATOR_EQUAL:
            return content == myContent;
            break;
        case Condition::OPERATOR_NOT_EQUAL:
            return content != myContent;
            break;
        case Condition::OPERATOR_LESS:
            return content < myContent;
            break;
        case Condition::OPERATOR_MORE:
            return content > myContent;
            break;
        case Condition::OPERATOR_LESS_EQUAL:
            return content <= myContent;
            break;
        case Condition::OPERATOR_MORE_EQUAL:
            return content >= myContent;
            break;
        default:
            return true;
            break;
    }
}

bool Condition::ifRight(float content)
{
    stringstream ss;
    ss << value;
    float myContent;
    ss >> myContent;
    
    switch (operate)
    {
        case Condition::OPERATOR_EQUAL:
            return content == myContent;
            break;
        case Condition::OPERATOR_NOT_EQUAL:
            return content != myContent;
            break;
        case Condition::OPERATOR_LESS:
            return content < myContent;
            break;
        case Condition::OPERATOR_MORE:
            return content > myContent;
            break;
        case Condition::OPERATOR_LESS_EQUAL:
            return content <= myContent;
            break;
        case Condition::OPERATOR_MORE_EQUAL:
            return content >= myContent;
            break;
        default:
            return true;
            break;
    }
}

bool Condition::ifRight(string content)
{
    string myContent = value;
    switch (operate)
    {
        case Condition::OPERATOR_EQUAL:
            return content == myContent;
            break;
        case Condition::OPERATOR_NOT_EQUAL:
            return content != myContent;
            break;
        case Condition::OPERATOR_LESS:
            return content < myContent;
            break;
        case Condition::OPERATOR_MORE:
            return content > myContent;
            break;
        case Condition::OPERATOR_LESS_EQUAL:
            return content <= myContent;
            break;
        case Condition::OPERATOR_MORE_EQUAL:
            return content >= myContent;
            break;
        default:
            return true;
            break;
    }
}

Condition::Condition(string a,string v,int o) {
    attributeName = a;
    value = v;
    operate = o;
}

