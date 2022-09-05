#include<vector>
#include<iostream>
struct treap {
	static unsigned int psuedo_random_value;
	static const unsigned int mod_value = 4294967291;
	static const unsigned int xor_value = 0x5bf03635;
	static const unsigned int offset_value = 0x4bd1cd77;
	static unsigned int gen_rand() {
		unsigned int tmp = ((unsigned long long)psuedo_random_value * psuedo_random_value) % mod_value;
		if(psuedo_random_value <= mod_value / 2) 
			psuedo_random_value = tmp;
		else psuedo_random_value = mod_value - tmp;
		return (psuedo_random_value + offset_value) ^ xor_value;
	}
	struct node {
		int val, sz, l, r, p;
		unsigned int pri;
		char attr;
	};
	static std::vector<node> seg;
	static void sync_node(int n) {
		if(seg[n].attr & 1) {
			seg[n].attr ^= 1;
			std::swap(seg[n].l, seg[n].r);
			seg[seg[n].l].attr ^= 3;
			seg[seg[n].r].attr ^= 3;
		}
	}
	static void set_left(int n, int l) {
		seg[l].p = n;
		seg[l].attr &= 1;
		seg[n].l = l;
	}
	static void set_right(int n, int r) {
		seg[r].p = n;
		seg[r].attr |= 2;
		seg[n].r = r;
	}
	static void merge(int l, int r, bool dir = true) {
		if(!r) return;
		if(seg[l].pri < seg[r].pri) {
			sync_node(l);
			seg[l].sz += seg[r].sz;
			if(dir) {
				if(seg[l].r) {
					merge(seg[l].r, r, dir);
				} else {
					set_right(l, r);
				}
			} else {
				if(seg[l].l) {
					merge(seg[l].l, r, dir);
				} else {
					set_left(l, r);
				}
			}
		} else {
			if(seg[l].attr & 2) {
				set_right(seg[l].p, r);
			} else {
				set_left(seg[l].p, r);
			}
			merge(r, l, dir ^ true);
		}
	}
	static int split(int n, int i) {
		if(!n) return 0;
		sync_node(n);
		if(i < seg[seg[n].l].sz) {
			seg[n].sz -= seg[seg[n].l].sz;
			int tmp = split(seg[n].l, i);
			if(seg[n].attr & 2) {
				set_right(seg[n].p, seg[n].l);
			} else {
				set_left(seg[n].p, seg[n].l);
			}
			set_left(n, tmp);
			seg[n].sz += seg[tmp].sz;
			seg[n].p = 0;
			return n;
		} else if(i > seg[seg[n].l].sz){
			seg[n].sz -= seg[seg[n].r].sz;
			int tmp = split(seg[n].r, i - seg[seg[n].l].sz - 1);
			seg[n].sz += seg[seg[n].r].sz;
			return tmp;
		} else {
			if(seg[n].attr & 2) {
				set_right(seg[n].p, seg[n].l);
			} else {
				set_left(seg[n].p, seg[n].l);
			}
			seg[n].sz -= seg[seg[n].l].sz;
			seg[n].l = 0;
			seg[n].p = 0;
			return n;
		}
	}
	static void print(int n, std::ostream& ost = std::cout) {
		if(!n) return;
		sync_node(n);
		print(seg[n].l, ost);
		ost << seg[n].val << " ";
		print(seg[n].r, ost);
	}
	static int get(int n, int x) {
		sync_node(n);
		if(x < seg[seg[n].l].sz) return get(seg[n].l, x);
		else if(x == seg[seg[n].l].sz) return n;
		else return get(seg[n].r, x - seg[seg[n].l].sz - 1);
	}
	int root;
	void push_back(int n) {
		int i = seg.size();
		seg.emplace_back(node{n, 1, 0, 0, 0, gen_rand(), 0});
		merge(root, i);
	}
	void merge(treap & oth) {
		merge(root, seg[oth.root].r);
	}
	treap split(int i) {
		int tmp = split(root, i);
		seg[tmp].p = seg.size();
		seg[tmp].attr |= 2;
		seg.emplace_back(node{0, 1 + seg[tmp].sz, 0, tmp, 0, 0, 0});
		return treap(seg.size() - 1);
	}
	void reverse() {
		seg[seg[root].r].attr ^= 1;
	}
	treap(int n) {
		root = n;
	}
	treap() {
		root = seg.size();
		seg.push_back(node{0, 1, 0, 0, 0, 0, 0});
	}
	int operator[](int n) {
		return seg[get(root, n)].val;
	}
	int key_of_val(int n) {
		if(!seg[n].p) return 0;
		int res = key_of_val(seg[n].p);
		sync_node(n);
		if(seg[n].attr & 2) res += seg[seg[n].l].sz + 1;
		else res -= seg[seg[n].r].sz + 1;
		return res;
	}
	void reverse(int l, int r) {
		if(r == seg[root].sz) {
			treap end = split(l);
			end.reverse();
			merge(end);
		} else {
			treap end = split(r + 1);
			treap mid = split(l);
			mid.reverse();
			merge(mid);
			merge(end);
		}
	}
	friend std::ostream & operator<<(std::ostream& os, treap& c);
};
std::ostream & operator<<(std::ostream& os, treap& c){
	treap::print(treap::seg[c.root].r, os);
    return os;
}
std::vector<treap::node> treap::seg = {{0, 0, 0, 0, 0, 0, 0}};
unsigned int treap::psuedo_random_value = 0xdeadbeef;
//end of template below is the solution to Reversal Sorting (CSES) implemented using this treap
using namespace std;
const int mx = 200005;
int loc[mx];
main() {
	treap t;
	int n; cin >> n;
	for(int i = 1; i <= n; i++) {
		int num; cin >> num;
		loc[num] = i + 1;
		t.push_back(num);
	}
	vector<pair<int, int>> ans;
	for(int i = 1; i <= n; i++) {
		int l = i;
		int r = t.key_of_val(loc[i]);
		if(l == r) continue;
		t.reverse(l, r);
		ans.push_back({l, r});
	}
	cout << ans.size() << endl;
	for(auto [l, r] : ans) cout << l << " " << r << endl;
}
