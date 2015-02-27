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
#include <string.h>
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




/* the implement of BPlusTree function */
//******** The definition of the functions of the class TreeNode **********

/**
 * Constructor: create the tree node.
 *
 * @param int the degreee
 * @param bool the flag that whether the node is a tree node or not
 *
 */
template <class KeyType>
TreeNode<KeyType>::TreeNode(int m_degree,bool newLeaf):count(0),parent(NULL),nextLeafNode(NULL),isLeaf(newLeaf),degree(m_degree)
{
    for(size_t i = 0;i < degree+1;i ++)
    {
        childs.push_back(NULL);
        keys.push_back(KeyType());
        vals.push_back(offsetNumber());
    }
    childs.push_back(NULL);
}

/**
 * @Deconstructor
 *
 */
template <class KeyType>
TreeNode<KeyType>::~TreeNode()
{
    
}

/**
 * Test if this node is the root or not.
 *
 * @return bool the flag that whether this node is the root or not
 *
 */
template <class KeyType>
bool TreeNode<KeyType>::isRoot()
{
    if(parent != NULL) return false;
    else return true;
}

/**
 * Search the key in the node
 *
 * @param KeyType
 * @param size_t return the position of the node by reference
 *
 * @return bool the flag that whether the key exists in the node
 *
 */
template <class KeyType>
bool TreeNode<KeyType>::search(KeyType key,size_t &index)
{
    if(count == 0 ) // no values in the node
    {
        index = 0;
        return false;
    }
    else
    {
        // test if key are beyond the area of the keys array
        if(keys[count-1] < key)
        {
            index = count;
            return false;
        }
        else if(keys[0] > key)
        {
            index = 0;
            return false;
        } // end of test
        else if(count <= 20) // sequential search
        {
            for(size_t i = 0;i < count;i ++)
            {
                if(keys[i] == key)
                {
                    index = i;
                    return true;
                }
                else if(keys[i] < key)
                {
                    continue;
                }
                else if(keys[i] > key)
                {
                    index = i;
                    return false;
                }
            }
        } // end sequential search
        else if(count > 20) // too many keys, binary search. 2* log(n,2) < (1+n)/2
        {
            size_t left = 0, right = count - 1, pos = 0;
            while(right>left+1)
            {
                pos = (right + left) / 2;
                if(keys[pos] == key)
                {
                    index = pos;
                    return true;
                }
                else if(keys[pos] < key)
                {
                    left = pos;
                }
                else if(keys[pos] > key)
                {
                    right = pos;
                }
            } // end while
            
            // right == left + 1
            if(keys[left] >= key)
            {
                index = left;
                return (keys[left] == key);
            }
            else if(keys[right] >= key)
            {
                index = right;
                return (keys[right] == key);
            }
            else if(keys[right] < key)
            {
                index = right ++;
                return false;
            }
        } // end binary search
    }
    return false;
}

/**
 * Splite this node to two and the new node will be the next node.
 *
 * @param KeyType & the key reference returns the key that will go to the upper level
 *
 * @return TreeNode *
 *
 */
