#include <iostream>
#include<vector>
#include<map>
#include <climits>
#include<algorithm>
#include <queue>

#define ll long long
using namespace std;

class SegmentTree {
    private:
        vector<ll> segmentTree, nums;
        ll n;

        void build(ll v, ll l, ll r){
            if( l == r){
                segmentTree[v] = nums[l];
                return;
            }
            
            ll mid = l + (r - l) / 2;
            build(2*v, l, mid);
            build(2*v+1, mid+1, r);
            segmentTree[v] = min(segmentTree[2*v], segmentTree[2*v + 1]);
        }
    
    public:
        SegmentTree(vector<ll> nums){
            this->nums = nums;
            this->n = nums.size();
            this->segmentTree = vector<ll>(this->n * 4);

            build(1, 0, this->n - 1);
        }

        ll getTheMin(ll v, ll l, ll r, ll ql, ll qr){
            if(qr < l || r < ql) return LLONG_MAX;
            if(ql <= l && r <= qr) return segmentTree[v];
            
            ll mid = l + (r - l) / 2;
            return min(
                    getTheMin(2*v, l, mid, ql, qr), 
                    getTheMin(2*v+1, mid+1, r, ql, qr)
                    );
        }
};


int main(){
    ll n, q;
    cin >> n >> q;
    vector<ll> nums(n);

    for(ll &x : nums) cin >> x;
    SegmentTree segmentTree(nums);

    while(q--){
        ll a, b;
        cin >> a >> b;
        ll x = segmentTree.getTheMin(1, 0, nums.size()-1, a-1, b-1);
        cout << x << endl;
    }
}