/*グループ7ソースコード
*/
// Group7.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

/*連絡板
 Sleepを用いて一時停止を行っているので、ここで一時停止挟んだ方が良いかもなと思うところがあったら↓の文(8行目)をコピペしてください
	Sleep(SleepTime * 1000 * n);
 また、SleepTimeを弄ると全体の一時停止時間も変わるので勝手に弄って遊んでみてください
 個別で弄りたいならnを適当な倍率に変更するなり後ろで+-いくつするなりしておいてください
 Sleep関数の引数の単位はミリ秒(1秒=1000ミリ秒)なのでそれだけ気を付けてください
 By尾﨑大夢
*/

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

/*
	部屋情報をまとめる
*/
struct room
{
	bool hasEnemy;		//餌となる敵がいる
	bool hasItem;		//アイテムがある
	bool hasBrave;		//勇者がいる
	bool hasPlayer;		//自キャラがいる
	bool PlayerVisited;	//自キャラが訪れたか
};

/*ダンジョンは5×5の2次配列で表記
　紙プロトタイプとの相違をなくすため、7x7の配列を作成する
*/
struct room dungeon[7][7];

//主人公の魔物の情報
struct Monster
{
	int roomX;			//部屋のX軸
	int roomY;			//部屋のY軸
	int getFeed;		//取得したえさの数
	int encountNum;		//勇者にもう出会ったか
	int lastMove;		//残り歩数
	bool moveRight;		//移動権、タイピングパートで失敗時に使用する
};

//主人公
struct Monster player;

//勇者の情報
struct Brave
{
	int roomX;			//部屋のX軸
	int roomY;			//部屋のY軸
};

//勇者
struct Brave enemy;

//システム系の各種定義
#define SleepTime 1			//一時停止の時間設定、Sleep内で1000倍して渡すので何秒止めたいかを書き込めばいい(1秒ならそのまま1でOK)
#define BraveDistance 4		//勇者との主人公の距離差設定、勇者は主人公との距離がこの数値以下になる様に移動します

//方向の定義
#define North 0
#define East  1
#define South 2
#define West  3

//難易度と制限時間
#define easy 1
#define normal 2
#define hard 3

#define easyTime 8		//易しいの時間、デフォルト6秒
#define normalTime 8	//普通の時間、デフォルト9秒
#define hardTime 8		//難しいの時間、デフォルト12秒

//戦闘結果
#define monsterWin  1	//プレイヤーの勝ち
#define monsterLose 2	//負け

//難易度を格納しておく変数
int level;

//ゲーム終了に絡んだ変数
bool GameFinished;	//ゲームが終わったか
bool clearFlag;		//ゲームクリアしたか

//制限時間の補正値
int collectTime;

//デバッグ用変数
bool debugMode;		//デバッグモードかどうか

//戦闘結果を保管しておく
int typingResult;

//アナザーモードか否か
bool anotherMode;
bool easyMode;

//勇者戦に入ったかと勇者戦の勝ち数
bool vsBrave;
int braveWins;

//プロトタイプ宣言
int main(void);
void initiative();
void initialiseDungeon();
void initialisePlayer();
void initialiseEnemy();
void initialiseGameEndConditions();
void printGameExplanation();
void showDungeon();
void showRoomInfo();
int getUserMove();
int getRandomNumber(int,int);
void movePlayer(int);
void tryGetFeed();
void tryItem();
void braveEncount();
int getBraveMove();
void braveMove(int);
void getClearEvent();
int checkBraveDistance();
int checkObjectDistance(int, int, int, int);
void selectDifficult();
void typingPart();
void afterTyping();
void braveBattle();
void showText(const char[]);
void showText(const char[], int);

int main(void)
{
	char c;
	int moveDirection;		//プレイヤー移動方向
	int braveDirection;		//勇者移動方向

	initiative();


	while (true)
	{
		if (player.moveRight){
			moveDirection = getUserMove();
			movePlayer(moveDirection);
		}
		showDungeon();
		showRoomInfo();
		player.moveRight = true;
		if (GameFinished){
			break;
		}
		tryGetFeed();
		if (GameFinished){
			break;
		}
		tryItem();
		braveEncount();
		if (GameFinished){
			break;
		}
		showText("勇者が移動するよ");
		if (anotherMode) {
			for (int i = 0; i < 2; i++) {
				braveDirection = getBraveMove();
				braveMove(braveDirection);
				braveEncount();
			}
		}
		else {
			braveDirection = getBraveMove();
			braveMove(braveDirection);
			braveEncount();
		}
		if (GameFinished) {
			break;
		}
	}
	getClearEvent();
	c = getchar();
	return 0;
}

/*初期化節
main関数がごちゃごちゃしてるのが嫌だったのでこっちにまとめました
製作：尾﨑大夢
*/
void initiative()
{
	srand((unsigned)time(NULL));			//乱数のシード値を取得、time.hはここで使用しています
	initialisePlayer();
	initialiseEnemy();
	initialiseDungeon();
	initialiseGameEndConditions();
	printGameExplanation();
	showDungeon();
	showRoomInfo();
}

// 作成：水谷　友也
// 魔物（主人公）の情報を初期化
void initialisePlayer()
{
	// 魔物が最初にいる部屋は(3,3)
	player.roomX = 3;
	player.roomY = 3;

	// 最初は勇者とのエンカウント数が0
	player.encountNum = 0;

	// 最初のエサの所持数は0
	player.getFeed = 0;

	// エサの所持数が6個になった際の残り歩数(最初は12ターン)
	player.lastMove = 12;

	// 移動権の初期化(原則true,特定の条件を満たしたときにfalseとなる)
	player.moveRight = true;
}

// 勇者（エネミー）の情報を初期化
// 作成：水谷　友也
void initialiseEnemy()
{
	// 勇者が最初にいる部屋の座標は生成した乱数(staratxy)により判断
	int startxy = getRandomNumber(1, 4);


	// 生成した乱数が1の場合(X,Y = 1,1)
	if (startxy == 1) {
		enemy.roomX = 1;
		enemy.roomY = 1;
	}
	// 生成した乱数が2の場合(X, Y = 1, 5)
	else if (startxy == 2) {
		enemy.roomX = 1;
		enemy.roomY = 5;
	}
	// 生成した乱数が3の場合(X, Y = 5, 1)
	else if (startxy == 3) {
		enemy.roomX = 5;
		enemy.roomY = 1;
	}
	// 生成した乱数が4の場合(X, Y = 5, 5)
	else if (startxy == 4) {
		enemy.roomX = 5;
		enemy.roomY = 5;
	}
}


