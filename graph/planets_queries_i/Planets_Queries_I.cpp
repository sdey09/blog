#include <iostream>
#include<vector>
#include<map>
#include <climits>
#include<algorithm>
#include <queue>

#define ll long long
using namespace std;

const ll MAXN = 2e5 + 5;
const ll MAXD = 30;
int parents[MAXN][MAXD];

// Binary Lifting problem

int main(){
    ios_base::sync_with_stdio(false);
	cin.tie(NULL);

    ll n, q;
    cin >> n >> q;

    
    for(ll i = 1; i < n+1; i++) cin >> parents[i][0];

    for(ll d = 1; d < MAXD; d ++){
        for(ll i = 1; i < n+1; i++){
            parents[i][d] = parents[parents[i][d - 1]][d - 1];
        }
    }

    while(q--){
        ll a, d;
        cin >> a >> d;
        for(ll i = 0; i < MAXD; i++){
            if(d & (1 << i)) a = parents[a][i];
        }

        cout << a << endl;
    }
}