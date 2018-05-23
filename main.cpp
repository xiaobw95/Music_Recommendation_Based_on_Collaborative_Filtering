#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <map>
#include <set>
#include <ctime>
#include <queue>
#include <cmath>
using namespace std;
typedef long long int64;
#define rep(x, n) for (int x = 1; x <= n; ++x)
#define zrp(x, n) for (int x = n; x; --x)
#define FOR(x, l, r) for (int x = l; x <= r; ++x)
#define foredge(i, x) for (int i = start[x]; i; i = e[i].l)

const int UN = 200000;
const int SN = 304671;
const double inf = 1e40;

map<string, int> user2ID;
map<string, int> song2ID;
string ID2Song[SN+5];

map<int, int> userRcd[UN+5];
set<int> songRcd[SN+5];
int userTotListened[UN+5];

double score[SN+5];
int sa[SN+5];

struct Term
{
	int id;
	double p;
	Term(){}
	Term(int _id, double _p) {id = _id; p = _p;}
	bool operator<(const Term &o)const {
		return p > o.p;
	}
}q[30];
priority_queue<Term> Q;

double t;

void init()
{
	char u[100], s[100];
	string str; int id, cnt;

	cerr << "Begin reading data from users.txt ...\n"; t = clock();
	freopen("data/users.txt", "r", stdin);
	id = 0;
	while (scanf("%s", u) != EOF) {
		user2ID[string(u)] = ++id;
	}
	fclose(stdin);
	cerr << "Finished! Time consuming: " << (clock() - t) / 1000000 << "s.\n";

	cerr << "Begin reading data from songs.txt ...\n"; t = clock();
	freopen("data/songs.txt", "r", stdin);
	id = 0;
	while (scanf("%s%*d", s) != EOF) {
		song2ID[string(s)] = ++id;
	}
	fclose(stdin);
	cerr << "Finished! Time consuming: " << (clock() - t) / 1000000 << "s.\n";

	cerr << "Begin reading data from train_triplets.txt ...\n"; t = clock();
	freopen("data/train_triplets.txt", "r", stdin);
	while (scanf("%s%s%d", u, s, &cnt) != EOF) {
		int uID = user2ID[string(u)], sID = song2ID[string(s)];
		userRcd[uID][sID] += cnt;
		userTotListened[uID] += cnt;
		songRcd[sID].insert(uID);
	}
	fclose(stdin);
	cerr << "Finished! Time consuming: " << (clock() - t) / 1000000 << "s.\n";
}

bool cmp(int x, int y) { return score[x] > score[y]; }
void recommend(int A)
{
	set<int> tmp;

	for (int i = 1; i <= SN; ++i) score[i] = -inf;

	cerr << "Step2 ... "; t = clock();
	for (typeof(userRcd[A].begin()) it = userRcd[A].begin(); it != userRcd[A].end(); ++it) {
		int a = it->first;
		double k = double(it->second) / userTotListened[A];
		t = int(ceil(k * 300));
		//cerr << a << " " << k << " " << t << endl;

		for (int b = 1; b <= SN; ++b) if (userRcd[A].find(b) == userRcd[A].end()) {
			int userCnt = 0;
			
			set<int>::iterator aIt = songRcd[a].begin(), bIt = songRcd[b].begin();
			while (aIt != songRcd[a].end() && bIt != songRcd[b].end()) {
				if (*aIt < *bIt) ++aIt; 
				else if (*aIt > *bIt) ++bIt;
				else {
					++userCnt;
					++aIt; ++bIt;
				}
			}

			double p = double(userCnt) / songRcd[a].size();

			if (Q.size() < t || p > Q.top().p) {
				if (Q.size() == t) Q.pop();
				Q.push(Term(b, p));
			}
		}

		while (Q.size() > 0) {
			int b; double p;
			b = Q.top().id;
			p = Q.top().p;
			score[b] = max(score[b], p);
			tmp.insert(b);
			//cerr << "b = " << b << " p = " << p << endl;
			Q.pop();
		}
	}
	cerr << "Finished! Time consuming: " << (clock() - t) / 1000000 << "s.\n";

	cerr << "Step3 ... "; t = clock();
	for (int B = 1; B <= UN; ++B) if (A != B) {
		int songCnt = 0;

		map<int, int>::iterator AIt = userRcd[A].begin(), BIt = userRcd[B].begin();
		while (AIt != userRcd[A].end() && BIt != userRcd[B].end()) {
			if (AIt->first < BIt->first) ++AIt;
			else if (AIt->first > BIt->first) ++BIt;
			else {
				++songCnt;
				++AIt; ++BIt;
			}
		}

		double p = double(songCnt) / (userRcd[A].size() * userRcd[B].size());

		if (Q.size() < 20 || p > Q.top().p) {
			if (Q.size() == 20) Q.pop();
			Q.push(Term(B, p));
		}
	}

	for (int i = 20; i > 0; --i) {
		q[i] = Q.top();
		//cerr << q[i].id << " " << q[i].p << endl;
		Q.pop();
	}
	cerr << "Finished! Time consuming: " << (clock() - t) / 1000000 << "s.\n";

	cerr << "Step3' ... "; t = clock();
	for (int i = 2; i <= 10; ++i) {
		int bu = -1; double bp = -inf;
		for (int j = i; j <= 20; ++j) {
			double p = inf;
			int B = q[j].id;

			for (int k = 1; k < i; ++k) {
				int A = q[k].id;

				int songCnt = 0;
				map<int, int>::iterator AIt = userRcd[A].begin(), BIt = userRcd[B].begin();
				while (AIt != userRcd[A].end() && BIt != userRcd[B].end()) {
					if (AIt->first < BIt->first) ++AIt;
					else if (AIt->first > BIt->first) ++BIt;
					else {
						++songCnt;
						++AIt; ++BIt;
					}
				}

				p = min(p, double(songCnt) / (userRcd[A].size() * userRcd[B].size()));
			}

			if (bu == -1 || p > bp) {
				bu = j; bp = p;
			}
		}

		swap(q[bu], q[i]);
	}
	cerr << "Finished! Time consuming: " << (clock() - t) / 1000000 << "s.\n";

	cerr << "Step4 ... "; t = clock();
	for (int b = 1; b <= SN; ++b) {
		for (int j = 1; j <= 10; ++j) {
			if (userRcd[q[j].id].find(b) == userRcd[q[j].id].end()) continue;
			score[b] += q[j].p * userRcd[q[j].id][b] * 10;
		}
	}
	cerr << "Finished! Time consuming: " << (clock() - t) / 1000000 << "s.\n";

	for (int i = 1; i <= SN; ++i) sa[i] = i;
	sort(sa + 1, sa + 1 + SN, cmp);
	for (int i = 1; i <= 100; ++i) {
		printf("%d ", sa[i]);
		//cerr << "score(" << sa[i] << ") = " << score[sa[i]] << endl;
	}
	cerr << "Tot Songs = " << tmp.size() << endl; 
	printf("\n");
}

void work()
{
	char u[100];

	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
	int num = 0;
	while (scanf("%s", u) != EOF) {
		num++;
		cerr << "No." << num << " user:\n"; 
		recommend(user2ID[string(u)]);
	}
	fclose(stdin);
	fclose(stdout);
}

int main()
{
	init();
	work();
	return 0;
}
