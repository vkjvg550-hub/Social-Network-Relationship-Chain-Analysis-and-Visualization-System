#include "AdjListDirNetwork.h"
#include "ShortestPathDij.h"
#include <string>
#include <fstream>
#include <limits>
using namespace std;

// 查找用户序号
// 现在的 GetOrder(ElemType &d) 参数不是 const 引用，所以用一个临时变量 temp 来传入。
int FindUser(const AdjListDirNetwork<string, int>& net, const string& user)
{
    string temp = user;
    return net.GetOrder(temp);
}

// 如果用户不存在，就自动插入用户；返回该用户的序号
int EnsureUser(AdjListDirNetwork<string, int>& net, const string& user)
{
    int pos = FindUser(net, user);

    if (pos == -1) {
        net.InsertVex(user);
        pos = FindUser(net, user);
    }

    return pos;
}

// 只读取文件第一行，得到最大用户数量
bool ReadMaxUserNum(const string& fileName, int& maxUserNum)
{
    ifstream fin(fileName.c_str());

    if (!fin) {
        cout << "无法打开文件：" << fileName << endl;
        return false;
    }

    if (!(fin >> maxUserNum)) {
        cout << "文件格式错误：第一行应为最大用户数量。" << endl;
        return false;
    }

    if (maxUserNum <= 0) {
        cout << "文件格式错误：最大用户数量必须大于0。" << endl;
        return false;
    }

    fin.close();
    return true;
}

// 从 friend_7.txt 读取社交网络
bool LoadSocialNetworkFromFile(
    const string& fileName,
    AdjListDirNetwork<string, int>& net,
    int& maxUserNum
)
{
    ifstream fin(fileName.c_str());

    if (!fin) {
        cout << "无法打开文件：" << fileName << endl;
        return false;
    }

    int fileMaxUserNum;
    fin >> fileMaxUserNum;
    maxUserNum = fileMaxUserNum;

    string user1, user2;
    int weight;

    int edgeCount = 0;

    while (fin >> user1 >> user2 >> weight) {
        int v1 = EnsureUser(net, user1);
        int v2 = EnsureUser(net, user2);

        net.InsertArc(v1, v2, weight);
        edgeCount++;
    }

    fin.close();

    cout << "读取完成：" << endl;
    cout << "最大用户数量：" << maxUserNum << endl;
    cout << "当前用户数量：" << net.GetVexNum() << endl;
    cout << "当前好友关系数：" << net.GetArcNum() << endl;

    return true;
}

// 显示所有用户
void DisplayAllUsers(AdjListDirNetwork<string, int>& net)
{
    if (net.IsEmpty()) {
        cout << "当前社交网络为空。" << endl;
        return;
    }

    cout << "当前用户列表：" << endl;

    for (int i = 0; i < net.GetVexNum(); i++) {
        string user;
        net.GetElem(i, user);
        cout << i << " : " << user << endl;
    }
}

// 保存社交网络到文件
// 输出格式与输入文件一致：
// 第一行：最大用户数量
// 后续每行：用户1 用户2 亲密度
void SaveSocialNetworkToFile(
    const string& fileName,
    const AdjListDirNetwork<string, int>& net,
    int maxUserNum
)
{
    ofstream fout(fileName.c_str());

    if (!fout) {
        cout << "无法创建输出文件：" << fileName << endl;
        return;
    }

    fout << maxUserNum << endl;

    for (int v = 0; v < net.GetVexNum(); v++) {
        for (int u = net.FirstAdjVex(v); u >= 0; u = net.NextAdjVex(v, u)) {
            // 无向图中一条边会存两次，这里只输出 v < u 的那一次
            if (v < u) {
                string user1, user2;
                net.GetElem(v, user1);
                net.GetElem(u, user2);

                fout << user1 << " " << user2 << " " << net.GetWeight(v, u) << endl;
            }
        }
    }

    fout.close();

    cout << "保存成功，输出文件为：" << fileName << endl;
}

// 显示菜单
void ShowMenu()
{
    cout << endl;
    cout << "1. 显示社交网络邻接表" << endl;
    cout << "2. 显示所有用户" << endl;
    cout << "3. 增加用户" << endl;
    cout << "4. 删除用户" << endl;
    cout << "5. 增加好友关系" << endl;
    cout << "6. 删除好友关系" << endl;
    cout << "7. 修改好友亲密度" << endl;
    cout << "8. 输出所有用户的度" << endl;
    cout << "9. 输出连通分量数量" << endl;
    cout << "10. 保存当前社交网络到文件" << endl;
    cout << "11. 查询两个非好友之间的所有关系链" << endl;
    cout << "12. 根据亲密度阈值发现小团体" << endl;
    cout << "13. 输出信息最快初始投放点" << endl;
    cout << "14. 挑战性问题1：求哈密顿通路" << endl;
    cout << "15. 挑战性问题2：输出信息传播速度最快的全用户路径" <<endl;
    cout << "0. 退出" << endl;
    cout << "请选择功能：";
}

