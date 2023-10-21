#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define SCREEN_WIDTH	(11)
#define SCREEN_HEIGHT	(24)

#define FPS			(50)
#define INTERVAL	(1000 / FPS)

#define ATOMIC_COLUWN	(11)
#define ATOMIC_ROW		(1)

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
	for (int y = 0; y < ATOMIC_ROW; y++)
		for (int x = 0; x < ATOMIC_COLUWN; x++)
			screen[atomic[y][x].y][atomic[y][x].x] = TILE_ATOMIC;

	//プレイヤーを描画
	screen[player.y][player.x] = TILE_PLAYER;

	system("cls");
	//マップを描画
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++)
			printf("%s", tileAA[screen[y][x]]);
		printf("\n");
	}
}

//初期化
void Init() {
	//インベーダーの初期位置
	for (int y = 0; y < ATOMIC_ROW; y++)
		for (int x = 0; x < ATOMIC_COLUWN; x++) {
			//1つおきにインベーダーを設置する
			atomic[y][x].x = x;
			atomic[y][x].y = y;
		}

	//隕石の最初の向き
	atomicDirection = DIRECTION_DOWN;
	atomicCount = 0;

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
				&& (atomic[y][x].y == player.y)) {
				return true;
		}
	return false;
}

//メイン
int main() {
	Init();

	clock_t lastClock = clock();
	while (1)
	{
		clock_t nowClock = clock();
		if (nowClock >= lastClock + INTERVAL) {
			lastClock = nowClock;

			//インベーダーは10カウントにつき1回動く
			atomicCount++;
			if (atomicCount > 10) {
				atomicCount = 0;

				int nextDirection = atomicDirection;
				//インベーダーの挙動
				for (int y = 0; y < ATOMIC_ROW; y++)
					for (int x = 0; x < ATOMIC_COLUWN; x++) {

						atomic[y][x].x += directions[atomicDirection].x;
						atomic[y][x].y += directions[atomicDirection].y;
					}
			}

			//弾がプレイヤーに当たったらゲームオーバー
			if (AtomicIntersectPlayer()) {
				Init();
				lastClock = clock();
				continue;
			}

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

			DrawScreen();
		}
	}
}