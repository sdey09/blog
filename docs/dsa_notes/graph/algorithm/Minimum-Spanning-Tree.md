Given a weighted, undirected graph G with n vertices and m edges. You want to find a spanning tree which connects all the vertices and has the least weight ( i.e. the sum of the weights of edges is minimal ). A spanning tree is a set of edges such that any vertex can reach any other by exactly one simple path. The spanning tree with the least weight is called a minimum spanning tree.

![[Screenshot 2026-07-21 at 8.21.43 AM.png]]

*This problem appears quite naturally in a lot of problems. For instance in the following problem: there are  $n$  cities and for each pair of cities we are given the cost to build a road between them. We have to build roads, such that we can get from each city to every other city, and the cost for building all roads is minimal.*

### Prim's Algorithm

#### Key concepts
- **Connected Graphs** - A connected graph is one where there is a path between every pair of nodes. Prim's algorithm works only on connected graphs because its goal is to connect all the nodes
- **Greedy Approach** - Prim's algorithm uses a greedy algorithm, which means it always picks the smallest and the cheapest option at each step.
- **Edge Selection** - In Prim's algorithm, edges are selected based on their weight. The algorithm picks the smallest weight that connects without forming a cycle.

```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <climits>

using namespace std;

typedef pair<int, int> iPair;

void primMST(vector<vector<iPair>>& graph, int V) {
    priority_queue<iPair, vector<iPair>, greater<iPair>> pq;
    vector<int> key(V, INT_MAX);
    vector<int> parent(V, -1);
    vector<bool> inMST(V, false);

    int src = 0;
    pq.push(make_pair(0, src));
    key[src] = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        inMST[u] = true;

        for (auto& [v, weight] : graph[u]) {
            if (!inMST[v] && key[v] > weight) {
                key[v] = weight;
                pq.push(make_pair(key[v], v));
                parent[v] = u;
            }
        }
    }

    cout << "Edges in MST:\n";
    for (int i = 1; i < V; ++i)
        cout << parent[i] << " - " << i << "\n";
}

int main() {
    int V = 5;
    vector<vector<iPair>> graph(V);

    graph[0].emplace_back(1, 2);
    graph[0].emplace_back(3, 6);
    graph[1].emplace_back(0, 2);
    graph[1].emplace_back(2, 3);
    graph[1].emplace_back(3, 8);
    graph[1].emplace_back(4, 5);
    graph[2].emplace_back(1, 3);
    graph[2].emplace_back(4, 7);
    graph[3].emplace_back(0, 6);
    graph[3].emplace_back(1, 8);
    graph[4].emplace_back(1, 5);
    graph[4].emplace_back(2, 7);

    primMST(graph, V);

    return 0;
}
```

### Time Complexity

**Simple Array** - `O(V^2)`
Binary Heap ( Min-Heap ) - `O((V+E) log V)`


### Kruskal's Algorithm

Given a weighted undirected graph, we wanted to find a substree of this graph which connects all vertices and has the least weight ( i.e. the sum of the wights of all the edges is minimum ). This is called minimum spanning tree.

```cpp

#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int parent[100005], rnk[100005];

int find(int x){
    if(parent[x] != x) parent[x] = find(parent[x]); // path compression
    return parent[x];
}

bool unite(int a, int b){
    a = find(a); b = find(b);
    if(a == b) return false; // already connected
    if(rnk[a] < rnk[b]) swap(a, b);
    parent[b] = a;
    if(rnk[a] == rnk[b]) rnk[a]++;
    return true;
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    cin >> n >> m;

    vector<array<int,3>> edges(m); // {cost, a, b}
    for(int i = 0; i < m; i++){
        int a, b, c;
        cin >> a >> b >> c;
        edges[i] = {c, a, b};
    }

    sort(edges.begin(), edges.end()); // sort by cost ascending

    for(int i = 1; i <= n; i++){
        parent[i] = i;
        rnk[i] = 0;
    }

    ll totalCost = 0;
    int edgesUsed = 0;

    for(auto &e : edges){
        int c = e[0], a = e[1], b = e[2];
        if(unite(a, b)){
            totalCost += c;
            edgesUsed++;
        }
    }

    if(edgesUsed == n - 1){
        cout << totalCost << "\n";
    } else {
        cout << "IMPOSSIBLE\n";
    }

    return 0;
}
```