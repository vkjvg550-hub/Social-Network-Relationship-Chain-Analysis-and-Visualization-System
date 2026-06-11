#ifndef __SHORTEST_PATH_DIJ_H__
#define __SHORTEST_PATH_DIJ_H__

#include "AdjListDirNetwork.h"			// 有向网邻接表 
#include "LinkStack.h"		            // 链式栈

template <class ElemType, class WeightType>
void DisplayPathAndDist(const AdjListDirNetwork<ElemType, WeightType> &g, int v0, int *path, WeightType *dist)
{
    ElemType e1, e2;
	WeightType infinity = g.GetInfinity();
    g.GetElem(v0, e1);
	for (int v1 = 0; v1 < g.GetVexNum(); v1++)	{
       cout << endl;
       g.GetElem(v1, e2);
       if (v1 != v0) 
	      if (dist[v1] < infinity )		{	// 存在从v0到v1最短路径
			 cout << "从顶点 " << e1 << " 到 " << e2 << ":";
			 cout << " 路径长度 = " << dist[v1];
			 cout << " 路径:";
			 LinkStack<int> s;	// 存储路上的顶点
			 int v = v1;
			 while (v != v0)	{
				s.Push(v);	// v为路径上的顶点
				v = path[v];// 取上路径上的前一个顶点
			 }
			 s.Push(v0);		// v是路径上起始点
			 while (!s.IsEmpty())			{	// 输出路上的顶点
				s.Pop(v);
                g.GetElem(v, e2);
				cout << e2 << " ";
			 }
		 }
	 	 else 	// 从v0到v1无路径
			cout << "从顶点 " << e1 << " 到 "  << e2 << "无路径";
   }
   cout << endl;
}


template <class ElemType, class WeightType>
void ShortestPathDij(const AdjListDirNetwork<ElemType, WeightType> &g, int v0, 
	int *path, WeightType *dist)
// 操作结果: 用Dijkstra算法求有向网g从顶点v0到其余顶点v的最短路径path和路径长度dist[v],
//	path[v]存储最短路径上终点的前一顶点的顶点号
{
	WeightType minVal, infinity = g.GetInfinity();
    int v, u;
	for (v = 0; v < g.GetVexNum(); v++){	// 初始化path和dist及顶点标志
 	    dist[v] = (v0 != v) ? g.GetWeight(v0, v) : 0;
   	    if (dist[v] == infinity)
            path[v] = -1;
        else 
            path[v] = v0;
		g.SetTag(v, UNVISITED);	// 置顶点标志
	}
	g.SetTag(v0, VISITED);		// U={v0}

	for (int i = 1; i < g.GetVexNum(); i++){	// 求g.GetVexNum() - 1个顶点的最短路径  
		minVal = infinity;
        u = v0;
		for (v = 0; v < g.GetVexNum(); v++)   // 查找最小的最短路径 
			if (g.GetTag(v) == UNVISITED && dist[v] < minVal)		{
            	// g.GetTag(v) == UNVISITED表示v∈V - U
				u = v;
				minVal = dist[v];
			}
		g.SetTag(u, VISITED);	// 将u并入U

		for (v = g.FirstAdjVex(u); v != -1; v = g.NextAdjVex(u, v))
			if (g.GetTag(v) == UNVISITED && minVal + g.GetWeight(u, v) < dist[v])	{
               	// 如v∈V - U且minVal + g.GetWeight(u, v) < dist[v],则修改dist[v]及path[v]
				dist[v] = minVal + g.GetWeight(u, v);
				path[v] = u;
			}
	}
}



template <class ElemType, class WeightType>
void SpreadPathDij(
	const AdjListDirNetwork<ElemType, WeightType>& g,
	int v0,
	int* path,
	double* dist
)
// 操作结果：用Dijkstra算法求从v0出发到其他用户的最短传播时间
// path[v]存储传播路径上v的前一个顶点
// dist[v]存储从v0到v的最短传播时间
{
	const double SPREAD_INFINITY = 1e100;

	double minVal;
	int v, u;

	// 初始化path、dist和顶点标志
	for (v = 0; v < g.GetVexNum(); v++) {
		if (v0 == v) {
			dist[v] = 0.0;
			path[v] = v0;
		}
		else {
			WeightType w = g.GetWeight(v0, v);

			// 如果v0和v之间有边，则传播权重 = 1 / 亲密度
			if (w != g.GetInfinity() && w > 0) {
				dist[v] = 1.0 / (double)w;
				path[v] = v0;
			}
			else {
				dist[v] = SPREAD_INFINITY;
				path[v] = -1;
			}
		}

		g.SetTag(v, UNVISITED);
	}

	g.SetTag(v0, VISITED);		// U={v0}

	// 求v0到其余顶点的最短传播时间
	for (int i = 1; i < g.GetVexNum(); i++) {
		minVal = SPREAD_INFINITY;
		u = -1;

		// 从未访问顶点中找dist最小的顶点u
		for (v = 0; v < g.GetVexNum(); v++) {
			if (g.GetTag(v) == UNVISITED && dist[v] < minVal) {
				u = v;
				minVal = dist[v];
			}
		}

		// 如果u == -1，说明剩下的顶点都不可达
		if (u == -1)
			break;

		g.SetTag(u, VISITED);	// 将u并入U

		// 遍历u的所有邻接点，尝试用u更新它们的传播时间
		for (v = g.FirstAdjVex(u); v != -1; v = g.NextAdjVex(u, v)) {
			if (g.GetTag(v) == UNVISITED) {
				WeightType w = g.GetWeight(u, v);

				if (w != g.GetInfinity() && w > 0) {
					double spreadTime = 1.0 / (double)w;

					if (minVal + spreadTime < dist[v]) {
						dist[v] = minVal + spreadTime;
						path[v] = u;
					}
				}
			}
		}
	}
}
template <class ElemType, class WeightType>
double GetBestSpreadTreeIntimacySum(
	const AdjListDirNetwork<ElemType, WeightType>& g,
	int source,
	int* path,
	double* dist,
	double spreadInfinity
)
// 根据Dijkstra得到的path[]，计算最优传播路径树中的亲密度之和
{
	double intimacySum = 0.0;

	for (int v = 0; v < g.GetVexNum(); v++) {
		if (v == source)
			continue;

		// 如果不可达，返回-1表示无效
		if (dist[v] >= spreadInfinity / 2 || path[v] == -1) {
			return -1;
		}

		// path[v] 是 v 在最优传播路径上的前一个顶点
		WeightType intimacy = g.GetWeight(path[v], v);

		if (intimacy == g.GetInfinity()) {
			return -1;
		}

		intimacySum += (double)intimacy;
	}

	return intimacySum;
}

