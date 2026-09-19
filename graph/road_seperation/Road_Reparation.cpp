#include <iostream>
#include<vector>
#include<map>
#include <climits>
#include<algorithm>
#include <queue>

#define ll long long
using namespace std;

ll parent[100005], rnk[100005];

ll find(ll x){
    if(parent[x] != x) parent[x] = find(parent[x]);
    return parent[x];
}

bool unite(ll a, ll b){
    a = find(a);
    b = find(b);
    if(a == b) return 0;

    if(rnk[a] < rnk[b]) swap(a, b);
    parent[b] = a;
    if(rnk[a] == rnk[b]) rnk[a] ++;
    return 1;
}

int main(){
    ll n, m;
    cin >> n >> m;
    
    vector<vector<ll>> edges(m, vector<ll>(3));

    for(ll i = 0; i < m; i++){
        ll a, b, c;
        cin >> a >> b >> c;
        edges[i] = {c, a, b};
    }
    sort(edges.begin(), edges.end());

    for(ll i = 1; i <= n; i++){
        parent[i] = i;
        rnk[i] = 0;
    }
    ll totalCost = 0;
    int edgeUsed = 0;

    for(auto &e : edges){
        ll c = e[0], a = e[1], b = e[2];
        if(unite(a, b)){
            totalCost += c;
            edgeUsed ++;
        }
    }

    if(edgeUsed == n - 1){
        cout << totalCost << "\n";
    } else {
        cout << "IMPOSSIBLE\n";
    }
}