template <class KeyType>
TreeNode<KeyType>* TreeNode<KeyType>::splite(KeyType &key)
{
    size_t minmumNode = (degree - 1) / 2;
    TreeNode* newNode = new TreeNode(degree,this->isLeaf);
    if(newNode == NULL)
    {
        cout << "Problems in allocate momeory of TreeNode in splite node of " << key << endl;
        exit(2);
    }
    
    if(isLeaf) // this is a leaf node
    {
        key = keys[minmumNode + 1];
        for(size_t i = minmumNode + 1;i < degree;i ++) // copy the right hand of the keys to the new node
        {
            newNode->keys[i-minmumNode-1] = keys[i];
            keys[i] = KeyType();
            newNode->vals[i-minmumNode-1] = vals[i];
            vals[i] = offsetNumber();
        }
        newNode->nextLeafNode = this->nextLeafNode;
        this->nextLeafNode = newNode;
        
        newNode->parent = this->parent;
        newNode->count = minmumNode;
        this->count = minmumNode + 1;
    } // end leaf
    else if(!isLeaf)
    {
        key = keys[minmumNode];
        for(size_t i = minmumNode + 1;i < degree+1;i ++)
        {
            newNode->childs[i-minmumNode-1] = this->childs[i];
            newNode->childs[i-minmumNode-1]->parent = newNode;
            this->childs[i] = NULL;
        }
        for(size_t i = minmumNode + 1;i < degree;i ++)
        {
            newNode->keys[i-minmumNode-1] = this->keys[i];
            this->keys[i] = KeyType();
        }
        this->keys[minmumNode] = KeyType();
        newNode->parent = this->parent;
        newNode->count = minmumNode;
        this->count = minmumNode;
    }
    return newNode;
}

/**
 * Add the key in the branch node and return the position added.
 *
 * @param KeyType &
 *
 * @return size_t the position to insert
 *
 */
template <class KeyType>
size_t TreeNode<KeyType>::add(KeyType &key)
{
    if(count == 0)
    {
        keys[0] = key;
        count ++;
        return 0;
    }
    else //count > 0
    {
        size_t index = 0; // record the index of the tree
        bool exist = search(key, index);
        if(exist)
        {
            cout << "Error:In add(Keytype &key),key has already in the tree!" << endl;
            exit(3);
        }
        else // add the key into the node
        {
            for(size_t i = count;i > index;i --)
                keys[i] = keys[i-1];
            keys[index] = key;
            
            for(size_t i = count + 1;i > index+1;i --)
                childs[i] = childs[i-1];
            childs[index+1] = NULL; // this child will link to another node
            count ++;
            
            return index;
        }
    }
}

/**
 * Add the key in the leaf node and return the position added.
 *
 * @param Keytype &
 * @param offsetNumber the value
 *
 * @return size_t the position to insert
 *
 */
template <class KeyType>
size_t TreeNode<KeyType>::add(KeyType &key,offsetNumber val)
{
    if(!isLeaf)
    {
        cout << "Error:add(KeyType &key,offsetNumber val) is a function for leaf nodes" << endl;
        return -1;
    }
    if(count == 0)
    {
        keys[0] = key;
        vals[0] = val;
        count ++;
        return 0;
    }
    else //count > 0
    {
        size_t index = 0; // record the index of the tree
        bool exist = search(key, index);
        if(exist)
        {
            cout << "Error:In add(Keytype &key, offsetNumber val),key has already in the tree!" << endl;
            exit(3);
        }
        else // add the key into the node
        {
            for(size_t i = count;i > index;i --)
            {
                keys[i] = keys[i-1];
                vals[i] = vals[i-1];
            }
            keys[index] = key;
            vals[index] = val;
            count ++;
            return index;
        }
    }
}

/**
 * Delete the key-value or key-child by the position.
 *
 * @param size_t the position to delete
 *
 * @return bool the falg that delation successes or not
 *
 */
template <class KeyType>
bool TreeNode<KeyType>::removeAt(size_t index)
{
    if(index > count)
    {
        cout << "Error:In removeAt(size_t index), index is more than count!" << endl;
        return false;
    }
    else
    {
        if(isLeaf)
        {
            for(size_t i = index;i < count-1;i ++)
            {
                keys[i] = keys[i+1];
                vals[i] = vals[i+1];
            }
            keys[count-1] = KeyType();
            vals[count-1] = offsetNumber();
        }
        else // is nonleaf
        {
            for(size_t i = index;i < count-1;i ++)
                keys[i] = keys[i+1];
            
            for(size_t i = index+1;i < count;i ++)
                childs[i] = childs[i+1];
            
            keys[count-1] = KeyType();
            childs[count] = NULL;
        }
        
        count --;
        return true;
    }
}

