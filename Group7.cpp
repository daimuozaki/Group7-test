/*�O���[�v7�\�[�X�R�[�h
*/
// Group7.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

/*�A����
 Sleep��p���Ĉꎞ��~���s���Ă���̂ŁA�����ňꎞ��~���񂾕����ǂ������ȂƎv���Ƃ��낪�������火�̕�(8�s��)���R�s�y���Ă�������
	Sleep(SleepTime * 1000 * n);
 �܂��ASleepTime��M��ƑS�̂̈ꎞ��~���Ԃ��ς��̂ŏ���ɘM���ėV��ł݂Ă�������
 �ʂŘM�肽���Ȃ�n��K���Ȕ{���ɕύX����Ȃ����+-��������Ȃ肵�Ă����Ă�������
 Sleep�֐��̈����̒P�ʂ̓~���b(1�b=1000�~���b)�Ȃ̂ł��ꂾ���C��t���Ă�������
 By�����喲
*/

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

/*
	���������܂Ƃ߂�
*/
struct room
{
	bool hasEnemy;		//�a�ƂȂ�G������
	bool hasItem;		//�A�C�e��������
	bool hasBrave;		//�E�҂�����
	bool hasPlayer;		//���L����������
	bool PlayerVisited;	//���L�������K�ꂽ��
};

/*�_���W������5�~5��2���z��ŕ\�L
�@���v���g�^�C�v�Ƃ̑�����Ȃ������߁A7x7�̔z����쐬����
*/
struct room dungeon[7][7];

//��l���̖����̏��
struct Monster
{
	int roomX;			//������X��
	int roomY;			//������Y��
	int getFeed;		//�擾���������̐�
	int encountNum;		//�E�҂ɂ����o�������
	int lastMove;		//�c�����
	bool moveRight;		//�ړ����A�^�C�s���O�p�[�g�Ŏ��s���Ɏg�p����
};

//��l��
struct Monster player;

//�E�҂̏��
struct Brave
{
	int roomX;			//������X��
	int roomY;			//������Y��
};

//�E��
struct Brave enemy;

//�V�X�e���n�̊e���`
#define SleepTime 1			//�ꎞ��~�̎��Ԑݒ�ASleep����1000�{���ēn���̂ŉ��b�~�߂��������������߂΂���(1�b�Ȃ炻�̂܂�1��OK)
#define BraveDistance 4		//�E�҂Ƃ̎�l���̋������ݒ�A�E�҂͎�l���Ƃ̋��������̐��l�ȉ��ɂȂ�l�Ɉړ����܂�

//�����̒�`
#define North 0
#define East  1
#define South 2
#define West  3

//��Փx�Ɛ�������
#define easy 1
#define normal 2
#define hard 3

#define easyTime 8		//�Ղ����̎��ԁA�f�t�H���g6�b
#define normalTime 8	//���ʂ̎��ԁA�f�t�H���g9�b
#define hardTime 8		//����̎��ԁA�f�t�H���g12�b

//�퓬����
#define monsterWin  1	//�v���C���[�̏���
#define monsterLose 2	//����

//��Փx���i�[���Ă����ϐ�
int level;

//�Q�[���I���ɗ��񂾕ϐ�
bool GameFinished;	//�Q�[�����I�������
bool clearFlag;		//�Q�[���N���A������

//�������Ԃ̕␳�l
int collectTime;

//�f�o�b�O�p�ϐ�
bool debugMode;		//�f�o�b�O���[�h���ǂ���

//�퓬���ʂ�ۊǂ��Ă���
int typingResult;

//�A�i�U�[���[�h���ۂ�
bool anotherMode;
bool easyMode;

//�E�Ґ�ɓ��������ƗE�Ґ�̏�����
bool vsBrave;
int braveWins;

//�v���g�^�C�v�錾
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
	int moveDirection;		//�v���C���[�ړ�����
	int braveDirection;		//�E�҈ړ�����

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
		showText("�E�҂��ړ������");
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

/*��������
main�֐��������Ⴒ���Ⴕ�Ă�̂����������̂ł������ɂ܂Ƃ߂܂���
����F�����喲
*/
void initiative()
{
	srand((unsigned)time(NULL));			//�����̃V�[�h�l���擾�Atime.h�͂����Ŏg�p���Ă��܂�
	initialisePlayer();
	initialiseEnemy();
	initialiseDungeon();
	initialiseGameEndConditions();
	printGameExplanation();
	showDungeon();
	showRoomInfo();
}

// �쐬�F���J�@�F��
// �����i��l���j�̏���������
void initialisePlayer()
{
	// �������ŏ��ɂ��镔����(3,3)
	player.roomX = 3;
	player.roomY = 3;

	// �ŏ��͗E�҂Ƃ̃G���J�E���g����0
	player.encountNum = 0;

	// �ŏ��̃G�T�̏�������0
	player.getFeed = 0;

	// �G�T�̏�������6�ɂȂ����ۂ̎c�����(�ŏ���12�^�[��)
	player.lastMove = 12;

	// �ړ����̏�����(����true,����̏����𖞂������Ƃ���false�ƂȂ�)
	player.moveRight = true;
}

