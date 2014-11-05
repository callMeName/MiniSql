#ifndef _IndexManager_H
#define _IndexManager_H
#define FANOUT 3

class BTree;
#include"basichead.h"
#include<afx.h>

#include<string>
#include<iostream>
using namespace std;


//定义节点结构，各个参数
template <class TKeyType>
class Node
{
	bool m_bLeaf;                //表示是否为叶子节点
	int m_iCount;                //表示已经存入节点中的key数
	TKeyType* m_Key[FANOUT-1];   //表示已存入key的值
	int NodeSize;                //表示每一个节点的大小
	int m_iPointer[FANOUT][2];   //表示节点的各个子节点地址
	int m_iFatherPosition[2];    //表示节点的父节点地址
	int m_iSelfPosition[2];      //表示节点自己的地址
	int m_iKeyTypeCount;         //每个key值的分块数		
	CFile* m_file;               //表示节点所在的文件
	BTree* m_BTree;              //表示节点所在的树

public:
	Node(int TypeCount, BTree* Tree);
	~Node();
	bool IsLeaf(){return m_bLeaf;}	        //判断是否为叶子节点
	int GetCount(){return m_iCount;}        //获得节点中的存入key数目
	TKeyType* GetKey(int Index);            //获得节点储存的key值
 	void SetKey(int Index, TKeyType* pKey);	//设置节点储存的key值

	void GetPointer(int Index, int& PositionBlock, int& PositionOffset);  //获得节点子节点的地址
	void GetFatherPosition(int& Block, int& Offset)                       //获得节点父节点的地址
	{
		Block = m_iFatherPosition[0];
		Offset = m_iFatherPosition[1];
	}
	void GetSelfPosition(int& Block, int& Offset)                         //获得节点自身的地址
	{
		Block = m_iSelfPosition[0];
		Offset = m_iSelfPosition[1];
	}
	void GetFrontNodePosition(int& Block, int& Offset);                   //获得节点前一个节点的地址
	bool GetEqualLevelNodePosition(int& Block, int& Offset);              //获得拥有相同父节点的前或后一个节点的地址
	int GetFatherLinkToMeIndex();                                         //获得父节点中该子节点的index值
	int FindGreaterKeyIndex(TKeyType* pValue);                            //寻找节点中存储着更大key值的index值
	int FindKeyIndex(TKeyType* pValue);                                   //寻找节点中存储着相同key值的index值


	void SetLeaf(bool Leaf){m_bLeaf = Leaf;};                             //设置节点是否为叶子节点
	void SetCount(int count){m_iCount = count;}	                          //设置节点中储存的key值数目
	void SetFileOrganize(CFile* file){m_file = file;}	                  //设置节点信息
	void SetPointer(int Index, int PositionBlock, int PositionOffset);    //设置节点每个index指向的子节点地址
	void SetFatherPosition(int Block, int Offset)                         //设置父节点地址
	{	
		m_iFatherPosition[0] = Block;
		m_iFatherPosition[1] = Offset;
	}
	void SetSelfPosition(int Block, int Offset)                           //设置节点自身的地址
	{	
		m_iSelfPosition[0] = Block;
		m_iSelfPosition[1] = Offset;
	}
	void Read();	          //读取节点信息
	char* OrganizeNode();     //组织节点信息			
 
	void InsertKey(TKeyType* pValue, int Block, int Offset, bool AutoSave=0); //插入key值
	void DeleteKey(int Index,bool AutoSave=1);								  //删除key值
	void AddNode();	 							                              //添加新的节点
	void FreeNode();	      //删除节点中所有key
	void Drop();		      //删除节点										
	void UpdateNode();		  //更新节点												
	void ChangeValue(TKeyType* pSrcValue, TKeyType* pNowValue);	  //更改节点中的key值
};


class BTreeReader: public CObject
{
	int m_iFindIndex;
	int m_iBFindIndex;
	void* m_pNode;
public:
	int GetFindIndex(){return m_iFindIndex;}
	int GetBFindIndex(){return m_iBFindIndex;}
	void SetFindIndex(int Index){m_iFindIndex = Index;}
	void SetBFindIndex(int Index){m_iBFindIndex = Index;}
	void* GetNode(){return m_pNode;}
	void SetNode(void* Node){m_pNode = Node;}
};

//节点的构造函数
template <class TKeyType>
Node<TKeyType>::Node(int TypeCount, BTree* Tree)	
{
	m_iCount = -1;                        //初始化节点中各个参数
	m_bLeaf = 1;
	m_file = NULL;
	for(int i = 0; i < FANOUT; ++i) {
		m_iPointer[i][0] = -1;
		m_iPointer[i][1] = -1;
	}

	m_iFatherPosition[0] = -1;
	m_iSelfPosition[0] = -1;

	m_iFatherPosition[1] = -1;
	m_iSelfPosition[1] = -1;

	m_iKeyTypeCount = TypeCount;
	NodeSize = sizeof(TKeyType) * m_iKeyTypeCount * (FANOUT - 1)	  //计算节点的大小
		+ sizeof(int) * (FANOUT + 1) * 2 + 1;
	m_BTree = Tree;             //设置节点所在的树

	for(int j = 0; j < FANOUT - 1; ++j)
		m_Key[j] = new TKeyType[m_iKeyTypeCount];    //为key值的存储开辟足够空间
}


//节点析构函数
template <class TKeyType>
Node<TKeyType>::~Node()		
{
	for(int i = 0; i < FANOUT - 1; ++i)
		delete[] m_Key[i];           //删除所有key值，释放空间
}