#ifdef _DEBUG
/**
 * For debug, print the whole tree
 *
 * @param
 *
 * @return void
 *
 */
template <class KeyType>
void TreeNode<KeyType>::debug_print()
{
    cout << "############DEBUG for node###############" << endl;
    
    cout << "Address: " << (void*)this << ",count: " << count << ",Parent: " << (void*)parent << ",isleaf: " << isLeaf << ",nextNode: " << (void*)nextLeafNode << endl;
    cout << "KEYS:{";
    for(size_t i = 0;i < count;i ++)
    {
        cout << keys[i] << " ";
    }
    cout << "}" << endl;
    if(isLeaf)
    {
        cout << "VALS:{";
        for(size_t i = 0;i < count;i ++)
        {
            cout << vals[i] << " ";
        }
        cout << "}" << endl;
    }
    else // nonleaf node
    {
        cout << "CHILDREN:{";
        for(size_t i = 0;i < count + 1;i ++)
        {
            cout << (void*)childs[i] << " ";
        }
        
        cout << "}" << endl;
    }
    cout << "#############END OF DEBUG IN NODE########"<< endl;
}
#endif

//******** The definition of the functions of the class BPlusTree **********


/**
 * Constructor: init the tree, allocate the memory of the root and then,if users have created the tree before, read from disk and rebuild it.
 *
 * @param string m_name
 * @param int keysize
 * @param int m_degree
 *
 */
template <class KeyType>
BPlusTree<KeyType>::BPlusTree(string m_name,int keysize,int m_degree):fileName(m_name),keyCount(0),level(0),nodeCount(0),root(NULL),leafHead(NULL),keySize(keysize),file(NULL),degree(m_degree)
{
    init_tree();
    readFromDiskAll();
}

/**
 * Deconstrucor: free the allocated memory and write back to the disk if required.
 *
 */
template <class KeyType>
BPlusTree<KeyType>:: ~BPlusTree()
{
    dropTree(root);
    keyCount = 0;
    root = NULL;
    level = 0;
}

/**
 * Init the tree,allocate memory for the root node.
 *
 * @return void
 *
 */
template <class KeyType>
void BPlusTree<KeyType>::init_tree()
{
    root = new TreeNode<KeyType>(degree,true);
    keyCount = 0;
    level = 1;
    nodeCount = 1;
    leafHead = root;
}

/**
 * Search the node to its leaf level to find the node contains the key
 *
 * @param Node
 * @param KeyType&
 * @param searchNodeParse& return the searching information by reference
 *
 * @return
 *
 */
template <class KeyType>
void BPlusTree<KeyType>::findToLeaf(Node pNode,KeyType key,searchNodeParse & snp)
{
    size_t index = 0;
    if(pNode->search(key,index)) // find the key in the node
    {
        if(pNode->isLeaf)
        {
            snp.pNode = pNode;
            snp.index = index;
            snp.ifFound = true;
        }
        else // the node is not a leaf, continue search until the leaf level
        {
            pNode = pNode -> childs[index + 1];
            while(!pNode->isLeaf)
            {
                pNode = pNode->childs[0];
            }
            snp.pNode = pNode;
            snp.index = 0;
            snp.ifFound = true;
        }
        
    }
    else // can not find the key in the node
    {
        if(pNode->isLeaf)
        {
            snp.pNode = pNode;
            snp.index = index;
            snp.ifFound = false;
        }
        else
        {
            findToLeaf(pNode->childs[index],key,snp);
        }
    }
}

/**
 * Insert the key in right position.Then, adjust the whole tree for the rules of b+ tree.
 *
 * @param KeyType&
 * @param offsetNumber the value
 *
 * @return bool the flag that the insertion successes or not
 *
 */