template <class ElemType, class WeightType>
void FindBestSpreadUserDij(const AdjListDirNetwork<ElemType, WeightType>& g)
// 操作结果：遍历每个用户作为初始投放点，输出信息传播最快的初始投放点
{
	if (g.GetVexNum() == 0) {
		cout << "当前社交网络为空。" << endl;
		return;
	}

	const double SPREAD_INFINITY = 1e100;
	const double EPS = 1e-9;

	int vexNum = g.GetVexNum();

	int* path = new int[vexNum];
	double* dist = new double[vexNum];

	double bestMaxTime = SPREAD_INFINITY;
	double bestSumTime = SPREAD_INFINITY;

	cout << endl;
	cout << "各用户作为信息初始投放点的传播情况：" << endl;

	// 第一遍：找出最优的 maxTime 和 sumTime
	for (int s = 0; s < vexNum; s++) {
		SpreadPathDij(g, s, path, dist);

		double maxTime = 0.0;
		double sumTime = 0.0;
		bool canReachAll = true;

		for (int i = 0; i < vexNum; i++) {
			if (dist[i] >= SPREAD_INFINITY / 2) {
				canReachAll = false;
				break;
			}

			if (dist[i] > maxTime)
				maxTime = dist[i];

			sumTime += dist[i];
		}

		ElemType user;
		g.GetElem(s, user);

		cout << "用户 " << user << "：";

		if (!canReachAll) {
			cout << "无法传播到所有用户" << endl;
			continue;
		}

		cout << "最慢传播权重 = " << maxTime
			<< "，总传播权重 = " << sumTime << endl;



		// 评价标准：
		// 1.优先选择最慢传播权重最小的用户
		// 2.如果最慢传播权重相同，再选择总传播权重最小的用户
		if (maxTime < bestMaxTime - EPS ||
			(fabs(maxTime - bestMaxTime) <= EPS && sumTime < bestSumTime)) {
			bestMaxTime = maxTime;
			bestSumTime = sumTime;
		}
	}

	if (bestMaxTime >= SPREAD_INFINITY / 2) {
		cout << endl;
		cout << "当前社交网络不连通，没有用户可以传播到所有用户。" << endl;

		delete[] path;
		delete[] dist;
		return;
	}

	// 第二遍：输出所有并列最优的用户
	cout << endl;
	cout << "推荐的信息初始投放点：" << endl;

	for (int s = 0; s < vexNum; s++) {
		SpreadPathDij(g, s, path, dist);

		double maxTime = 0.0;
		double sumTime = 0.0;
		bool canReachAll = true;

		for (int i = 0; i < vexNum; i++) {
			if (dist[i] >= SPREAD_INFINITY / 2) {
				canReachAll = false;
				break;
			}

			if (dist[i] > maxTime)
				maxTime = dist[i];

			sumTime += dist[i];
		}

		if (canReachAll &&
			fabs(maxTime - bestMaxTime) <= EPS &&
			fabs(sumTime - bestSumTime) <= EPS) {
			ElemType user;
			g.GetElem(s, user);

			/*cout << "用户 " << user
				<< "，最慢传播权重 = " << maxTime
				<< "，总传播权重 = " << sumTime << endl;*/
			double intimacySum = GetBestSpreadTreeIntimacySum(
				g, s, path, dist, SPREAD_INFINITY
			);

			cout << "用户 " << user
				<< "，最慢传播权重 = " << maxTime
				<< "，总传播权重 = " << sumTime
				<< "，最优传播通路亲密度之和 = " << intimacySum << endl;
		}
	}

	delete[] path;
	delete[] dist;
}

#endif