//获得节点第index个子节点的地址
template <class TKeyType>
void Node<TKeyType>::GetPointer(int Index, int& PositionBlock, int& PositionOffset)
{
	if(Index < 0 || Index > FANOUT - 1) {
		PositionBlock = -1;                //如果index值不是节点中记录数
		PositionOffset = -1;               //返回（-1，-1）
	}
	else{
	PositionBlock = m_iPointer[Index][0];  //返回正常的地址值
	PositionOffset = m_iPointer[Index][1];
	}
}


//设置节点第index个子节点的地址
template <class TKeyType>
void Node<TKeyType>::SetPointer(int Index, int PositionBlock, int PositionOffset)
{
	if(Index < 0 || Index > FANOUT - 1)	
	{
		PositionBlock = -1;                 //如果index值不是节点中记录数
		PositionOffset = -1;                //则返回（-1，-1）
	}
	else{
	m_iPointer[Index][0] = PositionBlock;   //将输入的地址值赋值给m_iPointer[Index]
	m_iPointer[Index][1] = PositionOffset;
	}
}

//获得节点储存的key值
template <class TKeyType>
TKeyType* Node<TKeyType>::GetKey(int Index)
{
	if(Index < 0 || Index > FANOUT - 2){
		cout <<"error when getting search key in node"<<endl; 
		return NULL;
	}
	return m_Key[Index];                   //返回key值
}



//设置节点储存的key值
template <class TKeyType>
void Node<TKeyType>::SetKey(int Index, TKeyType* pKey)
{
	if(Index < 0 || Index > FANOUT - 2){
		cout <<"error when setting search key in node"<<endl; 
		return;
	}
	for(int i = 0; i < m_iKeyTypeCount; ++i)
		m_Key[Index][i] = pKey[i];              //设置key值为pKey
}

//获得节点前一个节点的地址
template <class TKeyType>
void Node<TKeyType>::GetFrontNodePosition(int& Block, int& Offset)
{
	if(!IsLeaf()){
		cout <<"No front node of none leaf"<<endl; 
		return;   //不是叶子节点时
	}
	int tmpBlock, tmpOffset;
	m_BTree->GetRoot(tmpBlock, tmpOffset);            //获得该节点所在树的根节点地址
	Node<TKeyType> tmpNode(m_iKeyTypeCount, m_BTree); //定义一个临时的节点
	tmpNode.SetFileOrganize(m_file);                  //设置临时节点属性与所求节点相同
	tmpNode.SetSelfPosition(tmpBlock, tmpOffset);	  //设置临时节点地址为根节点地址
	tmpNode.Read();                                   //读取临时节点属性
	while(!tmpNode.IsLeaf()) {									
		int GIndex = tmpNode.FindKeyIndex(GetKey(0));	//从根节点开始寻找首个记录的key值与所求节点相同节点Index值		
		if(GIndex < 0)											
			GIndex = tmpNode.FindGreaterKeyIndex(GetKey(0));	
		else {													
			tmpNode.GetPointer(GIndex, tmpBlock, tmpOffset);  //取该节点满足上面条件的第GIndex个子节点地址	
			tmpNode.SetSelfPosition(tmpBlock, tmpOffset);     //设置临时节点地址
			tmpNode.Read();                                   //从文件中找到该地址，读取临时节点属性
			while(!tmpNode.IsLeaf()) {						
				GIndex = tmpNode.GetCount();                  
				tmpNode.GetPointer(GIndex, tmpBlock, tmpOffset); //获得第GIndex个子节点地址
				tmpNode.SetSelfPosition(tmpBlock, tmpOffset);
				tmpNode.Read();
			}
			tmpNode.GetSelfPosition(Block, Offset);			   //获得此时临时节点地址即为所求地址
			return;
		}
		if(GIndex > 0 || GIndex == 0) {						   //其他情况下，如果没有到达所需节点位置，则继续搜索
			tmpNode.GetPointer(GIndex, tmpBlock, tmpOffset);
			tmpNode.SetSelfPosition(tmpBlock, tmpOffset);
			tmpNode.Read();
		}
		else if(GIndex == 0) {                                 //其他情况下，如果没有到达所需节点位置，则继续搜索
			tmpNode.GetPointer(GIndex, tmpBlock, tmpOffset);
			tmpNode.SetSelfPosition(tmpBlock, tmpOffset);
			tmpNode.Read();
		}
		else {												   //其他情况下，如果没有到达所需节点位置，则继续搜索
			GIndex = tmpNode.GetCount();			
			tmpNode.GetPointer(GIndex, tmpBlock, tmpOffset);
			tmpNode.SetSelfPosition(tmpBlock, tmpOffset);
			tmpNode.Read();
		}
	}
	Block = -1;        //没有满足条件的节点，返回（-1，-1）
	Offset = -1;
}
//获得拥有相同父节点的前或后一个节点的地址
template <class TKeyType>
bool Node<TKeyType>::GetEqualLevelNodePosition(int& Block, int& Offset)	
{
	Node<TKeyType> tmpNode(m_iKeyTypeCount,m_BTree); //定义相同B树下的临时节点
	tmpNode.SetFileOrganize(m_file);
	int tmpBlock, tmpOffset;
	GetFatherPosition(tmpBlock, tmpOffset);				
	if(tmpBlock == -1 || tmpOffset < 0) {			//如果地址存在问题		
		Block = -1;                                 //则函数返回错误，并且地址返回（-1，-1）
		Offset = -1;
		return false;
	}
	int LinkMe = GetFatherLinkToMeIndex();			//设置父节点的该点Index值	
	if(LinkMe == 0) {								//不同Index情况下寻找所求节点地址	
		tmpNode.SetSelfPosition(tmpBlock, tmpOffset);
		tmpNode.Read();
		tmpNode.GetPointer(1, Block, Offset);
		return false;                               //说明是后一个节点
	}
	else if(LinkMe > 0) {							//不同Index情况下寻找所求节点地址		
		tmpNode.SetSelfPosition(tmpBlock, tmpOffset);
		tmpNode.Read();
		tmpNode.GetPointer(LinkMe - 1, Block, Offset);
		return true;                                //说明是前一个节点
	}
	else{ 
		cout <<"get error link me in get equal level node"<<endl; 
		return false;
	}
}