template <class KeyType>
bool BPlusTree<KeyType>::insertKey(KeyType &key,offsetNumber val)
{
    searchNodeParse snp;
    if(!root) init_tree();
    findToLeaf(root,key,snp);
    if(snp.ifFound)
    {
        cout << "Error:in insert key to index: the duplicated key!" << endl;
        return false;
    }
    else
    {
        snp.pNode->add(key,val);
        if(snp.pNode->count == degree)
        {
            adjustAfterinsert(snp.pNode);
        }
        keyCount ++;
        return true;
    }
}

/**
 * Adjust the node after insertion. Rrecursively call this function itself if the father node contradicts the rules.
 *
 * @param Node the pointer pointing to the node
 *
 * @return bool the flag
 *
 */
template <class KeyType>
bool BPlusTree<KeyType>::adjustAfterinsert(Node pNode)
{
    KeyType key;
    Node newNode = pNode->splite(key);
    nodeCount ++;
    
    if(pNode->isRoot()) // the node is the root
    {
        Node root = new TreeNode<KeyType>(degree,false);
        if(root == NULL)
        {
            cout << "Error: can not allocate memory for the new root in adjustAfterinsert" << endl;
            exit(1);
        }
        else
        {
            level ++;
            nodeCount ++;
            this->root = root;
            pNode->parent = root;
            newNode->parent = root;
            root->add(key);
            root->childs[0] = pNode;
            root->childs[1] = newNode;
            return true;
        }
    }// end root
    else // if it is not the root
    {
        Node parent = pNode->parent;
        size_t index = parent->add(key);
        
        parent->childs[index+1] = newNode;
        newNode->parent = parent;
        if(parent->count == degree)
            return adjustAfterinsert(parent);
        
        return true;
    }
}

/**
 * Search the tree to find the value of specific key
 *
 * @param KeyType&
 *
 * @return offsetnumber. The value of the key, -1 means not found in the tree
 *
 */
template <class KeyType>
offsetNumber BPlusTree<KeyType>::search(KeyType& key)
{
    if(!root) return -1;
    searchNodeParse snp;
    findToLeaf(root, key, snp);
    if(!snp.ifFound)
    {
        return -1; // Don't find the key in the tree;
    }
    else
    {
        return snp.pNode->vals[snp.index];
    }
    
}

/**
 * Delete the key-value or key-child by the inputed key.Then adjust the whole tree if required.
 *
 * @param KeyType&
 *
 * @return bool the flag
 *
 */
template <class KeyType>
bool BPlusTree<KeyType>::deleteKey(KeyType &key)
{
    searchNodeParse snp;
    if(!root)
    {
        cout << "ERROR: In deleteKey, no nodes in the tree " << fileName << "!" << endl;
        return false;
    }
    else
    {
        findToLeaf(root, key, snp);
        if(!snp.ifFound)
        {
            cout << "ERROR: In deleteKey, no keys in the tree " << fileName << "!" << endl;
            return false;
        }
        else // find the key in the leaf node
        {
            if(snp.pNode->isRoot())
            {
                snp.pNode->removeAt(snp.index);
                keyCount --;
                return adjustAfterDelete(snp.pNode);
            }
            else
            {
                if(snp.index == 0 && leafHead != snp.pNode) // the key exist in the branch.
                {
                    // go to upper level to update the branch level
                    size_t index = 0;
                    
                    Node now_parent = snp.pNode->parent;
                    bool if_found_inBranch = now_parent->search(key,index);
                    while(!if_found_inBranch)
                    {
                        if(now_parent->parent)
                            now_parent = now_parent->parent;
                        else
                        {
                            break;
                        }
                        if_found_inBranch = now_parent->search(key,index);
                    }// end of search in the branch
                    
                    now_parent -> keys[index] = snp.pNode->keys[1];
                    
                    snp.pNode->removeAt(snp.index);
                    keyCount--;
                    return adjustAfterDelete(snp.pNode);
                    
                }
                else //this key must just exist in the leaf too.
                {
                    snp.pNode->removeAt(snp.index);
                    keyCount--;
                    return adjustAfterDelete(snp.pNode);
                }
            }
        }
    }
}