int main()
{
    try {
        string inputFileName = "friend_7.txt";
        string outputFileName = "social_output.txt";

        int maxUserNum = 0;

        if (!ReadMaxUserNum(inputFileName, maxUserNum)) {
            return 1;
        }

        AdjListDirNetwork<string, int> net(maxUserNum, DEFAULT_INFINITY);

        if (!LoadSocialNetworkFromFile(inputFileName, net, maxUserNum)) {
            return 1;
        }

        int choice = -1;

        while (choice != 0) {
            ShowMenu();
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "输入无效，请重新输入数字。" << endl;
                continue;
            }

            switch (choice) {
            case 1:
            {
                if (net.IsEmpty()) {
                    cout << "当前社交网络为空。" << endl;
                }
                else {
                    net.Display();
                }
                break;
            }

            case 2:
            {
                DisplayAllUsers(net);
                break;
            }

            case 3:
            {
                string user;
                cout << "请输入要增加的用户编号，例如 U31：";
                cin >> user;

                if (FindUser(net, user) != -1) {
                    cout << "用户 " << user << " 已存在。" << endl;
                }
                else {
                    net.InsertVex(user);
                    cout << "用户 " << user << " 增加成功。" << endl;
                }

                break;
            }

            case 4:
            {
                string user;
                cout << "请输入要删除的用户编号：";
                cin >> user;

                if (FindUser(net, user) == -1) {
                    cout << "用户 " << user << " 不存在。" << endl;
                }
                else {
                    net.DeleteVex(user);
                    cout << "用户 " << user << " 删除成功。" << endl;
                }

                break;
            }

            case 5:
            {
                string user1, user2;
                int weight;

                cout << "请输入两个用户编号和亲密度，例如 U1 U31 80：";
                cin >> user1 >> user2 >> weight;

                if (user1 == user2) {
                    cout << "不能给同一个用户添加好友关系。" << endl;
                    break;
                }

                if (weight <= 0) {
                    cout << "亲密度必须大于0。" << endl;
                    break;
                }

                int v1 = EnsureUser(net, user1);
                int v2 = EnsureUser(net, user2);

                if (net.GetWeight(v1, v2) != net.GetInfinity()) {
                    cout << user1 << " 和 " << user2 << " 已经是好友。" << endl;
                }
                else {
                    net.InsertArc(v1, v2, weight);
                    cout << "好友关系增加成功。" << endl;
                }

                break;
            }

            case 6:
            {
                string user1, user2;

                cout << "请输入要删除好友关系的两个用户编号，例如 U1 U2：";
                cin >> user1 >> user2;

                int v1 = FindUser(net, user1);
                int v2 = FindUser(net, user2);

                if (v1 == -1 || v2 == -1) {
                    cout << "用户不存在，无法删除好友关系。" << endl;
                    break;
                }

                if (net.GetWeight(v1, v2) == net.GetInfinity()) {
                    cout << user1 << " 和 " << user2 << " 之间不存在好友关系。" << endl;
                    break;
                }

                net.DeleteArc(v1, v2);
                cout << "好友关系删除成功。" << endl;

                break;
            }

            case 7:
            {
                string user1, user2;
                int weight;

                cout << "请输入两个用户编号和新的亲密度，例如 U1 U2 90：";
                cin >> user1 >> user2 >> weight;

                if (weight <= 0) {
                    cout << "亲密度必须大于0。" << endl;
                    break;
                }

                int v1 = FindUser(net, user1);
                int v2 = FindUser(net, user2);

                if (v1 == -1 || v2 == -1) {
                    cout << "用户不存在，无法修改亲密度。" << endl;
                    break;
                }

                if (net.GetWeight(v1, v2) == net.GetInfinity()) {
                    cout << user1 << " 和 " << user2 << " 不是好友，无法修改亲密度。" << endl;
                    break;
                }

                net.SetWeight(v1, v2, weight);
                cout << "亲密度修改成功。" << endl;

                break;
            }

            case 8:
            {
                net.DisplayDegree();
                break;
            }

            case 9:
            {
                cout << "当前社交网络的连通分量数量为："
                    << net.ConnectedComponentNum() << endl;
                break;
            }

            case 10:
            {
                SaveSocialNetworkToFile(outputFileName, net, maxUserNum);
                break;
            }

            case 11:
            {
                string user1, user2;
                cout << "请输入两个非好友用户编号，例如 U1 U10：";
                cin >> user1 >> user2;

                net.FindAllRelationChains(user1, user2);

                break;
            }

            case 12:
            {
                int threshold;
                cout << "请输入亲密度阈值：";
                cin >> threshold;

                net.FindSmallGroups(threshold);

                break;
            }

            case 13:
            {
                FindBestSpreadUserDij(net);
                break;
            }

            case 14:
            {
                int maxCount=30;//防止路径太多刷屏

                net.FindHamiltonPaths(maxCount);

                break;
            }

            case 15:
            {
                int maxCount = 20;
                net.FindFastestHamiltonPath(maxCount);
                break;
            }

            case 0:
            {
                cout << "系统退出。" << endl;
                break;
            }

            default:
            {
                cout << "没有这个功能，请重新选择。" << endl;
                break;
            }
            }
        }
    }
    catch (Error err) {
        err.Show();
    }

    system("PAUSE");
    return 0;
}