/*ダンジョン初期化
Enemyの初期化が終わっていることが前提の部分があるのでそれらの後に実行されます
製作：尾﨑大夢
*/
void initialiseDungeon()
{
	int x;		//色々なオブジェクトのX座標を一時保管する変数
	int y;		//Y座標版

	dungeon[3][3].hasPlayer = true;		//主人公の初期位置は3，3で固定
	dungeon[3][3].PlayerVisited = true;	
	x = enemy.roomX;					//勇者の初期位置をダンジョンに記憶させていく
	y = enemy.roomY;					//
	dungeon[x][y].hasBrave = true;		//勇者の初期位置は4隅のいずれか

	for (int i = 0; i < 7; i++)			//餌の配置、全部で7個
	{
		while (true)
		{
			x = getRandomNumber(1, 5);	//餌のX座標を1～5でランダムに決定
			y = getRandomNumber(1, 5);	//Y座標も同様に処理
			if (x == 3 && y == 3){		//3,3は主人公の初期位置なので置きません

			}
			else{
				if (!dungeon[x][y].hasEnemy){		//3,3でなく、そのマスにエサが無い場合に初めて餌を設置する
					dungeon[x][y].hasEnemy = true;
					break;
				}
			}
		}
	}
	while (true)		//探知アイテムの設置、やり方は餌と同じ
	{
		x = getRandomNumber(1, 5);
		y = getRandomNumber(1, 5);
		if (x == 3 && y == 3)
		{

		}else{
			if (!dungeon[x][y].hasEnemy){
				dungeon[x][y].hasItem = true;
				break;
			}
		}
	}
}

// ゲームの終了条件を初期化　(西海　大輝)
void initialiseGameEndConditions(){
	GameFinished = false;   //エンディングに向かうためのリセットをする
	clearFlag = false;
}


/*ゲーム説明
製作:三部優之介君
*/
void printGameExplanation()
{
	char c, userSelection;
	bool legalSelect = false;
	printf("応用実験テーマ1　チーム７　presents\n\n");
	printf("                     ☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆\n\n");
	printf("                    ☆  ゆうしゃなんかにまけない！  ☆\n\n");
	printf("                     ☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆\n\n");
	printf("********************************************************************************\n");
	printf("\n                          僕の住処に勇者が来たようだ      \n");
	printf("\n          今の僕は弱体化の魔法をかけられていて力が発揮できないんだ。        \n");
	printf("\n                      このままでは勇者に討伐されちゃう…       \n");
	printf("\n                  勇者を撃退するにはエサを食べて強くなるしかない！                   \n");
	printf("\n********************************************************************************");



	do{
		printf("\n");
		printf("1) ルールを確認する\n");
		printf("2) ゲームを始める\n");
		printf("3) イージーモードで始める\n");
		//	printf("4) デバッグモードで始める\n");
		printf("Please enter your selection: ");

		// ユーザの選択を獲得
		c = getchar();
		userSelection = c;
		fflush(stdin);
		if (userSelection == '1')
		{
			printf("\n「ゆうしゃなんかにまけない」のルール:\n");
			printf("=======================================\n");
			printf("1) ダンジョンは5*5の正方形です。\n");
			printf("2) プレイヤーはダンジョンの中心からスタートします。\n");
			printf("3) プレイヤーは上下左右に動くことが出来ます\n");
			printf("4) 勇者はダンジョンの四隅のいずれかからスタートします。\n");
			printf("5) プレイヤーが動いたのち、勇者もランダムで移動します。\n");
			printf("6) 勇者に勝つにはエサを6個以上食べる必要があります。\n");
			printf("7) エサはダンジョンに全部で7個あります。\n");
			printf("8) エサを食べるにはタイピングゲームを成功させる必要があります。\n");
			printf("9) 問題をミスしても留まって再挑戦可能です。その場合は、勇者は通常通り移動を行います。\n");
			printf("10) エサを5個以下しか入手していない状態で勇者に遭遇しても1回逃げることができます。その場合は勇者は四隅のどこかに再配置されます。\n");
			printf("11) エサを6個以上集めると危険を察知した勇者がダンジョンから逃げてしまいます。12T以内に勇者を見つけて討伐しましょう。\n");
			printf("\n");
			printf("＠勝利条件＠\n");
			printf("=======================================\n");
			printf("1) ダンジョン内のエサを7個集める\n");
			printf("2) 6個目のエサを入手したターンから数えて12T以内に勇者と遭遇し戦闘を行う。\n");
			printf("3) エサを6個集めた状態で勇者と1度も遭遇せずに12T経過する。\n");
			printf("※「勇者戦での勝敗」や、「12T以内に勇者と遭遇できたか」といったことによりエンディングが分岐します。また、最も良いエンディングは勇者を倒すことで見ることができます\n");
			printf("\n");
			printf("＠敗北条件＠\n");
			printf("=======================================\n");
			printf("　エサが5個以下の状態で勇者に2回遭遇する\n");
			printf("\n");
			printf("それではがんばってください!\n");
			c = getchar();
			fflush(stdin);
			legalSelect = true;
		}
		else if (userSelection == '2'){
			printf("それではがんばってください!\n");
			c = getchar();
			fflush(stdin);
			legalSelect = true;
		}
		else if (userSelection == '3'){
			easyMode = true;
			collectTime += 3;
			player.encountNum -= 1;
			printf("イージーモードで始めます\n");
			printf("\n「ゆうしゃなんかにまけない」のルール:\n");
			printf("=======================================\n");
			printf("1) ダンジョンは5*5の正方形です。\n");
			printf("2) プレイヤーはダンジョンの中心からスタートします。\n");
			printf("3) プレイヤーは上下左右に動くことが出来ます\n");
			printf("4) 勇者はダンジョンの四隅のいずれかからスタートします。\n");
			printf("5) プレイヤーが動いたのち、勇者もランダムで移動します。\n");
			printf("6) 勇者に勝つにはエサを6個以上食べる必要があります。\n");
			printf("7) エサはダンジョンに全部で7個あります。\n");
			printf("8) エサを食べるにはタイピングゲームを成功させる必要があります。\n");
			printf("9) 問題をミスしても留まって再挑戦可能です。\n");
			printf("10) エサを5個以下しか入手していない状態で勇者に遭遇しても2回逃げることができます。その場合は勇者は四隅のどこかに再配置されます。\n");
			printf("11) エサを6個以上集めると危険を察知した勇者がダンジョンから逃げてしまいます。12T以内に勇者を見つけて討伐しましょう。\n");
			printf("\n");
			printf("＠勝利条件＠\n");
			printf("=======================================\n");
			printf("1) ダンジョン内のエサを7個集める\n");
			printf("2) 6個目のエサを入手したターンから数えて12T以内に勇者と遭遇し戦闘を行う。\n");
			printf("3) エサを6個集めた状態で勇者と1度も遭遇せずに12T経過する。\n");
			printf("※「勇者戦での勝敗」や、「12T以内に勇者と遭遇できたか」といったことによりエンディングが分岐します。また、最も良いエンディングは勇者を倒すことで見ることができます\n");
			printf("\n");
			printf("＠敗北条件＠\n");
			printf("=======================================\n");
			printf("　エサが5個以下の状態で勇者に3回遭遇する\n");
			printf("\n");
			printf("それではがんばってください!\n");
			c = getchar();
			fflush(stdin);
			legalSelect = true;
		}
		else if (userSelection == '*') {		//アナザーモードで始める、と言っても違いらしい違いはない
			anotherMode = true;
			collectTime -= 3;
			player.lastMove /= 2;
			printf("\nアナザーモードで始めます\n");
			printf("※プログラマーが血迷っただけなので真に受けないでね！\n");
			printf("アナザーモードのルール\n");
			printf("======================\n");
			printf("1) 勇者の移動回数が増加します。\n");
			printf("2) 既に訪れた部屋の情報が表示されません。\n");
			printf("　 ただし、現在いるマスの情報は表示されます。\n");
			printf("3) 勇者との距離が表示されなくなります。\n");
			printf("　 ただし、移動先のマスに勇者がいた場合は勇者がいる事が表示されます。\n");
			printf("4) タイピングゲームの制限時間が通常より短くなっています。\n");
			printf("　 また、タイピングゲームで出題される単語の難易度が上昇しています。\n");
			printf("5) 問題をミスした場合にその場で留まることが出来なくなります。\n");
			printf("6) 最も近いエサの場所を表示するアイテムが登場しなくなります。\n");
			printf("7) エサを6個以上食べてから勇者が逃げるまでのターン数が短くなっています。\n");
			printf("それではがんばってください!\n");
			c = getchar();
			fflush(stdin);
			legalSelect = true;
		}
		else if (userSelection == '4')	//デバッグモードで開始したときの処理、勇者の初期位置も開示します
		{
			if (debugMode) {
				debugMode = false;
				printf("デバッグモードをオフにしました\n");
			}
			else {
				debugMode = true;
				printf("デバッグモードをオンにしました\n");
			}
			printf("[%d,%d]", enemy.roomX, enemy.roomY);
			c = getchar();
			fflush(stdin);
		}
		else{
			printf("もう一度やり直してください\n");
			printf("====================================================");
		}
	} while (!legalSelect);
}

