#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <deque>
#include <algorithm>
#include <string>
using namespace std;

#include "graph.h"

bool print_node(GVex &vex) {
    cout << vex.data << " -> ";
    return true;
}

int main()
{
    CGraph graph(false);
    vector<VexType> vex = {"A","B","C","D","E","F"};
    vector<pair<VexType, VexType>> arc = {
            {"A","B"},
            {"A","C"},
            {"A","D"},
            {"B","C"},
            {"B","F"},
            {"C","E"},
            {"E","F"},
            {"E","D"},
        };

    vector<int> weights = {//权重表需要与边集合一一对应
            7,
            9,
            11,
            8,
            20,
            6,
            1,
            3,
        };
    graph.CreateGraph(vex, arc, weights);
    {
        size_t size = graph.VexSize();
        vector<bool> visited(size, false);
        vector<int> dis(size, INF);
        vector<int> parent(size, -1);
        graph.Dijkstra("A", visited, dis, parent);
        for(size_t i = 0; i < size; i++) {
            cout << vex[i] << " -> " 
            << (visited[i]?"true":"false") << " -> "
            << dis[i] << " -> " << vex[parent[i]] << endl;
        }
    }    

    {
        size_t size = graph.VexSize();
        cout << "dfs traverse:" << endl;
        vector<bool> visited(size, false);
        graph.DFSTraverse(visited, print_node);
        cout << "NULL" << endl;
    }

    {
        size_t size = graph.VexSize();
        cout << "bfs traverse:" << endl;
        vector<bool> visited(size, false);
        graph.BFSTraverse(visited, print_node);
        cout << "NULL" << endl;
    }
    return 0;
}