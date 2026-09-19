#include <iostream>
#include <vector>
#define ll long long
using namespace std;

class FenwickTree {
private:
    vector<ll> tree;
    ll n;

public:
    FenwickTree(ll n) {
        this->n = n;
        tree.assign(n + 1, 0); // 1-indexed
    }

    // point update: add 'val' at position i
    void update(ll i, ll val) {
        for (; i <= n; i += i & (-i))
            tree[i] += val;
    }

    // prefix sum from 1..i
    ll query(ll i) {
        ll sum = 0;
        for (; i > 0; i -= i & (-i))
            sum += tree[i];
        return sum;
    }
};

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    ll n, q;
    cin >> n >> q;

    vector<ll> a(n + 1); // 1-indexed
    for (ll i = 1; i <= n; i++) cin >> a[i];

    FenwickTree bit(n);

    // Initialize the difference array into the BIT
    // diff[1] = a[1], diff[i] = a[i]-a[i-1]
    bit.update(1, a[1]);
    for (ll i = 2; i <= n; i++) {
        bit.update(i, a[i] - a[i-1]);
    }

    while (q--) {
        ll tp;
        cin >> tp;

        if (tp == 1) {
            ll x, y, u;
            cin >> x >> y >> u;
            bit.update(x, u);
            if (y + 1 <= n) bit.update(y + 1, -u);
        } else {
            ll k;
            cin >> k;
            cout << bit.query(k) << "\n";
        }
    }

    return 0;
}