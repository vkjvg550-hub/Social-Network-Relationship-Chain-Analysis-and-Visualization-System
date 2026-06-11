#ifndef __ADJ_LIST_GRAPH_H__
#define __ADJ_LIST_GRAPH_H__
#include "Assistance.h"						// 辅助软件包

#include "AdjListDirNetworkArc.h"			// 网络邻接表的边结点类
#include "AdjListDirNetworkVex.h"			// 网络邻接表的顶点结点类

// 有向网的邻接表类//改成无向图
template <class ElemType, class WeightType>
class AdjListDirNetwork
{
protected:
// 邻接表的数据成员:
	int vexNum, vexMaxNum, arcNum;					// 顶点数目、允许的顶点最大数目和边数
	AdjListNetWorkVex<ElemType, WeightType> *vexTable;	// 顶点表
	mutable Status *tag;					        // 标志数组				
	WeightType infinity;							// 无穷大的值 

	//新增辅助函数
	bool ArcExists(int v1, int v2) const;
	bool DeleteArcNode(int v1, int v2);
	void DFS(int v) const;

	//关系链DFS
	void DFSRelationChains(int cur, int target, bool visited[], int path[], int pathLen, int& chainCount) const;//内部调用
	//要求4找小团体
	void DFSSmallGroup(
		int v,
		WeightType threshold,
		int group[],
		int& groupSize
	) const;
	//挑战性问题1 哈密顿通路
	//直接找pathLen==vexNum的路径输出
	void DFSHamiltonPath(
		int cur,
		bool visited[],
		int path[],
		int pathLen,
		int& pathCount,
		int maxCount
	) const;
	//挑战性问题2
	void DFSFastestHamiltonPath(
		int cur,
		bool visited[],
		int path[],
		int pathLen,
		double curTime,
		double& bestTime,
		int bestPath[],
		bool& found,
		int& checkedCount,
		int maxCount
	) const;
public:
// 抽象数据类型方法声明及重载编译系统默认方法声明:
	AdjListDirNetwork(ElemType es[], int vertexNum, int vertexMaxNum = DEFAULT_SIZE, 
		WeightType infinit = (WeightType)DEFAULT_INFINITY);
		// 以数组es[]为顶点数据,顶点个数为vertexNum,允许的顶点最大数目为vertexMaxNum,
        // infinit表示无穷大,边数为0构造有向网
	AdjListDirNetwork(int vertexMaxNum = DEFAULT_SIZE, 
		WeightType infinit = (WeightType)DEFAULT_INFINITY);
		// 构造允许的顶点最大数目为vertexMaxNum,infinit表示无穷大,边数为0的有向网
	~AdjListDirNetwork();						 // 析构函数
	void Clear();			                     // 清空有向网			 
	bool IsEmpty();                              // 判断有向网是否为空 
	int GetOrder(ElemType &d) const;             // 求顶点的序号	
	Status GetElem(int v, ElemType &d) const;    // 求顶点的元素值	
	Status SetElem(int v, const ElemType &d);    // 设置顶点的元素值
	WeightType GetInfinity() const;				 // 取无穷大的值			 
	int GetVexNum() const;						 // 求有向网的顶点个数			 
	int GetArcNum() const;						 // 求有向网的边数个数			 
	int FirstAdjVex(int v) const;				 // 求有向网中顶点v的第一个邻接点			 
	int NextAdjVex(int v1, int v2) const;		 // 求有向网中顶点v1的相对于v2的下一个邻接点			 
	void InsertVex(const ElemType &d);			 // 插入元素值为d的顶点		 
	//插入人物UX

	void InsertArc(int v1, int v2, WeightType w);// 插入从顶点为v1到v2、权为w的边		
	//插入人物关系亲密度

	void DeleteVex(const ElemType &d);			 // 删除元素值为d的顶点		
	//删除人物UX

	void DeleteArc(int v1, int v2);			     // 删除从顶点为v1到v2的边		
	//删除人物关系和亲密度

	WeightType GetWeight(int v1, int v2) const;	 // 求从顶点为v1到v2的边的权值
	void SetWeight(int v1, int v2, WeightType w);// 设置从顶点为v1到v2的边的权值
	Status GetTag(int v) const;				     // 求顶点v的标志		 
	void SetTag(int v, Status tag) const;	     // 设置顶点v的标志为tag	 
	AdjListDirNetwork(const AdjListDirNetwork<ElemType, WeightType> &copy);	// 复制构造函数
	AdjListDirNetwork<ElemType, WeightType> &operator =
		(const AdjListDirNetwork<ElemType, WeightType> &copy); // 重载赋值运算符 
    void Display();	// 显示有向网邻接表 

