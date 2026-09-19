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
    vector<ll> segemntTree, nums;
    void build(ll l, ll r, ll v){
        if(l == r){
            this->segemntTree[v] = this->nums[l];
            return;
        }

        ll mid = l + (r - l) / 2;
        build(l, mid, 2*v);
        build(mid+1, r, 2*v + 1);
        this->segemntTree[v] = this->segemntTree[2*v] + this->segemntTree[2*v + 1];
    }

public:

    SegmentTree(vector<ll> nums){
        this->nums = nums;
        this->segemntTree = vector<ll>(4 * nums.size());
        build(0, nums.size() - 1, 1);
    }

    ll getSum(ll nd, ll l, ll r, ll ql, ll qr){
        if(qr < l || r < ql) return 0;
        if(ql <= l && r <= qr) return this->segemntTree[nd];
        
        ll mid = l + (r - l) / 2;
        return getSum(2*nd, l, mid, ql, qr) + getSum(2*nd + 1, mid+1, r, ql, qr);
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

        ll s = segmentTree.getSum(1, 0, n-1, a-1, b-1);
        cout << s << endl;
    }
}