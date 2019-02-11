#pragma once
// Samsung Go Tournament Form C Connect6Algo Header (g++-4.8.3)

// <--------------- 이 Code를 수정하면  작동하지 않을 수 있습니다 ------------------>
#include <vector>
using namespace std;
#pragma once
#pragma warning(disable:4996)
extern int width, height;
extern int terminateAI;
extern unsigned s_time;

extern FILE *fp;


extern int isFree(int x, int y);
extern int showBoard(int x, int y);
extern void init();
extern void restart();
extern void mymove(int x[], int y[], int cnt = 2);
extern void opmove(int x[], int y[], int cnt = 2);
extern void block(int, int);
extern int setLine(char *fmt, ...);
extern void domymove(int x[], int y[], int cnt = 2);
extern void writeLog(char *log);
extern void test();
//extern void stoneInit(int x[], int y[], int cnt = 2);

void myturn(int cnt = 2);
char info[];

typedef pair<int, int> PAIR;

extern void update_connected_stone_dir(int x, int y, int len, int direc);
extern void put_stone_dir(int x, int y, int WB);
extern void update_op_stone(int x[], int y[], int WB);
pair<int, int>  find_optimal_pos();
pair<int, int> return_position();
extern void op_update();
extern void set_optimal_pos(int x, int y);
extern int get_optimal_pos_x();
extern int get_optimal_pos_y();
extern void op_stat(int x, int y);
void update_my_stone(int x, int y, int WB);
extern void update_opp_block(int x, int y, int len, int r);
extern PAIR get_optimal_pos();
extern bool find_five(vector<PAIR> * list2);
extern bool find_four(vector<PAIR> * list2);
extern bool find_exfour(vector<PAIR> * list2);
extern bool find_three(vector<PAIR> * list2);
extern bool find_two(vector<PAIR> * list2);
extern bool find_one(vector<PAIR> * list2);
extern void pos_score(int x, int y, double price);
extern int return_max_index();
extern bool is_inboard(int x, int y);
extern void rotation_block();
void find_two_x4(vector<PAIR> list);

class Stone {
public:
	int color = 0;
	int max = 0;
	int dir[8];
	bool blocked[8];

	Stone() {
		for (int i = 0; i < 8; i++) {
			dir[i] = 0;
			blocked[i] = false;
		}

	}

	/*
	0 = 위
	1 = 왼쪽 위
	2 = 왼쪽
	3 = 왼쪽 아래
	4 = 아래
	5 = 오른쪽 아래
	6 = 오른쪽
	7 = 오른쪽 위
	*/
};