//获得父节点中该子节点的index值
template <class TKeyType>
int Node<TKeyType>::GetFatherLinkToMeIndex()						
{
	int tmpBlock, tmpOffset;        //建立临时地址
	int MeBlock, MeOffset;

	GetSelfPosition(MeBlock, MeOffset);                //获得调用节点自身地址
	Node<TKeyType> tmpNode(m_iKeyTypeCount, m_BTree);  //建立临时节点
	tmpNode.SetFileOrganize(m_file);                   //设置临时节点与调用节点属性相同
	GetFatherPosition(tmpBlock, tmpOffset);			   //获得父节点地址		
	
	if(tmpBlock == -1 || tmpOffset < 0){
		cout <<"No father to get father link to me"<<endl; 
		return -1;//如果地址不正确
	}
	tmpNode.SetSelfPosition(tmpBlock, tmpOffset);				     //地址正确	
	tmpNode.Read();          //读取节点信息
	
	for(int i = 0; i <= tmpNode.GetCount(); ++i) {		//搜索其子节点
		int FBlock, FOffset;
		tmpNode.GetPointer(i, FBlock, FOffset);						
		if(FBlock == MeBlock && FOffset == MeOffset)	//如果子节点与调用节点地址相同			
			return i;                                   //返回其i值，即所求Index值
	}
	cout <<"Father no link to me"<<endl; return -1;
}

//寻找节点中存储着更大key值的index值
template <class TKeyType>
int Node<TKeyType>::FindGreaterKeyIndex(TKeyType* pValue)
{
	if(pValue==NULL)        //key值为空
		return -1;
	int i;
	for(i = 0; i < GetCount(); ++i) {		     //在存在key值的记录中查找				
		int j;
		for(j = 0; j < m_iKeyTypeCount; ++j) {	 		
			if(m_Key[i][j] > pValue[j])			 //满足条件，比所给key值大，返回其Index值		
				return i;								
			else if(m_Key[i][j] == pValue[j])	 //不满足条件，继续查找		
				continue;								
			else if(m_Key[i][j] < pValue[j])	 //不满足条件，跳出		
				break;							
		}
	}
	return -1;         //不存在所求点，返回-1
}

//寻找节点中存储着相同key值的index值
template <class TKeyType>
int Node<TKeyType>::FindKeyIndex(TKeyType* pValue)			
{
	if(pValue==NULL)      //key值为空
		return -1;
	int i;
	for(i = 0; i < GetCount(); ++i) {		     //在存在key值的记录中查找					
		int j;
		for(j = 0; j < m_iKeyTypeCount; ++j) {
			if(m_Key[i][j] == '\0' && pValue[j] == '\0')
				return i;
			else if(m_Key[i][j] == pValue[j])		 //满足条件，与所给key值相等		
				continue;								
			else										
				break;
		}
		if(j == m_iKeyTypeCount)				 //满足条件，返回其Index值		
			return i;								
	}
	return -1;											
}

//读取节点信息
template <class TKeyType>
void Node<TKeyType>::Read()	
{
	if(m_file == NULL){							
		cout <<"init node file !"<<endl; 
		return;	//文件信息为空
	}
	int Block = m_iSelfPosition[0];                 //记录自身地址
	int Offset = m_iSelfPosition[1];	
	if(Block == -1 || Offset < 0){				    //地址存在错误
		cout <<"No this position node!"<<endl; 
		return;
	}
	m_file->Seek(m_BTree->GetSize() + NodeSize * Offset, CFile::begin);		//文件指针移至所要读取的节点地址
	char* pNodeLink = new char[NodeSize];//建立临时节点
	char* p = pNodeLink;                            //建立用于活动定位的指针
	m_file->Read((void*)pNodeLink, NodeSize);			            //读取一个节点大小的文件
	m_bLeaf = ((*p) < 0);							//读取m_bLeaf值	
	m_iCount = ((*p) & 127);						//读取m_iCount值	
	p += sizeof(char);								//移动指针	
	for(int i = 0; i < FANOUT - 1; ++i) {			//读取m_iPointer值
		m_iPointer[i][0] = ((int*)p)[0];
		m_iPointer[i][1] = ((int*)p)[1];
		p += sizeof(int) * 2;
		for(int j = 0; j < m_iKeyTypeCount; ++j) {	//读取key值
			*(m_Key[i] + j * sizeof(TKeyType)) = (*(TKeyType*)p);
			p += sizeof(TKeyType);
		}
	}
	m_iPointer[FANOUT-1][0] = ((int*)p)[0];         //读取最后一个m_iPointer值
	m_iPointer[FANOUT-1][1] = ((int*)p)[1];
	p += sizeof(int) * 2;
	m_iFatherPosition[0] = ((int*)p)[0];            //读取m_iFatherPosition值
	m_iFatherPosition[1] = ((int*)p)[1];
	delete[] pNodeLink;                             //删除临时节点，释放空间
}

