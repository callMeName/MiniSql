//
//  BPlusTree.h
//  Minisql
//  Description: The basic b+ tree implement of template.
//
//  Created by xuyuhao on 14/11/8.
//  Copyright (c) 2014年 xuyuhao. All rights reserved.
//

#ifndef __Minisql__BPlusTree__
#define __Minisql__BPlusTree__
#include <vector>
#include <stdio.h>
#include "BufferManager.h"
#include "Minisql.h"
#include <string>
using namespace std;

static BufferManager bm;
//**********************TreeNode***************************//
typedef int offsetNumber; // the value of the tree node

template <typename KeyType>
class TreeNode{
public:
    size_t count;  // the count of keys
    TreeNode* parent;
    vector <KeyType> keys;
    vector <TreeNode*> childs;
    vector <offsetNumber> vals;
    
    TreeNode* nextLeafNode; // point to the next leaf node
    
    bool isLeaf; // the flag whether this node is a leaf node
    
private:
    int degree;
    
public:
    //create a new node. if the newLeaf = false, create a branch node.Otherwise, create a leaf node
    TreeNode(int degree,bool newLeaf=false);
    ~TreeNode();
    
public:
    bool isRoot();
    bool search(KeyType key,size_t &index);//search a key and return by the reference of a parameter
    TreeNode* splite(KeyType &key);
    size_t add(KeyType &key); //add the key in the branch and return the position
    size_t add(KeyType &key,offsetNumber val); // add a key-value in the leaf node and return the position
    bool removeAt(size_t index);

#ifdef _DEBUG
public:
    void debug_print();
#endif
};


//**********************BplusTree***************************//

template <typename KeyType>
class BPlusTree
{
private:
    typedef TreeNode<KeyType>* Node;

    // a struct helping to find the node containing a specific key
    struct searchNodeParse
    {
        Node pNode; // a pointer pointering to the node containing the key
        size_t index; // the position of the key
        bool ifFound; // the flag that whether the key is found.
    };
private:
    string fileName;
    Node root;
    Node leafHead; // the head of the leaf node
    size_t keyCount;
    size_t level;
    size_t nodeCount;
    fileNode* file; // the filenode of this tree
    int keySize; // the size of key
    int degree;
    
public:
    BPlusTree(string m_name,int keySize,int degree);
    ~BPlusTree();

    offsetNumber search(KeyType& key); // search the value of specific key
    bool insertKey(KeyType &key,offsetNumber val);
    bool deleteKey(KeyType &key);
    
    void dropTree(Node node);
    
    void readFromDiskAll();
    void writtenbackToDiskAll();
    void readFromDisk(blockNode* btmp);

private:
    void init_tree();// init the tree
    bool adjustAfterinsert(Node pNode);
    bool adjustAfterDelete(Node pNode);
    void findToLeaf(Node pNode,KeyType key,searchNodeParse &snp);
    
//DEBUG
#ifdef _DEBUG
public:
    void debug_print();

    void debug_print_node(Node pNode);
#endif

};




#include "BPlusTree_imp.h" // the implement of BPlusTree function

#endif /* defined(__Minisql__BPlusTree__) */