/*乱数生成器
最小値、最大値の順に投げ込めばその範囲の乱数を勝手に1つ返してくれる便利マシン
製作：尾﨑大夢
*/
int getRandomNumber(int min,int max)
{
	int i = rand() % (max - min + 1) + min;	//与えられた最小・最大値の範囲内になるように乱数を1個生成
	return i;								//生成した乱数を戻り値として返す
}

/*部屋情報表示
部屋の中に何があるかと勇者との距離を表示する
製作：尾﨑大夢
*/
void showRoomInfo()
{
	int x = player.roomX;		//主人公のX座標
	int y = player.roomY;		//主人公のY座標
//	int distance = checkBraveDistance();
	int distance = checkObjectDistance(x, y, enemy.roomX, enemy.roomY);	//主人公と勇者の距離
	if (dungeon[x][y].hasEnemy){	//餌があるなら
		showText("エサがあるよ");
	}
	if (dungeon[x][y].hasItem){		//探知アイテムがあるなら
		showText("エサの場所が分かるアイテムがあるよ");
	}
	if (dungeon[x][y].hasBrave){	//勇者がいるなら
		showText("勇者がいるよ");
	}
	else{							//勇者がいなくても距離によってどれくらい近いかを示す
		if (!anotherMode) {
			switch (distance)
			{
			case 1:						//距離1ならすごく近い
				printf("**********************\n");
				printf("勇者は凄く近くにいるよ\n");
				showText("**********************");
				break;
			case 2:						//距離2なら近い
				printf("******************\n");
				printf("勇者は近くにいるよ\n");
				showText("******************");
				break;
			default:					//3以上なら近くにはいない
				showText("勇者は近くにはいないよ");
				break;
			}
		}
		else {
			showText("勇者はこのマスにはいないよ");
		}
	}
}
/*ダンジョンの内装表示
製作：尾﨑大夢、三部優之介君
*/
void showDungeon()
{
	int x, y;
	if (debugMode){						//確認モード時のみ、すべての部屋の情報を開示
		for (int i = 1; i <= 5; i++){
			for (int j = 1; j <= 5; j++){
				dungeon[i][j].PlayerVisited = true;
			}
		}
	}
	if (anotherMode && !debugMode) {
		for (int i = 1; i <= 5; i++) {
			for (int j = 1; j <= 5; j++) {
				if (!dungeon[i][j].hasPlayer) {
					dungeon[i][j].PlayerVisited = false;
				}
				dungeon[i][j].hasItem = false;
			}
		}
	}
	// 部屋を一つずつ表示する
	for (y = 1; y <= 5; y++)
	{

		// 1列目：北
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].PlayerVisited)
			{
				if (y == 1){
					printf("@@@@@@@");
				}
				else{
					if (x == 1){
						printf("@******");
					}
					else if (x == 5){
						printf("******@");
					}
					else{
						printf("*******");
					}
				}
			}
			else
			{
				printf("       ");
			}
		}
		printf("\n");

		// 2列目：見やすくするための空きスペース
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].PlayerVisited)
			{
				if (x == 1){
					printf("@     *");
				}
				else if (x == 5){
					printf("*     @");
				}
				else{
					printf("*     *");
				}
			}
			else
			{
				printf("       ");
			}
		}
		printf("\n");

		// 3列目：エサ
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].PlayerVisited)
			{
				if (dungeon[x][y].hasEnemy == true)
				{
					if (x == 1){
						printf("@  &  *");
					}
					else if (x == 5){
						printf("*  &  @");
					}
					else{
						printf("*  &  *");
					}
				}
				else
				{
					if (x == 1){
						printf("@     *");
					}
					else if (x == 5){
						printf("*     @");
					}
					else{
						printf("*     *");
					}
				}
			}
			else
			{
				printf("       ");
			}
		}
		printf("\n");

		// 4列目：アイテムとプレイヤー
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].PlayerVisited)
			{
				if (dungeon[x][y].hasPlayer){
					if (dungeon[x][y].hasItem){
						if (x == 1){
							printf("@ I P *");
						}
						else if (x == 5){
							printf("* I P @");
						}
						else{
							printf("* I P *");
						}
					}
					else{
						if (x == 1){
							printf("@   P *");
						}
						else if (x == 5){
							printf("*   P @");
						}
						else{
							printf("*   P *");
						}
					}
				}
				else{
					if (dungeon[x][y].hasItem){
						if (x == 1){
							printf("@ I   *");
						}
						else if (x == 5){
							printf("* I   @");
						}
						else{
							printf("* I   *");
						}
					}
					else{
						if (x == 1){
							printf("@     *");
						}
						else if (x == 5){
							printf("*     @");
						}
						else{
							printf("*     *");
						}
					}
				}
			}
			else {
				printf("       ");
			}
		}
		printf("\n");

			// 5列目：
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].PlayerVisited)
			{
				if (debugMode && dungeon[x][y].hasBrave) {
					if (x == 1) {
						printf("@  Y  *");
					}
					else if (x == 5) {
						printf("*  Y  @");
					}
					else {
						printf("*  Y  *");
					}
				}
				else {
					if (x == 1) {
						printf("@     *");
					}
					else if (x == 5) {
						printf("*     @");
					}
					else {
						printf("*     *");
					}
				}
			}
			else
			{
				printf("       ");
			}
		}
		printf("\n");
			
			// 6列目：南
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].PlayerVisited)
			{
				if (y == 5){
					printf("@@@@@@@");
				}
				else{
					if (x == 1){
						printf("@******");
					}
					else if (x == 5){
						printf("******@");
					}
					else{
						printf("*******");
					}
				}
			}
			else
			{
				printf("       ");
			}
		}
		printf("\n");
	}
	// 記号の説明
	printf("P = プレイヤー");
	printf("　I = アイテム");
	printf(" 　& = エサ \n");
	printf("w:上,a:左,s:下,d:右\n");
	printf("食べたエサの数：%d個\n", player.getFeed);
	
}

