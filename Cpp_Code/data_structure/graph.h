#ifndef __GRAPH_H__
#define __GRAPH_H__

#define INF (0x7F7F7F7F)

// 16 前8位存储权重信息 | 后8位存储索引
#define WEIGHT(i) ((i&0xff00)>>8)
#define INDEX(i) (i&0xff)
#define MAKE(w, i) ((((w)&0xff)<<8) | i)

typedef vector<list<int>> GraphArc;
typedef string VexType;
typedef struct Vec {
    int idx;        //顶点索引
    VexType data;   //顶点信息
} GVex;

typedef pair<VexType, VexType> GArc;

typedef bool (*VisitFunc)(GVex &vex);

typedef vector< GVex > GraphVex;

/*
    应该提供哪些接口？ 有向图 无向图 权重图
    构造图
    插入边 删除边 顶点之间关系
    插入点 删除点 顶点相关的边信息也要删除
    修改权重信息
    获取节点信息
    获取最短路径
    遍历
    获取边信息
    有向图 度 信息
*/

class CGraph {
public:
    CGraph(bool direct = true) {}
    void CreateGraph(vector<VexType> &vex, vector< pair<VexType, VexType> > &arc, 
        const vector<int> &weights = vector<int>())
    {
        auto i = 0;
        for (auto iter = vex.begin(); iter != vex.end(); iter++, i++) {
            _graph_vex.push_back({i, *iter});
            _vex_idx.insert(make_pair(*iter, i));
        }
        for(auto j = 0; j < i; j++) {
            _graph_arc.push_back(list<int>());
        }
        bool has = weights.size() > 0;
        for (size_t i = 0; i < arc.size(); i++) {
            InsertArc(arc[i], has ? weights[i] : 0);
        }
    }

    inline size_t VexSize() {return _graph_vex.size();};

    //根据顶点信息查找位置
    int LocateVex(VexType &v);

    //根据位置查找顶点信息
    VexType GetVex(int idx);
    /*
        用于遍历某顶点的邻接点，
        此时用 vector<list<int>> 存储的邻接信息
        所以不需要实现该接口
        VexBegin();
        VexNext();
    */

   //修改idx的顶点的值
   bool PutVex(int idx, int value);

   // 往图中添加顶点
    bool InsertVex(VexType &v) {
        if (-1 != _get_idx(v))
            return false;
        int i = (int)_graph_vex.size();
        _graph_vex.push_back({i, v});
        _vex_idx.insert(make_pair(v, i));
        return true;
    }

    // 往图中删除顶点 除了删除顶点还需要删除相关的边信息
    bool DeleteVex(VexType &v) {
        int idx = _get_idx(v);
        if(-1 == idx)
            return false;
        auto &vexlist = _graph_arc[idx];
        for(auto iter = vexlist.begin(); iter != vexlist.end(); iter++) {
            _graph_arc[*iter].remove(idx);
        }
        _graph_arc[idx].clear();
        _graph_vex[idx].data.clear();
        _vex_idx.erase(v);
        return true;
    } 

    // 往图中添加一条边<'A', 'B'>
    bool InsertArc(const GArc &arc, int weight = 0) {
        int iFirst = _get_idx(arc.first);
        if (-1 == iFirst)
            return false;
        int iSecond = _get_idx(arc.second);
        if (-1 == iSecond)
            return false;
        int wFirst = MAKE(weight, iFirst);
        int wSecond = MAKE(weight, iSecond);
        auto &vexlist = _graph_arc[iFirst];
        if(find(vexlist.begin(), vexlist.end(), wSecond) != vexlist.end())
            return false;
        vexlist.push_back(wSecond); //不久存储了连接情况，wSecond里面还有权重信息
        if(!_direct) { //无向图 <'B', 'A'>
            auto &revlist = _graph_arc[iSecond];
            if(find(revlist.begin(), revlist.end(), wFirst) != revlist.end())
                return false;
            revlist.push_back(wFirst);
        }
        return true;
    }

