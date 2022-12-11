#include "stdio.h"

char queue[1024];
char visited[225];
char board[64];
static int left = 0;
static int right = 0;

int translate15(int sq) {
	int x = sq % 8;
	int y = sq / 8;
	return (x*2) + (y*2) * 15;
}

int translate8(int sq) {
	int x = sq % 15;
	int y = sq / 15;
	if (x&1 || y&1) return -1;
	return (x/2) + (y/2) * 8;
}

void add(int x, int y, int mv) {
	if (x < 0 || 15 <= x) return;
	if (y < 0 || 15 <= y) return;
	int sq = x + y*15;
	if (visited[sq]) return;
	int t = translate8(sq);
	if (t != -1 && board[t]) return;
	visited[sq] = mv;
	queue[right++] = sq;
}

void findPath(int dst, int st, int* loc) {
	for (int i=0; i<225; i++) visited[i] = 0;
	st = translate15(st);
	dst = translate15(dst);
	left=0, right=0;
	queue[right++] = st;

	while (left != right) {
		int t = queue[left++];
		int x = t % 15;
		int y = t / 15;

		add(x+1, y, 3+8);
		add(x, y+1, 2+12);
		add(x-1, y, 1+8);
		add(x, y-1, 2+4);
	}

	int cur = dst;
  	int i = 0;

	while (cur != st) {
    loc[i++] = translate15(cur);
		int dx = (visited[cur] & 3) - 2;
		int dy = ((visited[cur] >> 2) & 3) - 2;
		cur -= dx + 15*dy;
	}

  loc[i++] = translate15(cur);
  loc[i] = -1;
}