/*ユーザーから移動方向を入力してもらう関数
製作：尾﨑大夢
*/
int getUserMove()
{
	int direction = -1;		//入力された方向
	bool legalDirection = false;		//方向が適正かの確認用
	char c;					//入力された文字を保管しておく変数
	int x = player.roomX;	//プレイヤーのX座標
	int y = player.roomY;	//Y座標

	if (player.getFeed >= 6){
		player.lastMove--;
		printf("あと%d歩動けます\n",player.lastMove);
	}

	if (player.lastMove > 0){
		while (!legalDirection)	//適切な方向が入力されていない間この中をループし続ける
		{
			printf("行きたい方向を入力してね(W,A,S,D): ");	//方向入力
			c = getchar();
			scanf_s("%*c");

			fflush(stdin);			//2文字以上入力するとその後方向移動できなくなる不具合を確認したのでその対策、Enter1回でよくなった

			switch (c)
			{
			case 'W':					//Wは北
			case 'w':
				if (y - 1 > 0){
					direction = North;
					legalDirection = true;
				}
				else{
					printf("いけない\n");
				}
				break;
			case 'A':					//Aは西
			case 'a':
				if (x - 1 > 0){
					direction = West;
					legalDirection = true;
				}
				else{
					printf("いけない\n");
				}
				break;
			case 'S':					//Sは南
			case 's':
				if (y + 1 < 6){
					direction = South;
					legalDirection = true;
				}
				else{
					printf("いけない\n");
				}
				break;
			case 'D':					//Dは東
			case 'd':
				if (x + 1 < 6){
					direction = East;
					legalDirection = true;
				}
				else{
					printf("いけない\n");
				}
				break;
			default:					//それ以外はやり直し
				printf("そんなものない\n");
				break;
			}

		}
	}
	else{
		GameFinished = true;
	}
	return direction;
}

