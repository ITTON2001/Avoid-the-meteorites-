#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define SCREEN_WIDTH	(11)
#define SCREEN_HEIGHT	(24)

#define FPS			(3)
#define INTERVAL	(1000 / FPS)

#define ATOMIC_COLUWN	(11)
#define ATOMIC_ROW		(24)

enum {
	TILE_NONE,
	TILE_ATOMIC,
	TILE_PLAYER,
	TILE_MAX
};

enum {
	DIRECTION_RIGHT,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_MAX
};

typedef struct {
	int x, y;
}VEC2;

typedef struct {
	int x, y;
	bool isNone;
}ATOMIC;

typedef struct {
	int x, y;
}PLAYER;

//ベクトル
VEC2 directions[] = {
	{1,0},//DIRECTION_RIGHT,
	{0,1},//DIRECTION_DOWN,
	{-1,0},//DIRECTION_LEFT,
};

//AA
const char* tileAA[TILE_MAX] = {
	"　",	//TILE_NONE
	"〇",	//TILE_ATOMIC
	"▲",	//TILE_PLAYER
};


//定義
int screen[SCREEN_HEIGHT][SCREEN_WIDTH];
ATOMIC atomic[ATOMIC_ROW][ATOMIC_COLUWN];
PLAYER player;

int atomicDirection;
int atomicCount;

//描画
void DrawScreen() {
	//隕石が移動する際に残る残像を消す
	memset(screen, 0, sizeof screen);
	//隕石を描画
	for (int y = 0; y < ATOMIC_ROW; y++){
		for (int x = 0; x < ATOMIC_COLUWN; x++) {
			if (!atomic[y][x].isNone) {
				screen[atomic[y][x].y][atomic[y][x].x] = TILE_ATOMIC;
			}
			else {
				screen[atomic[y][x].y][atomic[y][x].x] = TILE_NONE;
			}
		}
	}

	//プレイヤーを描画
	screen[player.y][player.x] = TILE_PLAYER;

	system("cls");
	//マップを描画
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++)
			printf("%s", tileAA[screen[y][x]]);
			//printf("%d", screen[y][x]);
			//printf("%d,%d", atomic[y][x].y,atomic[y][x].x);
		printf("\n");
	}
}

//初期化
void Init() {
	// 現在の時間を使ってランダムなシードを設定
	srand(time(NULL));

	// ランダムな数値を得る
	int randomNumber = rand() % SCREEN_WIDTH;

	// 隕石の初期生成
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			atomic[y][x].x = x;
			atomic[y][x].y = y;
			if (atomic[y][x].y == 0 && atomic[y][x].x != randomNumber)
				atomic[y][x].isNone = false;
			else
				atomic[y][x].isNone = true;		

		}
		
	}


	//プレイヤーの初期位置
	player.x = SCREEN_WIDTH / 2;
	player.y = SCREEN_HEIGHT - 2;

	DrawScreen();
}

//プレイヤーの当たり判定
bool AtomicIntersectPlayer() {
	for (int y = 0; y < ATOMIC_ROW; y++)
		for (int x = 0; x < ATOMIC_COLUWN; x++)
			if ((atomic[y][x].x == player.x)
				&& (atomic[y][x].y == player.y)
				&& (!atomic[y][x].isNone)) {
				return true;
			}
	return false;
}

//メイン
int main() {
	Init();
	int check_count = 0;
	int Atomatic_count = 0;

	clock_t lastClock = clock();
	while (1)
	{
		clock_t nowClock = clock();
		if (nowClock >= lastClock + INTERVAL) {
			lastClock = nowClock;

			Atomatic_count += 1;

			//隕石の挙動			
			for (int y = ATOMIC_ROW - 1; y >= 0; y--) {
				for (int x = 0; x < ATOMIC_COLUWN; x++) {
					if (!atomic[y][x].isNone) {
						check_count += 1;
						atomic[y][x].isNone = true;
						atomic[y + 1][x].isNone = false;
					}
				}
			}

			//5ターンに一回隕石を落とす
			if (Atomatic_count == 5) {
				Atomatic_count = 0;

				// ランダムな数値を得る
				int randomNumber = rand() % SCREEN_WIDTH;

				for (int y = 0; y < SCREEN_HEIGHT; y++) {
					for (int x = 0; x < SCREEN_WIDTH; x++) {
						atomic[y][x].x = x;
						atomic[y][x].y = y;
						if (atomic[y][x].y == 0 && atomic[y][x].x != randomNumber)
							atomic[y][x].isNone = false;
					}
				}
			}

			//隕石がプレイヤーに当たったらゲームオーバー
			if (AtomicIntersectPlayer()) {
				Init();
				Atomatic_count = 0;
				lastClock = clock();
				continue;
			}

			//AtomicOffScreen();
			DrawScreen();
		}

		//プレイヤーの操作
		if (_kbhit()) {
			switch (_getch())
			{
			case 'a':
				player.x--;
				break;
			case 'd':
				player.x++;
				break;
			}

			if (player.x < 0)
				player.x = 0;
			if (player.x >= SCREEN_WIDTH)
				player.x = SCREEN_WIDTH - 1;

			//自分から隕石に当たってしまう
			if (AtomicIntersectPlayer()) {
				Atomatic_count = 0;
				Init();
				continue;
			}

			DrawScreen();
		}
	}
}