/**
 * Adjust the node after deletion. Rrecursively call this function itself if the father node contradicts the rules.
 *
 * @param Node the pointer pointing to the node
 *
 * @return bool the flag
 *
 */
template <class KeyType>
bool BPlusTree<KeyType>::adjustAfterDelete(Node pNode)
{
    size_t minmumKey = (degree - 1) / 2;
    if(((pNode->isLeaf)&&(pNode->count >= minmumKey)) || ((degree != 3)&&(!pNode->isLeaf)&&(pNode->count >= minmumKey - 1)) || ((degree ==3)&&(!pNode->isLeaf)&&(pNode->count < 0))) // do not need to adjust
    {
        return  true;
    }
    if(pNode->isRoot())
    {
        if(pNode->count > 0) //do not need to adjust
        {
            return true;
        }
        else
        {
            if(root->isLeaf) //the true will be an empty tree
            {
                delete pNode;
                root = NULL;
                leafHead = NULL;
                level --;
                nodeCount --;
            }
            else // root will be the leafhead
            {
                root = pNode -> childs[0];
                root -> parent = NULL;
                delete pNode;
                level --;
                nodeCount --;
            }
        }
    }// end root
    else
    {
        Node parent = pNode->parent,brother = NULL;
        if(pNode->isLeaf)
        {
            size_t index = 0;
            parent->search(pNode->keys[0],index);
            
            if((parent->childs[0] != pNode) && (index + 1 == parent->count)) //choose the left brother to merge or replace
            {
                brother = parent->childs[index];
                if(brother->count > minmumKey) // choose the most right key of brother to add to the left hand of the pnode
                {
                    for(size_t i = pNode->count;i > 0;i --)
                    {
                        pNode->keys[i] = pNode->keys[i-1];
                        pNode->vals[i] = pNode->vals[i-1];
                    }
                    pNode->keys[0] = brother->keys[brother->count-1];
                    pNode->vals[0] = brother->vals[brother->count-1];
                    brother->removeAt(brother->count-1);
                    
                    pNode->count ++;
                    parent->keys[index] = pNode->keys[0];
                    return true;
                    
                } // end add
                else // merge the node with its brother
                {
                    parent->removeAt(index);
                    
                    for(int i = 0;i < pNode->count;i ++)
                    {
                        brother->keys[i+brother->count] = pNode->keys[i];
                        brother->vals[i+brother->count] = pNode->vals[i];
                    }
                    brother->count += pNode->count;
                    brother->nextLeafNode = pNode->nextLeafNode;
                    
                    delete pNode;
                    nodeCount --;
                    
                    return adjustAfterDelete(parent);
                }// end merge
                
            }// end of the left brother
            else // choose the right brother
            {
                if(parent->childs[0] == pNode)
                    brother = parent->childs[1];
                else
                    brother = parent->childs[index+2];
                if(brother->count > minmumKey)//// choose the most left key of brother to add to the right hand of the node
                {
                    pNode->keys[pNode->count] = brother->keys[0];
                    pNode->vals[pNode->count] = brother->vals[0];
                    pNode->count ++;
                    brother->removeAt(0);
                    if(parent->childs[0] == pNode)
                        parent->keys[0] = brother->keys[0];
                    else
                        parent->keys[index+1] = brother->keys[0];
                    return true;
                    
                }// end add
                else // merge the node with its brother
                {
                    for(int i = 0;i < brother->count;i ++)
                    {
                        pNode->keys[pNode->count+i] = brother->keys[i];
                        pNode->vals[pNode->count+i] = brother->vals[i];
                    }
                    if(pNode == parent->childs[0])
                        parent->removeAt(0);
                    else
                        parent->removeAt(index+1);
                    pNode->count += brother->count;
                    pNode->nextLeafNode = brother->nextLeafNode;
                    delete brother;
                    nodeCount --;
                    
                    return adjustAfterDelete(parent);
                }//end merge
            }// end of the right brother
            
        }// end leaf
        else // branch node
        {
            size_t index = 0;
            parent->search(pNode->childs[0]->keys[0],index);
            if((parent->childs[0] != pNode) && (index + 1 == parent->count)) // choose the left brother to merge or replace
            {
                brother = parent->childs[index];
                if(brother->count > minmumKey - 1) // choose the most right key and child to add to the left hand of the pnode
                {
                    //modify the pnode
                    pNode->childs[pNode->count+1] = pNode->childs[pNode->count];
                    for(size_t i = pNode->count;i > 0;i --)
                    {
                        pNode->childs[i] = pNode->childs[i-1];
                        pNode->keys[i] = pNode->keys[i-1];
                    }
                    pNode->childs[0] = brother->childs[brother->count];
                    pNode->keys[0] = parent->keys[index];
                    pNode->count ++;
                    //modify the father
                    parent->keys[index]= brother->keys[brother->count-1];
                    //modify the brother and child
                    if(brother->childs[brother->count])
                    {
                        brother->childs[brother->count]->parent = pNode;
                    }
                    brother->removeAt(brother->count-1);
                    
                    return true;
                    
                }// end add
                else // merge the node with its brother
                {
                    //modify the brother and child
                    brother->keys[brother->count] = parent->keys[index];
                    parent->removeAt(index);
                    brother->count ++;
                    
                    for(int i = 0;i < pNode->count;i ++)
                    {
                        brother->childs[brother->count+i] = pNode->childs[i];
                        brother->keys[brother->count+i] = pNode->keys[i];
                        brother->childs[brother->count+i]-> parent= brother;
                    }
                    brother->childs[brother->count+pNode->count] = pNode->childs[pNode->count];
                    brother->childs[brother->count+pNode->count]->parent = brother;
                    
                    brother->count += pNode->count;
                    
                    
                    delete pNode;
                    nodeCount --;
                    
                    return adjustAfterDelete(parent);
                }
                
            }// end of the left brother
            else // choose the right brother
            {
                if(parent->childs[0] == pNode)
                    brother = parent->childs[1];
                else
                    brother = parent->childs[index+2];
                if(brother->count > minmumKey - 1)// choose the most left key and child to add to the right hand of the pnode
                {
                    //modifty the pnode and child
                    pNode->childs[pNode->count+1] = brother->childs[0];
                    pNode->keys[pNode->count] = brother->keys[0];
                    pNode->childs[pNode->count+1]->parent = pNode;
                    pNode->count ++;
                    //modify the fater
                    if(pNode == parent->childs[0])
                        parent->keys[0] = brother->keys[0];
                    else
                        parent->keys[index+1] = brother->keys[0];
                    //modify the brother
                    brother->childs[0] = brother->childs[1];
                    brother->removeAt(0);
                    
                    return true;
                }
                else // merge the node with its brother
                {
                    //modify the pnode and child
                    pNode->keys[pNode->count] = parent->keys[index];
                    
                    if(pNode == parent->childs[0])
                        parent->removeAt(0);
                    else
                        parent->removeAt(index+1);
                    
                    pNode->count ++;
                    
                    for(int i = 0;i < brother->count;i++)
                    {
                        pNode->childs[pNode->count+i] = brother->childs[i];
                        pNode->keys[pNode->count+i] = brother->keys[i];
                        pNode->childs[pNode->count+i]->parent = pNode;
                    }
                    pNode->childs[pNode->count+brother->count] = brother->childs[brother->count];
                    pNode->childs[pNode->count+brother->count]->parent = pNode;
                    
                    pNode->count += brother->count;
                    
                    
                    delete brother;
                    nodeCount --;
                    
                    return adjustAfterDelete(parent);
                    
                }
                
            }
            
        }
        
    }
    return false;
}

