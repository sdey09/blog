#include <iostream>
#include<vector>
#include<map>
#include <climits>
#include<algorithm>
#include <queue>

#define ll long long
using namespace std;

ll parent[100005], sz[100005];

ll find_set(ll v){
    if(v == parent[v]) return v;
    return parent[v] = find_set(parent[v]);
}

bool union_sets(ll a, ll b){
    a = find_set(a);
    b = find_set(b);
    if(a == b) return 0;

    if(sz[a] < sz[b]) swap(a, b);
    parent[b] = a;
    sz[a] += sz[b];
    return 1;
}

int main(){
    ll n, m;
    cin >> n >> m;
    for(ll i = 1; i < n+1; i++){
        parent[i] = i;
        sz[i] = 1;
    }

    ll mx = 1;
    ll numComp = n;

    while(m --){
        ll a, b;
        cin >> a >> b;
        if(union_sets(a, b)){
            numComp --;
            mx = max(mx, sz[find_set(a)]);
        }

        cout << numComp << " " << mx << endl;
    }


}