/*勇者の移動方向を決定する関数
製作：尾﨑大夢
*/
int getBraveMove()
{
	int direction = -1;					//決定した移動方向
	int c;								//勇者の入力方向は乱数で決定
	bool legalDirection = false;		//その方向に行けるかどうかの判定
	int x = enemy.roomX;				//勇者のX座標
	int y = enemy.roomY;				//Y座標
	int px = player.roomX;				//主人公のX座標
	int py = player.roomY;				//Y座標
	while (!legalDirection)				//適切な方向になるまでループ
	{
		c = getRandomNumber(1, 4);
		switch (c)
		{
		case 1:
			if (y - 1 > 0){			//1なら北
				if (checkObjectDistance(x, y - 1, px, py) <= BraveDistance){					//移動先の主人公との距離差が4以下ならその方向に移動
					direction = North;
					legalDirection = true;
				}
				else if (checkObjectDistance(x, y - 1, px, py) < checkObjectDistance(x, y, px, py)){	//そうでない場合は距離が縮まる方向なら移動
					direction = North;
					legalDirection = true;
				}
			}
			break;
		case 2:							//2なら西
			if (x - 1 > 0){
				if (checkObjectDistance(x - 1, y, px, py) <= BraveDistance){					//移動先の主人公との距離差が4以下ならその方向に移動
					direction = West;
					legalDirection = true;
				}
				else if (checkObjectDistance(x - 1, y, px, py) < checkObjectDistance(x, y, px, py)){	//そうでない場合は距離が縮まる方向なら移動
					direction = West;
					legalDirection = true;
				}
			}
			break;
		case 3:							//3なら南
			if (y + 1 < 6){
				if (checkObjectDistance(x, y + 1, px, py) <= BraveDistance){					//移動先の主人公との距離差が4以下ならその方向に移動
					direction = South;
					legalDirection = true;
				}
				else if (checkObjectDistance(x, y + 1, px, py) < checkObjectDistance(x, y, px, py)){	//そうでない場合は距離が縮まる方向なら移動
					direction = South;
					legalDirection = true;
				}
			}
			break;
		case 4:							//4なら東
			if (x + 1 < 6){
				if (checkObjectDistance(x + 1, y, px, py) <= BraveDistance){					//移動先の主人公との距離差が4以下ならその方向に移動
					direction = East;
					legalDirection = true;
				}
				else if (checkObjectDistance(x + 1, y, px, py) < checkObjectDistance(x, y, px, py)){	//そうでない場合は距離が縮まる方向なら移動
					direction = East;
					legalDirection = true;
				}
			}
			break;
		default:
			break;
		}
	}
	return direction;
}
/*プレイヤーの移動
製作：モハメド君
*/
void movePlayer(int direction)
{
	int currentX = player.roomX;
	int currentY = player.roomY;
	int newX = 3;
	int	newY = 3;
	//主人公を元の部屋から消す
	dungeon[currentX][currentY].hasPlayer = false;
	//　新しい部屋のX軸とY軸を方向から獲得
	if (direction == North)
	{
		newX = currentX;
		newY = currentY - 1;
	}
	else if (direction == East)
	{
		newX = currentX + 1;
		newY = currentY;
	}
	else if (direction == South)
	{
		newX = currentX;
		newY = currentY + 1;
	}
	else if (direction == West)
	{
		newX = currentX - 1;
		newY = currentY;
	}

	// 新しい場所の情報を追加
	player.roomX = newX;
	player.roomY = newY;
	dungeon[newX][newY].hasPlayer = true;
	dungeon[newX][newY].PlayerVisited = true;
}

// 作成：水谷　友也
// 勇者が移動した際の処理
void braveMove(int direction)
{
	int currentX = enemy.roomX;
	int currentY = enemy.roomY;
	int newX = enemy.roomX;
	int	newY = enemy.roomY;

	//　勇者を元の部屋から消す
	dungeon[currentX][currentY].hasBrave = false;

	//　新しい部屋のX軸とY軸を方向から獲得
	if (direction == North)
	{
		newX = currentX;
		newY = currentY - 1;
	}
	else if (direction == East)
	{
		newX = currentX + 1;
		newY = currentY;
	}
	else if (direction == South)
	{
		newX = currentX;
		newY = currentY + 1;
	}
	else if (direction == West)
	{
		newX = currentX - 1;
		newY = currentY;
	}

	// 新しい場所の情報を追加
	enemy.roomX = newX;
	enemy.roomY = newY;
	dungeon[newX][newY].hasBrave = true;
	if (debugMode){
		printf("[%d,%d]\n", newX, newY);
	}
}


//勇者と遭遇した時　(西海　大輝)
void braveEncount(void){
	int px = player.roomX;
	int py = player.roomY;
	bool legalInitBrave = false;
	if (dungeon[px][py].hasBrave) {   //プレイヤーと敵の座標が重なったとき
		player.moveRight = true;	//移動権のリセットを行う
		if (player.getFeed <= 5) {   //エサが5個以下のとき
			player.encountNum++;   //バッドエンカウント数を1増やす
			if (player.encountNum <= 1){
				printf("まだパワーアップしきれていないのに勇者と遭遇してしまった。\n");
				printf("何とか逃げ切れたけど次はないだろう…\n");
				printf("残りのエサを集めてパワーアップしないと!\n");
				do{
					dungeon[px][py].hasBrave = false;					//元の部屋から勇者を消して
					initialiseEnemy();									//勇者を再配置
					dungeon[enemy.roomX][enemy.roomY].hasBrave = true;	//勇者の転送先に勇者を配置して
					if (!dungeon[px][py].hasBrave){						//主人公と勇者が同じマスに居ないならループ脱出
						legalInitBrave = true;							//いるならまたループを行う、勇者を消すのはdo内1行目でOK
						if (debugMode){
							printf("[%d,%d]\n", enemy.roomX, enemy.roomY);
						}
					}
				} while (!legalInitBrave);
				
			}
			else{
				GameFinished = true;
			}
		}
		else if (player.getFeed >= 6) {   //エサが6個以上のとき
			braveBattle();
		}
	}
}

// エンディングテキスト 
//三部がつくったよ
void getClearEvent()
{
	showText("**********************************************************", SleepTime * 2);
	if (!vsBrave) {
		if (player.getFeed <= 5) {
			showText("再び勇者に遭遇してしまった…\n", SleepTime * 2);
			showText("今の力ではまだ勇者に太刀打ちできない…\n", SleepTime * 2);
			printf("プレイヤーは力尽きた…\n\n");
		}
		else if (player.getFeed <= 6) {
			showText("十分に力をつけたことで勇者をダンジョンから追い出すことに成功した。\n", SleepTime * 2);
			showText("しかし仕留めることが出来なかったので再び勇者がダンジョンに攻めてくることもあるかもしれない。\n", SleepTime * 2);
			printf("だがとりあえず今回はそれで良しとしよう。\n\n");
		}
		else {
			showText("かつての自分よりも力をつけることが出来た…\n", SleepTime * 2);
			printf("勇者をとらえることは出来なかったがもうこのダンジョンに来ることは無いだろう…\n\n");
		}
	}
	else {
		if (braveWins == 3) {
			showText("勇者を食べることで更なる力を手に入れた。\n", SleepTime * 2);
			showText("今の自分にかなうものは誰もいない…\n", SleepTime * 2);
			printf("プレイヤーは世界を征服した。\n\n");
			clearFlag = true;
		}
		else {
			showText("勇者の守りを突破することが出来なかった。\n", SleepTime * 2);
			showText("勇者に逃げられてしまった。\n", SleepTime * 2);
			printf("ダンジョンを守れたので今回はこれで良しとしよう。\n\n");
		}
	}

	Sleep(SleepTime * 1000 * 2);
	showText("これでこのゲームはおしまい。\n", SleepTime * 2);
	printf("さんきゅ～　　ふぉ～　　ぷれいんぐ　\n\n");
	if (clearFlag) {
		if (easyMode){
			Sleep(SleepTime * 1000 * 2);
			printf("次はノーマルモードで挑戦してみよう！\n\n");
		}
		else{
			Sleep(SleepTime * 1000 * 2);
			printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			printf("@おまけ：最初の画面で*を押すと…？@\n");
			printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		}
	}
}