    // 往图中删除一条边
    bool DeleteArc(GArc &arc) {
        int iFirst = _get_idx(arc.first);
        if (-1 == iFirst)
            return false;
        int iSecond = _get_idx(arc.second);
        if (-1 == iSecond)
            return false;
        
        auto &vexlist = _graph_arc[iFirst];
        
        bool has = false;
        auto iter = vexlist.begin();
        for(; iter != vexlist.end(); iter++) {
            if(INDEX(*iter) == iSecond) {
                has = true;
                break;
            }
        }

        if(!has)
            return false;
        vexlist.erase(iter);

        if (!_direct) {
            has = false;
            auto &revlist = _graph_arc[iSecond];
            auto iter = revlist.begin();
            for(; iter != revlist.end(); iter++) {
                if(INDEX(*iter) == iFirst) {
                    has = true;
                    break;
                }
            }
            if(!has)
                return false;
            revlist.erase(iter);
        }
        return true;
    }

    // 对图进行深度有限遍历（不能遍历删除）
    void DFSTraverse(vector<bool> &visted, VisitFunc func) {
        for (size_t i = 0; i < _graph_vex.size(); i++) { //有多个
            if(!visted[i]) {
                _dfs_traverse(i, visted, func);
            }
        }
    }

    // 对图进行广度优先遍历（不能遍历删除）
    void BFSTraverse(vector<bool> &visted, VisitFunc func) {
        for (size_t i = 0; i < _graph_vex.size(); i++) {
            if(!visted[i]) {
                _bfs_traverse(i, visted, func);
            }
        }
    }

    //Dijkstra算法:最短路径算法
    void Dijkstra(VexType s, vector<bool> &visted, vector<int> &dis, vector<int> &parent) {
        int iStart = _get_idx(s);
        if(-1 == iStart) {
            cout << "start not exist" << endl;
            return;
        }

        visted[iStart] = true;
        dis[iStart] = 0;
        parent[iStart] = iStart;

        int i = iStart;
        while(true) {
            auto &arclist = _graph_arc[i];
            int min = INF, midx = -1;
            for(auto iter = arclist.begin(); iter != arclist.end(); iter++) {
                int idx = INDEX(*iter);
                int wgt = WEIGHT(*iter);
                if(dis[idx] > wgt + dis[i]) {
                    dis[idx] = wgt + dis[i];
                    parent[idx] = i;
                }
            }

            //扫描
            for(size_t j = 0; j < dis.size(); j++) {
                if(!visted[j] && min > dis[j]) {
                    min = dis[j];
                    midx = j;
                }
            }

            if(midx == -1)
                break;

            //重新选取点
            visted[midx] = true;
            i = midx;
        }
    }

private:
    /*
        回溯 函数堆栈来实现
        实际开发过程 图纵深很深 不使用递归 除非是尾递归
        所有的递归 都能转换为递推
    */
    void _dfs_traverse(int idx, vector<bool> &visted, VisitFunc func) {
        if (visted[idx]) return;
        visted[idx] = true;
        func(_graph_vex[idx]);
        auto &arclist = _graph_arc[idx];
        for (auto iter = arclist.begin(); iter != arclist.end(); iter++) {
            int next = INDEX(*iter);
            if (!visted[next]) {
                _dfs_traverse(next, visted, func);
            }
        }
    }

    void _bfs_traverse(int idx, vector<bool> &visted, VisitFunc func){
        deque<int> queue;
        queue.push_back(idx);
        visted[idx] = true;
        func(_graph_vex[idx]);
        while (0 != queue.size()) {
            int i = queue.front();
            queue.pop_front();
            auto &arclist = _graph_arc[idx];
            for (auto iter = arclist.begin(); iter != arclist.end(); iter++) {
                int next = INDEX(*iter);
                if(!visted[next]) {
                    queue.push_back(next);
                    visted[next] = true;
                    func(_graph_vex[next]);
                }
            }
        }
    }

    int _get_idx(const VexType &v) {
        auto iter = _vex_idx.find(v);
        if (iter == _vex_idx.end()) return -1;
        return iter->second;
    }

private:
    GraphArc _graph_arc; //连接表
    GraphVex _graph_vex; //顶点集合

    map<VexType, int> _vex_idx; // 数据对应 idx
    bool _direct;               // 是否为有向图
};

#endif