	//实验函数
	int Degree(int v) const;//求顶点度
	void DisplayDegree() const;//输出所有顶点度
	int ConnectedComponentNum() const;//求连通分量数


	//研讨函数
	void FindAllRelationChains(const ElemType& user1, const ElemType& user2) const;//关系链对外接口
	void FindSmallGroups(WeightType threshold) const;//找小团体
	void FindHamiltonPaths(int maxCount = 20) const;//哈密顿通路
	void FindFastestHamiltonPath(int maxCount = 30) const;//最短哈密顿通路
};


//辅助函数
template <class ElemType, class WeightType>
bool AdjListDirNetwork<ElemType, WeightType>::ArcExists(int v1, int v2) const
//判断v1的链表里有没有v2
{
	AdjListNetworkArc<WeightType>* p = vexTable[v1].firstarc;

	while (p != NULL) {
		if (p->adjVex == v2)
			return true;
		p = p->nextarc;
	}

	return false;
}

template <class ElemType, class WeightType>
bool AdjListDirNetwork<ElemType, WeightType>::DeleteArcNode(int v1, int v2)
//只删一个方向的边结点 打包原有部分代码
{
	AdjListNetworkArc<WeightType>* p = vexTable[v1].firstarc;
	AdjListNetworkArc<WeightType>* q = NULL;

	while (p != NULL && p->adjVex != v2) {
		q = p;
		p = p->nextarc;
	}

	if (p == NULL)
		return false;

	if (q == NULL)
		vexTable[v1].firstarc = p->nextarc;
	else
		q->nextarc = p->nextarc;

	delete p;
	return true;
}


// 有向网的邻接表类的实现部分
template <class ElemType, class WeightType>
AdjListDirNetwork<ElemType, WeightType>::AdjListDirNetwork(ElemType es[],
       int vertexNum, int vertexMaxNum, WeightType infinit)
// 操作结果：构造顶点数据为es[],顶点数为numVex,顶点个数为vertexNum,infinit表示无穷大,边数为0的有向网
{
	if (vertexMaxNum < 0)
    	throw Error("允许的顶点最大数目不能为负!");        // 抛出异常

	if (vertexMaxNum < vertexNum)
    	throw Error("顶点数目不能大于允许的顶点最大数目!");// 抛出异常

	vexNum = vertexNum;			
	vexMaxNum = vertexMaxNum; 
	arcNum = 0;
	infinity = infinit;
	
	tag = new Status[vexMaxNum];
	vexTable = new AdjListNetWorkVex<ElemType, WeightType>[vexMaxNum];
 
	for (int v = 0; v < vexNum; v++)  {
		tag[v] = UNVISITED;
		vexTable[v].data = es[v];
		vexTable[v].firstarc = NULL;
	}
}

template <class ElemType, class WeightType>
AdjListDirNetwork<ElemType, WeightType>::AdjListDirNetwork(int vertexMaxNum, WeightType infinit)
// 操作结果：构造顶点最大数目为vertexMaxNum,infinit表示无穷大的空有向网
{
	if (vertexMaxNum < 0)
    	throw Error("允许的顶点最大数目不能为负!");// 抛出异常

	vexNum = 0;			
	vexMaxNum = vertexMaxNum;
	arcNum = 0;
	infinity = infinit;

	tag = new Status[vexMaxNum];
	vexTable = new AdjListNetWorkVex<ElemType, WeightType>[vexMaxNum];
}