/*勇者との距離を測る関数
多分↓のcheckObjectDistanceに差し替えになる
製作：尾﨑大夢
*/
int checkBraveDistance()
{
	int px, py, bx, by;
	px = player.roomX;		//主人公のX座標を取得
	py = player.roomY;		//Y座標
	bx = enemy.roomX;		//勇者のX座標
	by = enemy.roomY;		//Y座標
	int dx = abs(px - bx);	//主人公のX座標と勇者のX座標の差の絶対値を求める
	int dy = abs(py - by);	//同じことをY座標でも行う
	return dx + dy;			//X座標の差の絶対値とY座標の差の絶対値の和を返す
}

/*2点間の距離差を求める関数
checkBraveDistanceを汎用化したくて作りました
挙動も問題なさそうなので今後はこっちを使うかな
製作：尾﨑大夢
*/
int checkObjectDistance(int x1, int y1, int x2, int y2){
	int dx = abs(x1 - x2);
	int dy = abs(y1 - y2);
	return dx + dy;
}

/*最も近いエサの座標を返すアイテムの処理
初期化の壁ではない凡ミスで凍結してました
製作：尾﨑大夢
*/
void tryItem(){
	int px, py, fx, fy;
	int dsum;
	int dmin = 100;
	px = player.roomX;
	py = player.roomY;
	fx = 1;
	fy = 1;
	if(dungeon[px][py].hasItem){				//主人公のいるマスにアイテムがあるなら
		showText("（役に立ちそうだな…早速使ってみるとしよう）");
		for (int i = 1; i <= 5; i++){
			for (int j = 1; j <= 5; j++){
				if (dungeon[i][j].hasEnemy){	//1部屋ごとにエサがあるかの確認を行う
					dsum = checkObjectDistance(i, j, px, py);				//エサのマスとの距離を求めて
					if (dsum <= dmin){			//現行の最小値以下なら
						fx = i;					//X座標、Y座標、距離を更新
						fy = j;
						dmin = dsum;
					}
				}
			}
		}
		dungeon[px][py].hasItem = false;		//マスからアイテムを消して
		printf("最も近いエサは横%d,縦%dにあるよ\n", fx, fy);		//最も近いエサの座標を表示
	}
}

/*餌取得のためのタイピングパート
α版のソースが複数の関数を用いたタイプになったため
こっちも同様の方式に
そして元からあったこの関数はタイピングの実行チェック及び他関数の呼び出し元と化すのだった
製作：尾﨑大夢
*/
void tryGetFeed()
{
	int px = player.roomX;
	int py = player.roomY;
	if (dungeon[px][py].hasEnemy){
		selectDifficult();
		typingPart();
		afterTyping();
	}
}

/*タイピングゲームの難易度決定関数
今までに集めた餌が2個未満ならeasy
5個未満ならnormal
5個以上ならhard
製作：尾﨑大夢
*/
void selectDifficult(){
	int feedNum = player.getFeed;
	if (!anotherMode && !easyMode) {
		if (feedNum < 2) {
			level = easy;
		}
		else if (feedNum < 5) {
			level = normal;
		}
		else {
			level = hard;
		}
	}
	else if(anotherMode){
		if (feedNum < 3) {
			level = normal;
		}
		else {
			level = hard;
		}
	}
	else if (easyMode)
	{
		if (feedNum < 5){
			level = easy;
		}
		else{
			level = normal;
		}
	}
	if (level == easy) {
		showText("<スライムが現れた！>");
		showText("「お前なんかに食べられてたまるか…返り討ちにしてやる！!」");
	}
	else if (level == normal) {
		showText("<ゴブリンが現れた！>");
		showText("「あ？ んだてめぇ...やんのかおら!!ぶっ殺してやる!!!」");
	}
	else {
		showText("<巨人族が現れた！>");
		showText("「力なき小さな者よ...我に挑むとはいい度胸だ。相手をしてやろう！」");
	}
}

/*タイピングパート
変更点は開始前にreadyを挟んで任意に始められるようになった点、結果をtypingResultに保存している点
製作：尾﨑大夢
*/