/**
 * Drop the tree whose root is the inputed node.
 *
 * @param Node the pointer pointing to the node
 *
 * @return void
 *
 */
template <class KeyType>
void BPlusTree<KeyType>::dropTree(Node node)
{
    if(!node) return;
    if(!node->isLeaf) //if it has child
    {
        for(size_t i=0;i <= node->count;i++)
        {
            dropTree(node->childs[i] );
            node->childs[i] = NULL;
        }
    }
    delete node;
    nodeCount --;
    return;
}

/**
 * Read the whole existing tree from the disk.
 *
 * @return void
 *
 */
template <class KeyType>
void BPlusTree<KeyType>::readFromDiskAll()
{
    file = bm.getFile(fileName.c_str());
    blockNode* btmp = bm.getBlockHead(file);
    while (true)
    {
        if (btmp == NULL)
        {
            return;
        }
        
        readFromDisk(btmp);
        if(btmp->ifbottom) break;
        btmp = bm.getNextBlock(file, btmp);
    }
    
}

/**
 * Read a node from the disk.
 *
 * @param blockNode*
 *
 * @return void
 *
 */
template <class KeyType>
void BPlusTree<KeyType>::readFromDisk(blockNode* btmp)
{
    int valueSize = sizeof(offsetNumber);
    char* indexBegin = bm.get_content(*btmp);
    char* valueBegin = indexBegin + keySize;
    KeyType key;
    offsetNumber value;
    
    while(valueBegin - bm.get_content(*btmp) < bm.get_usingSize(*btmp))
        // there are available position in the block
    {
        key = *(KeyType*)indexBegin;
        value = *(offsetNumber*)valueBegin;
        insertKey(key, value);
        valueBegin += keySize + valueSize;
        indexBegin += keySize + valueSize;
    }
    
}

