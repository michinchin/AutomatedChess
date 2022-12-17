#include <stdio.h>

enum direction {
	END,
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

enum direction visited[256];
static char queue[1024];
static char board[64] = {0};
static int left = 0;
static int right = 0;

int to16(int sq) {
	int x = sq % 8;
	int y = sq / 8;
	return (x*2) + (y*2) * 16;
}

int to8(int sq) {
	int x = sq % 16;
	int y = sq / 16;
	if (x&1 || y&1) return -1;
	return (x/2) + (y/2) * 8;
}

void add(int x, int y, enum direction mv) {
	if (x < 0 || 16 <= x) return;
	if (y < 0 || 16 <= y) return;
	int sq = x + y*16;
	if (visited[sq] != END) return;
	visited[sq] = mv;

	int t = to8(sq);
	if (t != -1 && board[t]) return;

	queue[right++] = sq;
}

// Finds a path between st to dst and saves it to path
void findPath(int dst, int st, enum direction* path) {
	// Unblock the destination and start squares so that a valid path can be found.
	// Save the values that were here originally so that we can restore the board state after.
	int board_st = board[to8(st)];
	int board_dst = board[to8(dst)];
	board[to8(st)] = 0;
	board[to8(dst)] = 0;

	for (int i=0; i<256; i++) visited[i] = END;
	left=0, right=0;
	queue[right++] = st;

	while (left != right) {
		int t = queue[left++];
		int x = t % 16;
		int y = t / 16;

		add(x+1, y, RIGHT);
		add(x, y+1, UP);
		add(x-1, y, LEFT);
		add(x, y-1, DOWN);
	}

	int cur = dst;
	int i = 0;

	while (cur != st) {
		printf("%d %d\n", cur, visited[cur]);
		if      (visited[cur] == UP)    { cur -= 16; path[i++] = DOWN; }
		else if (visited[cur] == DOWN)  { cur += 16; path[i++] = UP; }
		else if (visited[cur] == RIGHT) { cur -= 1;  path[i++] = LEFT; }
		else if (visited[cur] == LEFT)  { cur += 1;  path[i++] = RIGHT; }
	}
  path[i] = END;

	board[to8(st)] = board_st;
	board[to8(dst)] = board_dst;
}