void typingPart(){
	char c, userSelection;

	time_t s_time, e_time, n_time;	//左から順に開始時間、終了時間、現在時間を格納
	bool collectJudge = false;		//正誤判定
	int spellNum;					//難易度ごとの問題の数を格納
	int questionNum;				//出題番号
	int defTime;					//既定の制限時間
	char  * qEasy[] = {				//易しいの時の問題、現在は150個	(三部君製作)
		"jazzy", "fuzzy", "muzzy", "whizz", "fezzy", "fizzy", "abuzz", "zuzim", "scuzz", "dizzy", "frizz", "huzza", "mezzo", "pizza", "jujus", "tizzy", "hajji", "zazen", "zanza", "zizit",
		"jumpy", "tazza", "jacky", "tazze", "zappy", "jimmy", "jimpy", "jiffy", "zippy", "jemmy", "quick", "jammy", "quack", "junky", "mujik", "jocko", "jugum", "zaxes", "zinky", "jumps",
		"jumbo", "kudzu", "quiff", "jocks", "kopje", "jacks", "quaky", "quaff", "jerky", "jibbs", "pazazz", "pizazz", "pizzaz", "jazzbo", "bezazz", "jazzed", "zizzle", "jazzes", "jazzer", "muzjik",
		"whizzy", "mizzly", "scuzzy", "fuzzed", "puzzle", "muzzle", "buzzed", "huzzah", "frizzy", "jujube", "mizzen", "fizzed", "fuzzes", "fizzle", "mizzle", "mezuza", "zigzag", "buzzes", "buzzer", "pizzle",
		"guzzle", "fezzed", "wizzen", "hazzan", "fezzes", "wizzes", "bizzes", "cozzes", "fizzer", "fizzes", "huzzas", "queazy", "nuzzle", "mezzos", "snazzy", "jojoba", "piazza", "pizzas", "piazze", "banjax",
		"pizzazz", "zyzzyva", "pizazzy", "jacuzzi", "jazzmen", "jazzman", "jazzing", "jazzily", "jazzbos", "zizzled", "quizzed", "quizzer", "zizzles", "quizzes", "buzzwig", "jazzers", "jazzier", "buzzcut", "muzjiks", "muzzily",
		"fuzzily", "schizzy", "fuzzing", "buzzing", "mezuzah", "muzzled", "whizzed", "jumbuck", "jejunum", "schnozz", "jimjams", "jukebox", "chazzan", "fizzing", "frizzly", "chazzen", "puzzled", "muezzin", "muzzles", "fizzled",
		"huzzahs", "whizzer", "grizzly", "jujuism", "buzzard", "puzzler", "muzzler", "jujubes", "mizzled", "puzzles"
	};

	char* qNormal[] = {				//普通の時の問題、100個(モハメド君製作)
		"razzmatazz", "bumfuzzled", "bumfuzzles", "whizzbangs", "bemuzzling", "puzzlingly", "unpuzzling",
		"embezzling", "unmuzzling", "zigzagging", "puzzlement", "scuzzballs", "dizzyingly", "blackjacks", "blizzardly", "bedazzling", "dazzlingly",
		"pozzolanic", "crackajack", "showbizzes", "schnozzles", "skyjacking", "jackknifed", "jackknives", "kibbutznik", "japanizing", "maximizing",
		"jarovizing", "cheapjacks", "equivoques", "lumberjack", "zigzaggers", "supplejack", "knickknack", "kickboxing", "mezzalunas", "jackknifes",
		"pozzolanas", "highjacked", "mozzarella", "squeezable", "applejacks", "bejumbling", "zombifying", "quantizing", "smokejacks", "podzolizes",
		"quincunxes", "zygomorphy", "equalizing", "mezzanines", "joypopping", "quartzitic", "oxygenized", "muckamucks", "frizziness", "maximizers",
		"sjamboking", "intermezzo", "intermezzi", "liquidized", "quadplexes", "jacqueries", "complexify", "amberjacks", "exchequers", "jaywalking",
		"blackbucks", "exorcizing", "exoenzymes", "jackscrews", "jackrabbit", "johnnycake", "carjacking", "kibbitzing", "mezzotints", "chiffchaff",
		"zinkifying", "quizmaster", "backchecks", "epoxidized", "flummoxing", "jackplanes", "czarevitch", "jacklights", "oxidizable", "circumflex",
		"osteosises", "restresses", "torosities", "terrorists", "artistries", "tertiaries", "retreaters", "serosities", "troostites", "teatasters",
		"tattooists", "isostasies", "oratresses"
	};

	char * qHard[] = {				//難しいの時の問題、100個(西海君、水谷君製作)
		"outjockeying", "communalized", "pulverizable", "psychologize", "objectivisms"
		, "subvocalized", "macadamizing", "juvenescence", "plasmolyzing", "stockjobbers"
		, "pancreozymin", "soliloquized", "polemicizing", "conjunctivas", "conjunctives"
		, "bolshevizing", "frizzinesses", "aximoatizing", "piggybacking", "conjunctivae"
		, "quicksilvers", "mechanizable", "exemplifying", "subjectivity", "subjunctives"
		, "sympathizing", "antikickback", "extemporizes", "extemporizer", "emblazonment"
		, "cockneyfying", "jejunenesses", "communalizes", "phagocytizes", "abjectivally"
		, "recognizably", "equalization", "vanquishable", "zooxanthella", "bodychecking"
		, "hypnotizable", "paralyzingly", "bushwhacking", "tranquilizer", "inexplicably"
		, "hyperbolized", "copolymerize", "complexified", "unmechanized", "inexpugnably"
		, "razzmatazzes", "quizzicality", "blackjacking", "embezzlement", "subjectivize", "katzenjammer"
		, "quinquennium", "puzzleheaded", "bedazzlement", "crackerjacks", "jackhammered", "backchecking"
		, "mezzotinting", "stockjobbing", "jeopardizing", "lexicalizing", "journalizing", "jacklighting"
		, "quixotically", "hypercomplex", "quinquennial", "hydroxyzines", "windjammings", "unpublicized"
		, "extravaganza", "subjectivism", "conjunctival", "pickabacking", "pickabacking", "objectifying"
		, "subjectively", "circumjacent", "zygapophysis", "zygapophysis", "extemporized", "maximization"
		, "polygamizing", "zygapophyses", "backflipping", "cyclazocines", "immobilizing", "nonoxidizing"
		, "tranquillize", "thymectomize", "phagocytized", "phagocytized", "unquenchable", "homozygously"
		, "projectively", "checkmarking",
	};
	char estr[100];					//タイプされた文字列を格納しておく配列

	printf("<Enterを1回押したら出題されます。>\n");			//ここから猶予期間パート

	c = getchar();
	userSelection = c;
	fflush(stdin);
															//猶予期間パートここまで

	time(&s_time);					//開始時間を獲得
	switch (level)					//難易度ごとに制限時間、問題数を取得
	{
	case easy:
		e_time = s_time + easyTime + collectTime;
		spellNum = sizeof qEasy / sizeof qEasy[0];
		defTime = easyTime + collectTime;
		break;
	case normal:
		e_time = s_time + normalTime + collectTime;
		spellNum = sizeof qNormal / sizeof qNormal[0];
		defTime = normalTime + collectTime;
		break;
	case hard:
		e_time = s_time + hardTime + collectTime;
		spellNum = sizeof qHard / sizeof qHard[0];
		defTime = hardTime + collectTime;
		break;
	}

	questionNum = getRandomNumber(0, spellNum - 1);		//問題を取得、前回作った乱数生成器を流用してます

	while (true){
		time(&n_time);				//現在時間を取得

		while (e_time >= n_time){	//現在時間が終了時間以下なら
			printf("あと%d秒\n", (int)(e_time - n_time));	//残り時間を表示
			switch (level){			//難易度ごとに分岐、基本は問題表示→文字入力→正誤判定と繰り返す
			case easy:
				while (!collectJudge && (e_time >= n_time))
				{
					printf("%s\n", qEasy[questionNum]);
					gets_s(estr);
					time(&n_time);
					if ((strcmp(qEasy[questionNum], estr) == 0) && (e_time >= n_time)){
						collectJudge = true;
					}
					else{
						time(&n_time);
						printf("あと%d秒\n", (int)(e_time - n_time));
					}
				}
				break;
			case normal:
				while (!collectJudge && (e_time >= n_time))
				{
					printf("%s\n", qNormal[questionNum]);
					gets_s(estr);
					time(&n_time);
					if ((strcmp(qNormal[questionNum], estr) == 0) && (e_time >= n_time)){
						collectJudge = true;
					}
					else{
						time(&n_time);
						printf("あと%d秒\n", (int)(e_time - n_time));
					}
				}
				break;
			case hard:
				while (!collectJudge && (e_time >= n_time))
				{
					printf("%s\n", qHard[questionNum]);
					gets_s(estr);
					time(&n_time);
					if ((strcmp(qHard[questionNum], estr) == 0) && (e_time >= n_time)){
						collectJudge = true;
					}
					else{
						time(&n_time);
						printf("あと%d秒\n", (int)(e_time - n_time));
					}
				}
				break;
			}
			if (collectJudge){		//正解したら現在時間を保存してWhileループを抜け出す
				time(&n_time);
				break;
			}
		}
		if (collectJudge){
			printf("正解！\n");
			printf("残り%d秒\n", (int)(e_time - n_time));
			if ((int)(e_time - n_time) > defTime / 2){
				collectTime--;
				printf("次から-1秒\n");
			}
			typingResult = monsterWin;
			break;
		}
		else{
			printf("時間切れ\n");
			collectTime++;
			printf("次から+1秒\n");
			typingResult = monsterLose;
			break;
		}
	}
}