/**
 * Write the whole tree data to the disk.
 *
 * @return void
 *
 */
template <class KeyType>
void BPlusTree<KeyType>::writtenbackToDiskAll()
{
    blockNode* btmp = bm.getBlockHead(file);
    Node ntmp = leafHead;
    int valueSize = sizeof(offsetNumber);
    while(ntmp != NULL)
    {
        bm.set_usingSize(*btmp, 0);
        bm.set_dirty(*btmp);
        for(int i = 0;i < ntmp->count;i ++)
        {
            char* key = (char*)&(ntmp->keys[i]);
            char* value = (char*)&(ntmp->vals[i]);
            memcpy(bm.get_content(*btmp)+bm.get_usingSize(*btmp),key,keySize);
            bm.set_usingSize(*btmp, bm.get_usingSize(*btmp) + keySize);
            memcpy(bm.get_content(*btmp)+bm.get_usingSize(*btmp),value,valueSize);
            bm.set_usingSize(*btmp, bm.get_usingSize(*btmp) + valueSize);
        }
        
        btmp = bm.getNextBlock(file, btmp);
        ntmp = ntmp->nextLeafNode;
    }
    while(1)// delete the empty node
    {
        if(btmp->ifbottom)
            break;
        bm.set_usingSize(*btmp, 0);
        bm.set_dirty(*btmp);
        btmp = bm.getNextBlock(file, btmp);
    }
    
}

#ifdef _DEBUG

template <class KeyType>
void BPlusTree<KeyType>::debug_print()
{
    cout << "############DEBUG FOR THE TREE############" << endl;
    cout << "name:" << fileName << " root:" << (void*)root << " leafHead:" << (void * )leafHead << " keycount:" << keyCount << " level:" << level << " nodeCount:" << nodeCount << endl;
    
    if(root)
        debug_print_node(root);
    cout << "#############END OF DEBUG FOR TREE########" << endl;
    
}


template <class KeyType>
void BPlusTree<KeyType>::debug_print_node(Node pNode)
{
    pNode->debug_print();
    if(!pNode->isLeaf)
        for(int i = 0 ;i < pNode->count+1;i ++)
            debug_print_node(pNode->childs[i]);
    
}
#endif


#endif /* defined(__Minisql__BPlusTree__) */