// �E�ҁi�G�l�~�[�j�̏���������
// �쐬�F���J�@�F��
void initialiseEnemy()
{
	// �E�҂��ŏ��ɂ��镔���̍��W�͐�����������(staratxy)�ɂ�蔻�f
	int startxy = getRandomNumber(1, 4);


	// ��������������1�̏ꍇ(X,Y = 1,1)
	if (startxy == 1) {
		enemy.roomX = 1;
		enemy.roomY = 1;
	}
	// ��������������2�̏ꍇ(X, Y = 1, 5)
	else if (startxy == 2) {
		enemy.roomX = 1;
		enemy.roomY = 5;
	}
	// ��������������3�̏ꍇ(X, Y = 5, 1)
	else if (startxy == 3) {
		enemy.roomX = 5;
		enemy.roomY = 1;
	}
	// ��������������4�̏ꍇ(X, Y = 5, 5)
	else if (startxy == 4) {
		enemy.roomX = 5;
		enemy.roomY = 5;
	}
}


/*�_���W����������
Enemy�̏��������I����Ă��邱�Ƃ��O��̕���������̂ł����̌�Ɏ��s����܂�
����F�����喲
*/
void initialiseDungeon()
{
	int x;		//�F�X�ȃI�u�W�F�N�g��X���W���ꎞ�ۊǂ���ϐ�
	int y;		//Y���W��

	dungeon[3][3].hasPlayer = true;		//��l���̏����ʒu��3�C3�ŌŒ�
	dungeon[3][3].PlayerVisited = true;	
	x = enemy.roomX;					//�E�҂̏����ʒu���_���W�����ɋL�������Ă���
	y = enemy.roomY;					//
	dungeon[x][y].hasBrave = true;		//�E�҂̏����ʒu��4���̂����ꂩ

	for (int i = 0; i < 7; i++)			//�a�̔z�u�A�S����7��
	{
		while (true)
		{
			x = getRandomNumber(1, 5);	//�a��X���W��1�`5�Ń����_���Ɍ���
			y = getRandomNumber(1, 5);	//Y���W�����l�ɏ���
			if (x == 3 && y == 3){		//3,3�͎�l���̏����ʒu�Ȃ̂Œu���܂���

			}
			else{
				if (!dungeon[x][y].hasEnemy){		//3,3�łȂ��A���̃}�X�ɃG�T�������ꍇ�ɏ��߂ĉa��ݒu����
					dungeon[x][y].hasEnemy = true;
					break;
				}
			}
		}
	}
	while (true)		//�T�m�A�C�e���̐ݒu�A�����͉a�Ɠ���
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

// �Q�[���̏I���������������@(���C�@��P)
void initialiseGameEndConditions(){
	GameFinished = false;   //�G���f�B���O�Ɍ��������߂̃��Z�b�g������
	clearFlag = false;
}


/*�Q�[������
����:�O���D�V��N
*/
void printGameExplanation()
{
	char c, userSelection;
	bool legalSelect = false;
	printf("���p�����e�[�}1�@�`�[���V�@presents\n\n");
	printf("                     ��������������������������������\n\n");
	printf("                    ��  �䂤����Ȃ񂩂ɂ܂��Ȃ��I  ��\n\n");
	printf("                     ��������������������������������\n\n");
	printf("********************************************************************************\n");
	printf("\n                          �l�̏Z���ɗE�҂������悤��      \n");
	printf("\n          ���̖l�͎�̉��̖��@���������Ă��ė͂������ł��Ȃ��񂾁B        \n");
	printf("\n                      ���̂܂܂ł͗E�҂ɓ������ꂿ�Ⴄ�c       \n");
	printf("\n                  �E�҂����ނ���ɂ̓G�T��H�ׂċ����Ȃ邵���Ȃ��I                   \n");
	printf("\n********************************************************************************");



	do{
		printf("\n");
		printf("1) ���[�����m�F����\n");
		printf("2) �Q�[�����n�߂�\n");
		printf("3) �C�[�W�[���[�h�Ŏn�߂�\n");
		//	printf("4) �f�o�b�O���[�h�Ŏn�߂�\n");
		printf("Please enter your selection: ");

		// ���[�U�̑I�����l��
		c = getchar();
		userSelection = c;
		fflush(stdin);
		if (userSelection == '1')
		{
			printf("\n�u�䂤����Ȃ񂩂ɂ܂��Ȃ��v�̃��[��:\n");
			printf("=======================================\n");
			printf("1) �_���W������5*5�̐����`�ł��B\n");
			printf("2) �v���C���[�̓_���W�����̒��S����X�^�[�g���܂��B\n");
			printf("3) �v���C���[�͏㉺���E�ɓ������Ƃ��o���܂�\n");
			printf("4) �E�҂̓_���W�����̎l���̂����ꂩ����X�^�[�g���܂��B\n");
			printf("5) �v���C���[���������̂��A�E�҂������_���ňړ����܂��B\n");
			printf("6) �E�҂ɏ��ɂ̓G�T��6�ȏ�H�ׂ�K�v������܂��B\n");
			printf("7) �G�T�̓_���W�����ɑS����7����܂��B\n");
			printf("8) �G�T��H�ׂ�ɂ̓^�C�s���O�Q�[���𐬌�������K�v������܂��B\n");
			printf("9) �����~�X���Ă����܂��čĒ���\�ł��B���̏ꍇ�́A�E�҂͒ʏ�ʂ�ړ����s���܂��B\n");
			printf("10) �G�T��5�ȉ��������肵�Ă��Ȃ���ԂŗE�҂ɑ������Ă�1�񓦂��邱�Ƃ��ł��܂��B���̏ꍇ�͗E�҂͎l���̂ǂ����ɍĔz�u����܂��B\n");
			printf("11) �G�T��6�ȏ�W�߂�Ɗ댯���@�m�����E�҂��_���W�������瓦���Ă��܂��܂��B12T�ȓ��ɗE�҂������ē������܂��傤�B\n");
			printf("\n");
			printf("������������\n");
			printf("=======================================\n");
			printf("1) �_���W�������̃G�T��7�W�߂�\n");
			printf("2) 6�ڂ̃G�T����肵���^�[�����琔����12T�ȓ��ɗE�҂Ƒ������퓬���s���B\n");
			printf("3) �G�T��6�W�߂���ԂŗE�҂�1�x������������12T�o�߂���B\n");
			printf("���u�E�Ґ�ł̏��s�v��A�u12T�ȓ��ɗE�҂Ƒ����ł������v�Ƃ��������Ƃɂ��G���f�B���O�����򂵂܂��B�܂��A�ł��ǂ��G���f�B���O�͗E�҂�|�����ƂŌ��邱�Ƃ��ł��܂�\n");
			printf("\n");
			printf("���s�k������\n");
			printf("=======================================\n");
			printf("�@�G�T��5�ȉ��̏�ԂŗE�҂�2�񑘋�����\n");
			printf("\n");
			printf("����ł͂���΂��Ă�������!\n");
			c = getchar();
			fflush(stdin);
			legalSelect = true;
		}
		else if (userSelection == '2'){
			printf("����ł͂���΂��Ă�������!\n");
			c = getchar();
			fflush(stdin);
			legalSelect = true;
		}
		else if (userSelection == '3'){
			easyMode = true;
			collectTime += 3;
			player.encountNum -= 1;
			printf("�C�[�W�[���[�h�Ŏn�߂܂�\n");
			printf("\n�u�䂤����Ȃ񂩂ɂ܂��Ȃ��v�̃��[��:\n");
			printf("=======================================\n");
			printf("1) �_���W������5*5�̐����`�ł��B\n");
			printf("2) �v���C���[�̓_���W�����̒��S����X�^�[�g���܂��B\n");
			printf("3) �v���C���[�͏㉺���E�ɓ������Ƃ��o���܂�\n");
			printf("4) �E�҂̓_���W�����̎l���̂����ꂩ����X�^�[�g���܂��B\n");
			printf("5) �v���C���[���������̂��A�E�҂������_���ňړ����܂��B\n");
			printf("6) �E�҂ɏ��ɂ̓G�T��6�ȏ�H�ׂ�K�v������܂��B\n");
			printf("7) �G�T�̓_���W�����ɑS����7����܂��B\n");
			printf("8) �G�T��H�ׂ�ɂ̓^�C�s���O�Q�[���𐬌�������K�v������܂��B\n");
			printf("9) �����~�X���Ă����܂��čĒ���\�ł��B\n");
			printf("10) �G�T��5�ȉ��������肵�Ă��Ȃ���ԂŗE�҂ɑ������Ă�2�񓦂��邱�Ƃ��ł��܂��B���̏ꍇ�͗E�҂͎l���̂ǂ����ɍĔz�u����܂��B\n");
			printf("11) �G�T��6�ȏ�W�߂�Ɗ댯���@�m�����E�҂��_���W�������瓦���Ă��܂��܂��B12T�ȓ��ɗE�҂������ē������܂��傤�B\n");
			printf("\n");
			printf("������������\n");
			printf("=======================================\n");
			printf("1) �_���W�������̃G�T��7�W�߂�\n");
			printf("2) 6�ڂ̃G�T����肵���^�[�����琔����12T�ȓ��ɗE�҂Ƒ������퓬���s���B\n");
			printf("3) �G�T��6�W�߂���ԂŗE�҂�1�x������������12T�o�߂���B\n");
			printf("���u�E�Ґ�ł̏��s�v��A�u12T�ȓ��ɗE�҂Ƒ����ł������v�Ƃ��������Ƃɂ��G���f�B���O�����򂵂܂��B�܂��A�ł��ǂ��G���f�B���O�͗E�҂�|�����ƂŌ��邱�Ƃ��ł��܂�\n");
			printf("\n");
			printf("���s�k������\n");
			printf("=======================================\n");
			printf("�@�G�T��5�ȉ��̏�ԂŗE�҂�3�񑘋�����\n");
			printf("\n");
			printf("����ł͂���΂��Ă�������!\n");
			c = getchar();
			fflush(stdin);
			legalSelect = true;
		}
		else if (userSelection == '*') {		//�A�i�U�[���[�h�Ŏn�߂�A�ƌ����Ă��Ⴂ�炵���Ⴂ�͂Ȃ�
			anotherMode = true;
			collectTime -= 3;
			player.lastMove /= 2;
			printf("\n�A�i�U�[���[�h�Ŏn�߂܂�\n");
			printf("���v���O���}�[���������������Ȃ̂Ő^�Ɏ󂯂Ȃ��łˁI\n");
			printf("�A�i�U�[���[�h�̃��[��\n");
			printf("======================\n");
			printf("1) �E�҂̈ړ��񐔂��������܂��B\n");
			printf("2) ���ɖK�ꂽ�����̏�񂪕\������܂���B\n");
			printf("�@ �������A���݂���}�X�̏��͕\������܂��B\n");
			printf("3) �E�҂Ƃ̋������\������Ȃ��Ȃ�܂��B\n");
			printf("�@ �������A�ړ���̃}�X�ɗE�҂������ꍇ�͗E�҂����鎖���\������܂��B\n");
			printf("4) �^�C�s���O�Q�[���̐������Ԃ��ʏ���Z���Ȃ��Ă��܂��B\n");
			printf("�@ �܂��A�^�C�s���O�Q�[���ŏo�肳���P��̓�Փx���㏸���Ă��܂��B\n");
			printf("5) �����~�X�����ꍇ�ɂ��̏�ŗ��܂邱�Ƃ��o���Ȃ��Ȃ�܂��B\n");
			printf("6) �ł��߂��G�T�̏ꏊ��\������A�C�e�����o�ꂵ�Ȃ��Ȃ�܂��B\n");
			printf("7) �G�T��6�ȏ�H�ׂĂ���E�҂�������܂ł̃^�[�������Z���Ȃ��Ă��܂��B\n");
			printf("����ł͂���΂��Ă�������!\n");
			c = getchar();
			fflush(stdin);
			legalSelect = true;
		}
		else if (userSelection == '4')	//�f�o�b�O���[�h�ŊJ�n�����Ƃ��̏����A�E�҂̏����ʒu���J�����܂�
		{
			if (debugMode) {
				debugMode = false;
				printf("�f�o�b�O���[�h���I�t�ɂ��܂���\n");
			}
			else {
				debugMode = true;
				printf("�f�o�b�O���[�h���I���ɂ��܂���\n");
			}
			printf("[%d,%d]", enemy.roomX, enemy.roomY);
			c = getchar();
			fflush(stdin);
		}
		else{
			printf("������x��蒼���Ă�������\n");
			printf("====================================================");
		}
	} while (!legalSelect);
}

/*����������
�ŏ��l�A�ő�l�̏��ɓ������߂΂��͈̗̔͂����������1�Ԃ��Ă����֗��}�V��
����F�����喲
*/
int getRandomNumber(int min,int max)
{
	int i = rand() % (max - min + 1) + min;	//�^����ꂽ�ŏ��E�ő�l�͈͓̔��ɂȂ�悤�ɗ�����1����
	return i;								//��������������߂�l�Ƃ��ĕԂ�
}

/*�������\��
�����̒��ɉ������邩�ƗE�҂Ƃ̋�����\������
����F�����喲
*/
void showRoomInfo()
{
	int x = player.roomX;		//��l����X���W
	int y = player.roomY;		//��l����Y���W
//	int distance = checkBraveDistance();
	int distance = checkObjectDistance(x, y, enemy.roomX, enemy.roomY);	//��l���ƗE�҂̋���
	if (dungeon[x][y].hasEnemy){	//�a������Ȃ�
		showText("�G�T�������");
	}
	if (dungeon[x][y].hasItem){		//�T�m�A�C�e��������Ȃ�
		showText("�G�T�̏ꏊ��������A�C�e���������");
	}
	if (dungeon[x][y].hasBrave){	//�E�҂�����Ȃ�
		showText("�E�҂������");
	}
	else{							//�E�҂����Ȃ��Ă������ɂ���Ăǂꂭ�炢�߂���������
		if (!anotherMode) {
			switch (distance)
			{
			case 1:						//����1�Ȃ炷�����߂�
				printf("**********************\n");
				printf("�E�҂͐����߂��ɂ����\n");
				showText("**********************");
				break;
			case 2:						//����2�Ȃ�߂�
				printf("******************\n");
				printf("�E�҂͋߂��ɂ����\n");
				showText("******************");
				break;
			default:					//3�ȏ�Ȃ�߂��ɂ͂��Ȃ�
				showText("�E�҂͋߂��ɂ͂��Ȃ���");
				break;
			}
		}
		else {
			showText("�E�҂͂��̃}�X�ɂ͂��Ȃ���");
		}
	}
}
/*�_���W�����̓����\��
����F�����喲�A�O���D�V��N
*/
void showDungeon()
{
	int x, y;
	if (debugMode){						//�m�F���[�h���̂݁A���ׂĂ̕����̏����J��
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
	// ����������\������
	for (y = 1; y <= 5; y++)
	{

		// 1��ځF�k
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

		// 2��ځF���₷�����邽�߂̋󂫃X�y�[�X
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

		// 3��ځF�G�T
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

		// 4��ځF�A�C�e���ƃv���C���[
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

			// 5��ځF
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
			
			// 6��ځF��
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
	// �L���̐���
	printf("P = �v���C���[");
	printf("�@I = �A�C�e��");
	printf(" �@& = �G�T \n");
	printf("w:��,a:��,s:��,d:�E\n");
	printf("�H�ׂ��G�T�̐��F%d��\n", player.getFeed);
	
}

/*���[�U�[����ړ���������͂��Ă��炤�֐�
����F�����喲
*/
int getUserMove()
{
	int direction = -1;		//���͂��ꂽ����
	bool legalDirection = false;		//�������K�����̊m�F�p
	char c;					//���͂��ꂽ������ۊǂ��Ă����ϐ�
	int x = player.roomX;	//�v���C���[��X���W
	int y = player.roomY;	//Y���W

	if (player.getFeed >= 6){
		player.lastMove--;
		printf("����%d�������܂�\n",player.lastMove);
	}

	if (player.lastMove > 0){
		while (!legalDirection)	//�K�؂ȕ��������͂���Ă��Ȃ��Ԃ��̒������[�v��������
		{
			printf("�s��������������͂��Ă�(W,A,S,D): ");	//��������
			c = getchar();
			scanf_s("%*c");

			fflush(stdin);			//2�����ȏ���͂���Ƃ��̌�����ړ��ł��Ȃ��Ȃ�s����m�F�����̂ł��̑΍�AEnter1��ł悭�Ȃ���

			switch (c)
			{
			case 'W':					//W�͖k
			case 'w':
				if (y - 1 > 0){
					direction = North;
					legalDirection = true;
				}
				else{
					printf("�����Ȃ�\n");
				}
				break;
			case 'A':					//A�͐�
			case 'a':
				if (x - 1 > 0){
					direction = West;
					legalDirection = true;
				}
				else{
					printf("�����Ȃ�\n");
				}
				break;
			case 'S':					//S�͓�
			case 's':
				if (y + 1 < 6){
					direction = South;
					legalDirection = true;
				}
				else{
					printf("�����Ȃ�\n");
				}
				break;
			case 'D':					//D�͓�
			case 'd':
				if (x + 1 < 6){
					direction = East;
					legalDirection = true;
				}
				else{
					printf("�����Ȃ�\n");
				}
				break;
			default:					//����ȊO�͂�蒼��
				printf("����Ȃ��̂Ȃ�\n");
				break;
			}

		}
	}
	else{
		GameFinished = true;
	}
	return direction;
}

/*�E�҂̈ړ����������肷��֐�
����F�����喲
*/
int getBraveMove()
{
	int direction = -1;					//���肵���ړ�����
	int c;								//�E�҂̓��͕����͗����Ō���
	bool legalDirection = false;		//���̕����ɍs���邩�ǂ����̔���
	int x = enemy.roomX;				//�E�҂�X���W
	int y = enemy.roomY;				//Y���W
	int px = player.roomX;				//��l����X���W
	int py = player.roomY;				//Y���W
	while (!legalDirection)				//�K�؂ȕ����ɂȂ�܂Ń��[�v
	{
		c = getRandomNumber(1, 4);
		switch (c)
		{
		case 1:
			if (y - 1 > 0){			//1�Ȃ�k
				if (checkObjectDistance(x, y - 1, px, py) <= BraveDistance){					//�ړ���̎�l���Ƃ̋�������4�ȉ��Ȃ炻�̕����Ɉړ�
					direction = North;
					legalDirection = true;
				}
				else if (checkObjectDistance(x, y - 1, px, py) < checkObjectDistance(x, y, px, py)){	//�����łȂ��ꍇ�͋������k�܂�����Ȃ�ړ�
					direction = North;
					legalDirection = true;
				}
			}
			break;
		case 2:							//2�Ȃ琼
			if (x - 1 > 0){
				if (checkObjectDistance(x - 1, y, px, py) <= BraveDistance){					//�ړ���̎�l���Ƃ̋�������4�ȉ��Ȃ炻�̕����Ɉړ�
					direction = West;
					legalDirection = true;
				}
				else if (checkObjectDistance(x - 1, y, px, py) < checkObjectDistance(x, y, px, py)){	//�����łȂ��ꍇ�͋������k�܂�����Ȃ�ړ�
					direction = West;
					legalDirection = true;
				}
			}
			break;
		case 3:							//3�Ȃ��
			if (y + 1 < 6){
				if (checkObjectDistance(x, y + 1, px, py) <= BraveDistance){					//�ړ���̎�l���Ƃ̋�������4�ȉ��Ȃ炻�̕����Ɉړ�
					direction = South;
					legalDirection = true;
				}
				else if (checkObjectDistance(x, y + 1, px, py) < checkObjectDistance(x, y, px, py)){	//�����łȂ��ꍇ�͋������k�܂�����Ȃ�ړ�
					direction = South;
					legalDirection = true;
				}
			}
			break;
		case 4:							//4�Ȃ瓌
			if (x + 1 < 6){
				if (checkObjectDistance(x + 1, y, px, py) <= BraveDistance){					//�ړ���̎�l���Ƃ̋�������4�ȉ��Ȃ炻�̕����Ɉړ�
					direction = East;
					legalDirection = true;
				}
				else if (checkObjectDistance(x + 1, y, px, py) < checkObjectDistance(x, y, px, py)){	//�����łȂ��ꍇ�͋������k�܂�����Ȃ�ړ�
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
/*�v���C���[�̈ړ�
����F���n���h�N
*/
void movePlayer(int direction)
{
	int currentX = player.roomX;
	int currentY = player.roomY;
	int newX = 3;
	int	newY = 3;
	//��l�������̕����������
	dungeon[currentX][currentY].hasPlayer = false;
	//�@�V����������X����Y�����������l��
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

	// �V�����ꏊ�̏���ǉ�
	player.roomX = newX;
	player.roomY = newY;
	dungeon[newX][newY].hasPlayer = true;
	dungeon[newX][newY].PlayerVisited = true;
}

// �쐬�F���J�@�F��
// �E�҂��ړ������ۂ̏���
void braveMove(int direction)
{
	int currentX = enemy.roomX;
	int currentY = enemy.roomY;
	int newX = enemy.roomX;
	int	newY = enemy.roomY;

	//�@�E�҂����̕����������
	dungeon[currentX][currentY].hasBrave = false;

	//�@�V����������X����Y�����������l��
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

	// �V�����ꏊ�̏���ǉ�
	enemy.roomX = newX;
	enemy.roomY = newY;
	dungeon[newX][newY].hasBrave = true;
	if (debugMode){
		printf("[%d,%d]\n", newX, newY);
	}
}


//�E�҂Ƒ����������@(���C�@��P)
void braveEncount(void){
	int px = player.roomX;
	int py = player.roomY;
	bool legalInitBrave = false;
	if (dungeon[px][py].hasBrave) {   //�v���C���[�ƓG�̍��W���d�Ȃ����Ƃ�
		player.moveRight = true;	//�ړ����̃��Z�b�g���s��
		if (player.getFeed <= 5) {   //�G�T��5�ȉ��̂Ƃ�
			player.encountNum++;   //�o�b�h�G���J�E���g����1���₷
			if (player.encountNum <= 1){
				printf("�܂��p���[�A�b�v������Ă��Ȃ��̂ɗE�҂Ƒ������Ă��܂����B\n");
				printf("���Ƃ������؂ꂽ���ǎ��͂Ȃ����낤�c\n");
				printf("�c��̃G�T���W�߂ăp���[�A�b�v���Ȃ���!\n");
				do{
					dungeon[px][py].hasBrave = false;					//���̕�������E�҂�������
					initialiseEnemy();									//�E�҂��Ĕz�u
					dungeon[enemy.roomX][enemy.roomY].hasBrave = true;	//�E�҂̓]����ɗE�҂�z�u����
					if (!dungeon[px][py].hasBrave){						//��l���ƗE�҂������}�X�ɋ��Ȃ��Ȃ烋�[�v�E�o
						legalInitBrave = true;							//����Ȃ�܂����[�v���s���A�E�҂������̂�do��1�s�ڂ�OK
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
		else if (player.getFeed >= 6) {   //�G�T��6�ȏ�̂Ƃ�
			braveBattle();
		}
	}
}

// �G���f�B���O�e�L�X�g 
//�O������������
void getClearEvent()
{
	showText("**********************************************************", SleepTime * 2);
	if (!vsBrave) {
		if (player.getFeed <= 5) {
			showText("�ĂїE�҂ɑ������Ă��܂����c\n", SleepTime * 2);
			showText("���̗͂ł͂܂��E�҂ɑ����ł��ł��Ȃ��c\n", SleepTime * 2);
			printf("�v���C���[�͗͐s�����c\n\n");
		}
		else if (player.getFeed <= 6) {
			showText("�\���ɗ͂��������ƂŗE�҂��_���W��������ǂ��o�����Ƃɐ��������B\n", SleepTime * 2);
			showText("�������d���߂邱�Ƃ��o���Ȃ������̂ōĂїE�҂��_���W�����ɍU�߂Ă��邱�Ƃ����邩������Ȃ��B\n", SleepTime * 2);
			printf("�����Ƃ肠��������͂���ŗǂ��Ƃ��悤�B\n\n");
		}
		else {
			showText("���Ă̎��������͂����邱�Ƃ��o�����c\n", SleepTime * 2);
			printf("�E�҂��Ƃ炦�邱�Ƃ͏o���Ȃ��������������̃_���W�����ɗ��邱�Ƃ͖������낤�c\n\n");
		}
	}
	else {
		if (braveWins == 3) {
			showText("�E�҂�H�ׂ邱�ƂōX�Ȃ�͂���ɓ��ꂽ�B\n", SleepTime * 2);
			showText("���̎����ɂ��Ȃ����̂͒N�����Ȃ��c\n", SleepTime * 2);
			printf("�v���C���[�͐��E�𐪕������B\n\n");
			clearFlag = true;
		}
		else {
			showText("�E�҂̎���˔j���邱�Ƃ��o���Ȃ������B\n", SleepTime * 2);
			showText("�E�҂ɓ������Ă��܂����B\n", SleepTime * 2);
			printf("�_���W��������ꂽ�̂ō���͂���ŗǂ��Ƃ��悤�B\n\n");
		}
	}

	Sleep(SleepTime * 1000 * 2);
	showText("����ł��̃Q�[���͂����܂��B\n", SleepTime * 2);
	printf("���񂫂�`�@�@�ӂ��`�@�@�Ղꂢ�񂮁@\n\n");
	if (clearFlag) {
		if (easyMode){
			Sleep(SleepTime * 1000 * 2);
			printf("���̓m�[�}�����[�h�Œ��킵�Ă݂悤�I\n\n");
		}
		else{
			Sleep(SleepTime * 1000 * 2);
			printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			printf("@���܂��F�ŏ��̉�ʂ�*�������Ɓc�H@\n");
			printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		}
	}
}

/*�E�҂Ƃ̋����𑪂�֐�
��������checkObjectDistance�ɍ����ւ��ɂȂ�
����F�����喲
*/
int checkBraveDistance()
{
	int px, py, bx, by;
	px = player.roomX;		//��l����X���W���擾
	py = player.roomY;		//Y���W
	bx = enemy.roomX;		//�E�҂�X���W
	by = enemy.roomY;		//Y���W
	int dx = abs(px - bx);	//��l����X���W�ƗE�҂�X���W�̍��̐�Βl�����߂�
	int dy = abs(py - by);	//�������Ƃ�Y���W�ł��s��
	return dx + dy;			//X���W�̍��̐�Βl��Y���W�̍��̐�Βl�̘a��Ԃ�
}

/*2�_�Ԃ̋����������߂�֐�
checkBraveDistance��ėp���������č��܂���
���������Ȃ������Ȃ̂ō���͂��������g������
����F�����喲
*/
int checkObjectDistance(int x1, int y1, int x2, int y2){
	int dx = abs(x1 - x2);
	int dy = abs(y1 - y2);
	return dx + dy;
}

/*�ł��߂��G�T�̍��W��Ԃ��A�C�e���̏���
�������̕ǂł͂Ȃ��}�~�X�œ������Ă܂���
����F�����喲
*/
void tryItem(){
	int px, py, fx, fy;
	int dsum;
	int dmin = 100;
	px = player.roomX;
	py = player.roomY;
	fx = 1;
	fy = 1;
	if(dungeon[px][py].hasItem){				//��l���̂���}�X�ɃA�C�e��������Ȃ�
		showText("�i���ɗ����������ȁc�����g���Ă݂�Ƃ��悤�j");
		for (int i = 1; i <= 5; i++){
			for (int j = 1; j <= 5; j++){
				if (dungeon[i][j].hasEnemy){	//1�������ƂɃG�T�����邩�̊m�F���s��
					dsum = checkObjectDistance(i, j, px, py);				//�G�T�̃}�X�Ƃ̋��������߂�
					if (dsum <= dmin){			//���s�̍ŏ��l�ȉ��Ȃ�
						fx = i;					//X���W�AY���W�A�������X�V
						fy = j;
						dmin = dsum;
					}
				}
			}
		}
		dungeon[px][py].hasItem = false;		//�}�X����A�C�e����������
		printf("�ł��߂��G�T�͉�%d,�c%d�ɂ����\n", fx, fy);		//�ł��߂��G�T�̍��W��\��
	}
}

/*�a�擾�̂��߂̃^�C�s���O�p�[�g
���ł̃\�[�X�������̊֐���p�����^�C�v�ɂȂ�������
�����������l�̕�����
�����Č����炠�������̊֐��̓^�C�s���O�̎��s�`�F�b�N�y�ё��֐��̌Ăяo�����Ɖ����̂�����
����F�����喲
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

/*�^�C�s���O�Q�[���̓�Փx����֐�
���܂łɏW�߂��a��2�����Ȃ�easy
5�����Ȃ�normal
5�ȏ�Ȃ�hard
����F�����喲
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
		showText("<�X���C�������ꂽ�I>");
		showText("�u���O�Ȃ񂩂ɐH�ׂ��Ă��܂邩�c�Ԃ蓢���ɂ��Ă��I!�v");
	}
	else if (level == normal) {
		showText("<�S�u���������ꂽ�I>");
		showText("�u���H �񂾂Ă߂�...���̂�����!!�Ԃ��E���Ă��!!!�v");
	}
	else {
		showText("<���l�������ꂽ�I>");
		showText("�u�͂Ȃ������Ȏ҂�...��ɒ��ނƂ͂����x�����B��������Ă�낤�I�v");
	}
}

/*�^�C�s���O�p�[�g
�ύX�_�͊J�n�O��ready������ŔC�ӂɎn�߂���悤�ɂȂ����_�A���ʂ�typingResult�ɕۑ����Ă���_
����F�����喲
*/

void typingPart(){
	char c, userSelection;

	time_t s_time, e_time, n_time;	//�����珇�ɊJ�n���ԁA�I�����ԁA���ݎ��Ԃ��i�[
	bool collectJudge = false;		//���딻��
	int spellNum;					//��Փx���Ƃ̖��̐����i�[
	int questionNum;				//�o��ԍ�
	int defTime;					//����̐�������
	char  * qEasy[] = {				//�Ղ����̎��̖��A���݂�150��	(�O���N����)
		"jazzy", "fuzzy", "muzzy", "whizz", "fezzy", "fizzy", "abuzz", "zuzim", "scuzz", "dizzy", "frizz", "huzza", "mezzo", "pizza", "jujus", "tizzy", "hajji", "zazen", "zanza", "zizit",
		"jumpy", "tazza", "jacky", "tazze", "zappy", "jimmy", "jimpy", "jiffy", "zippy", "jemmy", "quick", "jammy", "quack", "junky", "mujik", "jocko", "jugum", "zaxes", "zinky", "jumps",
		"jumbo", "kudzu", "quiff", "jocks", "kopje", "jacks", "quaky", "quaff", "jerky", "jibbs", "pazazz", "pizazz", "pizzaz", "jazzbo", "bezazz", "jazzed", "zizzle", "jazzes", "jazzer", "muzjik",
		"whizzy", "mizzly", "scuzzy", "fuzzed", "puzzle", "muzzle", "buzzed", "huzzah", "frizzy", "jujube", "mizzen", "fizzed", "fuzzes", "fizzle", "mizzle", "mezuza", "zigzag", "buzzes", "buzzer", "pizzle",
		"guzzle", "fezzed", "wizzen", "hazzan", "fezzes", "wizzes", "bizzes", "cozzes", "fizzer", "fizzes", "huzzas", "queazy", "nuzzle", "mezzos", "snazzy", "jojoba", "piazza", "pizzas", "piazze", "banjax",
		"pizzazz", "zyzzyva", "pizazzy", "jacuzzi", "jazzmen", "jazzman", "jazzing", "jazzily", "jazzbos", "zizzled", "quizzed", "quizzer", "zizzles", "quizzes", "buzzwig", "jazzers", "jazzier", "buzzcut", "muzjiks", "muzzily",
		"fuzzily", "schizzy", "fuzzing", "buzzing", "mezuzah", "muzzled", "whizzed", "jumbuck", "jejunum", "schnozz", "jimjams", "jukebox", "chazzan", "fizzing", "frizzly", "chazzen", "puzzled", "muezzin", "muzzles", "fizzled",
		"huzzahs", "whizzer", "grizzly", "jujuism", "buzzard", "puzzler", "muzzler", "jujubes", "mizzled", "puzzles"
	};

	char* qNormal[] = {				//���ʂ̎��̖��A100��(���n���h�N����)
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

	char * qHard[] = {				//����̎��̖��A100��(���C�N�A���J�N����)
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
	char estr[100];					//�^�C�v���ꂽ��������i�[���Ă����z��

	printf("<Enter��1�񉟂�����o�肳��܂��B>\n");			//��������P�\���ԃp�[�g

	c = getchar();
	userSelection = c;
	fflush(stdin);
															//�P�\���ԃp�[�g�����܂�

	time(&s_time);					//�J�n���Ԃ��l��
	switch (level)					//��Փx���Ƃɐ������ԁA��萔���擾
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

	questionNum = getRandomNumber(0, spellNum - 1);		//�����擾�A�O����������������𗬗p���Ă܂�

	while (true){
		time(&n_time);				//���ݎ��Ԃ��擾

		while (e_time >= n_time){	//���ݎ��Ԃ��I�����Ԉȉ��Ȃ�
			printf("����%d�b\n", (int)(e_time - n_time));	//�c�莞�Ԃ�\��
			switch (level){			//��Փx���Ƃɕ���A��{�͖��\�����������́����딻��ƌJ��Ԃ�
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
						printf("����%d�b\n", (int)(e_time - n_time));
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
						printf("����%d�b\n", (int)(e_time - n_time));
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
						printf("����%d�b\n", (int)(e_time - n_time));
					}
				}
				break;
			}
			if (collectJudge){		//���������猻�ݎ��Ԃ�ۑ�����While���[�v�𔲂��o��
				time(&n_time);
				break;
			}
		}
		if (collectJudge){
			printf("�����I\n");
			printf("�c��%d�b\n", (int)(e_time - n_time));
			if ((int)(e_time - n_time) > defTime / 2){
				collectTime--;
				printf("������-1�b\n");
			}
			typingResult = monsterWin;
			break;
		}
		else{
			printf("���Ԑ؂�\n");
			collectTime++;
			printf("������+1�b\n");
			typingResult = monsterLose;
			break;
		}
	}
}

/*�^�C�s���O�Q�[���I����̏���
�������A���s���̏������܂Ƃ߂ď����Ă��܂�
����F�����喲
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
		printf("�a�̐�+1,����%d��\n", player.getFeed);
		if (player.getFeed >= 7) {
			GameFinished = true;
		}
		printf("<�퓬�ɏ��������B>\n");
		printf("<�v���C���[�̓����X�^�[��H�ׂăp���[�A�b�v�����B>\n");
		if (player.getFeed <= 2) {
			showText("�i�܂��܂��H�ׂ���Ȃ��ȁc�����ƃG�T���W�߂Ȃ���c�j");
		}
		else if (player.getFeed <= 5) {
			showText("�i���Ȃ�͂��߂��Ă����ȁc�@���Ƃ����������I�j");
		}
		else if (player.getFeed <= 6){
			showText("�i�\���ɗ͂����߂������A���ƈ�C�G�T�H�ׂ邱�Ƃ��ł���ΗE�҂����|�ł��������c�j");
		}
//		c = getchar();
//		scanf_s("%*c");
//		fflush(stdin);
		break;
	case monsterLose:
		printf("<�����X�^�[�Ƃ̃o�g���ɕ����Ă��܂����c>\n");
		if (!anotherMode) {
			while (!legalAnswer) {
				printf("���̃}�X�ɗ��܂�܂����H\n���܂����ꍇ�A���̃^�[���ɓ����a�Ƃ̍Đ���s�����Ƃ��o���܂�\n1)�͂�\n2)������\n");
				c = getchar();
				userSelection = c;
				scanf_s("%*c");
				fflush(stdin);

				switch (userSelection) {
				case '1':
					printf("���̃^�[��������x�킢�܂�\n");
					player.moveRight = false;
					legalAnswer = true;
					break;
				case '2':
					printf("�����𗣂�ĕʂ̃}�X�Ɉړ����܂�\n");
					legalAnswer = true;
					break;
				default:
					printf("�������������̂ł������\n");
					break;
				}
			}
		}
		break;
	}
}

/*�E�Ґ�̊֐�
�E�Ґ�Ɉڂ������
�@�G�T��6�ȏ�H�ׂ�
�A�E�҂ƐڐG����
��Փxhard�Œ�A3�A�킵�܂�
����F�����喲
*/
void braveBattle() {
	vsBrave = true;
	if (easyMode){						//�C�[�W�[���[�h�Ɍ���normal�ŘA�킵�܂�
		level = normal;
	}
	else{
		level = hard;
	}
	printf("******************************************************************\n");
	showText("<�E�҂����ꂽ�I>");
	showText("�u����ȂƂ���œ|�����킯�ɂ͂����Ȃ��c��΂ɐ����c��񂾁c�I�v");
	showText("******************************************************************");
	for (int i = 0; i < 3; i++) {
		if (!anotherMode && !easyMode){
			collectTime = -1;			//�K�莞�Ԃ�8�b�ƍ��킹��7�b�ɂȂ�悤����
		}
		else if(anotherMode){
			collectTime = -2;			//�A�i�U�[��6�b�ɐݒ�
		}
		else if(easyMode){
			collectTime = 0;			//�C�[�W�[��8�b�ɐݒ�
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

/*��������f�B���C�t���ŏo�͂���֐�2�Z�b�g
���̓s�xSleep�����̂��߂�ǂ������ȂƂ������z���琶�܂ꂽ���C�̎Y��
!!!!!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!
���̊֐��͐��l���擾���ďo�͂���A�Ƃ�������ɂ͑Ή����Ă��܂���
	printf("%d\n",player.getFeed);
���̂悤�ȏo�͂ɂ͑Ή��ł��Ȃ����ߒ��߂ĕ��ʂ�printf�̌��Sleep�����đΉ����Ă�������
����F�����喲
*/
void showText(const char text[]) {
	printf("%s\n", text);
	Sleep(SleepTime * 1000);
}
/*��������f�B���C�t���ŏo�͂���֐�2�Z�b�g
���̓s�xSleep�����̂��߂�ǂ������ȂƂ������z���琶�܂ꂽ���C�̎Y��
!!!!!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!
���̊֐��͐��l���擾���ďo�͂���A�Ƃ�������ɂ͑Ή����Ă��܂���
printf("%d\n",player.getFeed);
���̂悤�ȏo�͂ɂ͑Ή��ł��Ȃ����ߒ��߂ĕ��ʂ�printf�̌��Sleep�����đΉ����Ă�������
����F�����喲
*/
void showText(const char text[], int Time) {
	printf("%s\n", text);
	Sleep(Time * 1000);
}
