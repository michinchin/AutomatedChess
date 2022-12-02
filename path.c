#include "stdio.h"

int queue[1024];
int visited[225];
int board[64];
int l = 0;
int r = 0;

void print_15(int sq) {
	int x = sq % 15;
	int y = sq / 15;
	printf("(%.1f, %.1f)\n", (float)x/2, (float)y/2);
}

int translate_15(int sq) {
	int x = sq % 8;
	int y = sq / 8;
	return (x*2) + (y*2) * 15;
}

int translate_8(int sq) {
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
	int t = translate_8(sq);
	if (t != -1 && board[t]) return;
	visited[sq] = mv;
	queue[r++] = sq;
}

void find_path(int dst, int st) {
	for (int i=0; i<225; i++) visited[i] = 0;
	st = translate_15(st);
	dst = translate_15(dst);
	l=0, r=0;
	queue[r++] = st;

	while (l != r) {
		int t = queue[l++];
		int x = t % 15;
		int y = t / 15;

		add(x+1, y, 3+8);
		add(x, y+1, 2+12);
		add(x-1, y, 1+8);
		add(x, y-1, 2+4);
	}

	int cur = dst;

	while (cur != st) {
		print_15(cur);
		int dx = (visited[cur] & 3) - 2;
		int dy = ((visited[cur] >> 2) & 3) - 2;
		cur -= dx + 15*dy;
	}

	print_15(cur);
}

int main() {
	for (int i=0; i<64; i++)
		board[i] = 0;
	board[1] = 1;

	while (1) {
		int from, to;
		scanf("%d %d", &from, &to);
		find_path(from, to);
	}
}