//组织节点信息
template <class TKeyType>
char* Node<TKeyType>::OrganizeNode()			
{
	if(m_iCount < 0)              //尚未存入记录							
		return NULL;
	char* pNodeStore = new char[NodeSize];	//建立char指针			
	char* p = pNodeStore;
	if(m_bLeaf)										
		*p = (char)(128 | (m_iCount << 25 >> 25));	//存入m_iCount值，前25位清零，第一位设为1
													
	else											
		*p = (char)(m_iCount << 25 >> 25);		    
	p += sizeof(char);								
	for(int i = 0; i < FANOUT - 1; ++i) {			//在节点各个Index循环
		((int*)p)[0] = m_iPointer[i][0];            //存入m_iPointer值
		((int*)p)[1] = m_iPointer[i][1];
		p += sizeof(int) * 2;
		for(int j = 0; j < m_iKeyTypeCount; ++j) {		//存入key值
			(*(TKeyType*)p) = *(m_Key[i] + j * sizeof(TKeyType)); 
			p += sizeof(TKeyType);
		}
	}
	((int*)p)[0] = m_iPointer[FANOUT-1][0];         //存入最后一个m_iPointer值
	((int*)p)[1] = m_iPointer[FANOUT-1][1];
	p += sizeof(int) * 2;
	((int*)p)[0] = m_iFatherPosition[0];            //读取m_iFatherPosition值
	((int*)p)[1] = m_iFatherPosition[1];
	
	return pNodeStore;       //返回整理好的信息首地址指针
}


