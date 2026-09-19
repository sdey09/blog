#include <iostream>
#include<vector>
#include<map>
#include <climits>
#include<algorithm>
#include <queue>

#define ll long long
using namespace std;

class SegmentTree{
    private:
        vector<ll> segmentTree, nums;
        void build(ll level, ll l, ll r){
            if(l == r){
                segmentTree[level] = nums[l];
                return;
            }
            
            ll mid = l + (r - l) / 2;
            build(2*level, l, mid);
            build(2*level+1, mid+1, r);
            segmentTree[level] = segmentTree[2*level] + segmentTree[2*level + 1]; 
        }
    
    public:
        SegmentTree(vector<ll> nums){
            ll n = nums.size();
            this->nums = nums;
            this->segmentTree = vector<ll>(4*n);

            this->build(1, 0, n-1);
        }

        ll getTheSum(ll level, ll l, ll r, ll ql, ll qr){
            if(l > qr || r < ql) return 0;
            if(ql <= l && r <= qr) return segmentTree[level];

            ll m = l + (r - l) / 2;
            return getTheSum(2*level, l, m, ql, qr) + getTheSum(2*level + 1, m+1, r, ql, qr);
        }

        void update(ll level, ll l, ll r, ll pos, ll newValue){
            if(l == r){
                segmentTree[level] = newValue;
                return;
            }

            ll mid = l + (r - l) / 2;
            if(pos <= mid) update(2*level, l, mid, pos, newValue);
            else update(2*level + 1, mid+1, r, pos, newValue);
            segmentTree[level] = segmentTree[2*level] + segmentTree[2*level + 1];
        }

};


int main(){
    ll n, q;
    cin >> n >> q;
    vector<ll> nums(n);
    for(ll &x : nums) cin >> x;

    SegmentTree segmentTree(nums);

    ll l = 0, r = n-1;

    while(q -- ){
        ll tp;
        cin >> tp;
        if(tp == 1){
            ll k, u;
            cin >> k >> u;
            segmentTree.update(1, l, r, k-1, u);
        }

        if(tp == 2){
            ll a, b;
            cin >> a >> b;
            ll s = segmentTree.getTheSum(1, l, r, a-1, b-1);
            cout << s << endl;
        }
    }
}