/*タイピングゲーム終了後の処理
成功時、失敗時の処理をまとめて書いています
製作：尾﨑大夢
*/
void afterTyping(){
	int px = player.roomX;
	int py = player.roomY;
	char c, userSelection;
	bool legalAnswer = false;
	switch (typingResult)
	{
	case monsterWin:
		dungeon[px][py].hasEnemy = false;
		player.getFeed++;
		printf("餌の数+1,現在%d個\n", player.getFeed);
		if (player.getFeed >= 7) {
			GameFinished = true;
		}
		printf("<戦闘に勝利した。>\n");
		printf("<プレイヤーはモンスターを食べてパワーアップした。>\n");
		if (player.getFeed <= 2) {
			showText("（まだまだ食べたりないな…もっとエサを集めなきゃ…）");
		}
		else if (player.getFeed <= 5) {
			showText("（かなり力が戻ってきたな…　あともう少しだ！）");
		}
		else if (player.getFeed <= 6){
			showText("（十分に力を取り戻したが、あと一匹エサ食べることができれば勇者を圧倒できそうだ…）");
		}
//		c = getchar();
//		scanf_s("%*c");
//		fflush(stdin);
		break;
	case monsterLose:
		printf("<モンスターとのバトルに負けてしまった…>\n");
		if (!anotherMode) {
			while (!legalAnswer) {
				printf("このマスに留まりますか？\n留まった場合、次のターンに同じ餌との再戦を行うことが出来ます\n1)はい\n2)いいえ\n");
				c = getchar();
				userSelection = c;
				scanf_s("%*c");
				fflush(stdin);

				switch (userSelection) {
				case '1':
					printf("次のターンもう一度戦います\n");
					player.moveRight = false;
					legalAnswer = true;
					break;
				case '2':
					printf("ここを離れて別のマスに移動します\n");
					legalAnswer = true;
					break;
				default:
					printf("何かおかしいのでもう一回\n");
					break;
				}
			}
		}
		break;
	}
}

/*勇者戦の関数
勇者戦に移る条件は
①エサを6個以上食べて
②勇者と接触する
難易度hard固定、3連戦します
製作：尾﨑大夢
*/
void braveBattle() {
	vsBrave = true;
	if (easyMode){						//イージーモードに限りnormalで連戦します
		level = normal;
	}
	else{
		level = hard;
	}
	printf("******************************************************************\n");
	showText("<勇者が現れた！>");
	showText("「こんなところで倒されるわけにはいかない…絶対に生き残るんだ…！」");
	showText("******************************************************************");
	for (int i = 0; i < 3; i++) {
		if (!anotherMode && !easyMode){
			collectTime = -1;			//規定時間の8秒と合わせて7秒になるよう調整
		}
		else if(anotherMode){
			collectTime = -2;			//アナザーは6秒に設定
		}
		else if(easyMode){
			collectTime = 0;			//イージーは8秒に設定
		}
		typingPart();
		if (typingResult == monsterWin) {
			braveWins++;
		}
		else {
			break;
		}
	}
	GameFinished = true;
}

/*文字列をディレイ付きで出力する関数2個セット
その都度Sleep書くのがめんどくさいなという発想から生まれた狂気の産物
!!!!!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!
この関数は数値を取得して出力する、という操作には対応していません
	printf("%d\n",player.getFeed);
↑のような出力には対応できないため諦めて普通にprintfの後にSleepをつけて対応してください
製作：尾﨑大夢
*/
void showText(const char text[]) {
	printf("%s\n", text);
	Sleep(SleepTime * 1000);
}
/*文字列をディレイ付きで出力する関数2個セット
その都度Sleep書くのがめんどくさいなという発想から生まれた狂気の産物
!!!!!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!
この関数は数値を取得して出力する、という操作には対応していません
printf("%d\n",player.getFeed);
↑のような出力には対応できないため諦めて普通にprintfの後にSleepをつけて対応してください
製作：尾﨑大夢
*/
void showText(const char text[], int Time) {
	printf("%s\n", text);
	Sleep(Time * 1000);
}