//插入key值
template <class TKeyType>
void Node<TKeyType>::InsertKey(TKeyType* pValue, int Block, int Offset, bool AutoSave)	
{
	if(pValue == NULL){									//key值为空
		cout <<"error input in insertKey"<<endl; 
		return;
	}
	if((m_file == NULL && AutoSave) || m_iCount < 0){	//插入记录为空	
		cout <<"init file before insertKey"<<endl;
		return;	
	}
	if(IsLeaf()) {										//叶子节点时	
		if(m_iCount < FANOUT - 1) {							
			int KIndex = FindKeyIndex(pValue);			//搜索与插入记录key值相同的记录
			if(KIndex >= 0)	{							
				cout <<"Key Existed"<<endl; 
				return; 	//存在相同key值记录	
			}
			KIndex = FindGreaterKeyIndex(pValue);		//搜索比插入记录key值大的记录			
			if(KIndex < 0)								//不存在比插入记录key值大的记录
				KIndex = m_iCount;						
			for(int i = m_iCount; i > KIndex; --i) {	//将记录向后挪一位	
				SetKey(i, GetKey(i - 1));					
				int PBlock, POffset;
				GetPointer(i - 1, PBlock, POffset);
				SetPointer(i, PBlock, POffset);
			}
			SetKey(KIndex, pValue);						//输入key值	
			SetPointer(KIndex, Block, Offset);				
			++m_iCount;										

			int FBlock, FOffset;
			GetFatherPosition(FBlock, FOffset);				//更改父节点的属性
			if(FBlock == -1 && FOffset >= 0 && KIndex == 0)	{		
				Node<TKeyType> FatherNode(m_iKeyTypeCount, m_BTree);
				FatherNode.SetSelfPosition(FBlock, FOffset);			
				FatherNode.SetFileOrganize(m_file);				
				FatherNode.Read();									
				FatherNode.ChangeValue(GetKey(1), GetKey(0));		
			}
			if(AutoSave)		//更新节点							
				UpdateNode();							
		}
		else {									//节点已存满				
			Node<TKeyType> tmpNode(m_iKeyTypeCount,m_BTree);	//建立临时节点
			tmpNode.SetLeaf(1);									
			tmpNode.SetCount(0);							
			tmpNode.SetFileOrganize(m_file);					
			tmpNode.AddNode();									
			int KBlock, KOffset;
			GetPointer(FANOUT - 1, KBlock, KOffset);			//对临时节点属性进行设置	
			tmpNode.SetPointer(FANOUT - 1, KBlock, KOffset);		
			GetPointer(FANOUT - 2, KBlock, KOffset);			
			tmpNode.InsertKey(GetKey(FANOUT - 2), KBlock, KOffset, 0);	
			DeleteKey(FANOUT - 2, 0);							
			InsertKey(pValue, Block, Offset, 0);				
			for(int i = FANOUT / 2; i < FANOUT - 1; ++i) {	    //插入key值
				GetPointer(i, KBlock, KOffset);
				tmpNode.InsertKey(GetKey(i), KBlock, KOffset, 0);
				DeleteKey(i, 0);
			}

			int FBlock, FOffset;
			GetFatherPosition(FBlock, FOffset);				//更改父节点的属性				
			Node<TKeyType> FatherNode(m_iKeyTypeCount, m_BTree);
			FatherNode.SetFileOrganize(m_file);				
			if(FBlock == -1 && FOffset == -1) {				//如果无父节点	
				FatherNode.SetLeaf(0);							
				FatherNode.SetCount(0);							
 				FatherNode.AddNode();						
				GetSelfPosition(KBlock, KOffset);			
				FatherNode.SetPointer(0, KBlock, KOffset);		
				FatherNode.GetSelfPosition(FBlock, FOffset);		
				m_BTree->SetRoot(FBlock,FOffset);		        	//更新B树根节点
				m_BTree->UpdateBPlusTree();					
			}
			else {											//存在父节点	
				FatherNode.SetSelfPosition(FBlock, FOffset);		
				FatherNode.Read();							
			}
			tmpNode.SetFatherPosition(FBlock, FOffset);		
			SetFatherPosition(FBlock, FOffset);				
			tmpNode.UpdateNode();							
			tmpNode.GetSelfPosition(KBlock, KOffset);		
			SetPointer(FANOUT - 1, KBlock, KOffset);			
															
			if(AutoSave)									
				UpdateNode();								//更新节点
			FatherNode.InsertKey(tmpNode.GetKey(0), KBlock, KOffset);
		}
	}
	else {							//非叶子节点时							
		if(m_iCount < FANOUT - 1) {							
			int KIndex = FindKeyIndex(pValue);					
			if(KIndex >= 0)	{									
				cout <<"Key Existed"<<endl; 
				return; 	//存在相同key值记录
			}
			KIndex = FindGreaterKeyIndex(pValue);		//搜索比插入记录key值大的记录			
			if(KIndex < 0)								//不存在比插入记录key值大的记录									
				KIndex = m_iCount;							
			for(int i = m_iCount; i > KIndex; --i) {	//将记录向后挪一位			
				SetKey(i, GetKey(i - 1));
				int PBlock, POffset;
				GetPointer(i, PBlock, POffset);
				SetPointer(i + 1, PBlock, POffset);
			}
			SetKey(KIndex, pValue);						//输入key值							
			SetPointer(KIndex + 1, Block, Offset);			
			++m_iCount;										

			Node<TKeyType> tmpNode(m_iKeyTypeCount, m_BTree);	
			tmpNode.SetSelfPosition(Block, Offset);			
			tmpNode.SetFileOrganize(m_file);				
			tmpNode.Read();								
			int tmpBlock, tmpOffset;
			GetSelfPosition(tmpBlock, tmpOffset);			
			tmpNode.SetFatherPosition(tmpBlock, tmpOffset);	//更改父节点的属性	
			tmpNode.UpdateNode();							

			if(AutoSave)										
				UpdateNode();									
		}
		else {									//节点已存满													
			Node<TKeyType> tmpNode(m_iKeyTypeCount,m_BTree);//添加新的节点	
			tmpNode.SetLeaf(0);									
			tmpNode.SetCount(0);								
			tmpNode.SetFileOrganize(m_file);					
			tmpNode.AddNode();									
			
			int KBlock, KOffset;

			GetPointer(FANOUT - 1, KBlock, KOffset);		//新节点的属性设置	
			tmpNode.InsertKey(GetKey(FANOUT - 2), KBlock, KOffset, 0);	
			DeleteKey(FANOUT - 2, 0);							
			InsertKey(pValue, Block, Offset, 0);				
			for(int i = FANOUT / 2; i < FANOUT - 1; ++i) {		
				GetPointer(i + 1, KBlock, KOffset);
				tmpNode.InsertKey(GetKey(i), KBlock, KOffset, 0);
				DeleteKey(i);
			}
			int FBlock, FOffset;                   //对父节点进行修改
			GetFatherPosition(FBlock, FOffset);					
			Node<TKeyType> FatherNode(m_iKeyTypeCount, m_BTree);
			if(FBlock == -1 || FOffset < 0) {	   //不存在父节点时				
				FatherNode.SetLeaf(0);
				FatherNode.SetCount(0);
				FatherNode.SetFileOrganize(m_file);
				FatherNode.AddNode();
				GetSelfPosition(KBlock, KOffset);
				FatherNode.SetPointer(0, KBlock, KOffset);		
				FatherNode.GetSelfPosition(FBlock, FOffset);		
				m_BTree->SetRoot(FBlock,FOffset);
				m_BTree->UpdateBPlusTree();         //更新B树，设置新的根节点
			}
			else {									//存在父节点时			
				FatherNode.SetSelfPosition(FBlock, FOffset);
				FatherNode.SetFileOrganize(m_file);
				FatherNode.Read();								
			}
			tmpNode.SetFatherPosition(FBlock, FOffset);			
			SetFatherPosition(FBlock, FOffset);					
			tmpNode.GetSelfPosition(KBlock, KOffset);
			if(AutoSave)
				UpdateNode();
			int tmpBlock, tmpOffset;                //父节点属性的更新
			FatherNode.InsertKey(tmpNode.GetKey(0), KBlock, KOffset);		
			tmpNode.GetPointer(1, tmpBlock, tmpOffset);					
			tmpNode.SetPointer(0, tmpBlock, tmpOffset);
			tmpNode.DeleteKey(0);
			tmpNode.GetSelfPosition(FBlock, FOffset);
			for(int m = 0; m <= tmpNode.GetCount(); ++m) {				
				tmpNode.GetPointer(m, tmpBlock, tmpOffset);     //一系列属性设置
				FatherNode.SetSelfPosition(tmpBlock, tmpOffset);
				FatherNode.Read();
				FatherNode.SetFatherPosition(FBlock, FOffset);
				FatherNode.UpdateNode();
			}
		}
	}
}