template <class ElemType, class WeightType>
AdjListDirNetwork<ElemType, WeightType>::~AdjListDirNetwork()
// 操作结果：释放邻接表有向网所占用空间
{
    Clear();                                // 释放边结点
	delete []tag;							// 释放标志
	delete []vexTable;						// 释放邻接表
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::Clear()
// 操作结果：释放所有的边结点，并把有向网的顶点数和边数设置为0.			 
{
	AdjListNetworkArc<WeightType> *p;
	for (int v = 0; v < vexNum; v++)	{	// 释放边结点
	    p = vexTable[v].firstarc;
	    while (p != NULL) {
			vexTable[v].firstarc = p->nextarc;
			delete p;
			p = vexTable[v].firstarc;
        }	
	}
    vexNum = 0;
	arcNum = 0;	
}

template <class ElemType, class WeightType>
bool AdjListDirNetwork<ElemType, WeightType>::IsEmpty()
// 操作结果：如果有向网为空返回true,否则返回false.
{
	return vexNum == 0;
}

template <class ElemType, class WeightType>
int AdjListDirNetwork<ElemType, WeightType>::GetOrder(ElemType &d) const
// 操作结果：求顶点d的序号.顶点的序号从0开始，图中不存在顶点d时返回-1. 
{
    int v;
    for (v = 0; v < vexNum; v++)
        if (d == vexTable[v].data)
           break;
           
	if (v < 0 || v >= vexNum)
		return -1;	// 顶点d不存在,返回-1
	else
		return v;	// 顶点d不存在,返回它的序号 
}	

template <class ElemType, class WeightType>
Status AdjListDirNetwork<ElemType, WeightType>::GetElem(int v, ElemType &d) const
// 操作结果：求序号为v的顶点值, v的取值范围为0 ≤ v ＜ vexNum, v合法时函数
//	通过d取得顶点值，并返回ENTRY_FOUND；否则函数返回NOT_PRESENT
{
	if (v < 0 || v >= vexNum)
		return NOT_PRESENT;			// 元素不存在
	else	{
		d = vexTable[v].data;		// 将顶点v的元素值赋给d
		return ENTRY_FOUND;			// 元素存在
	}
}	

template <class ElemType, class WeightType>
Status AdjListDirNetwork<ElemType, WeightType>::SetElem(int v, const ElemType &d)
// 操作结果：设置顶点的元素值v的取值范围为0 ≤ v ＜ vexNum, v合法时函数返回
//	SUCCESS, 否则函数返回RANGE_ERROR
{
	if (v < 0 || v >= vexNum)
		return RANGE_ERROR;			// 位置错
	else	{
		vexTable[v].data = d;		// 顶点元素
		return SUCCESS;				// 成功
	}
}

template <class ElemType, class WeightType>
WeightType AdjListDirNetwork<ElemType, WeightType>::GetInfinity() const
// 操作结果：返回无穷大的值 
{
	return infinity;
}

template <class ElemType, class WeightType>
int AdjListDirNetwork<ElemType, WeightType>::GetVexNum() const
// 操作结果：返回顶点个数			 
{
	return vexNum;
}

template <class ElemType, class WeightType>
int AdjListDirNetwork<ElemType, WeightType>::GetArcNum() const
// 操作结果：返回边数个数
{
	return arcNum;
}		 

template <class ElemType, class WeightType>
int AdjListDirNetwork<ElemType, WeightType>::FirstAdjVex(int v) const
// 操作结果：返回顶点v的第一个邻接点			 
{
	if (v < 0 || v >= vexNum)
       throw Error("v不合法!");// 抛出异常

	if (vexTable[v].firstarc == NULL)
	   return -1;              // 不存在邻接点
	else
	   return vexTable[v].firstarc->adjVex;
}

template <class ElemType, class WeightType>
int AdjListDirNetwork<ElemType, WeightType>::NextAdjVex(int v1, int v2) const
// 操作结果：返回顶点v1的相对于v2的下一个邻接点			 
{
	AdjListNetworkArc<WeightType> *p;
	if (v1 < 0 || v1 >= vexNum)
       throw Error("v1不合法!");	// 抛出异常
	if (v2 < 0 || v2 >= vexNum)
       throw Error("v2不合法!");	// 抛出异常
	if (v1 == v2)
       throw Error("v1不能等于v2!");// 抛出异常
	
	p = vexTable[v1].firstarc;
	while (p != NULL && p->adjVex != v2)
	    p = p->nextarc;

	if (p == NULL || p->nextarc == NULL)
		return -1;                   // 不存在下一个邻接点
	else
		return p->nextarc->adjVex;
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::InsertVex(const ElemType &d)
// 操作结果：在顶点表的表尾插入元素值为d的顶点。			 
{
	if (vexNum == vexMaxNum)
       throw Error("图的顶点数不能超过允许的最大数!");	// 抛出异常

	vexTable[vexNum].data = d;
	vexTable[vexNum].firstarc = NULL;
	tag[vexNum] = UNVISITED;
    vexNum++;
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::InsertArc(int v1, int v2, WeightType w)
// 操作结果：插入顶点为v1和v2,权为w的边			 
{
	if (v1 < 0 || v1 >= vexNum)
       throw Error("v1不合法!");	// 抛出异常
	if (v2 < 0 || v2 >= vexNum)
       throw Error("v2不合法!");	// 抛出异常
	if (v1 == v2)
       throw Error("v1不能等于v2!");// 抛出异常
	if (w == infinity)
       throw Error("w不能为无空大!");// 抛出异常
	//无向图插入前先判断一下该边是不是已经存在了
	if (ArcExists(v1, v2))
		return;

	vexTable[v1].firstarc = new AdjListNetworkArc<WeightType>(v2, w, vexTable[v1].firstarc);
	vexTable[v2].firstarc = new AdjListNetworkArc<WeightType>(v1, w, vexTable[v2].firstarc);

	arcNum++;
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::DeleteVex(const ElemType &d)
// 操作结果：删除元素值为d的顶点			 
{
	int v;

	for (v = 0; v < vexNum; v++)
		if (vexTable[v].data == d)
			break;

	if (v == vexNum)
		throw Error("图中不存在要删除的顶点!");

	while (vexTable[v].firstarc != NULL) {
		int u = vexTable[v].firstarc->adjVex;
		DeleteArc(v, u);
	}
	//删掉和v有关的所有边

	int last = vexNum - 1;
    //最后一个顶点序号

	if (v != last) {
		vexTable[v].data = vexTable[last].data;
		vexTable[v].firstarc = vexTable[last].firstarc;
		tag[v] = tag[last];

		vexTable[last].firstarc = NULL;
     //最后一个顶点直接移到v的位置

		for (int u = 0; u < last; u++) {
			AdjListNetworkArc<WeightType>* p = vexTable[u].firstarc;

			while (p != NULL) {
				if (p->adjVex == last)
					p->adjVex = v;//原来指向last的vex全部指到原来v的位置
				p = p->nextarc;
			}
		}
	}
	
	vexNum--;
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::DeleteArc(int v1, int v2)
// 操作结果：删除顶点为v1和v2的边			 
{
	if (v1 < 0 || v1 >= vexNum)
       throw Error("v1不合法!");	// 抛出异常
	if (v2 < 0 || v2 >= vexNum)
       throw Error("v2不合法!");	// 抛出异常
	if (v1 == v2) throw
       Error("v1不能等于v2!");		// 抛出异常

	if (DeleteArcNode(v1, v2))
	{
		DeleteArcNode(v2, v1);
		arcNum--;
	}
}

template <class ElemType, class WeightType>
WeightType AdjListDirNetwork<ElemType, WeightType>::GetWeight(int v1, int v2) const
// 操作结果：返回顶点为v1和v2的边的权值
{
	if (v1 < 0 || v1 >= vexNum)
       throw Error("v1不合法!");	// 抛出异常
	if (v2 < 0 || v2 >= vexNum)
       throw Error("v2不合法!");	// 抛出异常

	AdjListNetworkArc<WeightType> *p;
	p = vexTable[v1].firstarc;
	while (p != NULL && p->adjVex != v2) 
       p = p->nextarc;
    if (p != NULL)
	   return p->weight;			// 返回权值
	else
	   return infinity;				// 返回权值为infinity，表示边不存在
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::SetWeight(int v1, int v2, WeightType w)
// 操作结果：设置顶点为v1和v2的边的权值
{
	if (v1 < 0 || v1 >= vexNum)
       throw Error("v1不合法!");        // 抛出异常
	if (v2 < 0 || v2 >= vexNum)
       throw Error("v2不合法!");	    // 抛出异常
	if (v1 == v2)
       throw Error("v1不能等于v2!");    // 抛出异常
	if (w == infinity)
       throw Error("w不能为无空大!");   // 抛出异常

	AdjListNetworkArc<WeightType> *p;
	p = vexTable[v1].firstarc;
	while (p != NULL && p->adjVex != v2)
       p = p->nextarc;
    if (p != NULL)
	   p->weight = w;		            // 修改v1权值

	p = vexTable[v2].firstarc;
	while (p != NULL && p->adjVex != v1)//遍历到要改的v2->v1位置
		p = p->nextarc;
	if (p != NULL)
		p->weight = w;		            // 修改v2权值
}

template <class ElemType, class WeightType>
Status AdjListDirNetwork<ElemType, WeightType>::GetTag(int v) const
// 操作结果：返回顶点v的标志		 
{
	if (v < 0 || v >= vexNum)
       throw Error("v不合法!");		// 抛出异常

	return tag[v];
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::SetTag(int v, Status val) const
// 操作结果：设置顶点v的标志为val		 
{
	if (v < 0 || v >= vexNum)
       throw Error("v不合法!");		// 抛出异常

	tag[v] = val;
}

template <class ElemType, class WeightType>
AdjListDirNetwork<ElemType, WeightType>::AdjListDirNetwork(const AdjListDirNetwork<ElemType, WeightType> &copy)
// 操作结果：由有向网的邻接矩阵copy构造新有向网的邻接矩阵copy——复制构造函数
{
	AdjListNetworkArc<WeightType> *p, *q;
	infinity =copy.infinity;
	vexNum = copy.vexNum;
	vexMaxNum = copy.vexMaxNum;	
	arcNum = copy.arcNum;

	tag = new Status[vexMaxNum];
	vexTable = new AdjListNetWorkVex<ElemType, WeightType>[vexMaxNum];
	for (int v = 0; v < vexNum; v++)	{
		tag[v] = copy.tag[v];
		vexTable[v].data = copy.vexTable[v].data;
		vexTable[v].firstarc = NULL;
		p = copy.vexTable[v].firstarc;
		while (p != NULL) 
           if (vexTable[v].firstarc == NULL) {
              vexTable[v].firstarc = new AdjListNetworkArc<WeightType>(p->adjVex, p->weight);	
              q = vexTable[v].firstarc;
              p = p->nextarc;
           }
           else {   
              q->nextarc = new AdjListNetworkArc<WeightType>(p->adjVex, p->weight);	
              q = q->nextarc;
              p = p->nextarc;
           }   
	}
}

template <class ElemType, class WeightType>
AdjListDirNetwork<ElemType, WeightType> &AdjListDirNetwork<ElemType, WeightType>::operator =(const AdjListDirNetwork<ElemType, WeightType> &copy)
// 操作结果：将有向网的邻接矩阵copy赋值给当前有向网的邻接矩阵——赋值语句重载
{
	if (&copy != this)
	{
        Clear();                                    // 释放当前有向网边结点
	    delete []tag;								// 释放当前有向网标志数组 
	    delete []vexTable;							// 释放当前有向网顶点表 

	    AdjListNetworkArc<WeightType> *p, *q;
	    infinity =copy.infinity;
	    vexNum = copy.vexNum;
	    vexMaxNum = copy.vexMaxNum;	
	    arcNum = copy.arcNum;

	    tag = new Status[vexMaxNum];
	    vexTable = new AdjListNetWorkVex<ElemType, WeightType>[vexMaxNum];
	    for (int v = 0; v < vexNum; v++)	{
		   tag[v] = copy.tag[v];
		   vexTable[v].data = copy.vexTable[v].data;
		   vexTable[v].firstarc = NULL;
		   p = copy.vexTable[v].firstarc;
		   while (p != NULL) 
               if (vexTable[v].firstarc == NULL) {
                  vexTable[v].firstarc = new AdjListNetworkArc<WeightType>(p->adjVex, p->weight);	
                  q = vexTable[v].firstarc;
                  p = p->nextarc;
               }
               else {   
                  q->nextarc = new AdjListNetworkArc<WeightType>(p->adjVex, p->weight);	
                  q = q->nextarc;
                  p = p->nextarc;
               }   
	   }
    }
	return *this;
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::Display()
// 操作结果: 显示邻接矩阵无向网
{
	AdjListNetworkArc<WeightType> *p;
    cout << "无向网共有" << vexNum << "个顶点，" << arcNum << "条边。" << endl; 
	for (int v = 0; v < vexNum; v++)	{	// 显示第v个邻接链表
		cout << v << ":\t" << vexTable[v].data;				// 显示顶点号
	    p = vexTable[v].firstarc;
	    while (p != NULL) {
        	cout << "-->(" << p->adjVex << "," << p->weight << ")";
            p = p->nextarc; 
		}
		cout << endl; 
	}
}

template <class ElemType, class WeightType>
int AdjListDirNetwork<ElemType, WeightType>::Degree(int v) const
//2.求各个顶点的度
{
	if (v < 0 || v >= vexNum)
		throw Error("v不合法!");

	int count = 0;
	AdjListNetworkArc<WeightType>* p = vexTable[v].firstarc;

	while (p != NULL) {
		count++;
		p = p->nextarc;
	}

	return count;
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::DisplayDegree() const
//显示顶点度数
{
	ElemType e;

	for (int v = 0; v < vexNum; v++) {
		GetElem(v, e);
		cout << "顶点 " << e << " 的度为：" << Degree(v) << endl;
	}
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::DFS(int v) const
//深度优先遍历
{
	SetTag(v, VISITED);

	for (int u = FirstAdjVex(v); u >= 0; u = NextAdjVex(v, u)) {
		if (GetTag(u) == UNVISITED)
			DFS(u);
	}
}
template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::
     DFSRelationChains(int cur//当前正在访问的用户编号
	                 , int target//目标用户编号
	                 , bool visited[]
	                 , int path[]//保存正在尝试的路径
	                 , int pathLen//保存路径长度
	                 , int& chainCount//保存路径数目
                     ) const
//关系链DFS 从cur继续寻找target，输出所有简单路径
{
	visited[cur] = true;
	path[pathLen] = cur;//把cur作为路径里的一个节点
	pathLen++;
	//现在访问的节点是目标节点的话就直接输出一条关系链
	if (cur == target) {
		chainCount++;

		cout << "关系链 " << chainCount << "：";

		for (int i = 0; i < pathLen; i++) {
			ElemType user;
			GetElem(path[i], user);//循环获取用户名字输出

			cout << user;

			if (i != pathLen - 1) {
				cout << " -> ";
			}
		}

		cout << endl;
	}
	//现在访问的节点不是目标节点，递归找到目标节点
	else {
		//UX已经走不通置false，u仍为UX，下一个访问的节点为UX+1，不会重复访问进死循环
		for (int u = FirstAdjVex(cur); u >= 0; u = NextAdjVex(cur, u)) //找没访问过的节点
		{
			//u没在路径里出现过的话就深度遍历下去
			if (!visited[u]) {
				DFSRelationChains(u, target, visited, path, pathLen, chainCount);//cur换u，从u开始遍历，target不变
			}
		}
	}

	//UX走不通，跳回UX-1
	visited[cur] = false;
}
template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::
FindAllRelationChains(const ElemType& user1, const ElemType& user2) const
//关系链对外接口
//输出两个非好友用户之间的所有关系链
{
	ElemType temp1 = user1;
	ElemType temp2 = user2;

	int start = GetOrder(temp1);
	int target = GetOrder(temp2);
	//判断两个用户是否非好友
	if (start == -1 || target == -1) {
		cout << "用户不存在，无法查询关系链。" << endl;
		return;
	}

	if (start == target) {
		cout << "两个用户相同，不需要查询关系链。" << endl;
		return;
	}

	if (GetWeight(start, target) != infinity) {
		cout << user1 << " 和 " << user2 << " 已经是好友，不属于非好友关系链查询。" << endl;
		return;
	}

	bool* visited = new bool[vexNum];
	int* path = new int[vexNum];

	for (int i = 0; i < vexNum; i++) {
		visited[i] = false;
	}

	int chainCount = 0;

	cout << user1 << " 和 " << user2 << " 之间的所有关系链如下：" << endl;
	//调用DFS遍历关系链
	DFSRelationChains(start, target, visited, path, 0, chainCount);

	if (chainCount == 0) {
		cout << "没有找到关系链。" << endl;
	}
	else {
		cout << "共找到 " << chainCount << " 条关系链。" << endl;
	}

	delete[]visited;
	delete[]path;
}


template <class ElemType, class WeightType>
int AdjListDirNetwork<ElemType, WeightType>::ConnectedComponentNum() const
//求连通分量数

{
	for (int v = 0; v < vexNum; v++)
		SetTag(v, UNVISITED);

	int count = 0;

	for (int v = 0; v < vexNum; v++) {
		if (GetTag(v) == UNVISITED) {
			count++;
			DFS(v);//遍历v的连通分量全部打上VISITED
		}
	}

	return count;
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::DFSSmallGroup(
	int v,
	WeightType threshold,
	int group[],
	int& groupSize
) const
// 从顶点v出发，只沿着亲密度 >= threshold 的边进行DFS
{
	SetTag(v, VISITED);

	group[groupSize] = v;
	groupSize++;

	for (int u = FirstAdjVex(v); u >= 0; u = NextAdjVex(v, u)) {
		if (GetTag(u) == UNVISITED && GetWeight(v, u) >= threshold) {
			DFSSmallGroup(u, threshold, group, groupSize);
		}
	}
}//protected 带阈值的深度优先遍历，内部调用

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::FindSmallGroups(WeightType threshold) const
// 根据亲密度阈值发现小团体
{
	if (vexNum == 0) {
		cout << "当前社交网络为空。" << endl;
		return;
	}

	if (threshold <= 0) {
		cout << "亲密度阈值应大于0。" << endl;
		return;
	}
	/*for (int v = 0; v < vexNum; v++)
		SetTag(v, UNVISITED);

	int count = 0;

	for (int v = 0; v < vexNum; v++) {
		if (GetTag(v) == UNVISITED) {
			count++;
			DFS(v);
		}
	}*/ //原有的求连通分量函数
	for (int v = 0; v < vexNum; v++) {
		SetTag(v, UNVISITED);
	}

	int* group = new int[vexNum];//新增小团体成员存储数组
	int groupCount = 0;

	cout << "亲密度不低于 " << threshold << " 的小团体如下：" << endl;

	for (int v = 0; v < vexNum; v++) {
		if (GetTag(v) == UNVISITED) {
			int groupSize = 0;

			DFSSmallGroup(v, threshold, group, groupSize);

			// 一个人不能算小团体，所以只输出人数大于1的连通分量
			if (groupSize > 1) {
				groupCount++;

				cout << "小团体 " << groupCount << "：";

				for (int i = 0; i < groupSize; i++) {
					ElemType user;
					GetElem(group[i], user);

					cout << user;

					if (i != groupSize - 1) {
						cout << "、";
					}
				}

				cout << "，人数：" << groupSize << endl;
			}
		}
	}

	if (groupCount == 0) {
		cout << "没有发现满足该亲密度阈值的小团体。" << endl;
	}

	delete[] group;
}//外层调用

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::FindHamiltonPaths(int maxCount) const
// 挑战性问题1 public接口
{
	if (vexNum == 0) {
		cout << "当前社交网络为空。" << endl;
		return;
	}

	if (vexNum == 1) {
		ElemType user;
		GetElem(0, user);
		cout << "哈密顿通路1：" << user << endl;
		return;
	}

	// 如果图本身不连通，一定不存在哈密顿路径
	if (ConnectedComponentNum() != 1) {
		cout << "当前社交网络不连通，不存在经过所有用户一次的通路。" << endl;
		return;
	}

	bool* visited = new bool[vexNum];
	int* path = new int[vexNum];

	int pathCount = 0;

	cout << "经过所有用户且每个用户只经过一次的通路：" << endl;

	for (int start = 0; start < vexNum; start++) {
		for (int i = 0; i < vexNum; i++) {
			visited[i] = false;
		}

		DFSHamiltonPath(start, visited, path, 0, pathCount, maxCount);

		if (maxCount > 0 && pathCount >= maxCount) {
			break;
		}
	}

	if (pathCount == 0) {
		cout << "没有找到符合条件的通路。" << endl;
	}
	else if (maxCount > 0 && pathCount >= maxCount) {
		cout << "已达到输出上限，共输出 " << pathCount << " 条通路。" << endl;
	}
	else {
		cout << "共找到 " << pathCount << " 条路径。" << endl;
	}

	delete[] visited;
	delete[] path;
}

//挑战性问题1 protected工具函数
template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::DFSHamiltonPath(
	int cur,
	bool visited[],
	int path[],
	int pathLen,
	int& pathCount,
	int maxCount
) const
// 从cur继续搜索哈密顿路径
{
	visited[cur] = true;
	path[pathLen] = cur;
	pathLen++;

	if (pathLen == vexNum) {
		// 因为是无向图所以只输出 path[0] < path[vexNum - 1] 的情况
		if (path[0] < path[vexNum - 1]) {
			pathCount++;

			cout << "哈密顿通路 " << pathCount << "：";

			for (int i = 0; i < pathLen; i++) {
				ElemType user;
				GetElem(path[i], user);

				cout << user;

				if (i != pathLen - 1) {
					cout << " -> ";
				}
			}

			cout << endl;
		}
	}
	else {
		for (int u = FirstAdjVex(cur); u >= 0; u = NextAdjVex(cur, u)) {
			if (!visited[u]) {
				DFSHamiltonPath(u, visited, path, pathLen, pathCount, maxCount);

				if (maxCount > 0 && pathCount >= maxCount) {
					break;
				}
			}
		}
	}

	visited[cur] = false;
}

template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::FindFastestHamiltonPath(int maxCount) const
// 挑战性问题2：寻找信息传播速度最快的全用户路径
{
	if (vexNum == 0) {
		cout << "当前社交网络为空。" << endl;
		return;
	}

	if (vexNum == 1) {
		ElemType user;
		GetElem(0, user);

		cout << "最快通路：" << user << endl;
		cout << "总传播权重：0" << endl;
		return;
	}

	if (ConnectedComponentNum() != 1) {
		cout << "当前社交网络不连通，不存在经过所有用户一次的通路。" << endl;
		return;
	}

	bool* visited = new bool[vexNum];
	int* path = new int[vexNum];
	int* bestPath = new int[vexNum];

	double bestTime = 1e100;
	bool found = false;
	int checkedCount = 0;

	for (int start = 0; start < vexNum; start++) {
		for (int i = 0; i < vexNum; i++) {
			visited[i] = false;
		}

		DFSFastestHamiltonPath(
			start,
			visited,
			path,
			0,
			0.0,
			bestTime,
			bestPath,
			found,
			checkedCount,
			maxCount
		);

		if (maxCount > 0 && checkedCount >= maxCount) {
			break;
		}
	}

	if (!found) {
		cout << "没有找到经过所有用户且每个用户只经过一次的通路" << endl;
	}
	else {
		if (maxCount > 0 && checkedCount >= maxCount) {
			cout << "已达到检查上限，共检查 " << checkedCount << " 条哈密顿通路。" << endl;
			cout << "以下结果是在已检查通路中的最快路径：" << endl;
		}
		else {
			cout << "共检查 " << checkedCount << " 条哈密顿通路。" << endl;
			cout << "信息传播速度最快的通路为：" << endl;
		}

		for (int i = 0; i < vexNum; i++) {
			ElemType user;
			GetElem(bestPath[i], user);

			cout << user;

			if (i != vexNum - 1) {
				cout << " -> ";
			}
		}

		cout << endl;
		cout << "总传播权重：" << bestTime << endl;
	}

	delete[] visited;
	delete[] path;
	delete[] bestPath;
}
//挑战性问题2工具函数
template <class ElemType, class WeightType>
void AdjListDirNetwork<ElemType, WeightType>::DFSFastestHamiltonPath(
	int cur,
	bool visited[],
	int path[],
	int pathLen,
	double curTime,
	double& bestTime,
	int bestPath[],
	bool& found,
	int& checkedCount,
	int maxCount
) const
// 从cur继续搜索哈密顿路径，并在限定检查数量内记录传播时间最短的一条
{
	if (maxCount > 0 && checkedCount >= maxCount) {
		return;
	}

	visited[cur] = true;
	path[pathLen] = cur;
	pathLen++;

	// 剪枝：当前累计传播时间已经不可能优于当前最优结果
	if (found && curTime >= bestTime) {
		visited[cur] = false;
		return;
	}

	if (pathLen == vexNum) {
		// 无向图中 A->B->C 和 C->B->A 本质相同
		// 只统计 path[0] < path[vexNum - 1] 的方向，避免正反重复
		if (path[0] < path[vexNum - 1]) {
			checkedCount++;

			if (!found || curTime < bestTime) {
				found = true;
				bestTime = curTime;

				for (int i = 0; i < vexNum; i++) {
					bestPath[i] = path[i];
				}
			}
		}
	}
	else {
		for (int u = FirstAdjVex(cur); u >= 0; u = NextAdjVex(cur, u)) {
			if (maxCount > 0 && checkedCount >= maxCount) {
				break;
			}

			if (!visited[u]) {
				WeightType intimacy = GetWeight(cur, u);

				if (intimacy > 0 && intimacy != infinity) {
					double edgeTime = 1.0 / (double)intimacy;

					DFSFastestHamiltonPath(
						u,
						visited,
						path,
						pathLen,
						curTime + edgeTime,
						bestTime,
						bestPath,
						found,
						checkedCount,
						maxCount
					);
				}
			}
		}
	}

	visited[cur] = false;
}
#endif