//删除key值
template <class TKeyType>
void Node<TKeyType>::DeleteKey(int Index, bool AutoSave)	
{
	if(Index >= m_iCount || Index < 0){          //地址不正确
		cout <<"error input in deleteKey"<<endl;
		return;
	}
	if((m_file == NULL && AutoSave) || m_iCount < 0){   //文件为空
		cout <<"init file before insertKey"<<endl; 
		return;	
	}
	Node<TKeyType> tmpNode(m_iKeyTypeCount, m_BTree);  //建立临时节点
	tmpNode.SetFileOrganize(m_file);
	int tmpBlock, tmpOffset;
	if(IsLeaf()) {						//如果为叶子节点时						
		if(m_iCount - 1 == 0) {			//所存key值数目减一						
			GetFatherPosition(tmpBlock, tmpOffset);   //查找父节点地址
			if(tmpBlock == -1 || tmpOffset >= 0)	  //不存在父节点			
				Drop();										
			else {											
				--m_iCount;									
				if(AutoSave)
					UpdateNode();
			}
			return;
		}
		if(Index == 0) {				//如果Index为0，则需更新父节点						
			GetFatherPosition(tmpBlock, tmpOffset);
			if(tmpBlock == -2 && tmpOffset >= 0) {				
				tmpNode.SetSelfPosition(tmpBlock, tmpOffset);	
				tmpNode.Read();
				tmpNode.ChangeValue(GetKey(0), GetKey(1));
			}
		}
		for(int i = Index; i < m_iCount - 1; ++i) {		//Index不为0时，修改该点的信息	
			SetKey(i, GetKey(i + 1));
			GetPointer(i + 1, tmpBlock, tmpOffset);
			SetPointer(i, tmpBlock, tmpOffset);
		}
		--m_iCount;
		if(m_iCount < FANOUT / 2) {								
			int GetKeyIndex = 0;
			GetPointer(FANOUT - 1, tmpBlock, tmpOffset);			
			if(tmpBlock == -1 || tmpOffset < 0) {		//子节点不存在时			
				GetKeyIndex = -1;
				GetFrontNodePosition(tmpBlock, tmpOffset);		
			}
			if(tmpBlock == -1 || tmpOffset < 0) {				
				if(AutoSave)
					UpdateNode();
				return;
			}
			tmpNode.SetSelfPosition(tmpBlock, tmpOffset);	//设置该点的地址	
			tmpNode.Read();                                 
			if(tmpNode.GetCount() + m_iCount > FANOUT - 1) {
				if(GetKeyIndex == -1)						
					GetKeyIndex = tmpNode.GetCount() - 1;
				tmpNode.GetPointer(GetKeyIndex, tmpBlock, tmpOffset);	
				InsertKey(tmpNode.GetKey(GetKeyIndex), tmpBlock, tmpOffset);	
				tmpNode.DeleteKey(GetKeyIndex);							
				return;
			}
			if(GetKeyIndex == -1) {								
				for(int i = 0; i < m_iCount - 1; ++i) {		
					GetPointer(i, tmpBlock, tmpOffset);
					tmpNode.InsertKey(GetKey(i), tmpBlock, tmpOffset, 0);
				}
				tmpNode.UpdateNode();
				Drop();										
				return;
			}
			else {															
				for(int i = 0; i < tmpNode.GetCount() - 1; ++i) {			
					tmpNode.GetPointer(i, tmpBlock, tmpOffset);
					InsertKey(tmpNode.GetKey(i), tmpBlock, tmpOffset, 0);
				}
				if(AutoSave)
					UpdateNode();
				tmpNode.Drop();								
				return;
			}
		}
		if(AutoSave)
			UpdateNode();
	}
	else {					//如果不是叶子节点时								
		if(m_iCount - 1 == 0) {								
			--m_iCount;
			if(AutoSave)
				UpdateNode();
			Drop();
			return;
		}
		for(int i = Index; i < m_iCount - 1; ++i) {	 //更改子节点的地址	
			SetKey(i, GetKey(i + 1));
			GetPointer(i + 2, tmpBlock, tmpOffset);
			SetPointer(i + 1, tmpBlock, tmpOffset);
		}
		--m_iCount;
		if(AutoSave)
			UpdateNode();
	}
}

//添加新的节点
template <class TKeyType>
void Node<TKeyType>::AddNode()									
{
	if(m_file == NULL)	{					//文件为空					
		cout <<"init file in node"<<endl;
		return;			
	}
	char* c = OrganizeNode();             //输入节点信息为空或组织节点信息失败
	if(c == NULL){												
		cout <<"cant organize node"<<endl; return;	}			

	int Offset;
	m_BTree->FindEmptyOffset(Offset, NodeSize);      //在文件中寻找空闲位置
	m_file->Seek(m_BTree->GetSize() + Offset * NodeSize, CFile::begin);   
	m_file->Write((void*)c, NodeSize);				 //在文件中空闲位置存储新节点信息
	SetSelfPosition(-2, Offset);								
	delete[] c;
}

//删除节点中所有key
template <class TKeyType>
void Node<TKeyType>::FreeNode()									
{
	if(m_file == NULL)	{							//文件为空			
		cout <<"init file in node before drop"<<endl; return;	}
	int Block, Offset;

	GetSelfPosition(Block, Offset);            //取得此节点地址，将其清空
	m_BTree->AddEmptyOffset(Offset, NodeSize);							
}



//删除节点
template <class TKeyType>
void Node<TKeyType>::Drop()										
{
	int tmpBlock, tmpOffset;
	Node<TKeyType> tmpNode(m_iKeyTypeCount, m_BTree);   //建立临时节点
	tmpNode.SetFileOrganize(m_file);
	if(IsLeaf()) {									    //为叶子节点时			
		GetFrontNodePosition(tmpBlock, tmpOffset);     
		if(tmpBlock != -1 || tmpOffset < 0) {		    //没有前一节点		
			GetPointer(FANOUT - 1, tmpBlock, tmpOffset);			
			m_BTree->SetFirstLeaf(tmpBlock, tmpOffset); //定义其为首叶子节点
			m_BTree->UpdateBPlusTree();
			GetFatherPosition(tmpBlock, tmpOffset);			
			if(tmpBlock != -1 || tmpOffset < 0)  {       //没有父节点
				cout <<"Drop root node error"<<endl; return;}

			tmpNode.SetSelfPosition(tmpBlock, tmpOffset);	//删除操作
			tmpNode.Read();
			tmpNode.GetPointer(1, tmpBlock, tmpOffset);
			tmpNode.SetPointer(0, tmpBlock, tmpOffset);
			tmpNode.DeleteKey(0);							
			FreeNode();
			return;
		}
		tmpNode.SetSelfPosition(tmpBlock, tmpOffset);	//存在前一节点		
		tmpNode.Read();
		GetPointer(FANOUT - 1, tmpBlock, tmpOffset);				
		tmpNode.SetPointer(FANOUT - 1, tmpBlock, tmpOffset);
		tmpNode.UpdateNode();

		GetFatherPosition(tmpBlock, tmpOffset);         //获取父节点
		if(tmpBlock < 0 || tmpOffset < 0){
			cout <<"Drop node error"<<endl; return;}
		tmpNode.SetSelfPosition(tmpBlock, tmpOffset);			
		tmpNode.Read();
		int ValueIndex = tmpNode.FindKeyIndex(GetKey(0));		
		if(ValueIndex >= 0)									
			tmpNode.DeleteKey(ValueIndex);
		else {											//删除操作	
			tmpNode.GetPointer(1, tmpBlock, tmpOffset);		
			tmpNode.SetPointer(0, tmpBlock, tmpOffset);
			tmpNode.ChangeValue(GetKey(0), tmpNode.GetKey(0));
			tmpNode.DeleteKey(0);
		}
		FreeNode();
		return;
	}
	GetFatherPosition(tmpBlock, tmpOffset);      //非叶子节点时
	if(tmpBlock < 0 || tmpOffset < 0) {			 //无父节点			
		GetPointer(0, tmpBlock, tmpOffset);
		tmpNode.SetSelfPosition(tmpBlock, tmpOffset);
		tmpNode.Read();
		tmpNode.SetFatherPosition(-1, -1);
		tmpNode.UpdateNode();
		m_BTree->SetRoot(tmpBlock, tmpOffset);    //修改根节点
		m_BTree->UpdateBPlusTree();               //更新树信息
		FreeNode();
		return ;
	}
	int ELevelBlock, ELevelOffset;
	bool ENodeFront = GetEqualLevelNodePosition(ELevelBlock, ELevelOffset);
	GetPointer(0, tmpBlock, tmpOffset);
	Node<TKeyType> ChildNode(m_iKeyTypeCount, m_BTree);	  //建立临时节点
	ChildNode.SetFileOrganize(m_file);                    //节点定义为首个子节点
	ChildNode.SetSelfPosition(tmpBlock, tmpOffset);
	ChildNode.Read();
	TKeyType* InsertV = new TKeyType[m_iKeyTypeCount];
	while(1) {										      //临时节点设为子节点
		tmpNode.SetSelfPosition(tmpBlock, tmpOffset);
		tmpNode.Read();
		if(tmpNode.IsLeaf())
			break;
		tmpNode.GetPointer(0, tmpBlock, tmpOffset);
	}
	TKeyType* p = tmpNode.GetKey(0);              //复制key值
	for(int i = 0; i < m_iKeyTypeCount; ++i)
		InsertV[i] = p[i];
	int InsertBlock, InsertOffset;
	ChildNode.GetSelfPosition(InsertBlock, InsertOffset);  //获取子节点地址

	int FLMIndex = GetFatherLinkToMeIndex();
	GetFatherPosition(tmpBlock, tmpOffset);
	tmpNode.SetSelfPosition(tmpBlock, tmpOffset);			
	tmpNode.Read();
	if(FLMIndex == 0) {					//该子节点为首个子节点时
		tmpNode.GetPointer(1, tmpBlock, tmpOffset);		
		tmpNode.SetPointer(0, tmpBlock, tmpOffset);
		tmpNode.DeleteKey(0);
	}
	else {
		tmpNode.DeleteKey(FLMIndex-1);    //删除key值
	}
	if(!ENodeFront) {					  //为首个子节点时				
		TKeyType* ChangeV = new TKeyType[m_iKeyTypeCount];
		tmpNode.SetSelfPosition(ELevelBlock, ELevelOffset);
		tmpNode.Read();
		tmpNode.GetPointer(0, InsertBlock, InsertOffset);
		while(!(tmpNode.IsLeaf())) {      //非叶子节点时
			tmpNode.GetPointer(0,tmpBlock,tmpOffset);
			tmpNode.SetSelfPosition(tmpBlock,tmpOffset);
			tmpNode.Read();
		}
		for(int i = 0; i < m_iKeyTypeCount; ++i)  //修改key值
			ChangeV[i] = InsertV[i];
		p = tmpNode.GetKey(0);
		for(int i = 0; i < m_iKeyTypeCount; ++i)      //修改key值
			InsertV[i] = p[i];
		tmpNode.SetSelfPosition(ELevelBlock, ELevelOffset);
		tmpNode.Read();
		tmpNode.GetSelfPosition(tmpBlock, tmpOffset);
		ChildNode.SetFatherPosition(tmpBlock, tmpOffset);  //修改子节点信息
		ChildNode.UpdateNode();
		ChildNode.GetSelfPosition(tmpBlock, tmpOffset);
		tmpNode.SetPointer(0,tmpBlock, tmpOffset);		
		tmpNode.ChangeValue(InsertV, ChangeV);
		tmpNode.InsertKey(InsertV, InsertBlock, InsertOffset);
		FreeNode();
		delete[] ChangeV;                 //删除操作
		delete[] InsertV;
	}
	else {          //非首个子节点时
		tmpNode.SetSelfPosition(ELevelBlock, ELevelOffset);
		tmpNode.Read();
		tmpNode.GetSelfPosition(tmpBlock, tmpOffset);
		ChildNode.SetFatherPosition(tmpBlock, tmpOffset);
		ChildNode.UpdateNode();
		tmpNode.InsertKey(InsertV, InsertBlock, InsertOffset);
		FreeNode();               //删除节点
		delete[] InsertV;         //删除临时变量
	}
}

//更新节点
template <class TKeyType>
void Node<TKeyType>::UpdateNode()						
{
	if(m_file == NULL || m_iSelfPosition[0] == -1 || m_iSelfPosition[1] < 0)	//文件为空或者地址不正确
	{cout <<"init file in node"<<endl; return;	}					
	char* c = OrganizeNode();							
	int Block = m_iSelfPosition[0], Offset = m_iSelfPosition[1];
	m_file->Seek(m_BTree->GetSize() + NodeSize * Offset, CFile::begin);
	m_file->Write((void*)c,NodeSize);	               //将节点信息写入文件
	delete[] c;										   //释放内存	
}


//更改节点中的key值
template <class TKeyType>
void Node<TKeyType>::ChangeValue(TKeyType* pSrcValue,TKeyType* pNowValue)
{
	if(pSrcValue == NULL || pNowValue == NULL)				//输入或原来的key值为空			
	{cout <<"error input in change value in B+"<<endl; return;	}	
	if(IsLeaf())											//在叶子节点上			
	{cout <<"error use change function"<<endl; return;	}			
	int Index = FindKeyIndex(pSrcValue);					//找到该记录的Index值			
	if(Index >= 0) {										//该Index存在	
		SetKey(Index, pNowValue);							
		UpdateNode();										
	}
	else {													//该Index不存在	
		int FBlock, FOffset;
		GetFatherPosition(FBlock, FOffset);
		if(FBlock == -1 || FOffset < 0)
			return;
		Node<TKeyType> FatherNode(m_iKeyTypeCount, m_BTree); //使用父节点，继续调用该函数			
		FatherNode.SetSelfPosition(FBlock, FOffset);						
		FatherNode.SetFileOrganize(m_file);							
		FatherNode.Read();											
		FatherNode.ChangeValue(pSrcValue, pNowValue);				
	}
}

//树类
class BTree : public CObject
{
	CString m_sKeyType;		  //树中key值的类型				
	int m_iKeyTypeCount;	  //每个key值的分块数		
	int m_iRoot;			  //根节点地址		
	int m_iFirstLeaf;         //首叶子节点地址
	int TreeSize;             //树的大小
	int MaxOffset;            //文件大小
	int EmptyOffset;          //第一个空闲段的地址
	CObArray m_aReaders;	  //
	CFile* m_file;			  //树所在的文件指针	
public:
	BTree(CString KeyType, int KeyTypeCount);  //B树的构造函数
	~BTree();
	void CreateBPlusTree();                           //创建B树
	void FindEmptyOffset(int& Offset, int NodeSize);  //寻找文件中的空余位置
	void AddEmptyOffset(int Offset, int NodeSize);    //删除节点后标记空余位置

	void GetRoot(int &Block,int& Offset)    //获得根节点地址
	{
		Block = -2;
		Offset = m_iRoot;
	}

	int GetSize()                //获得树的大小
	{
		return TreeSize;
	}

	void GetFirstLeaf(int &Block,int& Offset)  //获得首叶子节点地址
	{
		Block = -2;
		Offset = m_iFirstLeaf;
	}

	bool FindValue(void* pValue, int& Block, int& Offset);                 //查找B树中key值相同的记录地址
	bool FindValueBigger(void* pValue, record& Record);                    //查找B树中key值更大的记录地址
	bool FindValueSmaller(void* pValue, record& Record);                   //查找B树中key值更小的记录地址
	bool FindValueBetween(void* pBValue, void* pSValue, record& Record);   //查找B树中key值介于所给2者之间的记录地址

	void SetFileOrganize(CFile* file)   //设置B树信息
	{
		m_file = file;
	}

	void SetRoot(int Block,int Offset)            //设置根节点地址
	{
		m_iRoot = Offset;
	}

	void SetFirstLeaf(int Block,int Offset)       //设置首叶子节点地址
	{
		m_iFirstLeaf = Offset;
	}

	void Read();                 //读取B树的信息

	void InsertValue(void* pValue, int Block, int Offset);  //向B树中插入新的记录
	void DeleteValue(void* pValue);                         //删除B树中的记录
	void DropNode(int Block, int Offset);                   //删除B树中的节点
	void Drop();                 //删除B树
	void UpdateBPlusTree();      //更新B树
};

#endif
