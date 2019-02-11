// Samsung Go Tournament Form C Connect6Algo (g++-4.8.3)

// <--------------- 이 Code를 수정하면  작동하지 않을 수 있습니다 ------------------>
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Connect6Algo.h"
#include <iostream>
#include <vector>

using namespace std;
unsigned s_time;
int terminateAI;
int width = 19, height = 19;
int cnt = 2;
int counts = 1;

int myColor;
FILE *fp = NULL;
static char cmd[256];
static HANDLE event1, event2;
#define BOARD_SIZE 20
int board[BOARD_SIZE][BOARD_SIZE];
int limitTime = 0;

bool two_check = false;
int op_stone_x[2] = { 20, 20 };
int op_stone_y[2] = { 20, 20 };

bool mode = true;

int optimal_x = 9;
int optimal_y = 9;
int chance = 2;
int four_count = 0;
bool start_attack = false;
int do_first = 2;
int twice = 0;
vector<pair<int, int> > my_list;
vector<PAIR> op_list;
vector<PAIR> block_list;
vector< pair <pair<double, bool >, PAIR> > score;
vector <int> score_count;

PAIR temp_pos = { 8, 9 };

int n = 0;

Stone stone[19][19];
Stone rotate_block[8];
Stone temp;

void set_first() {
	do_first = 1;
}

void set_optimal_pos(int x, int y) {
	optimal_x = x;
	optimal_y = y;
}
int get_optimal_pos_x() {
	return optimal_x;
}
int get_optimal_pos_y() {
	return optimal_y;
}



static void getLine() {
	int c, bytes;



	bytes = 0;
	do {
		c = getchar();
		if (c == EOF) exit(0);
		if (bytes < sizeof(cmd)) cmd[bytes++] = (char)c;
	} while (c != '\n');
	cmd[bytes - 1] = 0;
	if (cmd[bytes - 2] == '\r') cmd[bytes - 2] = 0;
}

int setLine(char *fmt, ...) {
	int i;
	va_list va;
	va_start(va, fmt);
	i = vprintf(fmt, va);
	putchar('\n');
	fflush(stdout);
	va_end(va);
	return i;
}

static const char *getParam(const char *command, const char *input) {
	int n1, n2;
	n1 = (int)strlen(command);
	n2 = (int)strlen(input);
	if (n1 > n2 || _strnicmp(command, input, n1)) return NULL;
	input += strlen(command);
	while (isspace(input[0])) input++;
	return input;
}

static void stop() {
	terminateAI = 1;
	WaitForSingleObject(event2, INFINITE);
}

static void start() {
	s_time = GetTickCount();
	stop();
}

static void turn() {
	terminateAI = 0;
	ResetEvent(event2);
	SetEvent(event1);
}

void domymove(int x[], int y[], int cnt) {
	char buf[200] = { " " };



	mymove(x, y, cnt);

	if (cnt == 1) {
		setLine("%d,%d", x[0], y[0]);
		sprintf_s(buf, "-- output: %d,%d \n", x[0], y[0]);
	}
	else
	{
		setLine("%d,%d %d,%d", x[0], y[0], x[1], y[1]);

		sprintf_s(buf, "-- output: %d,%d %d,%d \n", x[0], y[0], x[1], y[1]);
	}

	//writeLog(buf);

}

int showBoard(int x, int y) {
	return board[x][y];
}


static void doCommand() {
	const char *param;

	

	char buf[200] = { " " };
	sprintf_s(buf, "-- input: %s \n", cmd);
	//writeLog(buf);

	if ((param = getParam("START", cmd)) != 0) {
		start();
		init();
		setLine("OK");

		char buf[200] = { " " };
		sprintf_s(buf, "-- output: %s \n", "OK");
		//writeLog(buf);

	}
	else if ((param = getParam("BEGIN", cmd)) != 0) {
		myColor = 1;
		cnt = 1;
		counts = cnt;
		start();
		turn();
	}
	else if ((param = getParam("TURN", cmd)) != 0) {
		int x[2], y[2], r;
		if (((r = sscanf_s(param, "%d,%d %d,%d", &x[0], &y[0], &x[1], &y[1])) != 4 && r != 2)) {
			setLine("ERROR 형식에 맞지 않는 좌표가 입력되었습니다");

			char buf[200] = { " " };
			sprintf_s(buf, "-- output: %s \n", "ERROR 형식에 맞지 않는 좌표가 입력되었습니다");
			//writeLog(buf);

			//test();
			return;
		}
		else {
			for (int i = 0; i < (r / 2); i++) {
				if (x[i] < 0 || x[i] >= width || y[i] < 0 || y[i] >= height) {
					setLine("ERROR 형식에 맞지 않는 좌표가 입력되었습니다");

					char buf[200] = { " " };
					sprintf_s(buf, "-- output: %s \n", "ERROR 형식에 맞지 않는 좌표가 입력되었습니다");
					//writeLog(buf);

					return;
				}
			}


			cnt = 2;

			opmove(x, y, r / 2);
			turn();


		}
		//test();

	}
	else if ((param = getParam("INFO", cmd)) != 0) {
		setLine("%s", info);

		char buf[200] = { " " };
		sprintf_s(buf, "-- output: %s \n", info);
		//writeLog(buf);
	}
	else if ((param = getParam("BLOCK", cmd)) != 0) {
		int x, y;
		if (((sscanf_s(param, "%d,%d", &x, &y)) == 2)) {
			block(x, y);
			setLine("OK");

			char buf[200] = { " " };
			sprintf_s(buf, "-- output: %s \n", "OK");
			//writeLog(buf);
		}
	}
	else if ((param = getParam("LimitTime", cmd)) != 0) {
		sscanf_s(param, "%d", &limitTime);
	}

	else if ((param = getParam("QUIT", cmd)) != 0) {

		//test();
		exit(0);
	}

}



static DWORD WINAPI threadLoop(LPVOID) {
	while (1) {
		WaitForSingleObject(event1, INFINITE);
		myturn(cnt);
		counts = cnt;
		if (cnt == 1) cnt = 2;
		SetEvent(event2);
	}
}

void writeLog(char *myLog)
{

	//fp = fopen("myLog.txt", "a");
	////errno_t err;
	////err = fopen_s(&fp, "myLog.txt", "a");
	///*if (err == 0)
	//puts("파일오픈 성공!\n");
	//else {
	//puts("파일 오픈 실패\n");

	//}*/
	//if (fp != NULL)
	//{
	//	fprintf(fp, myLog);
	//}

	////fputs("My name is Hong \n", fp);
	////fputs("Your name is Yoon \n", fp);
	////fflush(stdout);
	//fclose(fp);
}

int main() {
	DWORD mode;


	if (GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode))
		puts("직접 실행 불가능한 파일입니다. 육목 알고리즘 대회 툴을 이용해 실행하세요.");

	DWORD tid;
	event1 = CreateEvent(0, FALSE, FALSE, 0);
	CreateThread(0, 0, threadLoop, 0, 0, &tid);
	event2 = CreateEvent(0, TRUE, TRUE, 0);


	//fp = fopen("myLog.txt", "a");
	//errno_t err;
	//err = fopen_s(&fp, "myLog.txt", "a");
	/*if (err == 0)
	puts("파일오픈 성공!\n");
	else {
	puts("파일 오픈 실패\n");

	}*/
	//if (fp != NULL)
	//{
	//	fprintf(fp, "test\n");
	//}

	////fputs("My name is Hong \n", fp);
	////fputs("Your name is Yoon \n", fp);
	////fflush(stdout);
	//fclose(fp);
	while (1) {

		getLine();
		doCommand();
	}

	return 0;
}

int isFree(int x, int y)
{
	return x >= 0 && y >= 0 && x < width && y < height && board[x][y] == 0;
}
bool is_inboard(int x, int y) {
	return x >= 0 && y >= 0 && x < width && y < height;
}

void init() {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			board[i][j] = 0;
		}
	}
}

void update_connected_stone_dir(int x, int y, int len, int direc) {
	if (direc == 4) {  //새로운 돌 위의 기존돌 dir update.
		for (int i = 1; i < len; i++) {
			//stone[x][y - i].dir[direc] += 1;
			stone[x][y - i].dir[direc] = stone[x][y - i + 1].dir[direc] + 1;
			stone[x][y - i].blocked[direc] = stone[x][y - i + 1].blocked[direc];

			if (stone[x][y - i].max < stone[x][y - i].dir[direc]) {
				stone[x][y - i].max = stone[x][y - i].dir[direc];
			}
		}
	}
	if (direc == 5) { //새로운 돌 왼쪽 위의 기존돌 dir update.
		for (int i = 1; i < len; i++) {
			stone[x - i][y - i].dir[direc] = stone[x - i + 1][y - i + 1].dir[direc] + 1;
			stone[x - i][y - i].blocked[direc] = stone[x - i + 1][y - i + 1].blocked[direc];

			if (stone[x - i][y - i].max < stone[x - i][y - i].dir[direc]) {
				stone[x - i][y - i].max = stone[x - i][y - i].dir[direc];
			}
		}
	}
	if (direc == 6) { //새로운 돌 왼쪽의 기존돌 dir update.
		for (int i = 1; i < len; i++) {
			//stone[x - i][y].dir[direc] += 1;
			stone[x - i][y].dir[direc] = stone[x - i + 1][y].dir[direc] + 1;
			stone[x - i][y].blocked[direc] = stone[x - i + 1][y].blocked[direc];

			if (stone[x - i][y].max < stone[x - i][y].dir[direc]) {
				stone[x - i][y].max = stone[x - i][y].dir[direc];
			}
		}
	}
	if (direc == 7) { //새로운 돌 왼쪽 아래의 기존돌 dir update.
		for (int i = 1; i < len; i++) {
			stone[x - i][y + i].dir[direc] = stone[x - i + 1][y + i - 1].dir[direc] + 1;
			stone[x - i][y + i].blocked[direc] = stone[x - i + 1][y + i - 1].blocked[direc];

			if (stone[x - i][y + i].max < stone[x - i][y + i].dir[direc]) {
				stone[x - i][y + i].max = stone[x - i][y + i].dir[direc];
			}
		}
	}
	if (direc == 0) { //새로운 돌 아래의 기존돌 dir update.
		for (int i = 1; i < len; i++) {
			//stone[x ][y+i].dir[direc] += 1;
			stone[x][y + i].dir[direc] = stone[x][y + i - 1].dir[direc] + 1;
			stone[x][y + i].blocked[direc] = stone[x][y + i - 1].blocked[direc];

			if (stone[x][y + i].max < stone[x][y + i].dir[direc]) {
				stone[x][y + i].max = stone[x][y + i].dir[direc];
			}
		}
	}
	if (direc == 1) { //새로운 돌 오른쪽 아래의 기존돌 dir update.
		for (int i = 1; i < len; i++) {
			stone[x + i][y + i].dir[direc] = stone[x + i - 1][y + i - 1].dir[direc] + 1;
			stone[x + i][y + i].blocked[direc] = stone[x + i - 1][y + i - 1].blocked[direc];


			if (stone[x + i][y + i].max < stone[x + i][y + i].dir[direc]) {
				stone[x + i][y + i].max = stone[x + i][y + i].dir[direc];
			}
		}
	}
	if (direc == 2) { //새로운 돌 오른쪽의 기존돌 dir update.
		for (int i = 1; i < len; i++) {
			//stone[x + i][y].dir[direc] += 1;
			stone[x + i][y].dir[direc] = stone[x + i - 1][y].dir[direc] + 1;
			stone[x + i][y].blocked[direc] = stone[x + i - 1][y].blocked[direc];

			if (stone[x + i][y].max < stone[x + i][y].dir[direc]) {
				stone[x + i][y].max = stone[x + i][y].dir[direc];
			}
		}
	}
	if (direc == 3) { //새로운 돌 오른쪽 위의 기존돌 dir update.
		for (int i = 1; i < len; i++) {
			stone[x + i][y - i].dir[direc] = stone[x + i - 1][y - i + 1].dir[direc] + 1;
			stone[x + i][y - i].blocked[direc] = stone[x + i - 1][y - i + 1].blocked[direc];


			if (stone[x + i][y - i].max < stone[x][y - i].dir[direc]) {
				stone[x + i][y - i].max = stone[x][y - i].dir[direc];
			}
		}
	}

}


void put_stone_dir(int x, int y, int WB) {
	int check = (WB % 2) + 1;

	if (!block_list.empty()) {
		if (stone[block_list.front().first][block_list.front().second].color != WB)
			rotation_block();
	}


	for (int r = 0; r < 8; r++) {
		stone[x][y].dir[r] = 1;
		switch (r) {

		case 0:  // 기존 돌 아래에 새로운 돌 놓기.   dir=↑
			if (y - 1 >= 0) {
				if (stone[x][y - 1].color == WB) {
					stone[x][y].dir[r] += stone[x][y - 1].dir[r];
					stone[x][y].blocked[r] = stone[x][y - 1].blocked[r];
					//update_connected_stone_dir(x, y, stone[x][y].dir[r], r + 4);


				}
				else if (stone[x][y - 1].color == check) {
					stone[x][y].blocked[r] = true;
					stone[x][y - 1].blocked[r + 4] = true;
					update_opp_block(x, y - 1, stone[x][y - 1].dir[r], r);

				}
				if (board[x][y - 1] == 3) {
					rotation_block();
					stone[x][y - 1].blocked[r + 4] = true;
					update_connected_stone_dir(x, y - 1, stone[x][y - 1].dir[r], r + 4);
					rotation_block();
				}

			}
			else {
				stone[x][y].blocked[r] = true;
			}
			break;
		case 1: // 기존 돌 오른쪽 아래에 새로운 돌 놓기.    dir=↖
			if (y - 1 >= 0 && x - 1 >= 0) {
				if (stone[x - 1][y - 1].color == WB) {
					stone[x][y].dir[r] += stone[x - 1][y - 1].dir[r];
					stone[x][y].blocked[r] = stone[x - 1][y - 1].blocked[r];
					//update_connected_stone_dir(x, y, stone[x][y].dir[r], r + 4);

				}
				else if (stone[x - 1][y - 1].color == check) {
					stone[x][y].blocked[r] = true;
					stone[x - 1][y - 1].blocked[r + 4] = true;
					update_opp_block(x - 1, y - 1, stone[x - 1][y - 1].dir[r], r);
				}
				if (board[x - 1][y - 1] == 3) {
					rotation_block();
					stone[x - 1][y - 1].blocked[r + 4] = true;
					update_connected_stone_dir(x - 1, y - 1, stone[x - 1][y - 1].dir[r], r + 4);
					rotation_block();
				}

			}
			else {
				stone[x][y].blocked[r] = true;
			}
			break;
		case 2: //기존 돌 오른쪽에 새로운 돌 놓기.   dir = ←
			if (x - 1 >= 0) {
				if (stone[x - 1][y].color == WB) {
					stone[x][y].dir[r] += stone[x - 1][y].dir[r];
					stone[x][y].blocked[r] = stone[x - 1][y].blocked[r];



					//update_connected_stone_dir(x, y, stone[x][y].dir[r], r + 4);
				}
				else if (stone[x - 1][y].color == check) {
					stone[x][y].blocked[r] = true;
					stone[x - 1][y].blocked[r + 4] = true;
					update_opp_block(x - 1, y, stone[x - 1][y].dir[r], r);
				}
				if (board[x - 1][y] == 3) {
					rotation_block();
					stone[x - 1][y].blocked[r + 4] = true;
					update_connected_stone_dir(x - 1, y, stone[x - 1][y].dir[r], r + 4);
					rotation_block();
				}
			}
			else {
				stone[x][y].blocked[r] = true;
			}

			break;
		case 3: //기존 돌 오른쪽 위에 새로운 돌 놓기.  dir = ↙
			if (x - 1 >= 0 && y + 1 < height) {
				if (stone[x - 1][y + 1].color == WB) {
					stone[x][y].dir[r] += stone[x - 1][y + 1].dir[r];
					stone[x][y].blocked[r] = stone[x - 1][y + 1].blocked[r];




					//update_connected_stone_dir(x, y, stone[x][y].dir[r], r + 4);
				}
				else if (stone[x - 1][y + 1].color == check) {
					stone[x][y].blocked[r] = true;
					stone[x - 1][y + 1].blocked[r + 4] = true;
					update_opp_block(x - 1, y + 1, stone[x - 1][y + 1].dir[r], r);
				}
				if (board[x - 1][y + 1] == 3) {
					rotation_block();
					stone[x - 1][y + 1].blocked[r + 4] = true;
					update_connected_stone_dir(x - 1, y + 1, stone[x - 1][y + 1].dir[r], r + 4);
					rotation_block();
				}

			}
			else {
				stone[x][y].blocked[r] = true;
			}

			break;
		case 4: //기존 돌 위에 놓일 경우 dir = ↓
			if (y + 1 < height) {
				if (stone[x][y + 1].color == WB) {
					stone[x][y].dir[r] += stone[x][y + 1].dir[r];
					stone[x][y].blocked[r] = stone[x][y + 1].blocked[r];
					//update_connected_stone_dir(x, y, stone[x][y].dir[r], r - 4);

				}
				else if (stone[x][y + 1].color == check) {
					stone[x][y].blocked[r] = true;
					stone[x][y + 1].blocked[r - 4] = true;
					update_opp_block(x, y + 1, stone[x][y + 1].dir[r], r);
				}

				if (board[x][y + 1] == 3) {
					rotation_block();
					stone[x][y + 1].blocked[r - 4] = true;
					update_connected_stone_dir(x, y + 1, stone[x][y + 1].dir[r], r - 4);
					rotation_block();
				}
			}
			else {
				stone[x][y].blocked[r] = true;
			}

			break;
		case 5: //기존 돌 왼쪽 위에 놓인 경우 dir = ↘
			if (y + 1 < height && x + 1 < width) {
				if (stone[x + 1][y + 1].color == WB) {
					stone[x][y].dir[r] += stone[x + 1][y + 1].dir[r];
					stone[x][y].blocked[r] = stone[x + 1][y + 1].blocked[r];
					//update_connected_stone_dir(x, y, stone[x][y].dir[r], r - 4);



				}
				else if (stone[x + 1][y + 1].color == check) {
					stone[x][y].blocked[r] = true;
					stone[x + 1][y + 1].blocked[r - 4] = true;
					update_opp_block(x + 1, y + 1, stone[x + 1][y + 1].dir[r], r);
				}
				if (board[x + 1][y + 1] == 3) {
					rotation_block();
					stone[x + 1][y + 1].blocked[r - 4] = true;
					update_connected_stone_dir(x + 1, y + 1, stone[x + 1][y + 1].dir[r], r - 4);
					rotation_block();
				}

			}
			else {
				stone[x][y].blocked[r] = true;
			}

			break;
		case 6: //기존 돌 왼쪽에 놓인 경우 dir = →
			if (x + 1 < width) {
				if (stone[x + 1][y].color == WB) {
					stone[x][y].dir[r] += stone[x + 1][y].dir[r];
					stone[x][y].blocked[r] = stone[x + 1][y].blocked[r];
					//update_connected_stone_dir(x, y, stone[x][y].dir[r], r - 4);


				}
				else if (stone[x + 1][y].color == check) {
					stone[x][y].blocked[r] = true;
					stone[x + 1][y].blocked[r - 4] = true;
					update_opp_block(x + 1, y, stone[x + 1][y].dir[r], r);
				}
				if (board[x + 1][y] == 3) {
					rotation_block();
					stone[x + 1][y].blocked[r - 4] = true;
					update_connected_stone_dir(x + 1, y, stone[x + 1][y].dir[r], r - 4);
					rotation_block();


				}
			}
			else {
				stone[x][y].blocked[r] = true;
			}

			break;
		case 7: //기존 돌 왼쪽 아래에 놓인 경우 dir = ↗
			if (x + 1 < width && y - 1 >= 0) {
				if (stone[x + 1][y - 1].color == WB) {
					stone[x][y].dir[r] += stone[x + 1][y - 1].dir[r];
					stone[x][y].blocked[r] = stone[x + 1][y - 1].blocked[r];
					//update_connected_stone_dir(x, y, stone[x][y].dir[r], r - 4);



				}
				else if (stone[x + 1][y - 1].color == check) {
					stone[x][y].blocked[r] = true;
					stone[x + 1][y - 1].blocked[r - 4] = true;
					update_opp_block(x + 1, y - 1, stone[x + 1][y - 1].dir[r], r);
				}
				if (board[x + 1][y - 1] == 3) {
					rotation_block();
					stone[x + 1][y - 1].blocked[r - 4] = true;
					update_connected_stone_dir(x + 1, y - 1, stone[x + 1][y - 1].dir[r], r - 4);
					rotation_block();


				}
			}
			else {
				stone[x][y].blocked[r] = true;
			}

			break;

		}//switch

		if (stone[x][y].max < stone[x][y].dir[r]) {  //max 
			stone[x][y].max = stone[x][y].dir[r];
		}

	}//for r




	 //기존돌 update
	for (int r = 0; r < 8; r++) {
		switch (r) {
		case 0:  // 기존 돌 아래에 새로운 돌 놓기.   dir=↑
			if (y - 1 >= 0) {
				if (stone[x][y - 1].color == WB) {
					update_connected_stone_dir(x, y, stone[x][y].dir[r], r + 4);
				}


			}
			break;
		case 1: // 기존 돌 오른쪽 아래에 새로운 돌 놓기.    dir=↖
			if (y - 1 >= 0 && x - 1 >= 0) {
				if (stone[x - 1][y - 1].color == WB) {
					update_connected_stone_dir(x, y, stone[x][y].dir[r], r + 4);
				}
			}
			break;
		case 2: //기존 돌 오른쪽에 새로운 돌 놓기.   dir = ←
			if (x - 1 >= 0) {
				if (stone[x - 1][y].color == WB) {
					update_connected_stone_dir(x, y, stone[x][y].dir[r], r + 4);
				}
			}
			break;
		case 3: //기존 돌 오른쪽 위에 새로운 돌 놓기.  dir = ↙
			if (x - 1 >= 0 && y + 1 < height) {
				if (stone[x - 1][y + 1].color == WB) {

					update_connected_stone_dir(x, y, stone[x][y].dir[r], r + 4);
				}
			}
			break;
		case 4: //기존 돌 위에 놓일 경우 dir = ↓
			if (y + 1 < height) {
				if (stone[x][y + 1].color == WB) {

					update_connected_stone_dir(x, y, stone[x][y].dir[r], r - 4);
				}
			}
			break;
		case 5: //기존 돌 왼쪽 위에 놓인 경우 dir = ↘
			if (y + 1 < height && x + 1 < width) {
				if (stone[x + 1][y + 1].color == WB) {

					update_connected_stone_dir(x, y, stone[x][y].dir[r], r - 4);
				}
			}
			break;
		case 6: //기존 돌 왼쪽에 놓인 경우 dir = →
			if (x + 1 < width) {
				if (stone[x + 1][y].color == WB) {

					update_connected_stone_dir(x, y, stone[x][y].dir[r], r - 4);
				}
			}
			break;
		case 7: //기존 돌 왼쪽 아래에 놓인 경우 dir = ↗
			if (x + 1 < width && y - 1 >= 0) {
				if (stone[x + 1][y - 1].color == WB) {

					update_connected_stone_dir(x, y, stone[x][y].dir[r], r - 4);
				}
			}
			break;


		}//switch
	}

}
void update_opp_block(int x, int y, int len, int r) {

	int check = 0;

	if (!block_list.empty()) {
		if (stone[block_list.front().first][block_list.front().second].color != stone[x][y].color) {
			rotation_block();
			check = 1;
		}
	}


	switch (r) {
	case 0:  // 기존 돌 아래에 새로운 돌 놓기.   dir=↑
		if (y - 1 >= 0) {
			for (int i = 0; i < len; i++) {
				stone[x][y - i].blocked[r + 4] = true;
			}
		}
		break;

	case 1: // 기존 돌 오른쪽 아래에 새로운 돌 놓기.    dir=↖
		if (y - 1 >= 0 && x - 1 >= 0) {
			for (int i = 0; i < len; i++) {
				stone[x - i][y - i].blocked[r + 4] = true;
			}
		}
		break;
	case 2: //기존 돌 오른쪽에 새로운 돌 놓기.   dir = ←
		if (x - 1 >= 0) {
			for (int i = 0; i < len; i++) {
				stone[x - i][y].blocked[r + 4] = true;
			}
		}
		break;
	case 3: //기존 돌 오른쪽 위에 새로운 돌 놓기.  dir = ↙
		if (x - 1 >= 0 && y + 1 < height) {
			for (int i = 0; i < len; i++) {
				stone[x - i][y + i].blocked[r + 4] = true;
			}
		}
		break;
	case 4: //기존 돌 위에 놓일 경우 dir = ↓
		if (y + 1 < height) {
			for (int i = 0; i < len; i++) {
				stone[x][y + i].blocked[r - 4] = true;
			}
		}
		break;
	case 5: //기존 돌 왼쪽 위에 놓인 경우 dir = ↘
		if (y + 1 < height && x + 1 < width) {
			for (int i = 0; i < len; i++) {
				stone[x + i][y + i].blocked[r - 4] = true;
			}
		}
		break;
	case 6: //기존 돌 왼쪽에 놓인 경우 dir = →
		if (x + 1 < width) {
			for (int i = 0; i < len; i++) {
				stone[x + i][y].blocked[r - 4] = true;
			}
		}
		break;
	case 7: //기존 돌 왼쪽 아래에 놓인 경우 dir = ↗
		if (x + 1 < width && y - 1 >= 0) {
			for (int i = 0; i < len; i++) {
				stone[x + i][y - i].blocked[r - 4] = true;
			}
		}
		break;


	}//switch
	if (check) {
		rotation_block();
	}

}
char c[10];
void mymove(int x[], int y[], int cnt) {

	for (int i = 0; i < cnt; i++) {

		
		if (isFree(x[i], y[i])) {
			board[x[i]][y[i]] = 1;
			stone[x[i]][y[i]].color = 1;

			//put_stone_dir(x[i], y[i],1);

			
		}
		else {
			setLine("ERROR 이미 돌이 있는 위치입니다. MY[%d, %d]", x[i], y[i]);
		}
	

	}
}




pair<int, int> find_optimal_pos() {



	//if (cnt == 2) {
	//	temp_pos = get_optimal_pos();
	//}
	//else {
	//	do {
	//		temp_pos.first++;
	//	} while (!isFree(temp_pos.first, temp_pos.second));
	//}


	temp_pos = get_optimal_pos();




	/*
	do{
	n++;
	temp_pos.first++;
	} while (!stone[temp_pos.first, temp_pos.second));
	*/


	update_my_stone(temp_pos.first, temp_pos.second, 1);


	/*for (int i = 0; i < 2; i++) {
	pos_x[i] = temp_pos.first;
	pos_y[i] = temp_pos.second;
	temp_pos.first++;
	temp_pos.second++;
	}
	set_optimal_pos(pos_x, pos_y);*/

	--counts;

	return temp_pos;
}
void update_my_stone(int x, int y, int WB) {//????
											//stone[x][y].color = WB;
	if (!block_list.empty()) {
		if (stone[block_list.front().first][block_list.front().second].color != 1) {
			rotation_block();
		}
	}
	//put_stone_dir(x, y, 1);

	if (stone[x][y].color == 0) {
		stone[x][y].color = WB;
		put_stone_dir(x, y, 1);
	}
	else
	{

		do {
			x = rand() % width;
			y = rand() % height;
			temp_pos.first = x;
			temp_pos.second = y;
		} while (!isFree(x, y));
	}


	//my_list.insert(my_list.begin(), make_pair(x, y));  //임시 
	my_list.push_back(make_pair(x, y));
	

	

}

void op_update() {
	counts = 2;

	update_op_stone(op_stone_x, op_stone_y, 2);  //상대가 착수한 돌 업데이트 

}

void update_op_stone(int x[], int y[], int WB) {



	for (int i = 0; i<cnt; i++) {
		if (!block_list.empty()) {
			if (stone[block_list.front().first][block_list.front().second].color != 2) {
				rotation_block();
			}
		}
		if (x[i] == 20)
			return;
		stone[x[i]][y[i]].color = WB;
		put_stone_dir(x[i], y[i], WB);
		//op_list.insert(op_list.begin(), make_pair(x[i], y[i])); //임시 
		op_list.push_back(make_pair(x[i], y[i]));
		//op_stat(x[i], y[i]);
	}

}

void opmove(int x[], int y[], int cnt) {

	for (int i = 0; i < cnt; i++) {
		if (isFree(x[i], y[i])) {
			board[x[i]][y[i]] = 2;

			stone[x[i]][y[i]].color = 2;
			op_stone_x[i] = x[i];
			op_stone_y[i] = y[i];

			

		}
		else {
			setLine("ERROR 이미 돌이 있는 위치입니다. OP[%d, %d]", x[i], y[i]);
		}
	}
}

void block(int x, int y) {
	if (isFree(x, y)) {
		board[x][y] = 3;
		stone[x][y].color = 2;
		put_stone_dir(x, y, 2);
		op_list.push_back(make_pair(x, y));

		block_list.push_back(make_pair(x, y));
		rotation_block();

		stone[x][y].color = 1;
		put_stone_dir(x, y, 1);

		my_list.push_back(make_pair(x, y));
		rotation_block();
	}

}

void rotation_block() {
	for (int i = 0; i<block_list.size(); i++) {
		temp = stone[block_list[i].first][block_list[i].second];
		stone[block_list[i].first][block_list[i].second] = rotate_block[i];
		rotate_block[i] = temp;
	}
}



void stoneInit(int x[], int y[], int cnt = 2) {
	for (int i = 0; i < cnt; i++) {
		for (int r = 0; r<8; r++) {
			//stone[x[i]][y[i]].dir[r]+=stone[x[i]][y[i]]
		}
	}
}

PAIR get_optimal_pos() {

	if (!start_attack)
		find_two_x4(my_list);

	if (counts == 2) {
		if (find_five(&my_list) && start_attack) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}
		else if (find_four(&my_list) && start_attack) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}

		else if (find_five(&op_list)) {

			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}
		else if (find_four(&op_list)) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}
		else if (find_three(&my_list) && start_attack) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

			
		}
		else if (find_three(&op_list)) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		
		}

		else if (find_two(&my_list) && start_attack) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

			
		}
		else if (find_two(&op_list) && start_attack) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

			
		}


		else if (find_one(&my_list)) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();
			if (my_list.size() == block_list.size()) {
				find_one(&op_list);
				temp_pos.first = get_optimal_pos_x();
				temp_pos.second = get_optimal_pos_y();
			}
			if (score[return_max_index()].first.first >= 4)
				start_attack = true;
			
		}
		else if (find_one(&op_list)) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

			
		}
		else {
			if (cnt != 1) {
				optimal_x = 0;
				optimal_y = 0;
			}
			do {
				temp_pos.first = get_optimal_pos_x();
				temp_pos.second = get_optimal_pos_y();
				optimal_x++;
				if (optimal_x == 19) {
					optimal_x = 0;
					optimal_y++;
				}
			} while (!isFree(temp_pos.first, temp_pos.second));

		}
		


	}
	else if (counts == 1) {
		
		if (find_five(&my_list) && start_attack) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}
		else if (find_five(&op_list)) {

			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}
		else if (find_four(&op_list)) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}
		else if (find_exfour(&my_list) && start_attack) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}
		else if (find_three(&my_list) && start_attack) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}
		else if (find_three(&op_list)) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}

		else if (find_two(&my_list) && start_attack) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}
		else if (find_two(&op_list) && start_attack) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();

		}


		else if (find_one(&my_list)) {
			temp_pos.first = get_optimal_pos_x();
			temp_pos.second = get_optimal_pos_y();
			if (my_list.size() == block_list.size()) {
				find_one(&op_list);
				temp_pos.first = get_optimal_pos_x();
				temp_pos.second = get_optimal_pos_y();
			}
			if (score[return_max_index()].first.first >= 4)
				start_attack = true;

		}
		/*else if (find_one(&op_list)) {
		temp_pos.first = get_optimal_pos_x();
		temp_pos.second = get_optimal_pos_y();

		}*/

		else {
			do {
				temp_pos.first = get_optimal_pos_x();
				temp_pos.second = get_optimal_pos_y();
				optimal_x++;
				if (optimal_x == 19) {
					optimal_x = 0;
					optimal_y++;
				}

			} while (!isFree(optimal_x, temp_pos.second));

		}

	}

	chance = 2;

	return temp_pos;
	//if (find_four(&op_list)) {
	//	temp_pos.first = get_optimal_pos_x();
	//	temp_pos.second = get_optimal_pos_y();
	//}
	///*else if (find_four(op_list)) {
	//	temp_pos.first = get_optimal_pos_x();
	//	temp_pos.second = get_optimal_pos_y();
	//}*/
	///*else if (find_three(op_list)) {
	//	temp_pos.first = get_optimal_pos_x();
	//	temp_pos.second = get_optimal_pos_y();
	//}*/
	//else {
	//	do {
	//		optimal_x++;
	//		temp_pos.first = get_optimal_pos_x();
	//		temp_pos.second = get_optimal_pos_y();


	//	} while (!isFree(optimal_x, temp_pos.second));
	//}
	//return temp_pos;
}

bool find_five(vector<PAIR > * list2) {
	int color = 0;
	int check = 0;
	vector<PAIR> list = *list2;

	if (!list2->empty()) {
		if (&list2[0] == &my_list) {
			color = 1;
		}
		else
			color = 2;

	}

	if (!block_list.empty()) {
		if (color != stone[block_list.front().first][block_list.front().second].color) {
			rotation_block();
		}
	}

	//for (int i = 0; i < list.size(); i++) {
	for (int i = list.size() - 1; i >= 0; i--) {
		for (int direc = 0; direc < 8; direc++) {
			if (stone[list[i].first][list[i].second].dir[(direc + 4) % 8] == 1) {
				switch (direc) {
				case 0:

					if (stone[list[i].first][list[i].second].dir[direc] == 5 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second - 5)) {
							if (stone[list[i].first][list[i].second - 5].color == 0) {
								if (is_inboard(list[i].first, list[i].second - 6)) {
									if (stone[list[i].first][list[i].second - 6].color == color)
										break;
								}
								optimal_x = list[i].first;
								optimal_y = list[i].second - 5;
								return true;
							}
						}
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second - 4)) {
							if (stone[list[i].first][list[i].second - 4].dir[direc] == 2 && stone[list[i].first][list[i].second - 4].color == color) {
								if (stone[list[i].first][list[i].second - 3].color == 0) {
									optimal_x = list[i].first;
									optimal_y = list[i].second - 3;
									return true;
								} //if isfree 
							}//if 떨어진 2개 
						}//if 떨어진 2개 is in board
					}//if 3개따리
					else if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second - 5)) {
							if (stone[list[i].first][list[i].second - 5].dir[direc] == 1 && stone[list[i].first][list[i].second - 5].color == color) {
								if (stone[list[i].first][list[i].second - 4].color == 0) {
									optimal_x = list[i].first;
									optimal_y = list[i].second - 4;
									return true;
								}
							}
						}
					} // 0000x0 경우 
					break;
				case 1:

					if (stone[list[i].first][list[i].second].dir[direc] == 5 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 5, list[i].second - 5)) {
							if (stone[list[i].first - 5][list[i].second - 5].color == 0) {
								if (is_inboard(list[i].first - 6, list[i].second - 6)) {
									if (stone[list[i].first - 6][list[i].second - 6].color == color)
										break;
								}
								optimal_x = list[i].first - 5;
								optimal_y = list[i].second - 5;
								return true;
							}
						}
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 4, list[i].second - 4)) {
							if (stone[list[i].first - 4][list[i].second - 4].dir[direc] == 2 && stone[list[i].first - 4][list[i].second - 4].color == color) {
								if (stone[list[i].first - 3][list[i].second - 3].color == 0) {
									optimal_x = list[i].first - 3;
									optimal_y = list[i].second - 3;
									return true;
								} //if isfree 
							}//if 떨어진 2개 
						}//if 떨어진 2개 is in board
					}//if 3개따리
					else if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 5, list[i].second - 5)) {
							if (stone[list[i].first - 5][list[i].second - 5].dir[direc] == 1 && stone[list[i].first - 5][list[i].second - 5].color == color) {
								if (stone[list[i].first - 4][list[i].second - 4].color == 0) {
									optimal_x = list[i].first - 4;
									optimal_y = list[i].second - 4;
									return true;
								}
							}
						}
					}
					break;

				case 2:

					if (stone[list[i].first][list[i].second].dir[direc] == 5 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 5, list[i].second)) {
							if (stone[list[i].first - 5][list[i].second].color == 0) {
								if (is_inboard(list[i].first - 6, list[i].second)) {
									if (stone[list[i].first - 6][list[i].second].color == color)
										break;
								}
								optimal_x = list[i].first - 5;
								optimal_y = list[i].second;
								return true;
							}
						}
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 4, list[i].second)) {
							if (stone[list[i].first - 4][list[i].second].dir[direc] == 2 && stone[list[i].first - 4][list[i].second].color == color) {
								if (stone[list[i].first - 3][list[i].second].color == 0) {
									optimal_x = list[i].first - 3;
									optimal_y = list[i].second;
									return true;
								} //if isfree 
							}//if 떨어진 2개 
						}//if 떨어진 2개 is in board
					}//if 3개따리
					else if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 5, list[i].second)) {
							if (stone[list[i].first - 5][list[i].second].dir[direc] == 1 && stone[list[i].first - 5][list[i].second].color == color) {
								if (stone[list[i].first - 4][list[i].second].color == 0) {
									optimal_x = list[i].first - 4;
									optimal_y = list[i].second;
									return true;
								}
							}
						}
					}
					break;

				case 3:

					if (stone[list[i].first][list[i].second].dir[direc] == 5 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 5, list[i].second + 5)) {
							if (stone[list[i].first - 5][list[i].second + 5].color == 0) {
								if (is_inboard(list[i].first - 6, list[i].second + 6)) {
									if (stone[list[i].first - 6][list[i].second + 6].color == color)
										break;
								}
								optimal_x = list[i].first - 5;
								optimal_y = list[i].second + 5;
								return true;
							}
						}
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 4, list[i].second + 4)) {
							if (stone[list[i].first - 4][list[i].second + 4].dir[direc] == 2 && stone[list[i].first - 4][list[i].second + 4].color == color) {
								if (stone[list[i].first - 3][list[i].second + 3].color == 0) {
									optimal_x = list[i].first - 3;
									optimal_y = list[i].second + 3;
									return true;
								} //if isfree 
							}//if 떨어진 2개 
						}//if 떨어진 2개 is in board
					}//if 3개따리
					else if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 5, list[i].second + 5)) {
							if (stone[list[i].first - 5][list[i].second + 5].dir[direc] == 1 && stone[list[i].first - 5][list[i].second + 5].color == color) {
								if (stone[list[i].first - 4][list[i].second + 4].color == 0) {
									optimal_x = list[i].first - 4;
									optimal_y = list[i].second + 4;
									return true;
								}
							}
						}
					}
					break;

				case 4:

					if (stone[list[i].first][list[i].second].dir[direc] == 5 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second + 5)) {
							if (stone[list[i].first][list[i].second + 5].color == 0) {
								if (is_inboard(list[i].first, list[i].second + 6)) {
									if (stone[list[i].first][list[i].second + 6].color == color)
										break;
								}
								optimal_x = list[i].first;
								optimal_y = list[i].second + 5;
								return true;
							}
						}
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second + 4)) {
							if (stone[list[i].first][list[i].second + 4].dir[direc] == 2 && stone[list[i].first][list[i].second + 4].color == color) {
								if (stone[list[i].first][list[i].second + 3].color == 0) {
									optimal_x = list[i].first;
									optimal_y = list[i].second + 3;
									return true;
								} //if isfree 
							}//if 떨어진 2개 
						}//if 떨어진 2개 is in board
					}//if 3개따리
					else if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second + 5)) {
							if (stone[list[i].first][list[i].second + 5].dir[direc] == 1 && stone[list[i].first][list[i].second + 5].color == color) {
								if (stone[list[i].first][list[i].second + 4].color == 0) {
									optimal_x = list[i].first;
									optimal_y = list[i].second + 4;
									return true;
								}
							}
						}
					}
					break;

				case 5:

					if (stone[list[i].first][list[i].second].dir[direc] == 5 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 5, list[i].second + 5)) {
							if (stone[list[i].first + 5][list[i].second + 5].color == 0) {
								if (is_inboard(list[i].first + 6, list[i].second + 6)) {
									if (stone[list[i].first + 6][list[i].second + 6].color == color)
										break;
								}
								optimal_x = list[i].first + 5;
								optimal_y = list[i].second + 5;
								return true;
							}
						}
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 4, list[i].second + 4)) {
							if (stone[list[i].first + 4][list[i].second + 4].dir[direc] == 2 && stone[list[i].first + 4][list[i].second + 4].color == color) {
								if (stone[list[i].first + 3][list[i].second + 3].color == 0) {
									optimal_x = list[i].first + 3;
									optimal_y = list[i].second + 3;
									return true;
								} //if isfree 
							}//if 떨어진 2개 
						}//if 떨어진 2개 is in board
					}//if 3개따리
					else if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 5, list[i].second + 5)) {
							if (stone[list[i].first + 5][list[i].second + 5].dir[direc] == 1 && stone[list[i].first + 5][list[i].second + 5].color == color) {
								if (stone[list[i].first + 4][list[i].second + 4].color == 0) {
									optimal_x = list[i].first + 4;
									optimal_y = list[i].second + 4;
									return true;
								}
							}
						}
					}
					break;

				case 6:

					if (stone[list[i].first][list[i].second].dir[direc] == 5 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 5, list[i].second)) {
							if (stone[list[i].first + 5][list[i].second].color == 0) {
								if (is_inboard(list[i].first + 6, list[i].second)) {
									if (stone[list[i].first + 6][list[i].second].color == color)
										break;
								}
								optimal_x = list[i].first + 5;
								optimal_y = list[i].second;
								return true;
							}
						}
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 4, list[i].second)) {
							if (stone[list[i].first + 4][list[i].second].dir[direc] == 2 && stone[list[i].first + 4][list[i].second].color == color) {
								if (stone[list[i].first + 3][list[i].second].color == 0) {
									optimal_x = list[i].first + 3;
									optimal_y = list[i].second;
									return true;
								} //if isfree 
							}//if 떨어진 2개 
						}//if 떨어진 2개 is in board
					}//if 3개따리
					else if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 5, list[i].second)) {
							if (stone[list[i].first + 5][list[i].second].dir[direc] == 1 && stone[list[i].first + 5][list[i].second].color == color) {
								if (stone[list[i].first + 4][list[i].second].color == 0) {
									optimal_x = list[i].first + 4;
									optimal_y = list[i].second;
									return true;
								}
							}
						}
					}
					break;

				case 7:

					if (stone[list[i].first][list[i].second].dir[direc] == 5 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 5, list[i].second - 5)) {
							if (stone[list[i].first + 5][list[i].second - 5].color == 0) {
								if (is_inboard(list[i].first + 6, list[i].second - 6)) {
									if (stone[list[i].first + 6][list[i].second - 6].color == color)
										break;
								}
								optimal_x = list[i].first + 5;
								optimal_y = list[i].second - 5;
								return true;
							}
						}
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 4, list[i].second - 4)) {
							if (stone[list[i].first + 4][list[i].second - 4].dir[direc] == 2 && stone[list[i].first + 4][list[i].second - 4].color == color) {
								if (stone[list[i].first + 3][list[i].second - 3].color == 0) {
									optimal_x = list[i].first + 3;
									optimal_y = list[i].second - 3;
									return true;
								} //if isfree 
							}//if 떨어진 2개 
						}//if 떨어진 2개 is in board
					}//if 3개따리
					else if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 5, list[i].second - 5)) {
							if (stone[list[i].first + 5][list[i].second - 5].dir[direc] == 1 && stone[list[i].first + 5][list[i].second - 5].color == color) {
								if (stone[list[i].first + 4][list[i].second - 4].color == 0) {
									optimal_x = list[i].first + 4;
									optimal_y = list[i].second - 4;
									return true;
								}
							}
						}
					}
					break;

				}//switch
			}

		}//for direc
	}//for i
	return false;

}




bool find_four(vector<PAIR> * list2) {
	score.clear();

	int color = 0;
	vector<PAIR> list = *list2;

	if (!list2->empty()) {
		if (&list2[0] == &my_list) {
			color = 1;
		}
		else
			color = 2;

	}
	if (!block_list.empty()) {
		if (color != stone[block_list.front().first][block_list.front().second].color) {
			rotation_block();
		}
	}

	//for (int i = 0; i < list.size(); i++) {
	for (int i = list.size() - 1; i >= 0; i--) {
		for (int direc = 0; direc < 8; direc++) {
			if (stone[list[i].first][list[i].second].dir[(direc + 4) % 8] == 1) {
				switch (direc) {


				case 0:

					if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooo
						if (is_inboard(list[i].first, list[i].second - 5)) {
							if (stone[list[i].first][list[i].second - 5].color == 0) {
								if (is_inboard(list[i].first, list[i].second - 6)) {
									if (stone[list[i].first][list[i].second - 6].color != color) {
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first, list[i].second + 2) && stone[list[i].first][list[i].second + 2].color != color) {
												pos_score(list[i].first, list[i].second - 4, 2);   // xxooooxx 일때
												pos_score(list[i].first, list[i].second - 5, 2);
											}
											else {
												pos_score(list[i].first, list[i].second - 4, 2);   // xooooxx 일때
												pos_score(list[i].first, list[i].second - 5, 1);
											}
										}
										else {   // ooooxx일때
											pos_score(list[i].first, list[i].second - 4, 1);   // ooooxx 일때
											pos_score(list[i].first, list[i].second - 5, 1);
										}
									}
									else {  // 7이 만들어질 가능성이 있을때  // ooooxxo
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											pos_score(list[i].first, list[i].second - 4, 1);
										}
									}
								}
								else { // 벽에 막혀 있을 때
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first, list[i].second + 2) && stone[list[i].first][list[i].second + 2].color != color) {
											pos_score(list[i].first, list[i].second - 4, 2);   // xxooooxx| 일때
											pos_score(list[i].first, list[i].second - 5, 2);

										}
										else {
											pos_score(list[i].first, list[i].second - 4, 2);   // xooooxx| 일때
											pos_score(list[i].first, list[i].second - 5, 1);
										}
									}
									else {   // ooooxx일때
										pos_score(list[i].first, list[i].second - 4, 1);   // ooooxx| 일때
										pos_score(list[i].first, list[i].second - 5, 1);
									}
								}
							}
							else {  // oooox 일때
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first][list[i].second - 5].color != color)
										pos_score(list[i].first, list[i].second - 4, 1);
								}
							}
						}
						else { // oooox| 일 때
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first, list[i].second - 4, 1);
							}
						}

					}

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooxo
						if (is_inboard(list[i].first, list[i].second - 5)) {
							if ((stone[list[i].first][list[i].second - 4].dir[direc] == 1 && stone[list[i].first][list[i].second - 4].color == color)) {
								if (stone[list[i].first][list[i].second - 3].color == 0) {
									if (stone[list[i].first][list[i].second - 4].blocked[direc] == false) {
										if (stone[list[i].first][list[i].second - 2].blocked[(direc + 4) % 8] == false) { // xoooxox|
											pos_score(list[i].first, list[i].second - 3, 2);
											pos_score(list[i].first, list[i].second - 5, 1);
											if (is_inboard(list[i].first, list[i].second + 2)) {
												if (stone[list[i].first][list[i].second + 2].color == 0) { // xxoooxox|
													pos_score(list[i].first, list[i].second + 1, 1);
												}
												else if (stone[list[i].first][list[i].second + 2].color != color) { //|xoooxox|
													pos_score(list[i].first, list[i].second + 1, 1);
												}
											}
										}
										else {
											pos_score(list[i].first, list[i].second - 3, 1);  // oooxox|
											pos_score(list[i].first, list[i].second - 5, 1);
										}

									} //위아래 막혀있는경우 
									else {
										if (stone[list[i].first][list[i].second - 2].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first, list[i].second + 2)) {
												if (stone[list[i].first][list[i].second + 2].color == 0) {
													pos_score(list[i].first, list[i].second + 1, 2);
												}
												else if (stone[list[i].first][list[i].second + 2].color != color) {
													pos_score(list[i].first, list[i].second + 1, 1);
												}
											}
											else {
												pos_score(list[i].first, list[i].second + 1, 2);
											}
										}
									}
								} // is free
							}// 떨어진거 
							else if ((stone[list[i].first][list[i].second - 5].dir[direc] == 1 && stone[list[i].first][list[i].second - 5].color == color)) {
								if (stone[list[i].first][list[i].second - 3].color == 0 && stone[list[i].first][list[i].second - 4].color == 0) {
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first, list[i].second + 2)) {
											if (stone[list[i].first][list[i].second + 2].color == 0) {
												pos_score(list[i].first, list[i].second - 3, 2);  // xxoooxxo
												pos_score(list[i].first, list[i].second - 4, 1);
											}
											else if (stone[list[i].first][list[i].second + 2].color != color) {
												pos_score(list[i].first, list[i].second - 3, 1);
												pos_score(list[i].first, list[i].second - 4, 1);
											}
										}
										else {
											pos_score(list[i].first, list[i].second - 3, 1);  // xoooxxo
											pos_score(list[i].first, list[i].second - 4, 1);
										}
									}
									else {
										pos_score(list[i].first, list[i].second - 3, 1); // |oooxxo
										pos_score(list[i].first, list[i].second - 4, 1);
									}
								}
							}
						}// is_inboard
						else if ((stone[list[i].first][list[i].second - 4].dir[direc] == 1 && stone[list[i].first][list[i].second - 4].color == color)) {
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first, list[i].second - 3, 1);
								if (is_inboard(list[i].first, list[i].second + 2)) {
									if (stone[list[i].first][list[i].second + 2].color == 0) {
										pos_score(list[i].first, list[i].second + 1, 2);
									}
									else if (stone[list[i].first][list[i].second + 2].color != color) {
										pos_score(list[i].first, list[i].second + 1, 1);
									}
								}
								else {
									pos_score(list[i].first, list[i].second + 1, 1);
								}
							}
						}
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { // ooxoo  ooxxoo
						if (is_inboard(list[i].first, list[i].second - 4)) {
							if ((stone[list[i].first][list[i].second - 3].dir[direc] == 2 && stone[list[i].first][list[i].second - 3].color == color)) {
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first][list[i].second - 3].blocked[direc] == false) {
										pos_score(list[i].first, list[i].second - 2, 1);
										pos_score(list[i].first, list[i].second - 5, 1);
									}
									else {
										pos_score(list[i].first, list[i].second - 2, 0.5);
									}
								}
								else {
									if (stone[list[i].first][list[i].second - 3].blocked[direc] == false) {
										pos_score(list[i].first, list[i].second - 2, 0.5);
										pos_score(list[i].first, list[i].second - 5, 1);
									}
								}
							}// 떨
							else if ((stone[list[i].first][list[i].second - 4].dir[direc] == 2 && stone[list[i].first][list[i].second - 4].color == color)) {
								if (stone[list[i].first][list[i].second - 2].color == 0 && stone[list[i].first][list[i].second - 3].color == 0) {
									pos_score(list[i].first, list[i].second - 2, 1);
								} //is free
							}
						}// is _inboard
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second - 5)) {
							if ((stone[list[i].first][list[i].second - 3].dir[direc] == 1 && stone[list[i].first][list[i].second - 3].color == color
								&& stone[list[i].first][list[i].second - 3].blocked[direc] == false && stone[list[i].first][list[i].second - 5].dir[direc] == 1 && stone[list[i].first][list[i].second - 5].color == color)) {
								if (is_inboard(list[i].first, list[i].second + 2)) {
									if (stone[list[i].first][list[i].second + 2].color == 0) {
										if (stone[list[i].first][list[i].second + 1].color == 0) {
											pos_score(list[i].first, list[i].second - 2, 2);  //xxooxoxo
										}
										else {
											pos_score(list[i].first, list[i].second - 2, 1); // ooxoxo
										}
									}
									else if (stone[list[i].first][list[i].second + 2].color != color) {
										pos_score(list[i].first, list[i].second - 2, 1);  // xooxoxo
									}
									pos_score(list[i].first, list[i].second - 4, 1);
								}
								else {
									pos_score(list[i].first, list[i].second - 2, 1);  // ooxoxo
									pos_score(list[i].first, list[i].second - 4, 1);
								}

							}// ooxoxo
						}// inboard
					}// if


					if (is_inboard(list[i].first, list[i].second - 5)) {// oxooxo
						if ((stone[list[i].first][list[i].second].dir[direc] == 1) && (stone[list[i].first][list[i].second].blocked[direc] == false)
							&& (stone[list[i].first][list[i].second - 2].blocked[direc] == false) && (stone[list[i].first][list[i].second - 2].dir[direc] == 2)) {
							if ((stone[list[i].first][list[i].second - 2].color == color) && (stone[list[i].first][list[i].second - 5].dir[direc] == 1)
								&& (stone[list[i].first][list[i].second - 5].color == color)) {
								if (is_inboard(list[i].first, list[i].second - 7)) {
									if (stone[list[i].first][list[i].second - 6].color == 0) {
										if (stone[list[i].first][list[i].second - 5].blocked[direc] == false) {
											pos_score(list[i].first, list[i].second - 4, 2);
										}
										else {
											pos_score(list[i].first, list[i].second - 4, 1);
										}
									}
									else if (stone[list[i].first][list[i].second - 6].color != color) {
										pos_score(list[i].first, list[i].second - 4, 1);
									}
									else {
										pos_score(list[i].first, list[i].second - 4, 2);
									}
								}
								else {
									pos_score(list[i].first, list[i].second - 4, 1);
								}
							}
						}
					}
					break;

				case 1:

					if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooo
						if (is_inboard(list[i].first - 5, list[i].second - 5)) {
							if (stone[list[i].first - 5][list[i].second - 5].color == 0) {
								if (is_inboard(list[i].first - 6, list[i].second - 6)) {
									if (stone[list[i].first - 6][list[i].second - 6].color != color) {
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first + 2, list[i].second + 2) && stone[list[i].first + 2][list[i].second + 2].color != color) {
												pos_score(list[i].first - 4, list[i].second - 4, 2);
												pos_score(list[i].first - 5, list[i].second - 5, 2);   // xxooooxx 일때

											}
											else {
												pos_score(list[i].first - 4, list[i].second - 4, 2);   // xooooxx 일때
												pos_score(list[i].first - 5, list[i].second - 5, 1);
											}
										}
										else {   // ooooxx일때
											pos_score(list[i].first - 4, list[i].second - 4, 1);   // ooooxx 일때
											pos_score(list[i].first - 5, list[i].second - 5, 1);
										}
									}
									else {  // 7이 만들어질 가능성이 있을때  // ooooxxo
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											pos_score(list[i].first - 4, list[i].second - 4, 1);
										}
									}
								}
								else { // 벽에 막혀 있을 때
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first + 2, list[i].second + 2) && stone[list[i].first + 2][list[i].second + 2].color != color) {
											pos_score(list[i].first - 4, list[i].second - 4, 2);
											pos_score(list[i].first - 5, list[i].second - 5, 2);   // xxooooxx| 일때

										}
										else {
											pos_score(list[i].first - 4, list[i].second - 4, 2);   // xooooxx| 일때
											pos_score(list[i].first - 5, list[i].second - 5, 1);
										}
									}
									else {   // ooooxx일때
										pos_score(list[i].first - 4, list[i].second - 4, 1);   // ooooxx| 일때
										pos_score(list[i].first - 5, list[i].second - 5, 1);
									}
								}
							}
							else {  // oooox 일때
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first - 5][list[i].second - 5].color != color)
										pos_score(list[i].first - 4, list[i].second - 4, 1);
								}
							}
						}
						else { // oooox| 일 때
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first - 4, list[i].second - 4, 1);
							}
						}

					}

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooxo
						if (is_inboard(list[i].first - 5, list[i].second - 5)) {
							if ((stone[list[i].first - 4][list[i].second - 4].dir[direc] == 1 && stone[list[i].first - 4][list[i].second - 4].color == color)) {
								if (stone[list[i].first - 3][list[i].second - 3].color == 0) {
									if (stone[list[i].first - 4][list[i].second - 4].blocked[direc] == false) {
										if (stone[list[i].first - 2][list[i].second - 2].blocked[(direc + 4) % 8] == false) { // xoooxox|
											pos_score(list[i].first - 3, list[i].second - 3, 2);
											pos_score(list[i].first - 5, list[i].second - 5, 1);
											if (is_inboard(list[i].first + 2, list[i].second + 2)) {
												if (stone[list[i].first + 2][list[i].second + 2].color == 0) { // xxoooxox|
													pos_score(list[i].first + 1, list[i].second + 1, 1);
												}
												else if (stone[list[i].first + 2][list[i].second + 2].color != color) { //|xoooxox|
													pos_score(list[i].first + 1, list[i].second + 1, 1);
												}
											}
										}
										else {
											pos_score(list[i].first - 3, list[i].second - 3, 1);  // oooxox|
											pos_score(list[i].first - 5, list[i].second - 5, 1);
										}

									} //위아래 막혀있는경우 
									else {
										if (stone[list[i].first - 2][list[i].second - 2].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first + 2, list[i].second + 2)) {
												if (stone[list[i].first + 2][list[i].second + 2].color == 0) {
													pos_score(list[i].first + 1, list[i].second + 1, 2);
												}
												else if (stone[list[i].first + 2][list[i].second + 2].color != color) {
													pos_score(list[i].first + 1, list[i].second + 1, 1);
												}
											}
											else {
												pos_score(list[i].first + 1, list[i].second + 1, 2);
											}
										}
									}
								} // is free
							}// 떨어진거 
							else if ((stone[list[i].first - 5][list[i].second - 5].dir[direc] == 1 && stone[list[i].first - 5][list[i].second - 5].color == color)) {
								if (stone[list[i].first - 3][list[i].second - 3].color == 0 && stone[list[i].first - 4][list[i].second - 4].color == 0) {
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first + 2, list[i].second + 2)) {
											if (stone[list[i].first + 2][list[i].second + 2].color == 0) {
												pos_score(list[i].first - 3, list[i].second - 3, 2);  // xxoooxxo
												pos_score(list[i].first - 4, list[i].second - 4, 1);
											}
											else if (stone[list[i].first + 2][list[i].second + 2].color != color) {
												pos_score(list[i].first - 3, list[i].second - 3, 1);
												pos_score(list[i].first - 4, list[i].second - 4, 1);
											}
										}
										else {
											pos_score(list[i].first - 3, list[i].second - 3, 1);  // xoooxxo
											pos_score(list[i].first - 4, list[i].second - 4, 1);
										}
									}
									else {
										pos_score(list[i].first - 3, list[i].second - 3, 1); // |oooxxo
										pos_score(list[i].first - 4, list[i].second - 4, 1);
									}
								}
							}
						}// is_inboard
						else if ((stone[list[i].first - 4][list[i].second - 4].dir[direc] == 1 && stone[list[i].first - 4][list[i].second - 4].color == color)) {
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first - 3, list[i].second - 3, 1);
								if (is_inboard(list[i].first + 2, list[i].second + 2)) {
									if (stone[list[i].first + 2][list[i].second + 2].color == 0) {
										pos_score(list[i].first + 1, list[i].second + 1, 2);
									}
									else if (stone[list[i].first + 2][list[i].second + 2].color != color) {
										pos_score(list[i].first + 1, list[i].second + 1, 1);
									}
								}
								else {
									pos_score(list[i].first + 1, list[i].second + 1, 1);
								}
							}
						}
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { // ooxoo  ooxxoo
						if (is_inboard(list[i].first - 4, list[i].second - 4)) {
							if ((stone[list[i].first - 3][list[i].second - 3].dir[direc] == 2 && stone[list[i].first - 3][list[i].second - 3].color == color)) {
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first - 3][list[i].second - 3].blocked[direc] == false) {
										pos_score(list[i].first - 2, list[i].second - 2, 1);
										pos_score(list[i].first - 5, list[i].second - 5, 1);
									}
									else {
										pos_score(list[i].first - 2, list[i].second - 2, 0.5);
									}
								}
								else {
									if (stone[list[i].first - 3][list[i].second - 3].blocked[direc] == false) {
										pos_score(list[i].first - 2, list[i].second - 2, 0.5);
										pos_score(list[i].first - 5, list[i].second - 5, 1);
									}
								}
							}// 떨
							else if ((stone[list[i].first - 4][list[i].second - 4].dir[direc] == 2 && stone[list[i].first - 4][list[i].second - 4].color == color)) {
								if (stone[list[i].first - 2][list[i].second - 2].color == 0 && stone[list[i].first - 3][list[i].second - 3].color == 0) {
									pos_score(list[i].first - 2, list[i].second - 2, 1);
								} //is free
							}
						}// is _inboard
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { //ooxoxo
						if (is_inboard(list[i].first - 5, list[i].second - 5)) {
							if ((stone[list[i].first - 3][list[i].second - 3].dir[direc] == 1 && stone[list[i].first - 3][list[i].second - 3].color == color
								&& stone[list[i].first - 3][list[i].second - 3].blocked[direc] == false && stone[list[i].first - 5][list[i].second - 5].dir[direc] == 1 && stone[list[i].first - 5][list[i].second - 5].color == color)) {
								if (is_inboard(list[i].first + 2, list[i].second + 2)) {
									if (stone[list[i].first + 2][list[i].second + 2].color == 0) {
										if (stone[list[i].first + 1][list[i].second + 1].color == 0) {
											pos_score(list[i].first - 2, list[i].second - 2, 2);  //xxooxoxo
										}
										else {
											pos_score(list[i].first - 2, list[i].second - 2, 1); // ooxoxo
										}
									}
									else if (stone[list[i].first + 2][list[i].second + 2].color != color) {
										pos_score(list[i].first - 2, list[i].second - 2, 1);  // xooxoxo
									}
									pos_score(list[i].first - 4, list[i].second - 4, 1);
								}
								else {
									pos_score(list[i].first - 2, list[i].second - 2, 1);  // ooxoxo
									pos_score(list[i].first - 4, list[i].second - 4, 1);
								}

							}// ooxoxo
						}// inboard
					}// if


					if (is_inboard(list[i].first - 5, list[i].second - 5)) {// oxooxo
						if ((stone[list[i].first][list[i].second].dir[direc] == 1) && (stone[list[i].first][list[i].second].blocked[direc] == false)
							&& (stone[list[i].first - 2][list[i].second - 2].blocked[direc] == false) && (stone[list[i].first - 2][list[i].second - 2].dir[direc] == 2)) {
							if ((stone[list[i].first - 2][list[i].second - 2].color == color) && (stone[list[i].first - 5][list[i].second - 5].dir[direc] == 1)
								&& (stone[list[i].first - 5][list[i].second - 5].color == color)) {
								if (is_inboard(list[i].first - 7, list[i].second - 7)) {
									if (stone[list[i].first - 6][list[i].second - 6].color == 0) {
										if (stone[list[i].first - 5][list[i].second - 5].blocked[direc] == false) {
											pos_score(list[i].first - 4, list[i].second - 4, 2);
										}
										else {
											pos_score(list[i].first - 4, list[i].second - 4, 1);
										}
									}
									else if (stone[list[i].first - 6][list[i].second - 6].color != color) {
										pos_score(list[i].first - 4, list[i].second - 4, 1);
									}
									else {
										pos_score(list[i].first - 4, list[i].second - 4, 2);
									}
								}
								else {
									pos_score(list[i].first - 4, list[i].second - 4, 1);
								}
							}
						}
					}
					break;

				case 2:

					if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooo
						if (is_inboard(list[i].first - 5, list[i].second)) {
							if (stone[list[i].first - 5][list[i].second].color == 0) {
								if (is_inboard(list[i].first - 6, list[i].second)) {
									if (stone[list[i].first - 6][list[i].second].color != color) {
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first + 2, list[i].second) && stone[list[i].first + 2][list[i].second].color != color) {
												pos_score(list[i].first - 4, list[i].second, 2);
												pos_score(list[i].first - 5, list[i].second, 2);  // xxooooxx 일때
											}
											else {
												pos_score(list[i].first - 4, list[i].second, 2);   // xooooxx 일때
												pos_score(list[i].first - 5, list[i].second, 1);
											}
										}
										else {   // ooooxx일때
											pos_score(list[i].first - 4, list[i].second, 1);   // ooooxx 일때
											pos_score(list[i].first - 5, list[i].second, 1);
										}
									}
									else {  // 7이 만들어질 가능성이 있을때  // ooooxxo
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											pos_score(list[i].first - 4, list[i].second, 1);
										}
									}
								}
								else { // 벽에 막혀 있을 때
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first + 2, list[i].second) && stone[list[i].first + 2][list[i].second].color != color) {
											pos_score(list[i].first - 4, list[i].second, 2);
											pos_score(list[i].first - 5, list[i].second, 2); // xxooooxx| 일때
										}
										else {
											pos_score(list[i].first - 4, list[i].second, 2);   // xooooxx| 일때
											pos_score(list[i].first - 5, list[i].second, 1);
										}
									}
									else {   // ooooxx일때
										pos_score(list[i].first - 4, list[i].second, 1);   // ooooxx| 일때
										pos_score(list[i].first - 5, list[i].second, 1);
									}
								}
							}
							else {  // oooox 일때
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first - 5][list[i].second].color != color)
										pos_score(list[i].first - 4, list[i].second, 1);
								}
							}
						}
						else { // oooox| 일 때
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first - 4, list[i].second, 1);
							}
						}

					}

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooxo
						if (is_inboard(list[i].first - 5, list[i].second)) {
							if ((stone[list[i].first - 4][list[i].second].dir[direc] == 1 && stone[list[i].first - 4][list[i].second].color == color)) {
								if (stone[list[i].first - 3][list[i].second].color == 0) {
									if (stone[list[i].first - 4][list[i].second].blocked[direc] == false) {
										if (stone[list[i].first - 2][list[i].second].blocked[(direc + 4) % 8] == false) { // xoooxox|
											pos_score(list[i].first - 3, list[i].second, 2);
											pos_score(list[i].first - 5, list[i].second, 1);
											if (is_inboard(list[i].first + 2, list[i].second)) {
												if (stone[list[i].first + 2][list[i].second].color == 0) { // xxoooxox|
													pos_score(list[i].first + 1, list[i].second, 1);
												}
												else if (stone[list[i].first + 2][list[i].second].color != color) { //|xoooxox|
													pos_score(list[i].first + 1, list[i].second, 1);
												}
											}
										}
										else {
											pos_score(list[i].first - 3, list[i].second, 1);  // oooxox|
											pos_score(list[i].first - 5, list[i].second, 1);
										}

									} //위아래 막혀있는경우 
									else {
										if (stone[list[i].first - 2][list[i].second].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first + 2, list[i].second)) {
												if (stone[list[i].first + 2][list[i].second].color == 0) {
													pos_score(list[i].first + 1, list[i].second, 2);
												}
												else if (stone[list[i].first + 2][list[i].second].color != color) {
													pos_score(list[i].first + 1, list[i].second, 1);
												}
											}
											else {
												pos_score(list[i].first + 1, list[i].second, 2);
											}
										}
									}
								} // is free
							}// 떨어진거 
							else if ((stone[list[i].first - 5][list[i].second].dir[direc] == 1 && stone[list[i].first - 5][list[i].second].color == color)) {
								if (stone[list[i].first - 3][list[i].second].color == 0 && stone[list[i].first - 4][list[i].second].color == 0) {
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first + 2, list[i].second)) {
											if (stone[list[i].first + 2][list[i].second].color == 0) {
												pos_score(list[i].first - 3, list[i].second, 2);  // xxoooxxo
												pos_score(list[i].first - 4, list[i].second, 1);
											}
											else if (stone[list[i].first + 2][list[i].second].color != color) {
												pos_score(list[i].first - 3, list[i].second, 1);
												pos_score(list[i].first - 4, list[i].second, 1);
											}
										}
										else {
											pos_score(list[i].first - 3, list[i].second, 1);  // xoooxxo
											pos_score(list[i].first - 4, list[i].second, 1);
										}
									}
									else {
										pos_score(list[i].first - 3, list[i].second, 1); // |oooxxo
										pos_score(list[i].first - 4, list[i].second, 1);
									}
								}
							}
						}// is_inboard
						else if ((stone[list[i].first - 4][list[i].second].dir[direc] == 1 && stone[list[i].first - 4][list[i].second].color == color)) {
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first - 3, list[i].second, 1);
								if (is_inboard(list[i].first + 2, list[i].second)) {
									if (stone[list[i].first + 2][list[i].second].color == 0) {
										pos_score(list[i].first + 1, list[i].second, 2);
									}
									else if (stone[list[i].first + 2][list[i].second].color != color) {
										pos_score(list[i].first + 1, list[i].second, 1);
									}
								}
								else {
									pos_score(list[i].first + 1, list[i].second, 1);
								}
							}
						}
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { // ooxoo  ooxxoo
						if (is_inboard(list[i].first - 4, list[i].second)) {
							if ((stone[list[i].first - 3][list[i].second].dir[direc] == 2 && stone[list[i].first - 3][list[i].second].color == color)) {
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first - 3][list[i].second].blocked[direc] == false) {
										pos_score(list[i].first - 2, list[i].second, 1);
										pos_score(list[i].first - 5, list[i].second, 1);
									}
									else {
										pos_score(list[i].first - 2, list[i].second, 0.5);
									}
								}
								else {
									if (stone[list[i].first - 3][list[i].second].blocked[direc] == false) {
										pos_score(list[i].first - 2, list[i].second, 0.5);
										pos_score(list[i].first - 5, list[i].second, 1);
									}
								}
							}// 떨
							else if ((stone[list[i].first - 4][list[i].second].dir[direc] == 2 && stone[list[i].first - 4][list[i].second].color == color)) {
								if (stone[list[i].first - 2][list[i].second].color == 0 && stone[list[i].first - 3][list[i].second].color == 0) {
									pos_score(list[i].first - 2, list[i].second, 1);
								} //is free
							}
						}// is _inboard
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { //ooxoxo
						if (is_inboard(list[i].first - 5, list[i].second)) {
							if ((stone[list[i].first - 3][list[i].second].dir[direc] == 1 && stone[list[i].first - 3][list[i].second].color == color
								&& stone[list[i].first - 3][list[i].second].blocked[direc] == false && stone[list[i].first - 5][list[i].second].dir[direc] == 1 && stone[list[i].first - 5][list[i].second].color == color)) {
								if (is_inboard(list[i].first + 2, list[i].second)) {
									if (stone[list[i].first + 2][list[i].second].color == 0) {
										if (stone[list[i].first + 1][list[i].second].color == 0) {
											pos_score(list[i].first - 2, list[i].second, 2);  //xxooxoxo
										}
										else {
											pos_score(list[i].first - 2, list[i].second, 1); // ooxoxo
										}
									}
									else if (stone[list[i].first + 2][list[i].second].color != color) {
										pos_score(list[i].first - 2, list[i].second, 1);  // xooxoxo
									}
									pos_score(list[i].first - 4, list[i].second, 1);
								}
								else {
									pos_score(list[i].first - 2, list[i].second, 1);  // ooxoxo
									pos_score(list[i].first - 4, list[i].second, 1);
								}

							}// ooxoxo
						}// inboard
					}// if


					if (is_inboard(list[i].first - 5, list[i].second)) {// oxooxo
						if ((stone[list[i].first][list[i].second].dir[direc] == 1) && (stone[list[i].first][list[i].second].blocked[direc] == false)
							&& (stone[list[i].first - 2][list[i].second].blocked[direc] == false) && (stone[list[i].first - 2][list[i].second].dir[direc] == 2)) {
							if ((stone[list[i].first - 2][list[i].second].color == color) && (stone[list[i].first - 5][list[i].second].dir[direc] == 1)
								&& (stone[list[i].first - 5][list[i].second].color == color)) {
								if (is_inboard(list[i].first - 7, list[i].second)) {
									if (stone[list[i].first - 6][list[i].second].color == 0) {
										if (stone[list[i].first - 5][list[i].second].blocked[direc] == false) {
											pos_score(list[i].first - 4, list[i].second, 2);
										}
										else {
											pos_score(list[i].first - 4, list[i].second, 1);
										}
									}
									else if (stone[list[i].first - 6][list[i].second].color != color) {
										pos_score(list[i].first - 4, list[i].second, 1);
									}
									else {
										pos_score(list[i].first - 4, list[i].second, 2);
									}
								}
								else {
									pos_score(list[i].first - 4, list[i].second, 1);
								}
							}
						}
					}
					break;

				case 3:

					if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooo
						if (is_inboard(list[i].first - 5, list[i].second + 5)) {
							if (stone[list[i].first - 5][list[i].second + 5].color == 0) {
								if (is_inboard(list[i].first - 6, list[i].second + 6)) {
									if (stone[list[i].first - 6][list[i].second + 6].color != color) {
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first + 2, list[i].second - 2) && stone[list[i].first + 2][list[i].second - 2].color != color) {
												pos_score(list[i].first - 4, list[i].second + 4, 2);
												pos_score(list[i].first - 5, list[i].second + 5, 2); // xxooooxx 일때
											}
											else {
												pos_score(list[i].first - 4, list[i].second + 4, 2);   // xooooxx 일때
												pos_score(list[i].first - 5, list[i].second + 5, 1);
											}
										}
										else {   // ooooxx일때
											pos_score(list[i].first - 4, list[i].second + 4, 1);   // ooooxx 일때
											pos_score(list[i].first - 5, list[i].second + 5, 1);
										}
									}
									else {  // 7이 만들어질 가능성이 있을때  // ooooxxo
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											pos_score(list[i].first - 4, list[i].second + 4, 1);
										}
									}
								}
								else { // 벽에 막혀 있을 때
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first + 2, list[i].second - 2) && stone[list[i].first + 2][list[i].second - 2].color != color) {
											pos_score(list[i].first - 4, list[i].second + 4, 2);
											pos_score(list[i].first - 5, list[i].second + 5, 2);  // xxooooxx| 일때
										}
										else {
											pos_score(list[i].first - 4, list[i].second + 4, 2);   // xooooxx| 일때
											pos_score(list[i].first - 5, list[i].second + 5, 1);
										}
									}
									else {   // ooooxx일때
										pos_score(list[i].first - 4, list[i].second + 4, 1);   // ooooxx| 일때
										pos_score(list[i].first - 5, list[i].second + 5, 1);
									}
								}
							}
							else {  // oooox 일때
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first - 5][list[i].second + 5].color != color)
										pos_score(list[i].first - 4, list[i].second + 4, 1);
								}
							}
						}
						else { // oooox| 일 때
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first - 4, list[i].second + 4, 1);
							}
						}

					}
					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooxo
						if (is_inboard(list[i].first - 5, list[i].second + 5)) {
							if ((stone[list[i].first - 4][list[i].second + 4].dir[direc] == 1 && stone[list[i].first - 4][list[i].second + 4].color == color)) {
								if (stone[list[i].first - 3][list[i].second + 3].color == 0) {
									if (stone[list[i].first - 4][list[i].second + 4].blocked[direc] == false) {
										if (stone[list[i].first - 2][list[i].second + 2].blocked[(direc + 4) % 8] == false) { // xoooxox|
											pos_score(list[i].first - 3, list[i].second + 3, 2);
											pos_score(list[i].first - 5, list[i].second + 5, 1);
											if (is_inboard(list[i].first + 2, list[i].second - 2)) {
												if (stone[list[i].first + 2][list[i].second - 2].color == 0) { // xxoooxox|
													pos_score(list[i].first + 1, list[i].second - 1, 1);
												}
												else if (stone[list[i].first + 2][list[i].second - 2].color != color) { //|xoooxox|
													pos_score(list[i].first + 1, list[i].second - 1, 1);
												}
											}
										}
										else {
											pos_score(list[i].first - 3, list[i].second + 3, 1);  // oooxox|
											pos_score(list[i].first - 5, list[i].second + 5, 1);
										}

									} //위아래 막혀있는경우 
									else {
										if (stone[list[i].first - 2][list[i].second + 2].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first + 2, list[i].second - 2)) {
												if (stone[list[i].first + 2][list[i].second - 2].color == 0) {
													pos_score(list[i].first + 1, list[i].second - 1, 2);
												}
												else if (stone[list[i].first + 2][list[i].second - 2].color != color) {
													pos_score(list[i].first + 1, list[i].second - 1, 1);
												}
											}
											else {
												pos_score(list[i].first + 1, list[i].second - 1, 2);
											}
										}
									}
								} // is free
							}// 떨어진거 
							else if ((stone[list[i].first - 5][list[i].second + 5].dir[direc] == 1 && stone[list[i].first - 5][list[i].second + 5].color == color)) {
								if (stone[list[i].first - 3][list[i].second + 3].color == 0 && stone[list[i].first - 4][list[i].second + 4].color == 0) {
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first + 2, list[i].second - 2)) {
											if (stone[list[i].first + 2][list[i].second - 2].color == 0) {
												pos_score(list[i].first - 3, list[i].second + 3, 2);  // xxoooxxo
												pos_score(list[i].first - 4, list[i].second + 4, 1);
											}
											else if (stone[list[i].first + 2][list[i].second - 2].color != color) {
												pos_score(list[i].first - 3, list[i].second + 3, 1);
												pos_score(list[i].first - 4, list[i].second + 4, 1);
											}
										}
										else {
											pos_score(list[i].first - 3, list[i].second + 3, 1);  // xoooxxo
											pos_score(list[i].first - 4, list[i].second + 4, 1);
										}
									}
									else {
										pos_score(list[i].first - 3, list[i].second + 3, 1); // |oooxxo
										pos_score(list[i].first - 4, list[i].second + 4, 1);
									}
								}
							}
						}// is_inboard
						else if ((stone[list[i].first - 4][list[i].second + 4].dir[direc] == 1 && stone[list[i].first - 4][list[i].second + 4].color == color)) {
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first - 3, list[i].second + 3, 1);
								if (is_inboard(list[i].first + 2, list[i].second - 2)) {
									if (stone[list[i].first + 2][list[i].second - 2].color == 0) {
										pos_score(list[i].first + 1, list[i].second - 1, 2);
									}
									else if (stone[list[i].first + 2][list[i].second - 2].color != color) {
										pos_score(list[i].first + 1, list[i].second - 1, 1);
									}
								}
								else {
									pos_score(list[i].first + 1, list[i].second - 1, 1);
								}
							}
						}
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { // ooxoo  ooxxoo
						if (is_inboard(list[i].first - 4, list[i].second + 4)) {
							if ((stone[list[i].first - 3][list[i].second + 3].dir[direc] == 2 && stone[list[i].first - 3][list[i].second + 3].color == color)) {
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first - 3][list[i].second + 3].blocked[direc] == false) {
										pos_score(list[i].first - 2, list[i].second + 2, 1);
										pos_score(list[i].first - 5, list[i].second + 5, 1);
									}
									else {
										pos_score(list[i].first - 2, list[i].second + 2, 0.5);
									}
								}
								else {
									if (stone[list[i].first - 3][list[i].second + 3].blocked[direc] == false) {
										pos_score(list[i].first - 2, list[i].second + 2, 0.5);
										pos_score(list[i].first - 5, list[i].second + 5, 1);
									}
								}
							}// 떨
							else if ((stone[list[i].first - 4][list[i].second + 4].dir[direc] == 2 && stone[list[i].first - 4][list[i].second + 4].color == color)) {
								if (stone[list[i].first - 2][list[i].second + 2].color == 0 && stone[list[i].first - 3][list[i].second + 3].color == 0) {
									pos_score(list[i].first - 2, list[i].second + 2, 1);
								} //is free
							}
						}// is _inboard
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { //ooxoxo
						if (is_inboard(list[i].first - 5, list[i].second + 5)) {
							if ((stone[list[i].first - 3][list[i].second + 3].dir[direc] == 1 && stone[list[i].first - 3][list[i].second + 3].color == color
								&& stone[list[i].first - 3][list[i].second + 3].blocked[direc] == false && stone[list[i].first - 5][list[i].second + 5].dir[direc] == 1 && stone[list[i].first - 5][list[i].second + 5].color == color)) {
								if (is_inboard(list[i].first + 2, list[i].second - 2)) {
									if (stone[list[i].first + 2][list[i].second - 2].color == 0) {
										if (stone[list[i].first + 1][list[i].second - 1].color == 0) {
											pos_score(list[i].first - 2, list[i].second + 2, 2);  //xxooxoxo
										}
										else {
											pos_score(list[i].first - 2, list[i].second + 2, 1); // ooxoxo
										}
									}
									else if (stone[list[i].first + 2][list[i].second - 2].color != color) {
										pos_score(list[i].first - 2, list[i].second + 2, 1);  // xooxoxo
									}
									pos_score(list[i].first - 4, list[i].second + 4, 1);
								}
								else {
									pos_score(list[i].first - 2, list[i].second + 2, 1);  // ooxoxo
									pos_score(list[i].first - 4, list[i].second + 4, 1);
								}

							}// ooxoxo
						}// inboard
					}// if


					if (is_inboard(list[i].first - 5, list[i].second + 5)) {// oxooxo
						if ((stone[list[i].first][list[i].second].dir[direc] == 1) && (stone[list[i].first][list[i].second].blocked[direc] == false)
							&& (stone[list[i].first - 2][list[i].second + 2].blocked[direc] == false) && (stone[list[i].first - 2][list[i].second + 2].dir[direc] == 2)) {
							if ((stone[list[i].first - 2][list[i].second + 2].color == color) && (stone[list[i].first - 5][list[i].second + 5].dir[direc] == 1)
								&& (stone[list[i].first - 5][list[i].second + 5].color == color)) {
								if (is_inboard(list[i].first - 7, list[i].second + 7)) {
									if (stone[list[i].first - 6][list[i].second + 6].color == 0) {
										if (stone[list[i].first - 5][list[i].second + 5].blocked[direc] == false) {
											pos_score(list[i].first - 4, list[i].second + 4, 2);
										}
										else {
											pos_score(list[i].first - 4, list[i].second + 4, 1);
										}
									}
									else if (stone[list[i].first - 6][list[i].second + 6].color != color) {
										pos_score(list[i].first - 4, list[i].second + 4, 1);
									}
									else {
										pos_score(list[i].first - 4, list[i].second + 4, 2);
									}
								}
								else {
									pos_score(list[i].first - 4, list[i].second + 4, 1);
								}
							}
						}
					}
					break;


				case 4:

					if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooo
						if (is_inboard(list[i].first, list[i].second + 5)) {
							if (stone[list[i].first][list[i].second + 5].color == 0) {
								if (is_inboard(list[i].first, list[i].second + 6)) {
									if (stone[list[i].first][list[i].second + 6].color != color) {
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first, list[i].second - 2) && stone[list[i].first][list[i].second - 2].color != color) {
												pos_score(list[i].first, list[i].second + 4, 2);
												pos_score(list[i].first, list[i].second + 5, 2); // xxooooxx 일때

											}
											else {
												pos_score(list[i].first, list[i].second + 4, 2);   // xooooxx 일때
												pos_score(list[i].first, list[i].second + 5, 1);
											}
										}
										else {   // ooooxx일때
											pos_score(list[i].first, list[i].second + 4, 1);   // ooooxx 일때
											pos_score(list[i].first, list[i].second + 5, 1);
										}
									}
									else {  // 7이 만들어질 가능성이 있을때  // ooooxxo
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											pos_score(list[i].first, list[i].second + 4, 1);
										}
									}
								}
								else { // 벽에 막혀 있을 때
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first, list[i].second - 2) && stone[list[i].first][list[i].second - 2].color != color) {
											pos_score(list[i].first, list[i].second + 4, 2);
											pos_score(list[i].first, list[i].second + 5, 2);   // xxooooxx| 일때

										}
										else {
											pos_score(list[i].first, list[i].second + 4, 2);   // xooooxx| 일때
											pos_score(list[i].first, list[i].second + 5, 1);
										}
									}
									else {   // ooooxx일때
										pos_score(list[i].first, list[i].second + 4, 1);   // ooooxx| 일때
										pos_score(list[i].first, list[i].second + 5, 1);
									}
								}
							}
							else {  // oooox 일때
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first][list[i].second + 5].color != color)
										pos_score(list[i].first, list[i].second + 4, 1);
								}
							}
						}
						else { // oooox| 일 때
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first, list[i].second + 4, 1);
							}
						}

					}

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooxo
						if (is_inboard(list[i].first, list[i].second + 5)) {
							if ((stone[list[i].first][list[i].second + 4].dir[direc] == 1 && stone[list[i].first][list[i].second + 4].color == color)) {
								if (stone[list[i].first][list[i].second + 3].color == 0) {
									if (stone[list[i].first][list[i].second + 4].blocked[direc] == false) {
										if (stone[list[i].first][list[i].second + 2].blocked[(direc + 4) % 8] == false) { // xoooxox|
											pos_score(list[i].first, list[i].second + 3, 2);
											pos_score(list[i].first, list[i].second + 5, 1);
											if (is_inboard(list[i].first, list[i].second - 2)) {
												if (stone[list[i].first][list[i].second - 2].color == 0) { // xxoooxox|
													pos_score(list[i].first, list[i].second - 1, 1);
												}
												else if (stone[list[i].first][list[i].second - 2].color != color) { //|xoooxox|
													pos_score(list[i].first, list[i].second - 1, 1);
												}
											}
										}
										else {
											pos_score(list[i].first, list[i].second + 3, 1);  // oooxox|
											pos_score(list[i].first, list[i].second + 5, 1);
										}

									} //위아래 막혀있는경우 
									else {
										if (stone[list[i].first][list[i].second + 2].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first, list[i].second - 2)) {
												if (stone[list[i].first][list[i].second - 2].color == 0) {
													pos_score(list[i].first, list[i].second - 1, 2);
												}
												else if (stone[list[i].first][list[i].second - 2].color != color) {
													pos_score(list[i].first, list[i].second - 1, 1);
												}
											}
											else {
												pos_score(list[i].first, list[i].second - 1, 2);
											}
										}
									}
								} // is free
							}// 떨어진거 
							else if ((stone[list[i].first][list[i].second + 5].dir[direc] == 1 && stone[list[i].first][list[i].second + 5].color == color)) {
								if (stone[list[i].first][list[i].second + 3].color == 0 && stone[list[i].first][list[i].second + 4].color == 0) {
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first, list[i].second - 2)) {
											if (stone[list[i].first][list[i].second - 2].color == 0) {
												pos_score(list[i].first, list[i].second + 3, 2);  // xxoooxxo
												pos_score(list[i].first, list[i].second + 4, 1);
											}
											else if (stone[list[i].first][list[i].second - 2].color != color) {
												pos_score(list[i].first, list[i].second + 3, 1);
												pos_score(list[i].first, list[i].second + 4, 1);
											}
										}
										else {
											pos_score(list[i].first, list[i].second + 3, 1);  // xoooxxo
											pos_score(list[i].first, list[i].second + 4, 1);
										}
									}
									else {
										pos_score(list[i].first, list[i].second + 3, 1); // |oooxxo
										pos_score(list[i].first, list[i].second + 4, 1);
									}
								}
							}
						}// is_inboard
						else if ((stone[list[i].first][list[i].second + 4].dir[direc] == 1 && stone[list[i].first][list[i].second + 4].color == color)) {
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first, list[i].second + 3, 1);
								if (is_inboard(list[i].first, list[i].second - 2)) {
									if (stone[list[i].first][list[i].second - 2].color == 0) {
										pos_score(list[i].first, list[i].second - 1, 2);
									}
									else if (stone[list[i].first][list[i].second - 2].color != color) {
										pos_score(list[i].first, list[i].second - 1, 1);
									}
								}
								else {
									pos_score(list[i].first, list[i].second - 1, 1);
								}
							}
						}
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { // ooxoo  ooxxoo
						if (is_inboard(list[i].first, list[i].second + 4)) {
							if ((stone[list[i].first][list[i].second + 3].dir[direc] == 2 && stone[list[i].first][list[i].second + 3].color == color)) {
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first][list[i].second + 3].blocked[direc] == false) {
										pos_score(list[i].first, list[i].second + 2, 1);
										pos_score(list[i].first, list[i].second + 5, 1);
									}
									else {
										pos_score(list[i].first, list[i].second + 2, 0.5);
									}
								}
								else {
									if (stone[list[i].first][list[i].second + 3].blocked[direc] == false) {
										pos_score(list[i].first, list[i].second + 2, 0.5);
										pos_score(list[i].first, list[i].second + 5, 1);
									}
								}
							}// 떨
							else if ((stone[list[i].first][list[i].second + 4].dir[direc] == 2 && stone[list[i].first][list[i].second + 4].color == color)) {
								if (stone[list[i].first][list[i].second + 2].color == 0 && stone[list[i].first][list[i].second + 3].color == 0) {
									pos_score(list[i].first, list[i].second + 2, 1);
								} //is free
							}
						}// is _inboard
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { //ooxoxo
						if (is_inboard(list[i].first, list[i].second + 5)) {
							if ((stone[list[i].first][list[i].second + 3].dir[direc] == 1 && stone[list[i].first][list[i].second + 3].color == color
								&& stone[list[i].first][list[i].second + 3].blocked[direc] == false && stone[list[i].first][list[i].second + 5].dir[direc] == 1 && stone[list[i].first][list[i].second + 5].color == color)) {
								if (is_inboard(list[i].first, list[i].second - 2)) {
									if (stone[list[i].first][list[i].second - 2].color == 0) {
										if (stone[list[i].first][list[i].second - 1].color == 0) {
											pos_score(list[i].first, list[i].second + 2, 2);  //xxooxoxo
										}
										else {
											pos_score(list[i].first, list[i].second + 2, 1); // ooxoxo
										}
									}
									else if (stone[list[i].first][list[i].second - 2].color != color) {
										pos_score(list[i].first, list[i].second + 2, 1);  // xooxoxo
									}
									pos_score(list[i].first, list[i].second + 4, 1);
								}
								else {
									pos_score(list[i].first, list[i].second + 2, 1);  // ooxoxo
									pos_score(list[i].first, list[i].second + 4, 1);
								}

							}// ooxoxo
						}// inboard
					}// if


					if (is_inboard(list[i].first, list[i].second + 5)) {// oxooxo
						if ((stone[list[i].first][list[i].second].dir[direc] == 1) && (stone[list[i].first][list[i].second].blocked[direc] == false)
							&& (stone[list[i].first][list[i].second + 2].blocked[direc] == false) && (stone[list[i].first][list[i].second + 2].dir[direc] == 2)) {
							if ((stone[list[i].first][list[i].second + 2].color == color) && (stone[list[i].first][list[i].second + 5].dir[direc] == 1)
								&& (stone[list[i].first][list[i].second + 5].color == color)) {
								if (is_inboard(list[i].first, list[i].second + 7)) {
									if (stone[list[i].first][list[i].second + 6].color == 0) {
										if (stone[list[i].first][list[i].second + 5].blocked[direc] == false) {
											pos_score(list[i].first, list[i].second + 4, 2);
										}
										else {
											pos_score(list[i].first, list[i].second + 4, 1);
										}
									}
									else if (stone[list[i].first][list[i].second + 6].color != color) {
										pos_score(list[i].first, list[i].second + 4, 1);
									}
									else {
										pos_score(list[i].first, list[i].second + 4, 2);
									}
								}
								else {
									pos_score(list[i].first, list[i].second + 4, 1);
								}
							}
						}
					}
					break;


				case 5:

					if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooo
						if (is_inboard(list[i].first + 5, list[i].second + 5)) {
							if (stone[list[i].first + 5][list[i].second + 5].color == 0) {
								if (is_inboard(list[i].first + 6, list[i].second + 6)) {
									if (stone[list[i].first + 6][list[i].second + 6].color != color) {
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first - 2, list[i].second - 2) && stone[list[i].first - 2][list[i].second - 2].color != color) {
												pos_score(list[i].first + 4, list[i].second + 4, 2);
												pos_score(list[i].first + 5, list[i].second + 5, 2);  // xxooooxx 일때

											}
											else {
												pos_score(list[i].first + 4, list[i].second + 4, 2);   // xooooxx 일때
												pos_score(list[i].first + 5, list[i].second + 5, 1);
											}
										}
										else {   // ooooxx일때
											pos_score(list[i].first + 4, list[i].second + 4, 1);   // ooooxx 일때
											pos_score(list[i].first + 5, list[i].second + 5, 1);
										}
									}
									else {  // 7이 만들어질 가능성이 있을때  // ooooxxo
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											pos_score(list[i].first + 4, list[i].second + 4, 1);
										}
									}
								}
								else { // 벽에 막혀 있을 때
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first - 2, list[i].second - 2) && stone[list[i].first - 2][list[i].second - 2].color != color) {
											pos_score(list[i].first + 4, list[i].second + 4, 2);
											pos_score(list[i].first + 5, list[i].second + 5, 2);  // xxooooxx| 일때

										}
										else {
											pos_score(list[i].first + 4, list[i].second + 4, 2);   // xooooxx| 일때
											pos_score(list[i].first + 5, list[i].second + 5, 1);
										}
									}
									else {   // ooooxx일때
										pos_score(list[i].first + 4, list[i].second + 4, 1);   // ooooxx| 일때
										pos_score(list[i].first + 5, list[i].second + 5, 1);
									}
								}
							}
							else {  // oooox 일때
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first + 5][list[i].second + 5].color != color)
										pos_score(list[i].first + 4, list[i].second + 4, 1);
								}
							}
						}
						else { // oooox| 일 때
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first + 4, list[i].second + 4, 1);
							}
						}

					}

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooxo
						if (is_inboard(list[i].first + 5, list[i].second + 5)) {
							if ((stone[list[i].first + 4][list[i].second + 4].dir[direc] == 1 && stone[list[i].first + 4][list[i].second + 4].color == color)) {
								if (stone[list[i].first + 3][list[i].second + 3].color == 0) {
									if (stone[list[i].first + 4][list[i].second + 4].blocked[direc] == false) {
										if (stone[list[i].first + 2][list[i].second + 2].blocked[(direc + 4) % 8] == false) { // xoooxox|
											pos_score(list[i].first + 3, list[i].second + 3, 2);
											pos_score(list[i].first + 5, list[i].second + 5, 1);
											if (is_inboard(list[i].first - 2, list[i].second - 2)) {
												if (stone[list[i].first - 2][list[i].second - 2].color == 0) { // xxoooxox|
													pos_score(list[i].first - 1, list[i].second - 1, 1);
												}
												else if (stone[list[i].first - 2][list[i].second - 2].color != color) { //|xoooxox|
													pos_score(list[i].first - 1, list[i].second - 1, 1);
												}
											}
										}
										else {
											pos_score(list[i].first + 3, list[i].second + 3, 1);  // oooxox|
											pos_score(list[i].first + 5, list[i].second + 5, 1);
										}

									} //위아래 막혀있는경우 
									else {
										if (stone[list[i].first + 2][list[i].second + 2].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first - 2, list[i].second - 2)) {
												if (stone[list[i].first - 2][list[i].second - 2].color == 0) {
													pos_score(list[i].first - 1, list[i].second - 1, 2);
												}
												else if (stone[list[i].first - 2][list[i].second - 2].color != color) {
													pos_score(list[i].first - 1, list[i].second - 1, 1);
												}
											}
											else {
												pos_score(list[i].first - 1, list[i].second - 1, 2);
											}
										}
									}
								} // is free
							}// 떨어진거 
							else if ((stone[list[i].first + 5][list[i].second + 5].dir[direc] == 1 && stone[list[i].first + 5][list[i].second + 5].color == color)) {
								if (stone[list[i].first + 3][list[i].second + 3].color == 0 && stone[list[i].first + 4][list[i].second + 4].color == 0) {
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first - 2, list[i].second - 2)) {
											if (stone[list[i].first - 2][list[i].second - 2].color == 0) {
												pos_score(list[i].first + 3, list[i].second + 3, 2);  // xxoooxxo
												pos_score(list[i].first + 4, list[i].second + 4, 1);
											}
											else if (stone[list[i].first - 2][list[i].second - 2].color != color) {
												pos_score(list[i].first + 3, list[i].second + 3, 1);
												pos_score(list[i].first + 4, list[i].second + 4, 1);
											}
										}
										else {
											pos_score(list[i].first + 3, list[i].second + 3, 1);  // xoooxxo
											pos_score(list[i].first + 4, list[i].second + 4, 1);
										}
									}
									else {
										pos_score(list[i].first + 3, list[i].second + 3, 1); // |oooxxo
										pos_score(list[i].first + 4, list[i].second + 4, 1);
									}
								}
							}
						}// is_inboard
						else if ((stone[list[i].first + 4][list[i].second + 4].dir[direc] == 1 && stone[list[i].first + 4][list[i].second + 4].color == color)) {
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first + 3, list[i].second + 3, 1);
								if (is_inboard(list[i].first - 2, list[i].second - 2)) {
									if (stone[list[i].first - 2][list[i].second - 2].color == 0) {
										pos_score(list[i].first - 1, list[i].second - 1, 2);
									}
									else if (stone[list[i].first - 2][list[i].second - 2].color != color) {
										pos_score(list[i].first - 1, list[i].second - 1, 1);
									}
								}
								else {
									pos_score(list[i].first - 1, list[i].second - 1, 1);
								}
							}
						}
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { // ooxoo  ooxxoo
						if (is_inboard(list[i].first + 4, list[i].second + 4)) {
							if ((stone[list[i].first + 3][list[i].second + 3].dir[direc] == 2 && stone[list[i].first + 3][list[i].second + 3].color == color)) {
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first + 3][list[i].second + 3].blocked[direc] == false) {
										pos_score(list[i].first + 2, list[i].second + 2, 1);
										pos_score(list[i].first + 5, list[i].second + 5, 1);
									}
									else {
										pos_score(list[i].first + 2, list[i].second + 2, 0.5);
									}
								}
								else {
									if (stone[list[i].first + 3][list[i].second + 3].blocked[direc] == false) {
										pos_score(list[i].first + 2, list[i].second + 2, 0.5);
										pos_score(list[i].first + 5, list[i].second + 5, 1);
									}
								}
							}// 떨
							else if ((stone[list[i].first + 4][list[i].second + 4].dir[direc] == 2 && stone[list[i].first + 4][list[i].second + 4].color == color)) {
								if (stone[list[i].first + 2][list[i].second + 2].color == 0 && stone[list[i].first + 3][list[i].second + 3].color == 0) {
									pos_score(list[i].first + 2, list[i].second + 2, 1);
								} //is free
							}
						}// is _inboard
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { //ooxoxo
						if (is_inboard(list[i].first + 5, list[i].second + 5)) {
							if ((stone[list[i].first + 3][list[i].second + 3].dir[direc] == 1 && stone[list[i].first + 3][list[i].second + 3].color == color
								&& stone[list[i].first + 3][list[i].second + 3].blocked[direc] == false && stone[list[i].first + 5][list[i].second + 5].dir[direc] == 1 && stone[list[i].first + 5][list[i].second + 5].color == color)) {
								if (is_inboard(list[i].first - 2, list[i].second - 2)) {
									if (stone[list[i].first - 2][list[i].second - 2].color == 0) {
										if (stone[list[i].first - 1][list[i].second - 1].color == 0) {
											pos_score(list[i].first + 2, list[i].second + 2, 2);  //xxooxoxo
										}
										else {
											pos_score(list[i].first + 2, list[i].second + 2, 1); // ooxoxo
										}
									}
									else if (stone[list[i].first - 2][list[i].second - 2].color != color) {
										pos_score(list[i].first + 2, list[i].second + 2, 1);  // xooxoxo
									}
									pos_score(list[i].first + 4, list[i].second + 4, 1);
								}
								else {
									pos_score(list[i].first + 2, list[i].second + 2, 1);  // ooxoxo
									pos_score(list[i].first + 4, list[i].second + 4, 1);
								}

							}// ooxoxo
						}// inboard
					}// if


					if (is_inboard(list[i].first + 5, list[i].second + 5)) {// oxooxo
						if ((stone[list[i].first][list[i].second].dir[direc] == 1) && (stone[list[i].first][list[i].second].blocked[direc] == false)
							&& (stone[list[i].first + 2][list[i].second + 2].blocked[direc] == false) && (stone[list[i].first + 2][list[i].second + 2].dir[direc] == 2)) {
							if ((stone[list[i].first + 2][list[i].second + 2].color == color) && (stone[list[i].first + 5][list[i].second + 5].dir[direc] == 1)
								&& (stone[list[i].first + 5][list[i].second + 5].color == color)) {
								if (is_inboard(list[i].first + 7, list[i].second + 7)) {
									if (stone[list[i].first + 6][list[i].second + 6].color == 0) {
										if (stone[list[i].first + 5][list[i].second + 5].blocked[direc] == false) {
											pos_score(list[i].first + 4, list[i].second + 4, 2);
										}
										else {
											pos_score(list[i].first + 4, list[i].second + 4, 1);
										}
									}
									else if (stone[list[i].first + 6][list[i].second + 6].color != color) {
										pos_score(list[i].first + 4, list[i].second + 4, 1);
									}
									else {
										pos_score(list[i].first + 4, list[i].second + 4, 2);
									}
								}
								else {
									pos_score(list[i].first + 4, list[i].second + 4, 1);
								}
							}
						}
					}
					break;

				case 6:

					if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooo
						if (is_inboard(list[i].first + 5, list[i].second)) {
							if (stone[list[i].first + 5][list[i].second].color == 0) {
								if (is_inboard(list[i].first + 6, list[i].second)) {
									if (stone[list[i].first + 6][list[i].second].color != color) {
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first - 2, list[i].second) && stone[list[i].first - 2][list[i].second].color != color) {
												pos_score(list[i].first + 4, list[i].second, 2);
												pos_score(list[i].first + 5, list[i].second, 2);// xxooooxx 일때

											}
											else {
												pos_score(list[i].first + 4, list[i].second, 2);   // xooooxx 일때
												pos_score(list[i].first + 5, list[i].second, 1);
											}
										}
										else {   // ooooxx일때
											pos_score(list[i].first + 4, list[i].second, 1);   // ooooxx 일때
											pos_score(list[i].first + 5, list[i].second, 1);
										}
									}
									else {  // 7이 만들어질 가능성이 있을때  // ooooxxo
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											pos_score(list[i].first + 4, list[i].second, 1);
										}
									}
								}
								else { // 벽에 막혀 있을 때
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first - 2, list[i].second) && stone[list[i].first - 2][list[i].second].color != color) {
											pos_score(list[i].first + 4, list[i].second, 2);
											pos_score(list[i].first + 5, list[i].second, 2);  // xxooooxx| 일때

										}
										else {
											pos_score(list[i].first + 4, list[i].second, 2);   // xooooxx| 일때
											pos_score(list[i].first + 5, list[i].second, 1);
										}
									}
									else {   // ooooxx일때
										pos_score(list[i].first + 4, list[i].second, 1);   // ooooxx| 일때
										pos_score(list[i].first + 5, list[i].second, 1);
									}
								}
							}
							else {  // oooox 일때
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first + 5][list[i].second].color != color)
										pos_score(list[i].first + 4, list[i].second, 1);
								}
							}
						}
						else { // oooox| 일 때
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first + 4, list[i].second, 1);
							}
						}

					}
					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooxo
						if (is_inboard(list[i].first + 5, list[i].second)) {
							if ((stone[list[i].first + 4][list[i].second].dir[direc] == 1 && stone[list[i].first + 4][list[i].second].color == color)) {
								if (stone[list[i].first + 3][list[i].second].color == 0) {
									if (stone[list[i].first + 4][list[i].second].blocked[direc] == false) {
										if (stone[list[i].first + 2][list[i].second].blocked[(direc + 4) % 8] == false) { // xoooxox|
											pos_score(list[i].first + 3, list[i].second, 2);
											pos_score(list[i].first + 5, list[i].second, 1);
											if (is_inboard(list[i].first - 2, list[i].second)) {
												if (stone[list[i].first - 2][list[i].second].color == 0) { // xxoooxox|
													pos_score(list[i].first - 1, list[i].second, 1);
												}
												else if (stone[list[i].first - 2][list[i].second].color != color) { //|xoooxox|
													pos_score(list[i].first - 1, list[i].second, 1);
												}
											}
										}
										else {
											pos_score(list[i].first + 3, list[i].second, 1);  // oooxox|
											pos_score(list[i].first + 5, list[i].second, 1);
										}

									} //위아래 막혀있는경우 
									else {
										if (stone[list[i].first + 2][list[i].second].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first - 2, list[i].second)) {
												if (stone[list[i].first - 2][list[i].second].color == 0) {
													pos_score(list[i].first - 1, list[i].second, 2);
												}
												else if (stone[list[i].first - 2][list[i].second].color != color) {
													pos_score(list[i].first - 1, list[i].second, 1);
												}
											}
											else {
												pos_score(list[i].first - 1, list[i].second, 2);
											}
										}
									}
								} // is free
							}// 떨어진거 
							else if ((stone[list[i].first + 5][list[i].second].dir[direc] == 1 && stone[list[i].first + 5][list[i].second].color == color)) {
								if (stone[list[i].first + 3][list[i].second].color == 0 && stone[list[i].first + 4][list[i].second].color == 0) {
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first - 2, list[i].second)) {
											if (stone[list[i].first - 2][list[i].second].color == 0) {
												pos_score(list[i].first + 3, list[i].second, 2);  // xxoooxxo
												pos_score(list[i].first + 4, list[i].second, 1);
											}
											else if (stone[list[i].first - 2][list[i].second].color != color) {
												pos_score(list[i].first + 3, list[i].second, 1);
												pos_score(list[i].first + 4, list[i].second, 1);
											}
										}
										else {
											pos_score(list[i].first + 3, list[i].second, 1);  // xoooxxo
											pos_score(list[i].first + 4, list[i].second, 1);
										}
									}
									else {
										pos_score(list[i].first + 3, list[i].second, 1); // |oooxxo
										pos_score(list[i].first + 4, list[i].second, 1);
									}
								}
							}
						}// is_inboard
						else if ((stone[list[i].first + 4][list[i].second].dir[direc] == 1 && stone[list[i].first + 4][list[i].second].color == color)) {
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first + 3, list[i].second, 1);
								if (is_inboard(list[i].first - 2, list[i].second)) {
									if (stone[list[i].first - 2][list[i].second].color == 0) {
										pos_score(list[i].first - 1, list[i].second, 2);
									}
									else if (stone[list[i].first - 2][list[i].second].color != color) {
										pos_score(list[i].first - 1, list[i].second, 1);
									}
								}
								else {
									pos_score(list[i].first - 1, list[i].second, 1);
								}
							}
						}
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { // ooxoo  ooxxoo
						if (is_inboard(list[i].first + 4, list[i].second)) {
							if ((stone[list[i].first + 3][list[i].second].dir[direc] == 2 && stone[list[i].first + 3][list[i].second].color == color)) {
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first + 3][list[i].second].blocked[direc] == false) {
										pos_score(list[i].first + 2, list[i].second, 1);
										pos_score(list[i].first + 5, list[i].second, 1);
									}
									else {
										pos_score(list[i].first + 2, list[i].second, 0.5);
									}
								}
								else {
									if (stone[list[i].first + 3][list[i].second].blocked[direc] == false) {
										pos_score(list[i].first + 2, list[i].second, 0.5);
										pos_score(list[i].first + 5, list[i].second, 1);
									}
								}
							}// 떨
							else if ((stone[list[i].first + 4][list[i].second].dir[direc] == 2 && stone[list[i].first + 4][list[i].second].color == color)) {
								if (stone[list[i].first + 2][list[i].second].color == 0 && stone[list[i].first + 3][list[i].second].color == 0) {
									pos_score(list[i].first + 2, list[i].second, 1);
								} //is free
							}
						}// is _inboard
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { //ooxoxo
						if (is_inboard(list[i].first + 5, list[i].second)) {
							if ((stone[list[i].first + 3][list[i].second].dir[direc] == 1 && stone[list[i].first + 3][list[i].second].color == color
								&& stone[list[i].first + 3][list[i].second].blocked[direc] == false && stone[list[i].first + 5][list[i].second].dir[direc] == 1 && stone[list[i].first + 5][list[i].second].color == color)) {
								if (is_inboard(list[i].first - 2, list[i].second)) {
									if (stone[list[i].first - 2][list[i].second].color == 0) {
										if (stone[list[i].first - 1][list[i].second].color == 0) {
											pos_score(list[i].first + 2, list[i].second, 2);  //xxooxoxo
										}
										else {
											pos_score(list[i].first + 2, list[i].second, 1); // ooxoxo
										}
									}
									else if (stone[list[i].first - 2][list[i].second].color != color) {
										pos_score(list[i].first + 2, list[i].second, 1);  // xooxoxo
									}
									pos_score(list[i].first + 4, list[i].second, 1);
								}
								else {
									pos_score(list[i].first + 2, list[i].second, 1);  // ooxoxo
									pos_score(list[i].first + 4, list[i].second, 1);
								}

							}// ooxoxo
						}// inboard
					}// if


					if (is_inboard(list[i].first + 5, list[i].second)) {// oxooxo
						if ((stone[list[i].first][list[i].second].dir[direc] == 1) && (stone[list[i].first][list[i].second].blocked[direc] == false)
							&& (stone[list[i].first + 2][list[i].second].blocked[direc] == false) && (stone[list[i].first + 2][list[i].second].dir[direc] == 2)) {
							if ((stone[list[i].first + 2][list[i].second].color == color) && (stone[list[i].first + 5][list[i].second].dir[direc] == 1)
								&& (stone[list[i].first + 5][list[i].second].color == color)) {
								if (is_inboard(list[i].first + 7, list[i].second)) {
									if (stone[list[i].first + 6][list[i].second].color == 0) {
										if (stone[list[i].first + 5][list[i].second].blocked[direc] == false) {
											pos_score(list[i].first + 4, list[i].second, 2);
										}
										else {
											pos_score(list[i].first + 4, list[i].second, 1);
										}
									}
									else if (stone[list[i].first + 6][list[i].second].color != color) {
										pos_score(list[i].first + 4, list[i].second, 1);
									}
									else {
										pos_score(list[i].first + 4, list[i].second, 2);
									}
								}
								else {
									pos_score(list[i].first + 4, list[i].second, 1);
								}
							}
						}
					}
					break;

				case 7:

					if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooo
						if (is_inboard(list[i].first + 5, list[i].second - 5)) {
							if (stone[list[i].first + 5][list[i].second - 5].color == 0) {
								if (is_inboard(list[i].first + 6, list[i].second - 6)) {
									if (stone[list[i].first + 6][list[i].second - 6].color != color) {
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first - 2, list[i].second + 2) && stone[list[i].first - 2][list[i].second + 2].color != color) {
												pos_score(list[i].first + 4, list[i].second - 4, 2);
												pos_score(list[i].first + 5, list[i].second - 5, 2); // xxooooxx 일때

											}
											else {
												pos_score(list[i].first + 4, list[i].second - 4, 2);   // xooooxx 일때
												pos_score(list[i].first + 5, list[i].second - 5, 1);
											}
										}
										else {   // ooooxx일때
											pos_score(list[i].first + 4, list[i].second - 4, 1);   // ooooxx 일때
											pos_score(list[i].first + 5, list[i].second - 5, 1);
										}
									}
									else {  // 7이 만들어질 가능성이 있을때  // ooooxxo
										if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
											pos_score(list[i].first + 4, list[i].second - 4, 1);
										}
									}
								}
								else { // 벽에 막혀 있을 때
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first - 2, list[i].second + 2) && stone[list[i].first - 2][list[i].second + 2].color != color) {
											pos_score(list[i].first + 4, list[i].second - 4, 2);
											pos_score(list[i].first + 5, list[i].second - 5, 2);  // xxooooxx| 일때

										}
										else {
											pos_score(list[i].first + 4, list[i].second - 4, 2);   // xooooxx| 일때
											pos_score(list[i].first + 5, list[i].second - 5, 1);
										}
									}
									else {   // ooooxx일때
										pos_score(list[i].first + 4, list[i].second - 4, 1);   // ooooxx| 일때
										pos_score(list[i].first + 5, list[i].second - 5, 1);
									}
								}
							}
							else {  // oooox 일때
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first + 5][list[i].second - 5].color != color)
										pos_score(list[i].first + 4, list[i].second - 4, 1);
								}
							}
						}
						else { // oooox| 일 때
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first + 4, list[i].second - 4, 1);
							}
						}

					}

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) { //oooxo
						if (is_inboard(list[i].first + 5, list[i].second - 5)) {
							if ((stone[list[i].first + 4][list[i].second - 4].dir[direc] == 1 && stone[list[i].first + 4][list[i].second - 4].color == color)) {
								if (stone[list[i].first + 3][list[i].second - 3].color == 0) {
									if (stone[list[i].first + 4][list[i].second - 4].blocked[direc] == false) {
										if (stone[list[i].first + 2][list[i].second - 2].blocked[(direc + 4) % 8] == false) { // xoooxox|
											pos_score(list[i].first + 3, list[i].second - 3, 2);
											pos_score(list[i].first + 5, list[i].second - 5, 1);
											if (is_inboard(list[i].first - 2, list[i].second + 2)) {
												if (stone[list[i].first - 2][list[i].second + 2].color == 0) { // xxoooxox|
													pos_score(list[i].first - 1, list[i].second + 1, 1);
												}
												else if (stone[list[i].first - 2][list[i].second + 2].color != color) { //|xoooxox|
													pos_score(list[i].first - 1, list[i].second + 1, 1);
												}
											}
										}
										else {
											pos_score(list[i].first + 3, list[i].second - 3, 1);  // oooxox|
											pos_score(list[i].first + 5, list[i].second - 5, 1);
										}

									} //위아래 막혀있는경우 
									else {
										if (stone[list[i].first + 2][list[i].second - 2].blocked[(direc + 4) % 8] == false) {
											if (is_inboard(list[i].first - 2, list[i].second + 2)) {
												if (stone[list[i].first - 2][list[i].second + 2].color == 0) {
													pos_score(list[i].first - 1, list[i].second + 1, 2);
												}
												else if (stone[list[i].first - 2][list[i].second + 2].color != color) {
													pos_score(list[i].first - 1, list[i].second + 1, 1);
												}
											}
											else {
												pos_score(list[i].first - 1, list[i].second + 1, 2);
											}
										}
									}
								} // is free
							}// 떨어진거 
							else if ((stone[list[i].first + 5][list[i].second - 5].dir[direc] == 1 && stone[list[i].first + 5][list[i].second - 5].color == color)) {
								if (stone[list[i].first + 3][list[i].second - 3].color == 0 && stone[list[i].first + 4][list[i].second - 4].color == 0) {
									if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
										if (is_inboard(list[i].first - 2, list[i].second + 2)) {
											if (stone[list[i].first - 2][list[i].second + 2].color == 0) {
												pos_score(list[i].first + 3, list[i].second - 3, 2);  // xxoooxxo
												pos_score(list[i].first + 4, list[i].second - 4, 1);
											}
											else if (stone[list[i].first - 2][list[i].second + 2].color != color) {
												pos_score(list[i].first + 3, list[i].second - 3, 1);
												pos_score(list[i].first + 4, list[i].second - 4, 1);
											}
										}
										else {
											pos_score(list[i].first + 3, list[i].second - 3, 1);  // xoooxxo
											pos_score(list[i].first + 4, list[i].second - 4, 1);
										}
									}
									else {
										pos_score(list[i].first + 3, list[i].second - 3, 1); // |oooxxo
										pos_score(list[i].first + 4, list[i].second - 4, 1);
									}
								}
							}
						}// is_inboard
						else if ((stone[list[i].first + 4][list[i].second - 4].dir[direc] == 1 && stone[list[i].first + 4][list[i].second - 4].color == color)) {
							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
								pos_score(list[i].first + 3, list[i].second - 3, 1);
								if (is_inboard(list[i].first - 2, list[i].second + 2)) {
									if (stone[list[i].first - 2][list[i].second + 2].color == 0) {
										pos_score(list[i].first - 1, list[i].second + 1, 2);
									}
									else if (stone[list[i].first - 2][list[i].second + 2].color != color) {
										pos_score(list[i].first - 1, list[i].second + 1, 1);
									}
								}
								else {
									pos_score(list[i].first - 1, list[i].second + 1, 1);
								}
							}
						}
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { // ooxoo  ooxxoo
						if (is_inboard(list[i].first + 4, list[i].second - 4)) {
							if ((stone[list[i].first + 3][list[i].second - 3].dir[direc] == 2 && stone[list[i].first + 3][list[i].second - 3].color == color)) {
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
									if (stone[list[i].first + 3][list[i].second - 3].blocked[direc] == false) {
										pos_score(list[i].first + 2, list[i].second - 2, 1);
										pos_score(list[i].first + 5, list[i].second - 5, 1);
									}
									else {
										pos_score(list[i].first + 2, list[i].second - 2, 0.5);
									}
								}
								else {
									if (stone[list[i].first + 3][list[i].second - 3].blocked[direc] == false) {
										pos_score(list[i].first + 2, list[i].second - 2, 0.5);
										pos_score(list[i].first + 5, list[i].second - 5, 1);
									}
								}
							}// 떨
							else if ((stone[list[i].first + 4][list[i].second - 4].dir[direc] == 2 && stone[list[i].first + 4][list[i].second - 4].color == color)) {
								if (stone[list[i].first + 2][list[i].second - 2].color == 0 && stone[list[i].first + 3][list[i].second - 3].color == 0) {
									pos_score(list[i].first + 2, list[i].second - 2, 1);
								} //is free
							}
						}// is _inboard
					}//if

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) { //ooxoxo
						if (is_inboard(list[i].first + 5, list[i].second - 5)) {
							if ((stone[list[i].first + 3][list[i].second - 3].dir[direc] == 1 && stone[list[i].first + 3][list[i].second - 3].color == color
								&& stone[list[i].first + 3][list[i].second - 3].blocked[direc] == false && stone[list[i].first + 5][list[i].second - 5].dir[direc] == 1 && stone[list[i].first + 5][list[i].second - 5].color == color)) {
								if (is_inboard(list[i].first - 2, list[i].second + 2)) {
									if (stone[list[i].first - 2][list[i].second + 2].color == 0) {
										if (stone[list[i].first - 1][list[i].second + 1].color == 0) {
											pos_score(list[i].first + 2, list[i].second - 2, 2);  //xxooxoxo
										}
										else {
											pos_score(list[i].first + 2, list[i].second - 2, 1); // ooxoxo
										}
									}
									else if (stone[list[i].first - 2][list[i].second + 2].color != color) {
										pos_score(list[i].first + 2, list[i].second - 2, 1);  // xooxoxo
									}
									pos_score(list[i].first + 4, list[i].second - 4, 1);
								}
								else {
									pos_score(list[i].first + 2, list[i].second - 2, 1);  // ooxoxo
									pos_score(list[i].first + 4, list[i].second - 4, 1);
								}

							}// ooxoxo
						}// inboard
					}// if


					if (is_inboard(list[i].first + 5, list[i].second - 5)) {// oxooxo
						if ((stone[list[i].first][list[i].second].dir[direc] == 1) && (stone[list[i].first][list[i].second].blocked[direc] == false)
							&& (stone[list[i].first + 2][list[i].second - 2].blocked[direc] == false) && (stone[list[i].first + 2][list[i].second - 2].dir[direc] == 2)) {
							if ((stone[list[i].first + 2][list[i].second - 2].color == color) && (stone[list[i].first + 5][list[i].second - 5].dir[direc] == 1)
								&& (stone[list[i].first + 5][list[i].second - 5].color == color)) {
								if (is_inboard(list[i].first + 7, list[i].second - 7)) {
									if (stone[list[i].first + 6][list[i].second - 6].color == 0) {
										if (stone[list[i].first + 5][list[i].second - 5].blocked[direc] == false) {
											pos_score(list[i].first + 4, list[i].second - 4, 2);
										}
										else {
											pos_score(list[i].first + 4, list[i].second - 4, 1);
										}
									}
									else if (stone[list[i].first + 6][list[i].second - 6].color != color) {
										pos_score(list[i].first + 4, list[i].second - 4, 1);
									}
									else {
										pos_score(list[i].first + 4, list[i].second - 4, 2);
									}
								}
								else {
									pos_score(list[i].first + 4, list[i].second - 4, 1);
								}
							}
						}
					}
					break;



				}//switch
			}//for direc
		}

	}//for i 

	bool loop = true;
	int max_index = 0;
	int max_index2 = 0;
	int x;
	int y;

	if (!score.empty()) {

		while (loop) {

			for (int i = 0; i < score.size(); i++) {
				x = score[i].second.first;
				y = score[i].second.second;
				if (is_inboard(x, y - 1) && is_inboard(x, y + 1) && stone[x][y - 1].color == 1 && stone[x][y + 1].color == 1) {
					if (stone[x][y - 1].dir[0] + stone[x][y + 1].dir[4] >= 6) {
						score[i].first.first = -44;
					}
				}
				if (is_inboard(x - 1, y - 1) && is_inboard(x + 1, y + 1) && stone[x - 1][y - 1].color == 1 && stone[x + 1][y + 1].color == 1) {
					if (stone[x - 1][y - 1].dir[1] + stone[x + 1][y + 1].dir[5] >= 6) {
						score[i].first.first = -44;
					}
				}
				if (is_inboard(x - 1, y) && is_inboard(x + 1, y) && stone[x - 1][y].color == 1 && stone[x + 1][y].color == 1) {
					if (stone[x - 1][y].dir[2] + stone[x + 1][y].dir[6] >= 6) {
						score[i].first.first = -44;
					}
				}
				if (is_inboard(x - 1, y + 1) && is_inboard(x + 1, y - 1) && stone[x - 1][y + 1].color == 1 && stone[x + 1][y - 1].color == 1) {
					if (stone[x - 1][y + 1].dir[3] + stone[x + 1][y - 1].dir[7] >= 6) {
						score[i].first.first = -44;
					}
				}

			}
			max_index = return_max_index();

			if (optimal_x != score[max_index].second.first || optimal_y != score[max_index].second.second)
				loop = false;
			else
				score.erase(score.begin() + max_index);



			if (score[max_index].first.first >= 2) {
				for (int i = 0; i < score.size(); i++) {
					if (score[i].first.first < 2) {
						//score.erase(score.begin() + i);
						score[i].first.first = -44;
					}
				}
			}
			mode = false;
			find_three(&my_list);
			find_three(&op_list);
			max_index2 = return_max_index();
			if (score[max_index].first.first == score[max_index2].first.first)
			{
				find_two(&my_list);
				find_two(&op_list);
				max_index2 = return_max_index();
			}

			if (optimal_x != score[max_index2].second.first || optimal_y != score[max_index2].second.second)
				loop = false;
			else
				score.erase(score.begin() + max_index);

			optimal_x = score[max_index2].second.first;
			optimal_y = score[max_index2].second.second;
			mode = true;


		}
		return true;
	}
	else {
		return false;
	}

	//   return false;
}bool find_exfour(vector<PAIR> * list2) {
	int color = 0;
	vector<PAIR> list = *list2;

	if (!list2->empty()) {
		if (&list2[0] == &my_list) {
			color = 1;
		}
		else
			color = 2;

	}
	if (!block_list.empty()) {
		if (color != stone[block_list.front().first][block_list.front().second].color) {
			rotation_block();
		}
	}
	//for (int i = 0; i < list.size(); i++) {
	for (int i = list.size() - 1; i >= 0; i--) {
		for (int direc = 0; direc < 8; direc++) {
			if (stone[list[i].first][list[i].second].dir[(direc + 4) % 8] == 1) {
				if (stone[list[i].first][list[i].second].dir[direc] == 4 && stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {
					for (int loc = 0; loc < 8; loc++) {
						if (stone[list[i].first][list[i].second].dir[loc] == 3 && stone[list[i].first][list[i].second].blocked[loc] == false
							&& stone[list[i].first][list[i].second].blocked[(loc + 4) % 8] == false && stone[list[i].first][list[i].second].dir[(loc + 4) % 8] == 1) {
							if (loc == 0) {
								if (stone[list[i].first][list[i].second - 4].color == color) {
									if ((stone[list[i].first][list[i].second - 4].dir[loc] == 1 && stone[list[i].first][list[i].second - 4].blocked[loc] == false)) {
										optimal_x = list[i].first;
										optimal_y = list[i].second - 3;
										return true;
									}
								}
							}
							else if (loc == 1) {
								if (stone[list[i].first - 4][list[i].second - 4].color == color) {
									if ((stone[list[i].first - 4][list[i].second - 4].dir[loc] == 1 && stone[list[i].first - 4][list[i].second - 4].blocked[loc] == false)) {
										optimal_x = list[i].first - 3;
										optimal_y = list[i].second - 3;
										return true;
									}
								}
							}
							else if (loc == 2) {
								if (stone[list[i].first - 4][list[i].second].color == color) {
									if ((stone[list[i].first - 4][list[i].second].dir[loc] == 1 && stone[list[i].first - 4][list[i].second].blocked[loc] == false)) {
										optimal_x = list[i].first - 3;
										optimal_y = list[i].second;
										return true;
									}
								}
							}
							else if (loc == 3) {
								if (stone[list[i].first - 4][list[i].second + 4].color == color) {
									if ((stone[list[i].first - 4][list[i].second + 4].dir[loc] == 1 && stone[list[i].first - 4][list[i].second + 4].blocked[loc] == false)) {
										optimal_x = list[i].first - 3;
										optimal_y = list[i].second + 3;
										return true;
									}
								}
							}
							else if (loc == 4) {
								if (stone[list[i].first][list[i].second + 4].color == color) {
									if ((stone[list[i].first][list[i].second + 4].dir[loc] == 1 && stone[list[i].first][list[i].second + 4].blocked[loc] == false)) {
										optimal_x = list[i].first;
										optimal_y = list[i].second + 3;
										return true;
									}
								}
							}
							else if (loc == 5) {
								if (stone[list[i].first + 4][list[i].second + 4].color == color) {
									if ((stone[list[i].first + 4][list[i].second + 4].dir[loc] == 1 && stone[list[i].first + 4][list[i].second + 4].blocked[loc] == false)) {
										optimal_x = list[i].first + 3;
										optimal_y = list[i].second + 3;
										return true;
									}
								}
							}
							else if (loc == 6) {
								if (stone[list[i].first + 4][list[i].second].color == color) {
									if ((stone[list[i].first + 4][list[i].second].dir[loc] == 1 && stone[list[i].first + 4][list[i].second].blocked[loc] == false)) {
										optimal_x = list[i].first + 3;
										optimal_y = list[i].second;
										return true;
									}
								}
							}
							else if (loc == 7) {
								if (stone[list[i].first + 4][list[i].second - 4].color == color) {
									if ((stone[list[i].first + 4][list[i].second - 4].dir[loc] == 1 && stone[list[i].first + 4][list[i].second - 4].blocked[loc] == false)) {
										optimal_x = list[i].first + 3;
										optimal_y = list[i].second - 3;
										return true;
									}
								}
							}
						}
						else if (stone[list[i].first][list[i].second].dir[loc] == 2 && stone[list[i].first][list[i].second].blocked[loc] == false) {
							if (loc == 0) {
								if (stone[list[i].first][list[i].second - 3].color == color) {
									if ((stone[list[i].first][list[i].second - 3].dir[loc] == 2 && stone[list[i].first][list[i].second - 3].blocked[loc] == false)) {
										optimal_x = list[i].first;
										optimal_y = list[i].second - 2;
										return true;
									}
								}
							}
							if (loc == 1) {
								if (stone[list[i].first - 3][list[i].second - 3].color == color) {
									if ((stone[list[i].first - 3][list[i].second - 3].dir[loc] == 2 && stone[list[i].first - 3][list[i].second - 3].blocked[loc] == false)) {
										optimal_x = list[i].first - 2;
										optimal_y = list[i].second - 2;
										return true;
									}
								}
							}
							if (loc == 2) {
								if (stone[list[i].first - 3][list[i].second].color == color) {
									if ((stone[list[i].first - 3][list[i].second].dir[loc] == 2 && stone[list[i].first - 3][list[i].second].blocked[loc] == false)) {
										optimal_x = list[i].first - 2;
										optimal_y = list[i].second;
										return true;
									}
								}
							}
							if (loc == 3) {
								if (stone[list[i].first - 3][list[i].second + 3].color == color) {
									if ((stone[list[i].first - 3][list[i].second + 3].dir[loc] == 2 && stone[list[i].first - 3][list[i].second + 3].blocked[loc] == false)) {
										optimal_x = list[i].first - 2;
										optimal_y = list[i].second + 2;
										return true;
									}
								}
							}
							if (loc == 4) {
								if (stone[list[i].first][list[i].second + 3].color == color) {
									if ((stone[list[i].first][list[i].second + 3].dir[loc] == 2 && stone[list[i].first][list[i].second + 3].blocked[loc] == false)) {
										optimal_x = list[i].first;
										optimal_y = list[i].second + 2;
										return true;
									}
								}
							}
							if (loc == 5) {
								if (stone[list[i].first + 3][list[i].second + 3].color == color) {
									if ((stone[list[i].first + 3][list[i].second + 3].dir[loc] == 2 && stone[list[i].first + 3][list[i].second + 3].blocked[loc] == false)) {
										optimal_x = list[i].first + 2;
										optimal_y = list[i].second + 2;
										return true;
									}
								}
							}
							if (loc == 6) {
								if (stone[list[i].first + 3][list[i].second].color == color) {
									if ((stone[list[i].first + 3][list[i].second].dir[loc] == 2 && stone[list[i].first + 3][list[i].second].blocked[loc] == false)) {
										optimal_x = list[i].first + 2;
										optimal_y = list[i].second;
										return true;
									}
								}
							}
							if (loc == 7) {
								if (stone[list[i].first + 3][list[i].second - 3].color == color) {
									if ((stone[list[i].first + 3][list[i].second - 3].dir[loc] == 2 && stone[list[i].first + 3][list[i].second - 3].blocked[loc] == false)) {
										optimal_x = list[i].first + 2;
										optimal_y = list[i].second - 2;
										return true;
									}
								}
							}
						}
						else if (stone[list[i].first][list[i].second].dir[loc] == 1 && stone[list[i].first][list[i].second].blocked[loc] == false) {
							if (loc == 0) {
								if (stone[list[i].first][list[i].second - 2].color == color) {
									if ((stone[list[i].first][list[i].second - 2].dir[loc] == 3 && stone[list[i].first][list[i].second - 2].blocked[loc] == false)) {
										optimal_x = list[i].first;
										optimal_y = list[i].second - 1;
										return true;
									}
								}
							}
							if (loc == 1) {
								if (stone[list[i].first - 2][list[i].second - 2].color == color) {
									if ((stone[list[i].first - 2][list[i].second - 2].dir[loc] == 3 && stone[list[i].first - 2][list[i].second - 2].blocked[loc] == false)) {
										optimal_x = list[i].first - 1;
										optimal_y = list[i].second - 1;
										return true;
									}
								}
							}
							if (loc == 2) {
								if (stone[list[i].first - 2][list[i].second].color == color) {
									if ((stone[list[i].first - 2][list[i].second].dir[loc] == 3 && stone[list[i].first - 2][list[i].second].blocked[loc] == false)) {
										optimal_x = list[i].first - 1;
										optimal_y = list[i].second;
										return true;
									}
								}
							}
							if (loc == 3) {
								if (stone[list[i].first - 2][list[i].second + 2].color == color) {
									if ((stone[list[i].first - 2][list[i].second + 2].dir[loc] == 3 && stone[list[i].first - 2][list[i].second + 2].blocked[loc] == false)) {
										optimal_x = list[i].first - 1;
										optimal_y = list[i].second + 1;
										return true;
									}
								}
							}
							if (loc == 4) {
								if (stone[list[i].first][list[i].second + 2].color == color) {
									if ((stone[list[i].first][list[i].second + 2].dir[loc] == 3 && stone[list[i].first][list[i].second + 2].blocked[loc] == false)) {
										optimal_x = list[i].first;
										optimal_y = list[i].second + 1;
										return true;
									}
								}
							}
							if (loc == 5) {
								if (stone[list[i].first + 2][list[i].second + 2].color == color) {
									if ((stone[list[i].first + 2][list[i].second + 2].dir[loc] == 3 && stone[list[i].first + 2][list[i].second + 2].blocked[loc] == false)) {
										optimal_x = list[i].first + 1;
										optimal_y = list[i].second + 1;
										return true;
									}
								}
							}
							if (loc == 6) {
								if (stone[list[i].first + 2][list[i].second].color == color) {
									if ((stone[list[i].first + 2][list[i].second].dir[loc] == 3 && stone[list[i].first + 2][list[i].second].blocked[loc] == false)) {
										optimal_x = list[i].first + 1;
										optimal_y = list[i].second;
										return true;
									}
								}
							}
							if (loc == 7) {
								if (stone[list[i].first + 2][list[i].second - 2].color == color) {
									if ((stone[list[i].first + 2][list[i].second - 2].dir[loc] == 3 && stone[list[i].first + 2][list[i].second - 2].blocked[loc] == false)) {
										optimal_x = list[i].first + 1;
										optimal_y = list[i].second - 1;
										return true;
									}//if
								}//if
							}//if
						}//else if
					}
				}
			}
		}
	}
	return false;

}


void pos_score(int x, int y, double price) {

	bool check_p = false;
	if (price >= 1.5)
		check_p = true;
	for (int i = 0; i < score.size(); i++) {
		if (score[i].second.first == x && score[i].second.second == y) {
			//if (mode)
			score[i].first.first += price;
			score[i].first.second = check_p;
			if (mode)
				score_count[i]++;
			
			two_check = true;
			/*else
			score[i].first.first += (price / 2);*/
			return;
		}
	}
	if (mode) {
		score.push_back(make_pair(make_pair(price, check_p), make_pair(x, y)));
		score_count.push_back(1);
	}

}


bool find_three(vector<PAIR> * list2) {

	if (mode) {
		score.clear();
		score_count.clear();
	}

	int color = 0;
	int check = 0;
	int plus = 0;


	vector<PAIR> list = *list2;

	if (!list2->empty()) {
		if (&list2[0] == &my_list) {
			color = 1;
		}
		else
			color = 2;

	}

	if (!block_list.empty()) {
		if (color != stone[block_list.front().first][block_list.front().second].color) {
			rotation_block();
		}
	}
	//for (int i = 0; i < list.size(); i++) {
	for (int i = list.size() - 1; i >= 0; i--) {
		for (int direc = 0; direc < 8; direc++) {

			if (stone[list[i].first][list[i].second].dir[(direc + 4) % 8] == 1) {

				switch (direc) {

				case 0:

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) {  //●○○○  3개짜리에서 막혀있는경우 
							if (is_inboard(list[i].first, list[i].second - 5)) { //●○○○x x x 3개까지 놓는공간이 보드 안인지 
								if (stone[list[i].first][list[i].second - 3].color == 0 && stone[list[i].first][list[i].second - 4].color == 0 && stone[list[i].first][list[i].second - 5].color == 0) {
									if (is_inboard(list[i].first, list[i].second - 6) && stone[list[i].first][list[i].second - 6].color == color)
										break;
									pos_score(list[i].first, list[i].second - 3, 1); //●○○○★ x x
									pos_score(list[i].first, list[i].second - 4, 1); //●○○○x ★ x
									if (is_inboard(list[i].first, list[i].second - 6) && (board[list[i].first][list[i].second - 6] == 1 || board[list[i].first][list[i].second - 6] == 3)) {
										//●○○○x x x ○  or  ●○○○x x x ◎
										pos_score(list[i].first, list[i].second - 5, 0);
									}
									else {
										pos_score(list[i].first, list[i].second - 5, 1);
									}

								}

							}

						}
						else { //X○○○X  3개짜리가 안막혀있는경우 
							if (is_inboard(list[i].first, list[i].second - 4) && is_inboard(list[i].first, list[i].second + 1)) {
								if (stone[list[i].first][list[i].second - 3].color == 0 && stone[list[i].first][list[i].second - 4].color == 0 && stone[list[i].first][list[i].second + 1].color == 0) {

									// 아직 7의 경우  고려 하지 않앗음. 

									if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second - 5].color != check) {
										if (stone[list[i].first][list[i].second - 5].color == 0) {   //요기 
											if (is_inboard(list[i].first, list[i].second - 6) && stone[list[i].first][list[i].second - 6].color == color)
												break;
											pos_score(list[i].first, list[i].second - 3, 2); //x○○○★ x x
											pos_score(list[i].first, list[i].second - 4, 1); //x○○○x ★ 
											pos_score(list[i].first, list[i].second - 5, 1);
										}
									}
									else {
										pos_score(list[i].first, list[i].second - 3, 1); //x○○○★ x ●
										pos_score(list[i].first, list[i].second - 4, 1); //x○○○x ★ 
									}

									if (is_inboard(list[i].first, list[i].second + 2) && stone[list[i].first][list[i].second + 2].color == check) {
										pos_score(list[i].first, list[i].second + 1, 1); //●★○○○x x 
									}

									//pos_score(list[i].first, list[i].second - 4, 1); //x○○○x ★ 

									//if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][ list[i].second - 5].color == 0)
									//	pos_score(list[i].first, list[i].second - 5, 1);

								}
							}

						}



					}//3개 연속 end if 

					else if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (is_inboard(list[i].first, list[i].second - 3) && stone[list[i].first][list[i].second - 3].dir[direc] == 1 && stone[list[i].first][list[i].second - 3].color == color) {//○○X○ 2개의 한칸 뒤에 한개

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first][list[i].second - 3].blocked[direc] == false) { //X○○X○X



								if (is_inboard(list[i].first, list[i].second + 2) && stone[list[i].first][list[i].second + 2].color == 0
									&& is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second - 5].color == 0) {
									pos_score(list[i].first, list[i].second - 2, 2); //XX○○★○XX
									plus++;
								}
								else
									pos_score(list[i].first, list[i].second - 2, 1); //●X○○★○X

								pos_score(list[i].first, list[i].second + 1, 1); //★○○X○X
								pos_score(list[i].first, list[i].second - 4, 1); //X○○X○★   여기까진 알 수 있음. 

								if (is_inboard(list[i].first, list[i].second + 2))  //★X○○X○
									if (stone[list[i].first][list[i].second + 2].color == 0)
										if (stone[list[i].first][list[i].second + 3].color != color)
											pos_score(list[i].first, list[i].second + 2, 1);

								if (is_inboard(list[i].first, list[i].second - 5)) //○○X○X★
									if (stone[list[i].first][list[i].second - 5].color == 0)
										if (stone[list[i].first][list[i].second - 6].color != color)
											pos_score(list[i].first, list[i].second - 5, 1);


							}
							else if (stone[list[i].first][list[i].second].blocked[direc + 4] == true) { //●○○X○XX 경우 
								if (is_inboard(list[i].first, list[i].second - 5)) {
									if (stone[list[i].first][list[i].second - 4].color == 0 && stone[list[i].first][list[i].second - 5].color == 0) {
										if (is_inboard(list[i].first, list[i].second - 6) && stone[list[i].first][list[i].second - 6].color == color)
											break;
										pos_score(list[i].first, list[i].second - 2, 1);
										pos_score(list[i].first, list[i].second - 4, 1);
										pos_score(list[i].first, list[i].second - 5, 1);
									}

								}
							}
							else if (stone[list[i].first][list[i].second - 3].blocked[direc] == true) { //XX○○X○● 경우 
								if (is_inboard(list[i].first, list[i].second + 2) && stone[list[i].first][list[i].second + 1].color == 0 && stone[list[i].first][list[i].second + 2].color == 0) {
									if (is_inboard(list[i].first, list[i].second + 3) && stone[list[i].first][list[i].second + 3].color == color)
										break;
									pos_score(list[i].first, list[i].second + 1, 1);
									pos_score(list[i].first, list[i].second + 2, 1);
									pos_score(list[i].first, list[i].second - 2, 1);
								}
							}

						}//두개의 한칸 뒤의 한개 end if


						else if (is_inboard(list[i].first, list[i].second - 4) && stone[list[i].first][list[i].second - 4].dir[direc] == 1
							&& stone[list[i].first][list[i].second - 4].color == color) {//○○XX○ 두칸뒤에 한개 
																						 //-----
							if (!stone[list[i].first][list[i].second - 3].color == 0)//자기돌이 있을수 잇음. 
								break;

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first][list[i].second - 4].blocked[direc] == false) { //X○○XX○X
								if (stone[list[i].first][list[i].second + 2].color != color && stone[list[i].first][list[i].second - 6].color != color) {
									pos_score(list[i].first, list[i].second + 1, 1); //★○○XX○X
									pos_score(list[i].first, list[i].second - 2, 1); //X○○★X○X
									pos_score(list[i].first, list[i].second - 3, 1); //X○○X★○X    
									pos_score(list[i].first, list[i].second - 5, 1); //X○○XX○★    
								}
								else if (stone[list[i].first][list[i].second + 2].color != color) {
									pos_score(list[i].first, list[i].second + 1, 1); //★○○XX○X
									pos_score(list[i].first, list[i].second - 2, 1); //X○○★X○X
									pos_score(list[i].first, list[i].second - 3, 1); //X○○X★○X    
								}
								else if (stone[list[i].first][list[i].second - 6].color != color) {
									pos_score(list[i].first, list[i].second - 2, 1); //X○○★X○X
									pos_score(list[i].first, list[i].second - 3, 1); //X○○X★○X    
									pos_score(list[i].first, list[i].second - 5, 1); //X○○XX○★  
								}
								else
									break;
							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○XX○X 경우 
								if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second - 5].color == 0) {
									if (is_inboard(list[i].first, list[i].second - 6) && stone[list[i].first][list[i].second - 6].color == color)
										break;
									pos_score(list[i].first, list[i].second - 2, 1); //●○○★X○X
									pos_score(list[i].first, list[i].second - 3, 1); //●○○X★○X
									pos_score(list[i].first, list[i].second - 5, 1); //●○○XX○★  


								}
							}
							else if (stone[list[i].first][list[i].second - 4].blocked[direc] == true) { //X○○XX○● 경우 
								if (is_inboard(list[i].first, list[i].second + 1) && stone[list[i].first][list[i].second + 1].color == 0) {
									if (is_inboard(list[i].first, list[i].second + 2) && stone[list[i].first][list[i].second + 2].color == color)
										break;
									pos_score(list[i].first, list[i].second + 1, 1); //★○○XX○●  
									pos_score(list[i].first, list[i].second - 2, 1); //X○○★X○●
									pos_score(list[i].first, list[i].second - 3, 1); //X○○X★○●
								}

							}
							//----

						}//2칸뒤 한개 end if

						else if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second - 5].dir[direc] == 1
							&& stone[list[i].first][list[i].second - 5].color == color) {//○○XXX○ 세칸뒤에 한개 

							if (stone[list[i].first][list[i].second - 3].color != 0 || stone[list[i].first][list[i].second - 4].color != 0)
								break;


							pos_score(list[i].first, list[i].second - 2, 1); //○○★XX○
							pos_score(list[i].first, list[i].second - 3, 1); //○○X★X○
							pos_score(list[i].first, list[i].second - 4, 1); //○○XX★○


						}//세칸 뒤 한개 end if 
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {//○X○X○ or ○XX○X○

						if (is_inboard(list[i].first, list[i].second - 4) && stone[list[i].first][list[i].second - 2].dir[direc] == 1 && stone[list[i].first][list[i].second - 4].dir[direc] == 1
							&& stone[list[i].first][list[i].second - 2].color == color && stone[list[i].first][list[i].second - 4].color == color) {  //○X○X○ 경우 
							if (stone[list[i].first][list[i].second - 1].color == 0 && stone[list[i].first][list[i].second - 3].color == 0) {
								if (is_inboard(list[i].first, list[i].second + 1) && is_inboard(list[i].first, list[i].second - 5)
									&& stone[list[i].first][list[i].second + 1].color == 0 && stone[list[i].first][list[i].second - 5].color == 0) { //양쪽 다 뚫린 경우 
									pos_score(list[i].first, list[i].second + 1, 1);
									pos_score(list[i].first, list[i].second - 1, 1.5);
									//pos_score(list[i].first, list[i].second - 3, 1);
									//pos_score(list[i].first, list[i].second - 5, 1);
								}
								else if (is_inboard(list[i].first, list[i].second + 1) && stone[list[i].first][list[i].second + 1].color == 0) { //왼쪽 만 뚫린 경우 
									pos_score(list[i].first, list[i].second + 1, 1);
									pos_score(list[i].first, list[i].second - 1, 1.5);
									//pos_score(list[i].first, list[i].second - 3, 1);
								}
								else if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second - 5].color == 0) { //오른쪽만 뚫린 경우 
									pos_score(list[i].first, list[i].second - 1, 1.5);
									//pos_score(list[i].first, list[i].second - 3, 1);
									//pos_score(list[i].first, list[i].second - 5, 1);
								}
								else {
									break;
								}
							}
						}//○X○X○ end if 
						else if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second - 3].dir[direc] == 1 && stone[list[i].first][list[i].second - 5].dir[direc] == 1
							&& stone[list[i].first][list[i].second - 3].color == color && stone[list[i].first][list[i].second - 5].color == color) {  //○XX○X○ 경우 
							if (stone[list[i].first][list[i].second - 1].color == 0 && stone[list[i].first][list[i].second - 2].color == 0 && stone[list[i].first][list[i].second - 4].color == 0) {
								pos_score(list[i].first, list[i].second - 1, 1.5);
								pos_score(list[i].first, list[i].second - 2, 1.5);
								pos_score(list[i].first, list[i].second - 4, 1.5);
							}
						}//○XX○X○ end if 

					}
					break;


				case 1:

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (stone[list[i].first][list[i].second].blocked[((direc + 4) % 8) % 8] == true) {  //●○○○  3개짜리에서 막혀있는경우 
							if (is_inboard(list[i].first - 5, list[i].second - 5)) { //●○○○x x x 3개까지 놓는공간이 보드 안인지 
								if (stone[list[i].first - 3][list[i].second - 3].color == 0 && stone[list[i].first - 4][list[i].second - 4].color == 0 && stone[list[i].first - 5][list[i].second - 5].color == 0) {
									if (is_inboard(list[i].first - 6, list[i].second - 6) && stone[list[i].first - 6][list[i].second - 6].color == color)
										break;
									pos_score(list[i].first - 3, list[i].second - 3, 1); //●○○○★ x x
									pos_score(list[i].first - 4, list[i].second - 4, 1); //●○○○x ★ x
									if (is_inboard(list[i].first - 6, list[i].second - 6) && (board[list[i].first - 6][list[i].second - 6] == 1 || board[list[i].first - 6][list[i].second - 6] == 3)) {
										//●○○○x x x ○  or  ●○○○x x x ◎
										pos_score(list[i].first - 5, list[i].second - 5, 0);
									}
									else {
										pos_score(list[i].first - 5, list[i].second - 5, 1);
									}

								}

							}

						}
						else { //X○○○X  3개짜리가 안막혀있는경우 
							if (is_inboard(list[i].first - 4, list[i].second - 4) && is_inboard(list[i].first + 1, list[i].second + 1)) {
								if (stone[list[i].first - 3][list[i].second - 3].color == 0 && stone[list[i].first - 4][list[i].second - 4].color == 0 && stone[list[i].first + 1][list[i].second + 1].color == 0) {


									// 아직 7의 경우  고려 하지 않앗음. 

									if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first - 5][list[i].second - 5].color != check) {

										if (stone[list[i].first - 5][list[i].second - 5].color == 0) {   //요기 
											if (is_inboard(list[i].first - 6, list[i].second - 6) && stone[list[i].first - 6][list[i].second - 6].color == color)
												break;
											pos_score(list[i].first - 3, list[i].second - 3, 2); //x○○○★ x x
											pos_score(list[i].first - 4, list[i].second - 4, 1); //x○○○x ★ 
											pos_score(list[i].first - 5, list[i].second - 5, 1);
										}
									}
									else {
										pos_score(list[i].first - 3, list[i].second - 3, 1); //x○○○★ x ●
										pos_score(list[i].first - 4, list[i].second - 4, 1); //x○○○x ★ 
									}

									if (is_inboard(list[i].first + 2, list[i].second + 2) && stone[list[i].first + 2][list[i].second + 2].color == check) {
										pos_score(list[i].first + 1, list[i].second + 1, 1); //●★○○○x x 
									}




								}
							}

						}

					}//3개 연속 end if 

					else if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (is_inboard(list[i].first - 3, list[i].second - 3) && stone[list[i].first - 3][list[i].second - 3].dir[direc] == 1 && stone[list[i].first - 3][list[i].second - 3].color == color) {//○○X○ 2개의 한칸 뒤에 한개

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first - 3][list[i].second - 3].blocked[direc] == false) { //X○○X○X

								if (is_inboard(list[i].first + 2, list[i].second + 2) && stone[list[i].first + 2][list[i].second + 2].color != 2
									&& is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first - 5][list[i].second - 5].color != 2) {
									pos_score(list[i].first - 2, list[i].second - 2, 2); //X○○★○X
									plus++;
								}
								else
									pos_score(list[i].first - 2, list[i].second - 2, 1); //●X○○★○X

								pos_score(list[i].first + 1, list[i].second + 1, 1); //★○○X○X
								pos_score(list[i].first - 4, list[i].second - 4, 1); //X○○X○★   여기까진 알 수 있음. 

								if (is_inboard(list[i].first + 2, list[i].second + 2))  //★X○○X○
									if (stone[list[i].first + 2][list[i].second + 2].color == 0)
										if (stone[list[i].first + 3][list[i].second + 3].color != color)
											pos_score(list[i].first + 2, list[i].second + 2, 1);

								if (is_inboard(list[i].first - 5, list[i].second - 5)) //○○X○X★
									if (stone[list[i].first - 5][list[i].second - 5].color == 0)
										if (stone[list[i].first - 6][list[i].second - 6].color != color)
											pos_score(list[i].first - 5, list[i].second - 5, 1);


							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○X○XX 경우 
								if (is_inboard(list[i].first - 5, list[i].second - 5)) {
									if (stone[list[i].first - 4][list[i].second - 4].color == 0 && stone[list[i].first - 5][list[i].second - 5].color == 0) {
										if (is_inboard(list[i].first - 6, list[i].second - 6) && stone[list[i].first - 6][list[i].second - 6].color == color)
											break;
										pos_score(list[i].first - 2, list[i].second - 2, 1);
										pos_score(list[i].first - 4, list[i].second - 4, 1);
										pos_score(list[i].first - 5, list[i].second - 5, 1);
									}

								}
							}
							else if (stone[list[i].first - 3][list[i].second - 3].blocked[direc] == true) { //XX○○X○● 경우 
								if (is_inboard(list[i].first + 2, list[i].second + 2) && stone[list[i].first + 1][list[i].second + 1].color == 0 && stone[list[i].first + 2][list[i].second + 2].color == 0) {
									if (is_inboard(list[i].first + 3, list[i].second + 3) && stone[list[i].first + 3][list[i].second + 3].color == color)
										break;
									pos_score(list[i].first + 1, list[i].second + 1, 1);
									pos_score(list[i].first + 2, list[i].second + 2, 1);
									pos_score(list[i].first - 2, list[i].second - 2, 1);
								}
							}

						}//두개의 한칸 뒤의 한개 end if


						else if (is_inboard(list[i].first - 4, list[i].second - 4) && stone[list[i].first - 4][list[i].second - 4].dir[direc] == 1
							&& stone[list[i].first - 4][list[i].second - 4].color == color) {//○○XX○ 두칸뒤에 한개 
																							 //-----
							if (!stone[list[i].first - 3][list[i].second - 3].color == 0)//자기돌이 있을수 잇음. 
								break;

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first - 4][list[i].second - 4].blocked[direc] == false) { //X○○XX○X
								if (stone[list[i].first + 2][list[i].second + 2].color != color && stone[list[i].first - 6][list[i].second - 6].color != color) {
									pos_score(list[i].first + 1, list[i].second + 1, 1); //★○○XX○X
									pos_score(list[i].first - 2, list[i].second - 2, 1); //X○○★X○X
									pos_score(list[i].first - 3, list[i].second - 3, 1); //X○○X★○X    
									pos_score(list[i].first - 5, list[i].second - 5, 1); //X○○XX○★    
								}
								else if (stone[list[i].first + 2][list[i].second + 2].color != color) {
									pos_score(list[i].first + 1, list[i].second + 1, 1); //★○○XX○X
									pos_score(list[i].first - 2, list[i].second - 2, 1); //X○○★X○X
									pos_score(list[i].first - 3, list[i].second - 3, 1); //X○○X★○X    
								}
								else if (stone[list[i].first - 6][list[i].second - 6].color != color) {
									pos_score(list[i].first - 2, list[i].second - 2, 1); //X○○★X○X
									pos_score(list[i].first - 3, list[i].second - 3, 1); //X○○X★○X    
									pos_score(list[i].first - 5, list[i].second - 5, 1); //X○○XX○★    
								}
								else
									break;
							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○XX○X 경우 
								if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first - 5][list[i].second - 5].color == 0) {
									if (is_inboard(list[i].first - 6, list[i].second - 6) && stone[list[i].first - 6][list[i].second - 6].color == color)
										break;
									pos_score(list[i].first - 2, list[i].second - 2, 1); //●○○★X○X
									pos_score(list[i].first - 3, list[i].second - 3, 1); //●○○X★○X
									pos_score(list[i].first - 5, list[i].second - 5, 1); //●○○XX○★  


								}
							}
							else if (stone[list[i].first - 4][list[i].second - 4].blocked[direc] == true) { //X○○XX○● 경우 
								if (is_inboard(list[i].first + 1, list[i].second + 1) && stone[list[i].first + 1][list[i].second + 1].color == 0) {
									if (is_inboard(list[i].first + 2, list[i].second + 2) && stone[list[i].first + 2][list[i].second + 2].color == color)
										break;
									pos_score(list[i].first + 1, list[i].second + 1, 1); //★○○XX○●  
									pos_score(list[i].first - 2, list[i].second - 2, 1); //X○○★X○●
									pos_score(list[i].first - 3, list[i].second - 3, 1); //X○○X★○●
								}

							}
							//----

						}//2칸뒤 한개 end if

						else if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first - 5][list[i].second - 5].dir[direc] == 1
							&& stone[list[i].first - 5][list[i].second - 5].color == color) {//○○XXX○ 세칸뒤에 한개 

							if (!stone[list[i].first - 3][list[i].second - 3].color == 0 || !stone[list[i].first - 4][list[i].second - 4].color == 0)
								break;


							pos_score(list[i].first - 2, list[i].second - 2, 1); //○○★XX○
							pos_score(list[i].first - 3, list[i].second - 3, 1); //○○X★X○
							pos_score(list[i].first - 4, list[i].second - 4, 1); //○○XX★○


						}//세칸 뒤 한개 end if 
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {//○X○X○ or ○XX○X○

						if (is_inboard(list[i].first - 4, list[i].second - 4) && stone[list[i].first - 2][list[i].second - 2].dir[direc] == 1 && stone[list[i].first - 4][list[i].second - 4].dir[direc] == 1
							&& stone[list[i].first - 2][list[i].second - 2].color == color && stone[list[i].first - 4][list[i].second - 4].color == color) {  //○X○X○ 경우 
							if (stone[list[i].first - 1][list[i].second - 1].color == 0 && stone[list[i].first - 3][list[i].second - 3].color == 0) {
								if (is_inboard(list[i].first + 1, list[i].second + 1) && is_inboard(list[i].first - 5, list[i].second - 5)
									&& stone[list[i].first + 1][list[i].second + 1].color == 0 && stone[list[i].first - 5][list[i].second - 5].color == 0) { //양쪽 다 뚫린 경우 
									pos_score(list[i].first + 1, list[i].second + 1, 1);
									pos_score(list[i].first - 1, list[i].second - 1, 1.5);
									//	pos_score(list[i].first - 3, list[i].second - 3, 1);
									//pos_score(list[i].first - 5, list[i].second - 5, 1);
								}
								else if (is_inboard(list[i].first + 1, list[i].second + 1) && stone[list[i].first + 1][list[i].second + 1].color == 0) { //왼쪽 만 뚫린 경우 
									pos_score(list[i].first + 1, list[i].second + 1, 1);
									pos_score(list[i].first - 1, list[i].second - 1, 1.5);
									//	pos_score(list[i].first - 3, list[i].second - 3, 1);
								}
								else if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first - 5][list[i].second - 5].color == 0) { //오른쪽만 뚫린 경우 
									pos_score(list[i].first - 1, list[i].second - 1, 1.5);
									//	pos_score(list[i].first - 3, list[i].second - 3, 1);
									//	pos_score(list[i].first - 5, list[i].second - 5, 1);
								}
								else {
									break;
								}
							}
						}//○X○X○ end if 
						else if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first - 3][list[i].second - 3].dir[direc] == 1 && stone[list[i].first - 5][list[i].second - 5].dir[direc] == 1
							&& stone[list[i].first - 3][list[i].second - 3].color == color && stone[list[i].first - 5][list[i].second - 5].color == color) {  //○XX○X○ 경우 
							if (stone[list[i].first - 1][list[i].second - 1].color == 0 && stone[list[i].first - 2][list[i].second - 2].color == 0 && stone[list[i].first - 4][list[i].second - 4].color == 0) {
								pos_score(list[i].first - 1, list[i].second - 1, 1.5);
								pos_score(list[i].first - 2, list[i].second - 2, 1.5);
								pos_score(list[i].first - 4, list[i].second - 4, 1.5);
							}
						}//○XX○X○ end if 

					}
					break;

				case 2:

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (stone[list[i].first][list[i].second].blocked[((direc + 4) % 8) % 8] == true) {  //●○○○  3개짜리에서 막혀있는경우 
							if (is_inboard(list[i].first - 5, list[i].second)) { //●○○○x x x 3개까지 놓는공간이 보드 안인지 
								if (stone[list[i].first - 3][list[i].second].color == 0 && stone[list[i].first - 4][list[i].second].color == 0 && stone[list[i].first - 5][list[i].second].color == 0) {
									if (is_inboard(list[i].first - 6, list[i].second) && stone[list[i].first - 6][list[i].second].color == color)
										break;
									pos_score(list[i].first - 3, list[i].second, 1); //●○○○★ x x
									pos_score(list[i].first - 4, list[i].second, 1); //●○○○x ★ x
									if (is_inboard(list[i].first - 6, list[i].second) && (board[list[i].first - 6][list[i].second] == 1 || board[list[i].first - 6][list[i].second] == 3)) {
										//●○○○x x x ○  or  ●○○○x x x ◎
										pos_score(list[i].first - 5, list[i].second, 0);
									}
									else {
										pos_score(list[i].first - 5, list[i].second, 1);
									}

								}

							}

						}
						else { //X○○○X  3개짜리가 안막혀있는경우 
							if (is_inboard(list[i].first - 4, list[i].second) && is_inboard(list[i].first + 1, list[i].second)) {
								if (stone[list[i].first - 3][list[i].second].color == 0 && stone[list[i].first - 4][list[i].second].color == 0 && stone[list[i].first + 1][list[i].second].color == 0) {


									// 아직 7의 경우  고려 하지 않앗음. 

									if (is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first - 5][list[i].second].color != check) {
										if (stone[list[i].first - 5][list[i].second].color == 0) {   //요기 
											if (is_inboard(list[i].first - 6, list[i].second) && stone[list[i].first - 6][list[i].second].color == color)
												break;
											pos_score(list[i].first - 3, list[i].second, 2); //x○○○★ x x
											pos_score(list[i].first - 4, list[i].second, 1); //x○○○x ★ 
											pos_score(list[i].first - 5, list[i].second, 1);
										}
									}
									else {
										pos_score(list[i].first - 3, list[i].second, 1); //x○○○★ x ●
										pos_score(list[i].first - 4, list[i].second, 1); //x○○○x ★ 
									}

									if (is_inboard(list[i].first + 2, list[i].second) && stone[list[i].first + 2][list[i].second].color == check) {
										pos_score(list[i].first + 1, list[i].second, 1); //●★○○○x x 
									}




								}
							}

						}

					}//3개 연속 end if 

					else if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (is_inboard(list[i].first - 3, list[i].second) && stone[list[i].first - 3][list[i].second].dir[direc] == 1 && stone[list[i].first - 3][list[i].second].color == color) {//○○X○ 2개의 한칸 뒤에 한개

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first - 3][list[i].second].blocked[direc] == false) { //X○○X○X

								if (is_inboard(list[i].first + 2, list[i].second) && stone[list[i].first + 2][list[i].second].color != 2
									&& is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first - 5][list[i].second].color != 2) {
									pos_score(list[i].first - 2, list[i].second, 2); //X○○★○X
									plus++;
								}
								else
									pos_score(list[i].first - 2, list[i].second, 1); //●X○○★○X

								pos_score(list[i].first + 1, list[i].second, 1); //★○○X○X
								pos_score(list[i].first - 4, list[i].second, 1); //X○○X○★   여기까진 알 수 있음. 

								if (is_inboard(list[i].first + 2, list[i].second))  //★X○○X○
									if (stone[list[i].first + 2][list[i].second].color == 0)
										if (stone[list[i].first + 2][list[i].second].color != color)
											pos_score(list[i].first + 2, list[i].second, 1);

								if (is_inboard(list[i].first - 5, list[i].second)) //○○X○X★
									if (stone[list[i].first - 5][list[i].second].color == 0)
										if (stone[list[i].first - 6][list[i].second].color != color)
											pos_score(list[i].first - 5, list[i].second, 1);


							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○X○XX 경우 
								if (is_inboard(list[i].first - 5, list[i].second)) {
									if (stone[list[i].first - 4][list[i].second].color == 0 && stone[list[i].first - 5][list[i].second].color == 0) {  ///여기부터 
										if (is_inboard(list[i].first - 6, list[i].second) && stone[list[i].first - 6][list[i].second].color == color)
											break;
										pos_score(list[i].first - 2, list[i].second, 1);
										pos_score(list[i].first - 4, list[i].second, 1);
										pos_score(list[i].first - 5, list[i].second, 1);
									}

								}
							}
							else if (stone[list[i].first - 3][list[i].second].blocked[direc] == true) { //XX○○X○● 경우 
								if (is_inboard(list[i].first + 2, list[i].second) && stone[list[i].first + 1][list[i].second].color == 0 && stone[list[i].first + 2][list[i].second].color == 0) {
									if (is_inboard(list[i].first + 3, list[i].second) && stone[list[i].first + 3][list[i].second].color == color)
										break;
									pos_score(list[i].first + 1, list[i].second, 1);
									pos_score(list[i].first + 2, list[i].second, 1);
									pos_score(list[i].first - 2, list[i].second, 1);
								}
							}

						}//두개의 한칸 뒤의 한개 end if


						else if (is_inboard(list[i].first - 4, list[i].second) && stone[list[i].first - 4][list[i].second].dir[direc] == 1
							&& stone[list[i].first - 4][list[i].second].color == color) {//○○XX○ 두칸뒤에 한개 
																						 //-----
							if (!stone[list[i].first - 3][list[i].second].color == 0)//자기돌이 있을수 잇음. 
								break;

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first - 4][list[i].second].blocked[direc] == false) { //X○○XX○X
								if (stone[list[i].first + 2][list[i].second].color != color && stone[list[i].first - 6][list[i].second].color != color) {
									pos_score(list[i].first + 1, list[i].second, 1); //★○○XX○X
									pos_score(list[i].first - 2, list[i].second, 1); //X○○★X○X
									pos_score(list[i].first - 3, list[i].second, 1); //X○○X★○X    
									pos_score(list[i].first - 5, list[i].second, 1); //X○○XX○★  
								}
								else if (stone[list[i].first + 2][list[i].second].color != color) {
									pos_score(list[i].first + 1, list[i].second, 1); //★○○XX○X
									pos_score(list[i].first - 2, list[i].second, 1); //X○○★X○X
									pos_score(list[i].first - 3, list[i].second, 1); //X○○X★○X 
								}
								else if (stone[list[i].first - 6][list[i].second].color != color) {
									pos_score(list[i].first - 2, list[i].second, 1); //X○○★X○X
									pos_score(list[i].first - 3, list[i].second, 1); //X○○X★○X    
									pos_score(list[i].first - 5, list[i].second, 1); //X○○XX○★  
								}
								else
									break;

							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○XX○X 경우 
								if (is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first - 5][list[i].second].color == 0) {
									if (is_inboard(list[i].first - 6, list[i].second) && stone[list[i].first - 6][list[i].second].color == color)
										break;
									pos_score(list[i].first - 2, list[i].second, 1); //●○○★X○X
									pos_score(list[i].first - 3, list[i].second, 1); //●○○X★○X
									pos_score(list[i].first - 5, list[i].second, 1); //●○○XX○★  


								}
							}
							else if (stone[list[i].first - 4][list[i].second].blocked[direc] == true) { //X○○XX○● 경우 
								if (is_inboard(list[i].first + 1, list[i].second) && stone[list[i].first + 1][list[i].second].color == 0) {
									if (is_inboard(list[i].first + 2, list[i].second) && stone[list[i].first + 2][list[i].second].color == color)
										break;
									pos_score(list[i].first + 1, list[i].second, 1); //★○○XX○●  
									pos_score(list[i].first - 2, list[i].second, 1); //X○○★X○●
									pos_score(list[i].first - 3, list[i].second, 1); //X○○X★○●
								}

							}
							//----

						}//2칸뒤 한개 end if

						else if (is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first - 5][list[i].second].dir[direc] == 1
							&& stone[list[i].first - 5][list[i].second].color == color) {//○○XXX○ 세칸뒤에 한개 

							if (!stone[list[i].first - 3][list[i].second].color == 0 || !stone[list[i].first - 4][list[i].second].color == 0)
								break;


							pos_score(list[i].first - 2, list[i].second, 1); //○○★XX○
							pos_score(list[i].first - 3, list[i].second, 1); //○○X★X○
							pos_score(list[i].first - 4, list[i].second, 1); //○○XX★○


						}//세칸 뒤 한개 end if 
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {//○X○X○ or ○XX○X○

						if (is_inboard(list[i].first - 4, list[i].second) && stone[list[i].first - 2][list[i].second].dir[direc] == 1 && stone[list[i].first - 4][list[i].second].dir[direc] == 1
							&& stone[list[i].first - 2][list[i].second].color == color && stone[list[i].first - 4][list[i].second].color == color) {  //○X○X○ 경우 
							if (stone[list[i].first - 1][list[i].second].color == 0 && stone[list[i].first - 3][list[i].second].color == 0) {
								if (is_inboard(list[i].first + 1, list[i].second) && is_inboard(list[i].first - 5, list[i].second)
									&& stone[list[i].first + 1][list[i].second].color == 0 && stone[list[i].first][list[i].second].color == 0) { //양쪽 다 뚫린 경우 
									pos_score(list[i].first + 1, list[i].second, 1);
									pos_score(list[i].first - 1, list[i].second, 1.5);
									//	pos_score(list[i].first - 3, list[i].second, 1);
									//	pos_score(list[i].first - 5, list[i].second, 1);
								}
								else if (is_inboard(list[i].first + 1, list[i].second) && stone[list[i].first + 1][list[i].second].color == 0) { //왼쪽 만 뚫린 경우 
									pos_score(list[i].first + 1, list[i].second, 1);
									pos_score(list[i].first - 1, list[i].second, 1.5);
									//	pos_score(list[i].first - 3, list[i].second, 1);
								}
								else if (is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first - 5][list[i].second].color == 0) { //오른쪽만 뚫린 경우 
									pos_score(list[i].first - 1, list[i].second, 1.5);
									//	pos_score(list[i].first - 3, list[i].second, 1);
									//	pos_score(list[i].first - 5, list[i].second, 1);
								}
								else {
									break;
								}
							}
						}//○X○X○ end if 
						else if (is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first - 3][list[i].second].dir[direc] == 1 && stone[list[i].first - 5][list[i].second].dir[direc] == 1
							&& stone[list[i].first - 3][list[i].second].color == color && stone[list[i].first - 5][list[i].second].color == color) {  //○XX○X○ 경우 
							if (stone[list[i].first - 1][list[i].second].color == 0 && stone[list[i].first - 2][list[i].second].color == 0 && stone[list[i].first - 4][list[i].second].color == 0) {
								pos_score(list[i].first - 1, list[i].second, 1.5);
								pos_score(list[i].first - 2, list[i].second, 1.5);
								pos_score(list[i].first - 4, list[i].second, 1.5);
							}
						}//○XX○X○ end if 

					}
					break;
				case 3:

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (stone[list[i].first][list[i].second].blocked[((direc + 4) % 8) % 8] == true) {  //●○○○  3개짜리에서 막혀있는경우 
							if (is_inboard(list[i].first - 5, list[i].second + 5)) { //●○○○x x x 3개까지 놓는공간이 보드 안인지 
								if (stone[list[i].first - 3][list[i].second + 3].color == 0 && stone[list[i].first - 4][list[i].second + 4].color == 0 && stone[list[i].first - 5][list[i].second + 5].color == 0) {
									if (is_inboard(list[i].first - 6, list[i].second + 6) && stone[list[i].first - 6][list[i].second + 6].color == color)
										break;
									pos_score(list[i].first - 3, list[i].second + 3, 1); //●○○○★ x x
									pos_score(list[i].first - 4, list[i].second + 4, 1); //●○○○x ★ x
									if (is_inboard(list[i].first - 6, list[i].second + 6) && (board[list[i].first - 6][list[i].second + 6] == 1 || board[list[i].first - 6][list[i].second + 6] == 3)) {
										//●○○○x x x ○  or  ●○○○x x x ◎
										pos_score(list[i].first - 5, list[i].second + 5, 0);
									}
									else {
										pos_score(list[i].first - 5, list[i].second + 5, 1);
									}

								}

							}

						}
						else { //X○○○X  3개짜리가 안막혀있는경우 
							if (is_inboard(list[i].first - 4, list[i].second + 4) && is_inboard(list[i].first + 1, list[i].second - 1)) {
								if (stone[list[i].first - 3][list[i].second + 3].color == 0 && stone[list[i].first - 4][list[i].second + 4].color == 0 && stone[list[i].first + 1][list[i].second - 1].color == 0) {

									// 아직 7의 경우  고려 하지 않앗음. 

									if (is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first - 5][list[i].second + 5].color != check) {
										if (stone[list[i].first - 5][list[i].second + 5].color == 0) {   //요기 
											if (is_inboard(list[i].first - 6, list[i].second + 6) && stone[list[i].first - 6][list[i].second + 6].color == color)
												break;
											pos_score(list[i].first - 3, list[i].second + 3, 2); //x○○○★ x x
											pos_score(list[i].first - 4, list[i].second + 4, 1); //x○○○x ★ 
											pos_score(list[i].first - 5, list[i].second + 5, 1);
										}
									}
									else {
										pos_score(list[i].first - 3, list[i].second + 3, 1); //x○○○★ x ●
										pos_score(list[i].first - 4, list[i].second + 4, 1); //x○○○x ★ 
									}

									if (is_inboard(list[i].first + 2, list[i].second - 2) && stone[list[i].first + 2][list[i].second - 2].color == check) {
										pos_score(list[i].first + 1, list[i].second - 1, 1); //●★○○○x x 
									}




								}
							}

						}
					}//3개 연속 end if 

					else if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (is_inboard(list[i].first - 3, list[i].second + 3) && stone[list[i].first - 3][list[i].second + 3].dir[direc] == 1 && stone[list[i].first - 3][list[i].second + 3].color == color) {//○○X○ 2개의 한칸 뒤에 한개

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first - 3][list[i].second + 3].blocked[direc] == false) { //X○○X○X

								if (is_inboard(list[i].first + 2, list[i].second - 2) && stone[list[i].first + 2][list[i].second - 2].color != 2
									&& is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first - 5][list[i].second + 5].color != 2) {
									pos_score(list[i].first - 2, list[i].second + 2, 2); //X○○★○X
									plus++;
								}
								else
									pos_score(list[i].first - 2, list[i].second + 2, 1); //●X○○★○X

								pos_score(list[i].first + 1, list[i].second - 1, 1); //★○○X○X
								pos_score(list[i].first - 4, list[i].second + 4, 1); //X○○X○★   여기까진 알 수 있음. 

								if (is_inboard(list[i].first + 2, list[i].second - 2))  //★X○○X○
									if (stone[list[i].first + 2][list[i].second - 2].color == 0)
										if (stone[list[i].first + 3][list[i].second - 3].color != color)
											pos_score(list[i].first + 2, list[i].second - 2, 1);

								if (is_inboard(list[i].first - 5, list[i].second + 5)) //○○X○X★
									if (stone[list[i].first - 5][list[i].second + 5].color == 0)
										if (stone[list[i].first - 6][list[i].second + 6].color != color)
											pos_score(list[i].first - 5, list[i].second + 5, 1);


							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○X○XX 경우 
								if (is_inboard(list[i].first - 5, list[i].second + 5)) {
									if (stone[list[i].first - 4][list[i].second + 4].color == 0 && stone[list[i].first - 5][list[i].second + 5].color == 0) {
										if (is_inboard(list[i].first - 6, list[i].second + 6) && stone[list[i].first - 6][list[i].second + 6].color == color)
											break;
										pos_score(list[i].first - 2, list[i].second + 2, 1);
										pos_score(list[i].first - 4, list[i].second + 4, 1);
										pos_score(list[i].first - 5, list[i].second + 5, 1);
									}

								}
							}
							else if (stone[list[i].first - 3][list[i].second + 3].blocked[direc] == true) { //XX○○X○● 경우 
								if (is_inboard(list[i].first + 2, list[i].second - 2) && stone[list[i].first + 1][list[i].second - 1].color == 0 && stone[list[i].first + 2][list[i].second - 2].color == 0) {
									if (is_inboard(list[i].first + 3, list[i].second - 3) && stone[list[i].first + 3][list[i].second - 3].color == color)
										break;
									pos_score(list[i].first + 1, list[i].second - 1, 1);
									pos_score(list[i].first + 2, list[i].second - 2, 1);
									pos_score(list[i].first - 2, list[i].second + 2, 1);
								}
							}

						}//두개의 한칸 뒤의 한개 end if


						else if (is_inboard(list[i].first - 4, list[i].second + 4) && stone[list[i].first - 4][list[i].second + 4].dir[direc] == 1
							&& stone[list[i].first - 4][list[i].second + 4].color == color) {//○○XX○ 두칸뒤에 한개 
																							 //-----
							if (!stone[list[i].first - 3][list[i].second + 3].color == 0)//자기돌이 있을수 잇음. 
								break;

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first - 4][list[i].second + 4].blocked[direc] == false) { //X○○XX○X
								if (stone[list[i].first + 2][list[i].second - 2].color != color && stone[list[i].first - 6][list[i].second + 6].color != color) {
									pos_score(list[i].first + 1, list[i].second - 1, 1); //★○○XX○X
									pos_score(list[i].first - 2, list[i].second + 2, 1); //X○○★X○X
									pos_score(list[i].first - 3, list[i].second + 3, 1); //X○○X★○X    
									pos_score(list[i].first - 5, list[i].second + 5, 1); //X○○XX○★    
								}
								else if (stone[list[i].first + 2][list[i].second - 2].color != color) {
									pos_score(list[i].first + 1, list[i].second - 1, 1); //★○○XX○X
									pos_score(list[i].first - 2, list[i].second + 2, 1); //X○○★X○X
									pos_score(list[i].first - 3, list[i].second + 3, 1); //X○○X★○X    
								}
								else if (stone[list[i].first - 6][list[i].second + 6].color != color) {
									pos_score(list[i].first - 2, list[i].second + 2, 1); //X○○★X○X
									pos_score(list[i].first - 3, list[i].second + 3, 1); //X○○X★○X    
									pos_score(list[i].first - 5, list[i].second + 5, 1); //X○○XX○★  
								}
								else
									break;
							}

							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○XX○X 경우 
								if (is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first - 5][list[i].second + 5].color == 0) {
									if (is_inboard(list[i].first - 6, list[i].second + 6) && stone[list[i].first - 6][list[i].second + 6].color == color)
										break;
									pos_score(list[i].first - 2, list[i].second + 2, 1); //●○○★X○X
									pos_score(list[i].first - 3, list[i].second + 3, 1); //●○○X★○X
									pos_score(list[i].first - 5, list[i].second + 5, 1); //●○○XX○★  


								}
							}
							else if (stone[list[i].first - 4][list[i].second + 4].blocked[direc] == true) { //X○○XX○● 경우 
								if (is_inboard(list[i].first + 1, list[i].second - 1) && stone[list[i].first + 1][list[i].second - 1].color == 0) {
									if (is_inboard(list[i].first + 2, list[i].second - 2) && stone[list[i].first + 2][list[i].second - 2].color == color)
										break;
									pos_score(list[i].first + 1, list[i].second - 1, 1); //★○○XX○●  
									pos_score(list[i].first - 2, list[i].second + 2, 1); //X○○★X○●
									pos_score(list[i].first - 3, list[i].second + 3, 1); //X○○X★○●
								}

							}
							//----

						}//2칸뒤 한개 end if

						else if (is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first - 5][list[i].second + 5].dir[direc] == 1
							&& stone[list[i].first - 5][list[i].second + 5].color == color) {//○○XXX○ 세칸뒤에 한개 

							if (!stone[list[i].first - 3][list[i].second + 3].color == 0 || !stone[list[i].first - 4][list[i].second + 4].color == 0)
								break;


							pos_score(list[i].first - 2, list[i].second + 2, 1); //○○★XX○
							pos_score(list[i].first - 3, list[i].second + 3, 1); //○○X★X○
							pos_score(list[i].first - 4, list[i].second + 4, 1); //○○XX★○


						}//세칸 뒤 한개 end if 
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {//○X○X○ or ○XX○X○

						if (is_inboard(list[i].first - 4, list[i].second + 4) && stone[list[i].first - 2][list[i].second + 2].dir[direc] == 1 && stone[list[i].first - 4][list[i].second + 4].dir[direc] == 1
							&& stone[list[i].first - 2][list[i].second + 2].color == color && stone[list[i].first - 4][list[i].second + 4].color == color) {  //○X○X○ 경우 
							if (stone[list[i].first - 1][list[i].second + 1].color == 0 && stone[list[i].first - 3][list[i].second + 3].color == 0) {
								if (is_inboard(list[i].first + 1, list[i].second - 1) && is_inboard(list[i].first - 5, list[i].second + 5)
									&& stone[list[i].first + 1][list[i].second - 1].color == 0 && stone[list[i].first - 5][list[i].second + 5].color == 0) { //양쪽 다 뚫린 경우 
									pos_score(list[i].first + 1, list[i].second - 1, 1.5);
									pos_score(list[i].first - 1, list[i].second + 1, 1);
									//	pos_score(list[i].first - 3, list[i].second + 3, 1);
									//	pos_score(list[i].first - 5, list[i].second + 5, 1);
								}
								else if (is_inboard(list[i].first + 1, list[i].second - 1) && stone[list[i].first + 1][list[i].second - 1].color == 0) { //왼쪽 만 뚫린 경우 
									pos_score(list[i].first + 1, list[i].second - 1, 1.5);
									pos_score(list[i].first - 1, list[i].second + 1, 1);
									//	pos_score(list[i].first - 3, list[i].second + 3, 1);
								}
								else if (is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first - 5][list[i].second + 5].color == 0) { //오른쪽만 뚫린 경우 
									pos_score(list[i].first - 1, list[i].second + 1, 1);
									//	pos_score(list[i].first - 3, list[i].second + 3, 1);
									//	pos_score(list[i].first - 5, list[i].second + 5, 1);
								}
								else {
									break;
								}
							}
						}//○X○X○ end if 
						else if (is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first - 3][list[i].second + 3].dir[direc] == 1 && stone[list[i].first - 5][list[i].second + 5].dir[direc] == 1
							&& stone[list[i].first - 3][list[i].second + 3].color == color && stone[list[i].first - 5][list[i].second + 5].color == color) {  //○XX○X○ 경우 
							if (stone[list[i].first - 1][list[i].second + 1].color == 0 && stone[list[i].first - 2][list[i].second + 2].color == 0 && stone[list[i].first - 4][list[i].second + 4].color == 0) {
								pos_score(list[i].first - 1, list[i].second + 1, 1.5);
								pos_score(list[i].first - 2, list[i].second + 2, 1.5);
								pos_score(list[i].first - 4, list[i].second + 4, 1.5);
							}
						}//○XX○X○ end if 

					}
					break;

				case 4:

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (stone[list[i].first][list[i].second].blocked[((direc + 4) % 8) % 8] == true) {  //●○○○  3개짜리에서 막혀있는경우 
							if (is_inboard(list[i].first, list[i].second + 5)) { //●○○○x x x 3개까지 놓는공간이 보드 안인지 
								if (stone[list[i].first][list[i].second + 3].color == 0 && stone[list[i].first][list[i].second + 4].color == 0 && stone[list[i].first][list[i].second + 5].color == 0) {
									if (is_inboard(list[i].first, list[i].second + 6) && stone[list[i].first][list[i].second + 6].color == color)
										break;
									pos_score(list[i].first, list[i].second + 3, 1); //●○○○★ x x
									pos_score(list[i].first, list[i].second + 4, 1); //●○○○x ★ x
									if (is_inboard(list[i].first, list[i].second + 6) && (board[list[i].first][list[i].second + 6] == 1 || board[list[i].first][list[i].second + 6] == 3)) {
										//●○○○x x x ○  or  ●○○○x x x ◎
										pos_score(list[i].first, list[i].second + 5, 0);
									}
									else {
										pos_score(list[i].first, list[i].second + 5, 1);
									}

								}

							}

						}
						else { //X○○○X  3개짜리가 안막혀있는경우 
							if (is_inboard(list[i].first, list[i].second + 4) && is_inboard(list[i].first, list[i].second - 1)) {
								if (stone[list[i].first][list[i].second + 3].color == 0 && stone[list[i].first][list[i].second + 4].color == 0 && stone[list[i].first][list[i].second - 1].color == 0) {
									// 아직 7의 경우  고려 하지 않앗음. 

									if (is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second + 5].color != check) {
										if (stone[list[i].first][list[i].second + 5].color == 0) {   //요기 
											if (is_inboard(list[i].first, list[i].second + 6) && stone[list[i].first][list[i].second + 6].color == color)
												break;
											pos_score(list[i].first, list[i].second + 3, 2); //x○○○★ x x
											pos_score(list[i].first, list[i].second + 4, 1); //x○○○x ★ 
											pos_score(list[i].first, list[i].second + 5, 1);
										}
									}
									else {
										pos_score(list[i].first, list[i].second + 3, 1); //x○○○★ x ●
										pos_score(list[i].first, list[i].second + 4, 1); //x○○○x ★ 
									}

									if (is_inboard(list[i].first, list[i].second - 2) && stone[list[i].first][list[i].second - 2].color == check) {
										pos_score(list[i].first, list[i].second - 1, 1); //●★○○○x x 
									}





								}
							}

						}
					}//3개 연속 end if 

					else if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (is_inboard(list[i].first, list[i].second + 3) && stone[list[i].first][list[i].second + 3].dir[direc] == 1 && stone[list[i].first][list[i].second + 3].color == color) {//○○X○ 2개의 한칸 뒤에 한개

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first][list[i].second + 3].blocked[direc] == false) { //X○○X○X

								if (is_inboard(list[i].first, list[i].second - 2) && stone[list[i].first][list[i].second - 2].color != 2
									&& is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second + 5].color != 2) {
									pos_score(list[i].first, list[i].second + 2, 2); //X○○★○X
									plus++;
								}
								else
									pos_score(list[i].first, list[i].second + 2, 1); //●X○○★○X

								pos_score(list[i].first, list[i].second - 1, 1); //★○○X○X
								pos_score(list[i].first, list[i].second + 4, 1); //X○○X○★   여기까진 알 수 있음. 

								if (is_inboard(list[i].first, list[i].second - 2))  //★X○○X○
									if (stone[list[i].first][list[i].second - 2].color == 0)
										if (stone[list[i].first][list[i].second - 3].color != color)
											pos_score(list[i].first, list[i].second - 2, 1);

								if (is_inboard(list[i].first, list[i].second + 5)) //○○X○X★
									if (stone[list[i].first][list[i].second + 5].color == 0)
										if (stone[list[i].first][list[i].second + 6].color != color)
											pos_score(list[i].first, list[i].second + 5, 1);


							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○X○XX 경우 
								if (is_inboard(list[i].first, list[i].second + 5)) {
									if (stone[list[i].first][list[i].second + 4].color == 0 && stone[list[i].first][list[i].second + 5].color == 0) {
										if (is_inboard(list[i].first, list[i].second + 6) && stone[list[i].first][list[i].second + 6].color == color)
											break;
										pos_score(list[i].first, list[i].second + 2, 1);
										pos_score(list[i].first, list[i].second + 4, 1);
										pos_score(list[i].first, list[i].second + 5, 1);
									}

								}
							}
							else if (stone[list[i].first][list[i].second + 3].blocked[direc] == true) { //XX○○X○● 경우 
								if (is_inboard(list[i].first, list[i].second - 2) && stone[list[i].first][list[i].second - 1].color == 0 && stone[list[i].first][list[i].second - 2].color == 0) {
									if (is_inboard(list[i].first, list[i].second - 3) && stone[list[i].first][list[i].second - 3].color == color)
										break;
									pos_score(list[i].first, list[i].second - 1, 1);
									pos_score(list[i].first, list[i].second - 2, 1);
									pos_score(list[i].first, list[i].second + 2, 1);
								}
							}

						}//두개의 한칸 뒤의 한개 end if


						else if (is_inboard(list[i].first, list[i].second + 4) && stone[list[i].first][list[i].second + 4].dir[direc] == 1
							&& stone[list[i].first][list[i].second + 4].color == color) {//○○XX○ 두칸뒤에 한개 
																						 //-----
							if (!stone[list[i].first][list[i].second + 3].color == 0)//자기돌이 있을수 잇음. 
								break;

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first][list[i].second + 4].blocked[direc] == false) { //X○○XX○X
								if (stone[list[i].first][list[i].second - 2].color != color && stone[list[i].first][list[i].second + 6].color != color) {
									pos_score(list[i].first, list[i].second - 1, 1); //★○○XX○X
									pos_score(list[i].first, list[i].second + 2, 1); //X○○★X○X
									pos_score(list[i].first, list[i].second + 3, 1); //X○○X★○X    
									pos_score(list[i].first, list[i].second + 5, 1); //X○○XX○★    
								}
								else if (stone[list[i].first][list[i].second - 2].color != color) {
									pos_score(list[i].first, list[i].second - 1, 1); //★○○XX○X
									pos_score(list[i].first, list[i].second + 2, 1); //X○○★X○X
									pos_score(list[i].first, list[i].second + 3, 1); //X○○X★○X   
								}
								else if (stone[list[i].first][list[i].second + 6].color != color) {
									pos_score(list[i].first, list[i].second + 2, 1); //X○○★X○X
									pos_score(list[i].first, list[i].second + 3, 1); //X○○X★○X    
									pos_score(list[i].first, list[i].second + 5, 1); //X○○XX○★    
								}
								else
									break;

							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○XX○X 경우 
								if (is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second + 5].color == 0) {
									if (is_inboard(list[i].first, list[i].second + 6) && stone[list[i].first][list[i].second + 6].color == color)
										break;
									pos_score(list[i].first, list[i].second + 2, 1); //●○○★X○X
									pos_score(list[i].first, list[i].second + 3, 1); //●○○X★○X
									pos_score(list[i].first, list[i].second + 5, 1); //●○○XX○★  


								}
							}
							else if (stone[list[i].first][list[i].second + 4].blocked[direc] == true) { //X○○XX○● 경우 
								if (is_inboard(list[i].first, list[i].second - 1) && stone[list[i].first][list[i].second - 1].color == 0) {
									if (is_inboard(list[i].first, list[i].second - 2) && stone[list[i].first][list[i].second - 2].color == color)
										break;
									pos_score(list[i].first, list[i].second - 1, 1); //★○○XX○●  
									pos_score(list[i].first, list[i].second + 2, 1); //X○○★X○●
									pos_score(list[i].first, list[i].second + 3, 1); //X○○X★○●
								}

							}
							//----

						}//2칸뒤 한개 end if

						else if (is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second + 5].dir[direc] == 1
							&& stone[list[i].first][list[i].second + 5].color == color) {//○○XXX○ 세칸뒤에 한개 

							if (!stone[list[i].first][list[i].second + 3].color == 0 || !stone[list[i].first][list[i].second + 4].color == 0)
								break;


							pos_score(list[i].first, list[i].second + 2, 1); //○○★XX○
							pos_score(list[i].first, list[i].second + 3, 1); //○○X★X○
							pos_score(list[i].first, list[i].second + 4, 1); //○○XX★○


						}//세칸 뒤 한개 end if 
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {//○X○X○ or ○XX○X○

						if (is_inboard(list[i].first, list[i].second + 4) && stone[list[i].first][list[i].second + 2].dir[direc] == 1 && stone[list[i].first][list[i].second + 4].dir[direc] == 1
							&& stone[list[i].first][list[i].second + 2].color == color && stone[list[i].first][list[i].second + 4].color == color) {  //○X○X○ 경우 
							if (stone[list[i].first][list[i].second + 1].color == 0 && stone[list[i].first][list[i].second + 3].color == 0) {
								if (is_inboard(list[i].first, list[i].second - 1) && is_inboard(list[i].first, list[i].second + 5)
									&& stone[list[i].first][list[i].second - 1].color == 0 && stone[list[i].first][list[i].second + 5].color == 0) { //양쪽 다 뚫린 경우 
									pos_score(list[i].first, list[i].second - 1, 1.5);
									pos_score(list[i].first, list[i].second + 1, 1);
									//	pos_score(list[i].first, list[i].second + 3, 1);
									//	pos_score(list[i].first, list[i].second + 5, 1);
								}
								else if (is_inboard(list[i].first, list[i].second - 1) && stone[list[i].first][list[i].second - 1].color == 0) { //왼쪽 만 뚫린 경우 
									pos_score(list[i].first, list[i].second - 1, 1.5);
									pos_score(list[i].first, list[i].second + 1, 1);
									//	pos_score(list[i].first, list[i].second + 3, 1);
								}
								else if (is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second + 5].color == 0) { //오른쪽만 뚫린 경우 
									pos_score(list[i].first, list[i].second + 1, 1);
									//	pos_score(list[i].first, list[i].second + 3, 1);
									//	pos_score(list[i].first, list[i].second + 5, 1);
								}
								else {
									break;
								}
							}
						}//○X○X○ end if 
						else if (is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second + 3].dir[direc] == 1 && stone[list[i].first][list[i].second + 5].dir[direc] == 1
							&& stone[list[i].first][list[i].second + 3].color == color && stone[list[i].first][list[i].second + 5].color == color) {  //○XX○X○ 경우 
							if (stone[list[i].first][list[i].second + 1].color == 0 && stone[list[i].first][list[i].second + 2].color == 0 && stone[list[i].first][list[i].second + 4].color == 0) {
								pos_score(list[i].first, list[i].second + 1, 1.5);
								pos_score(list[i].first, list[i].second + 2, 1.5);
								pos_score(list[i].first, list[i].second + 4, 1.5);
							}
						}//○XX○X○ end if 

					}
					break;

				case 5:

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (stone[list[i].first][list[i].second].blocked[((direc + 4) % 8) % 8] == true) {  //●○○○  3개짜리에서 막혀있는경우 
							if (is_inboard(list[i].first + 5, list[i].second + 5)) { //●○○○x x x 3개까지 놓는공간이 보드 안인지 
								if (stone[list[i].first + 3][list[i].second + 3].color == 0 && stone[list[i].first + 4][list[i].second + 4].color == 0 && stone[list[i].first + 5][list[i].second + 5].color == 0) {
									if (is_inboard(list[i].first + 6, list[i].second + 6) && stone[list[i].first + 6][list[i].second + 6].color == color)
										break;
									pos_score(list[i].first + 3, list[i].second + 3, 1); //●○○○★ x x
									pos_score(list[i].first + 4, list[i].second + 4, 1); //●○○○x ★ x
									if (is_inboard(list[i].first + 6, list[i].second + 6) && (board[list[i].first + 6][list[i].second + 6] == 1 || board[list[i].first + 6][list[i].second + 6] == 3)) {
										//●○○○x x x ○  or  ●○○○x x x ◎
										pos_score(list[i].first + 5, list[i].second + 5, 0);
									}
									else {
										pos_score(list[i].first + 5, list[i].second + 5, 1);
									}

								}

							}

						}
						else { //X○○○X  3개짜리가 안막혀있는경우 
							if (is_inboard(list[i].first + 4, list[i].second + 4) && is_inboard(list[i].first - 1, list[i].second - 1)) {
								if (stone[list[i].first + 3][list[i].second + 3].color == 0 && stone[list[i].first + 4][list[i].second + 4].color == 0 && stone[list[i].first - 1][list[i].second - 1].color == 0) {

									// 아직 7의 경우  고려 하지 않앗음. 

									if (is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first + 5][list[i].second + 5].color != check) {
										if (stone[list[i].first + 5][list[i].second + 5].color == 0) {   //요기 
											if (is_inboard(list[i].first + 6, list[i].second + 6) && stone[list[i].first + 6][list[i].second + 6].color == color)
												break;
											pos_score(list[i].first + 3, list[i].second + 3, 2); //x○○○★ x x
											pos_score(list[i].first + 4, list[i].second + 4, 1); //x○○○x ★ 
											pos_score(list[i].first + 5, list[i].second + 5, 1);
										}
									}
									else {
										pos_score(list[i].first + 3, list[i].second + 3, 1); //x○○○★ x ●
										pos_score(list[i].first + 4, list[i].second + 4, 1); //x○○○x ★ 
									}

									if (is_inboard(list[i].first - 2, list[i].second - 2) && stone[list[i].first - 2][list[i].second - 2].color == check) {
										pos_score(list[i].first - 1, list[i].second - 1, 1); //●★○○○x x 
									}


									\

								}
							}

						}
					}//3개 연속 end if 

					else if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (is_inboard(list[i].first + 3, list[i].second + 3) && stone[list[i].first + 3][list[i].second + 3].dir[direc] == 1 && stone[list[i].first + 3][list[i].second + 3].color == color) {//○○X○ 2개의 한칸 뒤에 한개

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first + 3][list[i].second + 3].blocked[direc] == false) { //X○○X○X

								if (is_inboard(list[i].first - 2, list[i].second - 2) && stone[list[i].first - 2][list[i].second - 2].color != 2
									&& is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first + 5][list[i].second + 5].color != 2) {
									pos_score(list[i].first + 2, list[i].second + 2, 2); //X○○★○X
									plus++;
								}
								else
									pos_score(list[i].first + 2, list[i].second + 2, 1); //●X○○★○X

								pos_score(list[i].first - 1, list[i].second - 1, 1); //★○○X○X
								pos_score(list[i].first + 4, list[i].second + 4, 1); //X○○X○★   여기까진 알 수 있음. 

								if (is_inboard(list[i].first - 2, list[i].second - 2))  //★X○○X○
									if (stone[list[i].first - 2][list[i].second - 2].color == 0)
										if (stone[list[i].first - 3][list[i].second - 3].color != color)
											pos_score(list[i].first - 2, list[i].second - 2, 1);

								if (is_inboard(list[i].first + 5, list[i].second + 5)) //○○X○X★
									if (stone[list[i].first + 5][list[i].second + 5].color == 0)
										if (stone[list[i].first + 6][list[i].second + 6].color != color)
											pos_score(list[i].first + 5, list[i].second + 5, 1);


							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○X○XX 경우 
								if (is_inboard(list[i].first + 5, list[i].second + 5)) {
									if (stone[list[i].first + 4][list[i].second + 4].color == 0 && stone[list[i].first + 5][list[i].second + 5].color == 0) {
										if (is_inboard(list[i].first + 6, list[i].second + 6) && stone[list[i].first + 6][list[i].second + 6].color == color)
											break;
										pos_score(list[i].first + 2, list[i].second + 2, 1);
										pos_score(list[i].first + 4, list[i].second + 4, 1);
										pos_score(list[i].first + 5, list[i].second + 5, 1);
									}

								}
							}
							else if (stone[list[i].first + 3][list[i].second + 3].blocked[direc] == true) { //XX○○X○● 경우 
								if (is_inboard(list[i].first - 2, list[i].second - 2) && stone[list[i].first - 1][list[i].second - 1].color == 0 && stone[list[i].first - 2][list[i].second - 2].color == 0) {
									if (is_inboard(list[i].first - 3, list[i].second - 3) && stone[list[i].first - 3][list[i].second - 3].color == color)
										break;
									pos_score(list[i].first - 1, list[i].second - 1, 1);
									pos_score(list[i].first - 2, list[i].second - 2, 1);
									pos_score(list[i].first + 2, list[i].second + 2, 1);
								}
							}

						}//두개의 한칸 뒤의 한개 end if


						else if (is_inboard(list[i].first + 4, list[i].second + 4) && stone[list[i].first + 4][list[i].second + 4].dir[direc] == 1
							&& stone[list[i].first + 4][list[i].second + 4].color == color) {//○○XX○ 두칸뒤에 한개 
																							 //-----
							if (!stone[list[i].first + 3][list[i].second + 3].color == 0)//자기돌이 있을수 잇음. 
								break;

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first + 4][list[i].second + 4].blocked[direc] == false) { //X○○XX○X
								if (stone[list[i].first - 2][list[i].second - 2].color != color && stone[list[i].first + 6][list[i].second + 6].color != color) {
									pos_score(list[i].first - 1, list[i].second - 1, 1); //★○○XX○X
									pos_score(list[i].first + 2, list[i].second + 2, 1); //X○○★X○X
									pos_score(list[i].first + 3, list[i].second + 3, 1); //X○○X★○X    
									pos_score(list[i].first + 5, list[i].second + 5, 1); //X○○XX○★   
								}
								else if (stone[list[i].first - 2][list[i].second - 2].color != color) {
									pos_score(list[i].first - 1, list[i].second - 1, 1); //★○○XX○X
									pos_score(list[i].first + 2, list[i].second + 2, 1); //X○○★X○X
									pos_score(list[i].first + 3, list[i].second + 3, 1); //X○○X★○X    

								}
								else if (stone[list[i].first + 6][list[i].second + 6].color != color) {
									pos_score(list[i].first + 2, list[i].second + 2, 1); //X○○★X○X
									pos_score(list[i].first + 3, list[i].second + 3, 1); //X○○X★○X    
									pos_score(list[i].first + 5, list[i].second + 5, 1); //X○○XX○★   
								}
								else
									break;

							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○XX○X 경우 
								if (is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first + 5][list[i].second + 5].color == 0) {
									if (is_inboard(list[i].first + 6, list[i].second + 6) && stone[list[i].first + 6][list[i].second + 6].color == color)
										break;
									pos_score(list[i].first + 2, list[i].second + 2, 1); //●○○★X○X
									pos_score(list[i].first + 3, list[i].second + 3, 1); //●○○X★○X
									pos_score(list[i].first + 5, list[i].second + 5, 1); //●○○XX○★  


								}
							}
							else if (stone[list[i].first + 4][list[i].second + 4].blocked[direc] == true) { //X○○XX○● 경우 
								if (is_inboard(list[i].first - 1, list[i].second - 1) && stone[list[i].first - 1][list[i].second - 1].color == 0) {
									if (is_inboard(list[i].first - 2, list[i].second - 2) && stone[list[i].first - 2][list[i].second - 2].color == color)
										break;
									pos_score(list[i].first - 1, list[i].second - 1, 1); //★○○XX○●  
									pos_score(list[i].first + 2, list[i].second + 2, 1); //X○○★X○●
									pos_score(list[i].first + 3, list[i].second + 3, 1); //X○○X★○●
								}

							}
							//----

						}//2칸뒤 한개 end if

						else if (is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first + 5][list[i].second + 5].dir[direc] == 1
							&& stone[list[i].first + 5][list[i].second + 5].color == color) {//○○XXX○ 세칸뒤에 한개 

							if (!stone[list[i].first + 3][list[i].second + 3].color == 0 || !stone[list[i].first + 4][list[i].second + 4].color == 0)
								break;


							pos_score(list[i].first + 2, list[i].second + 2, 1); //○○★XX○
							pos_score(list[i].first + 3, list[i].second + 3, 1); //○○X★X○
							pos_score(list[i].first + 4, list[i].second + 4, 1); //○○XX★○


						}//세칸 뒤 한개 end if 
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {//○X○X○ or ○XX○X○

						if (is_inboard(list[i].first + 4, list[i].second + 4) && stone[list[i].first + 2][list[i].second + 2].dir[direc] == 1 && stone[list[i].first + 4][list[i].second + 4].dir[direc] == 1
							&& stone[list[i].first + 2][list[i].second + 2].color == color && stone[list[i].first + 4][list[i].second + 4].color == color) {  //○X○X○ 경우 
							if (stone[list[i].first + 1][list[i].second + 1].color == 0 && stone[list[i].first + 3][list[i].second + 3].color == 0) {
								if (is_inboard(list[i].first - 1, list[i].second - 1) && is_inboard(list[i].first + 5, list[i].second + 5)
									&& stone[list[i].first - 1][list[i].second - 1].color == 0 && stone[list[i].first + 5][list[i].second + 5].color == 0) { //양쪽 다 뚫린 경우 
									pos_score(list[i].first - 1, list[i].second - 1, 1.5);
									pos_score(list[i].first + 1, list[i].second + 1, 1);
									//	pos_score(list[i].first + 3, list[i].second + 3, 1);
									//	pos_score(list[i].first + 5, list[i].second + 5, 1);
								}
								else if (is_inboard(list[i].first - 1, list[i].second - 1) && stone[list[i].first - 1][list[i].second - 1].color == 0) { //왼쪽 만 뚫린 경우 
									pos_score(list[i].first - 1, list[i].second - 1, 1.5);
									pos_score(list[i].first + 1, list[i].second + 1, 1);
									//	pos_score(list[i].first + 3, list[i].second + 3, 1);
								}
								else if (is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first + 5][list[i].second + 5].color == 0) { //오른쪽만 뚫린 경우 
									pos_score(list[i].first + 1, list[i].second + 1, 1);
									//	pos_score(list[i].first + 3, list[i].second + 3, 1);
									//	pos_score(list[i].first + 5, list[i].second + 5, 1);
								}
								else {
									break;
								}
							}
						}//○X○X○ end if 
						else if (is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first + 3][list[i].second + 3].dir[direc] == 1 && stone[list[i].first + 5][list[i].second + 5].dir[direc] == 1
							&& stone[list[i].first + 3][list[i].second + 3].color == color && stone[list[i].first + 5][list[i].second + 5].color == color) {  //○XX○X○ 경우 
							if (stone[list[i].first + 1][list[i].second + 1].color == 0 && stone[list[i].first + 2][list[i].second + 2].color == 0 && stone[list[i].first + 4][list[i].second + 4].color == 0) {
								pos_score(list[i].first + 1, list[i].second + 1, 1.5);
								pos_score(list[i].first + 2, list[i].second + 2, 1.5);
								pos_score(list[i].first + 4, list[i].second + 4, 1.5);
							}
						}//○XX○X○ end if 

					}
					break;

				case 6:

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (stone[list[i].first][list[i].second].blocked[((direc + 4) % 8) % 8] == true) {  //●○○○  3개짜리에서 막혀있는경우 
							if (is_inboard(list[i].first + 5, list[i].second)) { //●○○○x x x 3개까지 놓는공간이 보드 안인지 
								if (stone[list[i].first + 3][list[i].second].color == 0 && stone[list[i].first + 4][list[i].second].color == 0 && stone[list[i].first + 5][list[i].second].color == 0) {
									if (is_inboard(list[i].first + 6, list[i].second) && stone[list[i].first + 6][list[i].second].color == color)
										break;
									pos_score(list[i].first + 3, list[i].second, 1); //●○○○★ x x
									pos_score(list[i].first + 4, list[i].second, 1); //●○○○x ★ x
									if (is_inboard(list[i].first + 6, list[i].second) && (board[list[i].first + 6][list[i].second] == 1 || board[list[i].first + 6][list[i].second] == 3)) {
										//●○○○x x x ○  or  ●○○○x x x ◎
										pos_score(list[i].first + 5, list[i].second, 0);
									}
									else {
										pos_score(list[i].first + 5, list[i].second, 1);
									}

								}

							}

						}
						else { //X○○○X  3개짜리가 안막혀있는경우 
							if (is_inboard(list[i].first + 4, list[i].second) && is_inboard(list[i].first - 1, list[i].second)) {
								if (stone[list[i].first + 3][list[i].second].color == 0 && stone[list[i].first + 4][list[i].second].color == 0 && stone[list[i].first - 1][list[i].second].color == 0) {
									// 아직 7의 경우  고려 하지 않앗음. 

									if (is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first + 5][list[i].second].color != check) {
										if (stone[list[i].first + 5][list[i].second].color == 0) {   //요기 
											if (is_inboard(list[i].first + 6, list[i].second) && stone[list[i].first + 6][list[i].second].color == color)
												break;
											pos_score(list[i].first + 3, list[i].second, 2); //x○○○★ x x
											pos_score(list[i].first + 4, list[i].second, 1); //x○○○x ★ 
											pos_score(list[i].first + 5, list[i].second, 1);
										}
									}
									else {
										pos_score(list[i].first + 3, list[i].second, 1); //x○○○★ x ●
										pos_score(list[i].first + 4, list[i].second, 1); //x○○○x ★ 
									}

									if (is_inboard(list[i].first - 2, list[i].second) && stone[list[i].first - 2][list[i].second].color == check) {
										pos_score(list[i].first - 1, list[i].second, 1); //●★○○○x x 
									}




								}
							}

						}
					}//3개 연속 end if 

					else if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (is_inboard(list[i].first + 3, list[i].second) && stone[list[i].first + 3][list[i].second].dir[direc] == 1 && stone[list[i].first + 3][list[i].second].color == color) {//○○X○ 2개의 한칸 뒤에 한개

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first + 3][list[i].second].blocked[direc] == false) { //X○○X○X

								if (is_inboard(list[i].first - 2, list[i].second) && stone[list[i].first - 2][list[i].second].color != 2
									&& is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first + 5][list[i].second].color != 2) {
									pos_score(list[i].first + 2, list[i].second, 2); //X○○★○X
									plus++;
								}
								else
									pos_score(list[i].first + 2, list[i].second, 1); //●X○○★○X

								pos_score(list[i].first - 1, list[i].second, 1); //★○○X○X
								pos_score(list[i].first + 4, list[i].second, 1); //X○○X○★   여기까진 알 수 있음. 

								if (is_inboard(list[i].first - 2, list[i].second))  //★X○○X○
									if (stone[list[i].first - 2][list[i].second].color == 0)
										if (stone[list[i].first - 3][list[i].second].color != color)
											pos_score(list[i].first - 2, list[i].second, 1);

								if (is_inboard(list[i].first + 5, list[i].second)) //○○X○X★
									if (stone[list[i].first + 5][list[i].second].color == 0)
										if (stone[list[i].first + 6][list[i].second].color != color)
											pos_score(list[i].first + 5, list[i].second, 1);


							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○X○XX 경우 
								if (is_inboard(list[i].first + 5, list[i].second)) {
									if (stone[list[i].first + 4][list[i].second].color == 0 && stone[list[i].first + 5][list[i].second].color == 0) {
										if (is_inboard(list[i].first + 6, list[i].second) && stone[list[i].first + 6][list[i].second].color == color)
											break;
										pos_score(list[i].first + 2, list[i].second, 1);
										pos_score(list[i].first + 4, list[i].second, 1);
										pos_score(list[i].first + 5, list[i].second, 1);
									}

								}
							}
							else if (stone[list[i].first + 3][list[i].second].blocked[direc] == true) { //XX○○X○● 경우 
								if (is_inboard(list[i].first - 2, list[i].second) && stone[list[i].first - 1][list[i].second].color == 0 && stone[list[i].first - 2][list[i].second].color == 0) {
									if (is_inboard(list[i].first - 3, list[i].second) && stone[list[i].first - 3][list[i].second].color == color)
										break;
									pos_score(list[i].first - 1, list[i].second, 1);
									pos_score(list[i].first - 2, list[i].second, 1);
									pos_score(list[i].first + 2, list[i].second, 1);
								}
							}

						}//두개의 한칸 뒤의 한개 end if


						else if (is_inboard(list[i].first + 4, list[i].second) && stone[list[i].first + 4][list[i].second].dir[direc] == 1
							&& stone[list[i].first + 4][list[i].second].color == color) {//○○XX○ 두칸뒤에 한개 
																						 //-----
							if (!stone[list[i].first + 3][list[i].second].color == 0)//자기돌이 있을수 잇음. 
								break;

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first + 4][list[i].second].blocked[direc] == false) { //X○○XX○X
								if (stone[list[i].first - 2][list[i].second].color != color && stone[list[i].first + 6][list[i].second].color != color) {
									pos_score(list[i].first - 1, list[i].second, 1); //★○○XX○X
									pos_score(list[i].first + 2, list[i].second, 1); //X○○★X○X
									pos_score(list[i].first + 3, list[i].second, 1); //X○○X★○X    
									pos_score(list[i].first + 5, list[i].second, 1); //X○○XX○★    
								}
								else if (stone[list[i].first - 2][list[i].second].color != color) {
									pos_score(list[i].first - 1, list[i].second, 1); //★○○XX○X
									pos_score(list[i].first + 2, list[i].second, 1); //X○○★X○X
									pos_score(list[i].first + 3, list[i].second, 1); //X○○X★○X    

								}
								else if (stone[list[i].first + 6][list[i].second].color != color) {
									pos_score(list[i].first + 2, list[i].second, 1); //X○○★X○X
									pos_score(list[i].first + 3, list[i].second, 1); //X○○X★○X    
									pos_score(list[i].first + 5, list[i].second, 1); //X○○XX○★    
								}
								else
									break;
							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○XX○X 경우 
								if (is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first + 5][list[i].second].color == 0) {
									if (is_inboard(list[i].first + 6, list[i].second) && stone[list[i].first + 6][list[i].second].color == color)
										break;
									pos_score(list[i].first + 2, list[i].second, 1); //●○○★X○X
									pos_score(list[i].first + 3, list[i].second, 1); //●○○X★○X
									pos_score(list[i].first + 5, list[i].second, 1); //●○○XX○★  


								}
							}
							else if (stone[list[i].first + 4][list[i].second].blocked[direc] == true) { //X○○XX○● 경우 
								if (is_inboard(list[i].first - 1, list[i].second) && stone[list[i].first - 1][list[i].second].color == 0) {
									if (is_inboard(list[i].first - 2, list[i].second) && stone[list[i].first - 2][list[i].second].color == color)
										break;
									pos_score(list[i].first - 1, list[i].second, 1); //★○○XX○●  
									pos_score(list[i].first + 2, list[i].second, 1); //X○○★X○●
									pos_score(list[i].first + 3, list[i].second, 1); //X○○X★○●
								}

							}
							//----

						}//2칸뒤 한개 end if

						else if (is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first + 5][list[i].second].dir[direc] == 1
							&& stone[list[i].first + 5][list[i].second].color == color) {//○○XXX○ 세칸뒤에 한개 

							if (!stone[list[i].first + 3][list[i].second].color == 0 || !stone[list[i].first + 4][list[i].second].color == 0)
								break;


							pos_score(list[i].first + 2, list[i].second, 1); //○○★XX○
							pos_score(list[i].first + 3, list[i].second, 1); //○○X★X○
							pos_score(list[i].first + 4, list[i].second, 1); //○○XX★○


						}//세칸 뒤 한개 end if 
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {//○X○X○ or ○XX○X○

						if (is_inboard(list[i].first + 4, list[i].second) && stone[list[i].first + 2][list[i].second].dir[direc] == 1 && stone[list[i].first + 4][list[i].second].dir[direc] == 1
							&& stone[list[i].first + 2][list[i].second].color == color && stone[list[i].first + 4][list[i].second].color == color) {  //○X○X○ 경우 
							if (stone[list[i].first + 1][list[i].second].color == 0 && stone[list[i].first + 3][list[i].second].color == 0) {
								if (is_inboard(list[i].first - 1, list[i].second) && is_inboard(list[i].first + 5, list[i].second)
									&& stone[list[i].first - 1][list[i].second].color == 0 && stone[list[i].first + 5][list[i].second].color == 0) { //양쪽 다 뚫린 경우 
									pos_score(list[i].first - 1, list[i].second, 1);
									pos_score(list[i].first + 1, list[i].second, 1.5);
									//	pos_score(list[i].first + 3, list[i].second, 1);
									//	pos_score(list[i].first + 5, list[i].second, 1);
								}
								else if (is_inboard(list[i].first - 1, list[i].second) && stone[list[i].first - 1][list[i].second].color == 0) { //왼쪽 만 뚫린 경우 
									pos_score(list[i].first - 1, list[i].second, 1);
									pos_score(list[i].first + 1, list[i].second, 1.5);
									//	pos_score(list[i].first + 3, list[i].second, 1);
								}
								else if (is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first + 5][list[i].second].color == 0) { //오른쪽만 뚫린 경우 
									pos_score(list[i].first + 1, list[i].second, 1);
									//	pos_score(list[i].first + 3, list[i].second, 1);
									//	pos_score(list[i].first + 5, list[i].second, 1);
								}
								else {
									break;
								}
							}
						}//○X○X○ end if 
						else if (is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first + 3][list[i].second].dir[direc] == 1 && stone[list[i].first + 5][list[i].second].dir[direc] == 1
							&& stone[list[i].first + 3][list[i].second].color == color && stone[list[i].first + 5][list[i].second].color == color) {  //○XX○X○ 경우 
							if (stone[list[i].first + 1][list[i].second].color == 0 && stone[list[i].first + 2][list[i].second].color == 0 && stone[list[i].first + 4][list[i].second].color == 0) {
								pos_score(list[i].first + 1, list[i].second, 1.5);
								pos_score(list[i].first + 2, list[i].second, 1.5);
								pos_score(list[i].first + 4, list[i].second, 1.5);
							}
						}//○XX○X○ end if 

					}
					break;

				case 7:

					if (stone[list[i].first][list[i].second].dir[direc] == 3 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (stone[list[i].first][list[i].second].blocked[((direc + 4) % 8) % 8] == true) {  //●○○○  3개짜리에서 막혀있는경우 
							if (is_inboard(list[i].first + 5, list[i].second - 5)) { //●○○○x x x 3개까지 놓는공간이 보드 안인지 
								if (stone[list[i].first + 3][list[i].second - 3].color == 0 && stone[list[i].first + 4][list[i].second - 4].color == 0 && stone[list[i].first + 5][list[i].second - 5].color == 0) {
									if (is_inboard(list[i].first + 6, list[i].second - 6) && stone[list[i].first + 6][list[i].second - 6].color == color)
										break;
									pos_score(list[i].first + 3, list[i].second - 3, 1); //●○○○★ x x
									pos_score(list[i].first + 4, list[i].second - 3, 1); //●○○○x ★ x
									if (is_inboard(list[i].first + 6, list[i].second - 6) && (board[list[i].first + 6][list[i].second - 6] == 1 || board[list[i].first + 6][list[i].second - 6] == 3)) {
										//●○○○x x x ○  or  ●○○○x x x ◎
										pos_score(list[i].first + 5, list[i].second - 5, 0);
									}
									else {
										pos_score(list[i].first + 5, list[i].second - 5, 1);
									}

								}

							}

						}
						else { //X○○○X  3개짜리가 안막혀있는경우 
							if (is_inboard(list[i].first + 4, list[i].second - 4) && is_inboard(list[i].first - 1, list[i].second + 1)) {
								if (stone[list[i].first + 3][list[i].second - 3].color == 0 && stone[list[i].first + 4][list[i].second - 4].color == 0 && stone[list[i].first - 1][list[i].second + 1].color == 0) {

									// 아직 7의 경우  고려 하지 않앗음. 

									if (is_inboard(list[i].first + 5, list[i].second - 5) && stone[list[i].first + 5][list[i].second - 5].color != check) {
										if (stone[list[i].first + 5][list[i].second - 5].color == 0) {   //요기 
											if (is_inboard(list[i].first + 6, list[i].second - 6) && stone[list[i].first + 6][list[i].second - 6].color == color)
												break;
											pos_score(list[i].first + 3, list[i].second - 3, 2); //x○○○★ x x
											pos_score(list[i].first + 4, list[i].second - 4, 1); //x○○○x ★ 
											pos_score(list[i].first + 5, list[i].second - 5, 1);
										}
									}
									else {
										pos_score(list[i].first + 3, list[i].second - 3, 1); //x○○○★ x ●
										pos_score(list[i].first + 4, list[i].second - 4, 1); //x○○○x ★ 
									}

									if (is_inboard(list[i].first - 2, list[i].second + 2) && stone[list[i].first - 2][list[i].second + 2].color == check) {
										pos_score(list[i].first - 1, list[i].second + 1, 1); //●★○○○x x 
									}



								}
							}

						}
					}//3개 연속 end if 

					else if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {

						if (is_inboard(list[i].first + 3, list[i].second - 3) && stone[list[i].first + 3][list[i].second - 3].dir[direc] == 1 && stone[list[i].first + 3][list[i].second - 3].color == color) {//○○X○ 2개의 한칸 뒤에 한개

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first + 3][list[i].second - 3].blocked[direc] == false) { //X○○X○X

								if (is_inboard(list[i].first - 2, list[i].second + 2) && stone[list[i].first - 2][list[i].second + 2].color != 2
									&& is_inboard(list[i].first + 5, list[i].second - 5) && stone[list[i].first + 5][list[i].second - 5].color != 2) {
									pos_score(list[i].first + 2, list[i].second - 2, 2); //X○○★○X
									plus++;
								}
								else
									pos_score(list[i].first + 2, list[i].second - 2, 1); //●X○○★○X

								pos_score(list[i].first - 1, list[i].second + 1, 1); //★○○X○X
								pos_score(list[i].first + 4, list[i].second - 4, 1); //X○○X○★   여기까진 알 수 있음. 

								if (is_inboard(list[i].first - 2, list[i].second + 2))  //★X○○X○
									if (stone[list[i].first - 2][list[i].second + 2].color == 0)
										if (stone[list[i].first - 3][list[i].second + 3].color != color)
											pos_score(list[i].first - 2, list[i].second + 2, 1);

								if (is_inboard(list[i].first + 5, list[i].second - 5)) //○○X○X★
									if (stone[list[i].first + 5][list[i].second - 5].color == 0)
										if (stone[list[i].first + 6][list[i].second - 6].color != color)
											pos_score(list[i].first + 5, list[i].second - 5, 1);


							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○X○XX 경우 
								if (is_inboard(list[i].first + 5, list[i].second - 5)) {
									if (stone[list[i].first + 4][list[i].second - 4].color == 0 && stone[list[i].first + 5][list[i].second - 5].color == 0) {
										if (is_inboard(list[i].first + 6, list[i].second - 6) && stone[list[i].first + 6][list[i].second - 6].color == color)
											break;
										pos_score(list[i].first + 2, list[i].second - 2, 1);
										pos_score(list[i].first + 4, list[i].second - 4, 1);
										pos_score(list[i].first + 5, list[i].second - 5, 1);
									}

								}
							}
							else if (stone[list[i].first + 3][list[i].second - 3].blocked[direc] == true) { //XX○○X○● 경우 
								if (is_inboard(list[i].first - 2, list[i].second + 2) && stone[list[i].first - 1][list[i].second + 1].color == 0 && stone[list[i].first - 2][list[i].second + 2].color == 0) {
									if (is_inboard(list[i].first - 3, list[i].second + 3) && stone[list[i].first - 3][list[i].second + 3].color == color)
										break;
									pos_score(list[i].first - 1, list[i].second + 1, 1);
									pos_score(list[i].first - 2, list[i].second + 2, 1);
									pos_score(list[i].first + 2, list[i].second - 2, 1);
								}
							}

						}//두개의 한칸 뒤의 한개 end if


						else if (is_inboard(list[i].first + 4, list[i].second - 4) && stone[list[i].first + 4][list[i].second - 4].dir[direc] == 1
							&& stone[list[i].first + 4][list[i].second - 4].color == color) {//○○XX○ 두칸뒤에 한개 
																							 //-----
							if (!stone[list[i].first + 3][list[i].second - 3].color == 0)//자기돌이 있을수 잇음. 
								break;

							if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && stone[list[i].first + 4][list[i].second - 4].blocked[direc] == false) { //X○○XX○X
								if (stone[list[i].first - 2][list[i].second + 2].color != color && stone[list[i].first + 6][list[i].second - 6].color != color) {
									pos_score(list[i].first - 1, list[i].second + 1, 1); //★○○XX○X
									pos_score(list[i].first + 2, list[i].second - 2, 1); //X○○★X○X
									pos_score(list[i].first + 3, list[i].second - 3, 1); //X○○X★○X    
									pos_score(list[i].first + 5, list[i].second - 5, 1); //X○○XX○★   
								}
								else if (stone[list[i].first - 2][list[i].second + 2].color != color) {
									pos_score(list[i].first - 1, list[i].second + 1, 1); //★○○XX○X
									pos_score(list[i].first + 2, list[i].second - 2, 1); //X○○★X○X
									pos_score(list[i].first + 3, list[i].second - 3, 1); //X○○X★○X   

								}
								else if (stone[list[i].first + 6][list[i].second - 6].color != color) {
									pos_score(list[i].first + 2, list[i].second - 2, 1); //X○○★X○X
									pos_score(list[i].first + 3, list[i].second - 3, 1); //X○○X★○X    
									pos_score(list[i].first + 5, list[i].second - 5, 1); //X○○XX○★   
								}
								else
									break;

							}
							else if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == true) { //●○○XX○X 경우 
								if (is_inboard(list[i].first + 5, list[i].second - 5) && stone[list[i].first + 5][list[i].second - 5].color == 0) {
									if (is_inboard(list[i].first + 6, list[i].second - 6) && stone[list[i].first + 6][list[i].second - 6].color == color)
										break;
									pos_score(list[i].first + 2, list[i].second - 2, 1); //●○○★X○X
									pos_score(list[i].first + 3, list[i].second - 3, 1); //●○○X★○X
									pos_score(list[i].first + 5, list[i].second - 5, 1); //●○○XX○★  


								}
							}
							else if (stone[list[i].first + 4][list[i].second - 4].blocked[direc] == true) { //X○○XX○● 경우 
								if (is_inboard(list[i].first - 1, list[i].second + 1) && stone[list[i].first - 1][list[i].second + 1].color == 0) {
									if (is_inboard(list[i].first - 2, list[i].second + 2) && stone[list[i].first - 2][list[i].second + 2].color == color)
										break;
									pos_score(list[i].first - 1, list[i].second + 1, 1); //★○○XX○●  
									pos_score(list[i].first + 2, list[i].second - 2, 1); //X○○★X○●
									pos_score(list[i].first + 3, list[i].second - 3, 1); //X○○X★○●
								}

							}
							//----

						}//2칸뒤 한개 end if

						else if (is_inboard(list[i].first + 5, list[i].second - 5) && stone[list[i].first + 5][list[i].second - 5].dir[direc] == 1
							&& stone[list[i].first + 5][list[i].second - 5].color == color) {//○○XXX○ 세칸뒤에 한개 

							if (!stone[list[i].first + 3][list[i].second - 3].color == 0 || !stone[list[i].first + 4][list[i].second - 4].color == 0)
								break;


							pos_score(list[i].first + 2, list[i].second - 2, 1); //○○★XX○
							pos_score(list[i].first + 3, list[i].second - 3, 1); //○○X★X○
							pos_score(list[i].first + 4, list[i].second - 4, 1); //○○XX★○


						}//세칸 뒤 한개 end if 
					}
					else if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {//○X○X○ or ○XX○X○

						if (is_inboard(list[i].first + 4, list[i].second - 4) && stone[list[i].first + 2][list[i].second - 2].dir[direc] == 1 && stone[list[i].first + 4][list[i].second - 4].dir[direc] == 1
							&& stone[list[i].first + 2][list[i].second - 2].color == color && stone[list[i].first + 4][list[i].second - 4].color == color) {  //○X○X○ 경우 
							if (stone[list[i].first + 1][list[i].second - 1].color == 0 && stone[list[i].first + 3][list[i].second - 3].color == 0) {
								if (is_inboard(list[i].first - 1, list[i].second + 1) && is_inboard(list[i].first + 5, list[i].second - 5)
									&& stone[list[i].first - 1][list[i].second + 1].color == 0 && stone[list[i].first + 5][list[i].second - 5].color == 0) { //양쪽 다 뚫린 경우 
									pos_score(list[i].first - 1, list[i].second + 1, 1);
									pos_score(list[i].first + 1, list[i].second - 1, 1.5);
									//	pos_score(list[i].first + 3, list[i].second - 3, 1);
									//	pos_score(list[i].first + 5, list[i].second - 5, 1);
								}
								else if (is_inboard(list[i].first - 1, list[i].second + 1) && stone[list[i].first - 1][list[i].second + 1].color == 0) { //왼쪽 만 뚫린 경우 
									pos_score(list[i].first - 1, list[i].second + 1, 1);
									pos_score(list[i].first + 1, list[i].second - 1, 1.5);
									//	pos_score(list[i].first + 3, list[i].second - 3, 1);
								}
								else if (is_inboard(list[i].first + 5, list[i].second - 5) && stone[list[i].first + 5][list[i].second - 5].color == 0) { //오른쪽만 뚫린 경우 
									pos_score(list[i].first + 1, list[i].second - 1, 1);
									//	pos_score(list[i].first + 3, list[i].second - 3, 1);
									//	pos_score(list[i].first + 5, list[i].second - 5, 1);
								}
								else {
									break;
								}
							}
						}//○X○X○ end if 
						else if (is_inboard(list[i].first + 5, list[i].second - 5) && stone[list[i].first + 3][list[i].second - 3].dir[direc] == 1 && stone[list[i].first + 5][list[i].second - 5].dir[direc] == 1
							&& stone[list[i].first + 3][list[i].second - 3].color == color && stone[list[i].first + 5][list[i].second - 5].color == color) {  //○XX○X○ 경우 
							if (stone[list[i].first + 1][list[i].second - 1].color == 0 && stone[list[i].first + 2][list[i].second - 2].color == 0 && stone[list[i].first + 4][list[i].second - 4].color == 0) {
								pos_score(list[i].first + 1, list[i].second - 1, 1.5);
								pos_score(list[i].first + 2, list[i].second - 2, 1.5);
								pos_score(list[i].first + 4, list[i].second - 4, 1.5);
							}
						}//○XX○X○ end if 

					}
					break;


				}//switch
			}
		}//for direc
	}//for i 

	if (mode) {
		vector< pair<pair<double, bool>, PAIR> > temp_score;
		vector< pair<pair<double, bool>, PAIR> > swap_score;
		bool loop = true;
		int max_index = 0;
		int max_index2 = 0;
		int three_count = 0;

		int x;
		int y;
		if (!score.empty()) {
			three_count += plus;
			for (int i = 0; i < score.size(); i++) {
				if (score[i].first.first >= 2) {
					three_count++;
					if (score_count[i] >= 2)
						three_count++;
				}

			}

			while (loop) {
				for (int i = 0; i < score.size(); i++) {
					x = score[i].second.first;
					y = score[i].second.second;
					if (is_inboard(x, y - 1) && is_inboard(x, y + 1) && stone[x][y - 1].color == 1 && stone[x][y + 1].color == 1) {
						if (stone[x][y - 1].dir[0] + stone[x][y + 1].dir[4] >= 6) {
							score[i].first.first = -44;
						}
					}
					if (is_inboard(x - 1, y - 1) && is_inboard(x + 1, y + 1) && stone[x - 1][y - 1].color == 1 && stone[x + 1][y + 1].color == 1) {
						if (stone[x - 1][y - 1].dir[1] + stone[x + 1][y + 1].dir[5] >= 6) {
							score[i].first.first = -44;
						}
					}
					if (is_inboard(x - 1, y) && is_inboard(x + 1, y) && stone[x - 1][y].color == 1 && stone[x + 1][y].color == 1) {
						if (stone[x - 1][y].dir[2] + stone[x + 1][y].dir[6] >= 6) {
							score[i].first.first = -44;
						}
					}
					if (is_inboard(x - 1, y + 1) && is_inboard(x + 1, y - 1) && stone[x - 1][y + 1].color == 1 && stone[x + 1][y - 1].color == 1) {
						if (stone[x - 1][y + 1].dir[3] + stone[x + 1][y - 1].dir[7] >= 6) {
							score[i].first.first = -44;
						}
					}


				}


				max_index = return_max_index();

				if (optimal_x != score[max_index].second.first || optimal_y != score[max_index].second.second) {
					loop = false;
				}
				else
					score.erase(score.begin() + max_index);

				//optimal_x = score[max_index].second.first;
				//optimal_y = score[max_index].second.second;
			}
			//if (three_count >= 2) {
			if (score[max_index].first.first >= 2) {
				for (int i = 0; i < score.size(); i++) {
					if (score[i].first.first < 2) {
						//score.erase(score.begin() + i);
						score[i].first.first = -44;
					}
				}
			}
			//}
			mode = false;

			temp_score.clear();
			temp_score.assign(score.begin(), score.end());
			/*for (int i = 0; i < score.size(); i++) {
			temp_score.push_back(make_pair(score[i].first, make_pair(score[i].second.first, score[i].second.second)));
			}*/

			
			find_two(&op_list);
			max_index = return_max_index();

			swap_score.clear(); //op 정보 
			swap_score.assign(score.begin(), score.end());


			score.clear();
			score.assign(temp_score.begin(), temp_score.end());
			/*for (int i = 0; i < temp_score.size(); i++) {
			score.push_back(make_pair(temp_score[i].first, make_pair(temp_score[i].second.first, temp_score[i].second.second)));
			}*/
			
			two_check = false;
			find_two(&my_list);
			max_index2 = return_max_index();

			if (!two_check) {
				find_one(&my_list);
				max_index2 = return_max_index();
			}

			if (color == 1) {
				if (swap_score[max_index].first.first >= 3 && swap_score[max_index].first.second) {
					max_index = max_index;
				}
				else if (score[max_index2].first.first >= 3 && score[max_index2].first.second) {
					max_index = max_index2;
				}
				else {
					if (swap_score[max_index].first.first < score[max_index2].first.first) {
						max_index = max_index2;
					}//s
				}
			}
			else {
				if (swap_score[max_index].first.first >= 3 && swap_score[max_index].first.second) {
					max_index = max_index;
				}
				else if (score[max_index2].first.first >= 3 && score[max_index2].first.second) {
					max_index = max_index2;
				}
				else {
					if (swap_score[max_index].first.first < score[max_index2].first.first) {
						max_index = max_index2;
					}//s
				}
			}

			/*if (score[max_index].first < score[max_index2].first) {
			max_index = max_index2;
			}*/


			if (optimal_x != score[max_index].second.first || optimal_y != score[max_index].second.second)
				loop = false;
			else
				score.erase(score.begin() + max_index);
			optimal_x = score[max_index].second.first;
			optimal_y = score[max_index].second.second;

			mode = true;


			//case 0
			//return true;

			/*//case1
			if (three_count >= 2 || color == 1) {
			return true;
			}
			else
			{
			return false;
			}*/

			////case2
			//if (three_count >= 2 ) {
			//	return true;
			//}
			//else
			//{
			//	return false;
			//}

			//case3
			if (score[max_index].first.first >= 2.5 && (score[max_index].first.second || score_count[max_index] >= 3)) {
				return true;
			}


			if (three_count >= 2 || color == 1)
				return true;
			else
				return false;


		}
		else {
			return false;
		}
	}
	else {
		return false;
	}

}

int return_max_index() {
	int max_index;
	double max = 0;

	for (int i = 0; i < score.size(); i++) {
		if (max < score[i].first.first) {
			max = score[i].first.first;
			max_index = i;
		}
	}

	return max_index;

}


bool find_two(vector<PAIR> * list2) {
	
	if (mode)
		score.clear();

	int color = 0;
	int check = 0;
	vector<PAIR> list = *list2;

	if (!list2->empty()) {
		if (&list2[0] == &my_list) {
			color = 1;
		}
		else
			color = 2;

	}
	if (!block_list.empty()) {
		if (color != stone[block_list.front().first][block_list.front().second].color) {
			rotation_block();
		}
	}
	//for (int i = 0; i < list.size(); i++) {
	for (int i = list.size() - 1; i >= 0; i--) {
		for (int direc = 0; direc < 8; direc++) {
			twice = 0;
			if (stone[list[i].first][list[i].second].dir[(direc + 4) % 8] == 1) {
				switch (direc) {
				case 0:

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second - 4) && stone[list[i].first][list[i].second - 2].color == 0
							&& stone[list[i].first][list[i].second - 3].color == 0 && stone[list[i].first][list[i].second - 4].color == 0) {

							if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second - 5].color == 0) { //○○xxxx 
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && is_inboard(list[i].first, list[i].second + 3)
									&& stone[list[i].first][list[i].second + 2].color == 0 && stone[list[i].first][list[i].second + 3].color == 0) { //xxx○○★xxx 좌 3 free
									pos_score(list[i].first, list[i].second - 2, 2);
									pos_score(list[i].first, list[i].second - 3, 1.5);
								}
								else {
									pos_score(list[i].first, list[i].second - 2, 1);
									pos_score(list[i].first, list[i].second - 3, 1);
								}

								pos_score(list[i].first, list[i].second - 4, 1);
								pos_score(list[i].first, list[i].second - 5, 1);


							}//inboard 5 
							if (stone[list[i].first][list[i].second - 5].color != 0) { //x○○xxx● 
								if (stone[list[i].first][list[i].second - 5].color != 0 && stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {

									pos_score(list[i].first, list[i].second - 2, 1);
									pos_score(list[i].first, list[i].second - 3, 1);
									pos_score(list[i].first, list[i].second - 4, 1);
									if (stone[list[i].first][list[i].second + 2].color != 0) {
										pos_score(list[i].first, list[i].second + 1, 1);
									}



								}

							}//end //○○xxx● 

						}//ooxxx if  x free 
						else if (is_inboard(list[i].first, list[i].second - 3) && stone[list[i].first][list[i].second - 2].color == 0
							&& stone[list[i].first][list[i].second - 3].color == 0) {
							if (is_inboard(list[i].first, list[i].second + 2) && stone[list[i].first][list[i].second + 1].color == 0
								&& stone[list[i].first][list[i].second + 2].color == 0) {
								pos_score(list[i].first, list[i].second - 2, 1);
								pos_score(list[i].first, list[i].second - 3, 1);

							}

						}//ooxx  if x free

					}//2
					else if (is_inboard(list[i].first, list[i].second - 2) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first][list[i].second - 2].dir[direc] == 1 && stone[list[i].first][list[i].second - 2].blocked[direc] == false
						&& stone[list[i].first][list[i].second - 2].color == color) { //OXOxxx
						if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second - 4].color == 0 && stone[list[i].first][list[i].second - 5].color == 0) {
							if (is_inboard(list[i].first, list[i].second + 2) && stone[list[i].first][list[i].second + 1].color == 0 && stone[list[i].first][list[i].second + 2].color == 0) {
								pos_score(list[i].first, list[i].second - 1, 1);

							}
							else {
								if (is_inboard(list[i].first, list[i].second + 1) && stone[list[i].first][list[i].second + 1].color == 0)
									pos_score(list[i].first, list[i].second + 1, 1);
								else {
									if (is_inboard(list[i].first, list[i].second - 6) && stone[list[i].first][list[i].second - 6].color == 1)
										break; // 2에서 7나오는 길 점수 부여안함. 
								}
								pos_score(list[i].first, list[i].second - 1, 2);
							}
							pos_score(list[i].first, list[i].second - 3, 1);
							pos_score(list[i].first, list[i].second - 4, 1);
							pos_score(list[i].first, list[i].second - 5, 1);

						}
						else if (is_inboard(list[i].first, list[i].second - 4) && stone[list[i].first][list[i].second - 4].color == 0
							&& is_inboard(list[i].first, list[i].second + 1) && stone[list[i].first][list[i].second + 1].color == 0) { //xoxoxx
							if (is_inboard(list[i].first, list[i].second + 2) && stone[list[i].first][list[i].second + 2].color == 0) {
								pos_score(list[i].first, list[i].second - 1, 1);

							}
							else {
								pos_score(list[i].first, list[i].second + 1, 1);
								pos_score(list[i].first, list[i].second - 1, 2);
							}
							pos_score(list[i].first, list[i].second - 3, 1);
							pos_score(list[i].first, list[i].second - 4, 1);

						}

					}//end OXO 
					else if (is_inboard(list[i].first, list[i].second - 3) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first][list[i].second - 2].color == 0 && stone[list[i].first][list[i].second - 3].dir[direc] == 1 && stone[list[i].first][list[i].second - 3].blocked[direc] == false
						&& stone[list[i].first][list[i].second - 3].color == color) { //OXXO
						if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second - 5].color == 0) {//OXXOXX
							if (is_inboard(list[i].first, list[i].second + 1) && stone[list[i].first][list[i].second + 1].color == 0) {
								pos_score(list[i].first, list[i].second - 1, 0.75);
								pos_score(list[i].first, list[i].second - 2, 0.75);

							}
							else {

								pos_score(list[i].first, list[i].second - 1, 1.5);
								pos_score(list[i].first, list[i].second - 2, 1.5);
							}
							pos_score(list[i].first, list[i].second - 4, 1);
							pos_score(list[i].first, list[i].second - 5, 1);

						}
						else {
							if (is_inboard(list[i].first, list[i].second + 1) && stone[list[i].first][list[i].second].color == 0) {
								pos_score(list[i].first, list[i].second - 1, 0.75);
								pos_score(list[i].first, list[i].second - 2, 0.75);
								pos_score(list[i].first, list[i].second - 4, 1);
							}

						}

					}// end OXXO
					else if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first][list[i].second - 2].color == 0 && stone[list[i].first][list[i].second - 3].color == 0 &&
						stone[list[i].first][list[i].second - 4].dir[direc] == 1 && stone[list[i].first][list[i].second - 4].blocked[direc] == false
						&& stone[list[i].first][list[i].second - 4].color == color) { //OXXXO
						if (is_inboard(list[i].first, list[i].second + 1) && stone[list[i].first][list[i].second + 1].color == 0) {
							pos_score(list[i].first, list[i].second - 1, 0.75);
							pos_score(list[i].first, list[i].second - 2, 0.75);
							pos_score(list[i].first, list[i].second - 3, 0.75);
						}
						else {
							pos_score(list[i].first, list[i].second - 1, 1);
							pos_score(list[i].first, list[i].second - 2, 1);
							pos_score(list[i].first, list[i].second - 3, 1);
						}
						pos_score(list[i].first, list[i].second - 5, 1);


					}// end OXXXO
					else if (is_inboard(list[i].first, list[i].second - 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first][list[i].second - 2].color == 0 && stone[list[i].first][list[i].second - 3].color == 0 &&
						stone[list[i].first][list[i].second - 5].dir[direc] == 1 && stone[list[i].first][list[i].second - 5].blocked[(direc + 4) % 8] == false
						&& stone[list[i].first][list[i].second - 5].color == color) { //OXXXXO
						pos_score(list[i].first, list[i].second - 1, 0.75);
						pos_score(list[i].first, list[i].second - 2, 0.75);
						pos_score(list[i].first, list[i].second - 3, 0.75);
						pos_score(list[i].first, list[i].second - 4, 0.75);
					}


					break;

				case 1:

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 4, list[i].second - 4) && stone[list[i].first - 2][list[i].second - 2].color == 0
							&& stone[list[i].first - 3][list[i].second - 3].color == 0 && stone[list[i].first - 4][list[i].second - 4].color == 0) {

							if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first - 5][list[i].second - 5].color == 0) { //○○xxxx 
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && is_inboard(list[i].first + 3, list[i].second + 3)
									&& stone[list[i].first + 2][list[i].second + 2].color == 0 && stone[list[i].first + 3][list[i].second + 3].color == 0) { //xxx○○★xxx 좌 3 free
									pos_score(list[i].first - 2, list[i].second - 2, 2);
									pos_score(list[i].first - 3, list[i].second - 3, 1.5);
								}
								else {
									pos_score(list[i].first - 2, list[i].second - 2, 1);
									pos_score(list[i].first - 3, list[i].second - 3, 1);
								}

								pos_score(list[i].first - 4, list[i].second - 4, 1);
								pos_score(list[i].first - 5, list[i].second - 5, 1);


							}//inboard 5 
							if (stone[list[i].first - 5][list[i].second - 5].color != 0) { //x○○xxx● 
								if (stone[list[i].first - 5][list[i].second - 5].color != 0 && stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {

									pos_score(list[i].first - 2, list[i].second - 2, 1);
									pos_score(list[i].first - 3, list[i].second - 3, 1);
									pos_score(list[i].first - 4, list[i].second - 4, 1);
									if (stone[list[i].first + 2][list[i].second + 2].color != 0) {
										pos_score(list[i].first + 1, list[i].second + 1, 1);
									}



								}

							}//end //○○xxx● 

						}//ooxxx if  x free 
						else if (is_inboard(list[i].first - 3, list[i].second - 3) && stone[list[i].first - 2][list[i].second - 2].color == 0
							&& stone[list[i].first - 3][list[i].second - 3].color == 0) {
							if (is_inboard(list[i].first + 2, list[i].second + 2) && stone[list[i].first + 1][list[i].second + 1].color == 0
								&& stone[list[i].first + 2][list[i].second + 2].color == 0) {
								pos_score(list[i].first - 2, list[i].second - 2, 1);
								pos_score(list[i].first - 3, list[i].second - 3, 1);

							}

						}//ooxx  if x free

					}//2
					else if (is_inboard(list[i].first - 2, list[i].second - 2) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second - 2].dir[direc] == 1 && stone[list[i].first - 2][list[i].second - 2].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second - 2].color == color) { //OXOxxx
						if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first - 4][list[i].second - 4].color == 0 && stone[list[i].first - 5][list[i].second - 5].color == 0) {
							if (is_inboard(list[i].first + 2, list[i].second + 2) && stone[list[i].first + 1][list[i].second + 1].color == 0 && stone[list[i].first + 2][list[i].second + 2].color == 0) {
								pos_score(list[i].first - 1, list[i].second - 1, 1);

							}
							else {
								if (is_inboard(list[i].first + 1, list[i].second + 1) && stone[list[i].first + 1][list[i].second + 1].color == 0)
									pos_score(list[i].first + 1, list[i].second + 1, 1);
								else
								{
									if (is_inboard(list[i].first - 6, list[i].second - 6) && stone[list[i].first - 6][list[i].second - 6].color == color)
										break; // 2에서 7나오는 길 점수 부여안함. 
								}
								pos_score(list[i].first - 1, list[i].second - 1, 2);
							}
							pos_score(list[i].first - 3, list[i].second - 3, 1);
							pos_score(list[i].first - 4, list[i].second - 4, 1);
							pos_score(list[i].first - 5, list[i].second - 5, 1);

						}
						else if (is_inboard(list[i].first - 4, list[i].second - 4) && stone[list[i].first - 4][list[i].second - 4].color == 0
							&& is_inboard(list[i].first + 1, list[i].second + 1) && stone[list[i].first + 1][list[i].second + 1].color == 0) { //xoxoxx
							if (is_inboard(list[i].first + 2, list[i].second + 2) && stone[list[i].first + 2][list[i].second + 2].color == 0) {
								pos_score(list[i].first - 1, list[i].second - 1, 1);

							}
							else {
								pos_score(list[i].first + 1, list[i].second + 1, 1);
								pos_score(list[i].first - 1, list[i].second - 1, 2);
							}
							pos_score(list[i].first - 3, list[i].second - 3, 1);
							pos_score(list[i].first - 4, list[i].second - 4, 1);

						}

					}//end OXO 
					else if (is_inboard(list[i].first - 3, list[i].second - 3) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second - 2].color == 0 && stone[list[i].first - 3][list[i].second - 3].dir[direc] == 1 && stone[list[i].first - 3][list[i].second - 3].blocked[direc] == false
						&& stone[list[i].first - 3][list[i].second - 3].color == color) { //OXXO
						if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first - 5][list[i].second - 5].color == 0) {//OXXOXX
							if (is_inboard(list[i].first + 1, list[i].second + 1) && stone[list[i].first + 1][list[i].second + 1].color == 0) {
								pos_score(list[i].first - 1, list[i].second - 1, 0.75);
								pos_score(list[i].first - 2, list[i].second - 2, 0.75);

							}
							else {

								pos_score(list[i].first - 1, list[i].second - 1, 1.5);
								pos_score(list[i].first - 2, list[i].second - 2, 1.5);
							}
							pos_score(list[i].first - 4, list[i].second - 4, 1);
							pos_score(list[i].first - 5, list[i].second - 5, 1);

						}
						else {
							if (is_inboard(list[i].first + 1, list[i].second + 1) && stone[list[i].first][list[i].second].color == 0) {
								pos_score(list[i].first - 1, list[i].second - 1, 0.75);
								pos_score(list[i].first - 2, list[i].second - 2, 0.75);
								pos_score(list[i].first - 4, list[i].second - 4, 1);
							}

						}

					}// end OXXO
					else if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second - 2].color == 0 && stone[list[i].first - 3][list[i].second - 3].color == 0 &&
						stone[list[i].first - 4][list[i].second - 4].dir[direc] == 1 && stone[list[i].first - 4][list[i].second - 4].blocked[direc] == false
						&& stone[list[i].first - 4][list[i].second - 4].color == color) { //OXXXO
						if (is_inboard(list[i].first + 1, list[i].second + 1) && stone[list[i].first + 1][list[i].second + 1].color == 0) {
							pos_score(list[i].first - 1, list[i].second - 1, 0.75);
							pos_score(list[i].first - 2, list[i].second - 2, 0.75);
							pos_score(list[i].first - 3, list[i].second - 3, 0.75);
						}
						else {
							pos_score(list[i].first - 1, list[i].second - 1, 1);
							pos_score(list[i].first - 2, list[i].second - 2, 1);
							pos_score(list[i].first - 3, list[i].second - 3, 1);
						}
						pos_score(list[i].first - 5, list[i].second - 5, 1);


					}// end OXXXO
					else if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second - 2].color == 0 && stone[list[i].first - 3][list[i].second - 3].color == 0 &&
						stone[list[i].first - 5][list[i].second - 5].dir[direc] == 1 && stone[list[i].first - 5][list[i].second - 5].blocked[(direc + 4) % 8] == false
						&& stone[list[i].first - 5][list[i].second - 5].color == color) { //OXXXXO
						pos_score(list[i].first - 1, list[i].second - 1, 0.75);
						pos_score(list[i].first - 2, list[i].second - 2, 0.75);
						pos_score(list[i].first - 3, list[i].second - 3, 0.75);
						pos_score(list[i].first - 4, list[i].second - 4, 0.75);
					}


					break;

				case 2:

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 4, list[i].second) && stone[list[i].first - 2][list[i].second].color == 0
							&& stone[list[i].first - 3][list[i].second].color == 0 && stone[list[i].first - 4][list[i].second].color == 0) {

							if (is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first - 5][list[i].second].color == 0) { //○○xxxx 
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && is_inboard(list[i].first + 3, list[i].second)
									&& stone[list[i].first + 2][list[i].second].color == 0 && stone[list[i].first + 3][list[i].second].color == 0) { //xxx○○★xxx 좌 3 free
									pos_score(list[i].first - 2, list[i].second, 2);
									pos_score(list[i].first - 3, list[i].second, 1.5);
								}
								else {
									pos_score(list[i].first - 2, list[i].second, 1);
									pos_score(list[i].first - 3, list[i].second, 1);
								}

								pos_score(list[i].first - 4, list[i].second, 1);
								pos_score(list[i].first - 5, list[i].second, 1);


							}//inboard 5 
							if (stone[list[i].first - 5][list[i].second - 5].color != 0) { //x○○xxx● 
								if (stone[list[i].first - 5][list[i].second - 5].color != 0 && stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {

									pos_score(list[i].first - 2, list[i].second, 1);
									pos_score(list[i].first - 3, list[i].second, 1);
									pos_score(list[i].first - 4, list[i].second, 1);
									if (stone[list[i].first + 2][list[i].second].color != 0) {
										pos_score(list[i].first + 1, list[i].second, 1);
									}



								}

							}//end //○○xxx● 

						}//ooxxx if  x free 
						else if (is_inboard(list[i].first - 3, list[i].second) && stone[list[i].first - 2][list[i].second].color == 0
							&& stone[list[i].first - 3][list[i].second].color == 0) {
							if (is_inboard(list[i].first + 2, list[i].second) && stone[list[i].first + 1][list[i].second].color == 0
								&& stone[list[i].first + 2][list[i].second].color == 0) {
								pos_score(list[i].first - 2, list[i].second, 1);
								pos_score(list[i].first - 3, list[i].second, 1);

							}

						}//ooxx  if x free

					}//2
					else if (is_inboard(list[i].first - 2, list[i].second) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second].dir[direc] == 1 && stone[list[i].first - 2][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second].color == color) { //OXOxxx
						if (is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first - 4][list[i].second].color == 0 && stone[list[i].first - 5][list[i].second].color == 0) {
							if (is_inboard(list[i].first + 2, list[i].second) && stone[list[i].first + 1][list[i].second].color == 0 && stone[list[i].first + 2][list[i].second].color == 0) {
								pos_score(list[i].first - 1, list[i].second, 1);

							}
							else {
								if (is_inboard(list[i].first + 1, list[i].second) && stone[list[i].first + 1][list[i].second].color == 0)
									pos_score(list[i].first + 1, list[i].second, 1);
								else
								{
									if (is_inboard(list[i].first - 6, list[i].second) && stone[list[i].first - 6][list[i].second].color == color)
										break; // 2에서 7나오는 길 점수 부여안함. 
								}
								pos_score(list[i].first - 1, list[i].second, 2);
							}
							pos_score(list[i].first - 3, list[i].second, 1);
							pos_score(list[i].first - 4, list[i].second, 1);
							pos_score(list[i].first - 5, list[i].second, 1);

						}
						else if (is_inboard(list[i].first - 4, list[i].second) && stone[list[i].first - 4][list[i].second].color == 0
							&& is_inboard(list[i].first + 1, list[i].second) && stone[list[i].first + 1][list[i].second].color == 0) { //xoxoxx
							if (is_inboard(list[i].first + 2, list[i].second) && stone[list[i].first + 2][list[i].second].color == 0) {
								pos_score(list[i].first - 1, list[i].second, 1);

							}
							else {
								pos_score(list[i].first + 1, list[i].second, 1);
								pos_score(list[i].first - 1, list[i].second, 2);
							}
							pos_score(list[i].first - 3, list[i].second, 1);
							pos_score(list[i].first - 4, list[i].second, 1);

						}

					}//end OXO 
					else if (is_inboard(list[i].first - 3, list[i].second) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second].color == 0 && stone[list[i].first - 3][list[i].second].dir[direc] == 1 && stone[list[i].first - 3][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 3][list[i].second].color == color) { //OXXO
						if (is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first - 5][list[i].second].color == 0) {//OXXOXX
							if (is_inboard(list[i].first + 1, list[i].second) && stone[list[i].first + 1][list[i].second].color == 0) {
								pos_score(list[i].first - 1, list[i].second, 0.75);
								pos_score(list[i].first - 2, list[i].second, 0.75);

							}
							else {

								pos_score(list[i].first - 1, list[i].second, 1.5);
								pos_score(list[i].first - 2, list[i].second, 1.5);
							}
							pos_score(list[i].first - 4, list[i].second, 1);
							pos_score(list[i].first - 5, list[i].second, 1);

						}
						else {
							if (is_inboard(list[i].first + 1, list[i].second) && stone[list[i].first][list[i].second].color == 0) {
								pos_score(list[i].first - 1, list[i].second, 0.75);
								pos_score(list[i].first - 2, list[i].second, 0.75);
								pos_score(list[i].first - 4, list[i].second, 1);
							}

						}

					}// end OXXO
					else if (is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second].color == 0 && stone[list[i].first - 3][list[i].second].color == 0 &&
						stone[list[i].first - 4][list[i].second].dir[direc] == 1 && stone[list[i].first - 4][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 4][list[i].second].color == color) { //OXXXO
						if (is_inboard(list[i].first + 1, list[i].second) && stone[list[i].first + 1][list[i].second].color == 0) {
							pos_score(list[i].first - 1, list[i].second, 0.75);
							pos_score(list[i].first - 2, list[i].second, 0.75);
							pos_score(list[i].first - 3, list[i].second, 0.75);
						}
						else {
							pos_score(list[i].first - 1, list[i].second, 1);
							pos_score(list[i].first - 2, list[i].second, 1);
							pos_score(list[i].first - 3, list[i].second, 1);
						}
						pos_score(list[i].first - 5, list[i].second, 1);


					}// end OXXXO
					else if (is_inboard(list[i].first - 5, list[i].second) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second].color == 0 && stone[list[i].first - 3][list[i].second].color == 0 &&
						stone[list[i].first - 5][list[i].second].dir[direc] == 1 && stone[list[i].first - 5][list[i].second].blocked[(direc + 4) % 8] == false
						&& stone[list[i].first - 5][list[i].second].color == color) { //OXXXXO
						pos_score(list[i].first - 1, list[i].second, 0.75);
						pos_score(list[i].first - 2, list[i].second, 0.75);
						pos_score(list[i].first - 3, list[i].second, 0.75);
						pos_score(list[i].first - 4, list[i].second, 0.75);
					}


					break;

				case 3:

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 4, list[i].second + 4) && stone[list[i].first - 2][list[i].second + 2].color == 0
							&& stone[list[i].first - 3][list[i].second + 3].color == 0 && stone[list[i].first - 4][list[i].second + 4].color == 0) {

							if (is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first - 5][list[i].second + 5].color == 0) { //○○xxxx 
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && is_inboard(list[i].first + 3, list[i].second - 3)
									&& stone[list[i].first + 2][list[i].second - 2].color == 0 && stone[list[i].first + 3][list[i].second - 3].color == 0) { //xxx○○★xxx 좌 3 free
									pos_score(list[i].first - 2, list[i].second + 2, 2);
									pos_score(list[i].first - 3, list[i].second + 3, 1.5);
								}
								else {
									pos_score(list[i].first - 2, list[i].second + 2, 1);
									pos_score(list[i].first - 3, list[i].second + 3, 1);
								}

								pos_score(list[i].first - 4, list[i].second + 4, 1);
								pos_score(list[i].first - 5, list[i].second + 5, 1);


							}//inboard 5 
							if (stone[list[i].first - 5][list[i].second + 5].color != 0) { //x○○xxx● 
								if (stone[list[i].first - 5][list[i].second + 5].color != 0 && stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {

									pos_score(list[i].first - 2, list[i].second + 2, 1);
									pos_score(list[i].first - 3, list[i].second + 3, 1);
									pos_score(list[i].first - 4, list[i].second + 4, 1);
									if (stone[list[i].first + 2][list[i].second - 2].color != 0) {
										pos_score(list[i].first + 1, list[i].second - 1, 1);
									}



								}

							}//end //○○xxx● 

						}//ooxxx if  x free 
						else if (is_inboard(list[i].first - 3, list[i].second + 3) && stone[list[i].first - 2][list[i].second + 2].color == 0
							&& stone[list[i].first - 3][list[i].second + 3].color == 0) {
							if (is_inboard(list[i].first + 2, list[i].second - 2) && stone[list[i].first + 1][list[i].second - 1].color == 0
								&& stone[list[i].first + 2][list[i].second - 2].color == 0) {
								pos_score(list[i].first - 2, list[i].second + 2, 1);
								pos_score(list[i].first - 3, list[i].second + 3, 1);

							}

						}//ooxx  if x free

					}//2
					else if (is_inboard(list[i].first - 2, list[i].second + 2) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second + 2].dir[direc] == 1 && stone[list[i].first - 2][list[i].second + 2].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second + 2].color == color) { //OXOxxx
						if (is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first - 4][list[i].second + 4].color == 0 && stone[list[i].first - 5][list[i].second + 5].color == 0) {
							if (is_inboard(list[i].first + 2, list[i].second - 2) && stone[list[i].first + 1][list[i].second - 1].color == 0 && stone[list[i].first + 2][list[i].second - 2].color == 0) {
								pos_score(list[i].first - 1, list[i].second + 1, 1);

							}
							else {
								if (is_inboard(list[i].first + 1, list[i].second - 1) && stone[list[i].first + 1][list[i].second - 1].color == 0)
									pos_score(list[i].first + 1, list[i].second - 1, 1);
								else
								{
									if (is_inboard(list[i].first - 6, list[i].second + 6) && stone[list[i].first - 6][list[i].second + 6].color == color)
										break; // 2에서 7나오는 길 점수 부여안함. 
								}
								pos_score(list[i].first - 1, list[i].second + 1, 2);
							}
							pos_score(list[i].first - 3, list[i].second + 3, 1);
							pos_score(list[i].first - 4, list[i].second + 4, 1);
							pos_score(list[i].first - 5, list[i].second + 5, 1);

						}
						else if (is_inboard(list[i].first - 4, list[i].second + 4) && stone[list[i].first - 4][list[i].second + 4].color == 0
							&& is_inboard(list[i].first + 1, list[i].second - 1) && stone[list[i].first + 1][list[i].second - 1].color == 0) { //xoxoxx
							if (is_inboard(list[i].first + 2, list[i].second - 2) && stone[list[i].first + 2][list[i].second - 2].color == 0) {
								pos_score(list[i].first - 1, list[i].second + 1, 1);

							}
							else {
								pos_score(list[i].first + 1, list[i].second - 1, 1);
								pos_score(list[i].first - 1, list[i].second + 1, 2);
							}
							pos_score(list[i].first - 3, list[i].second + 3, 1);
							pos_score(list[i].first - 4, list[i].second + 4, 1);

						}

					}//end OXO 
					else if (is_inboard(list[i].first - 3, list[i].second + 3) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second + 2].color == 0 && stone[list[i].first - 3][list[i].second + 3].dir[direc] == 1 && stone[list[i].first - 3][list[i].second + 3].blocked[direc] == false
						&& stone[list[i].first - 3][list[i].second + 3].color == color) { //OXXO
						if (is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first - 5][list[i].second + 5].color == 0) {//OXXOXX
							if (is_inboard(list[i].first + 1, list[i].second - 1) && stone[list[i].first + 1][list[i].second - 1].color == 0) {
								pos_score(list[i].first - 1, list[i].second + 1, 0.75);
								pos_score(list[i].first - 2, list[i].second + 2, 0.75);

							}
							else {

								pos_score(list[i].first - 1, list[i].second + 1, 1.5);
								pos_score(list[i].first - 2, list[i].second + 2, 1.5);
							}
							pos_score(list[i].first - 4, list[i].second + 4, 1);
							pos_score(list[i].first - 5, list[i].second + 5, 1);

						}
						else {
							if (is_inboard(list[i].first + 1, list[i].second - 1) && stone[list[i].first][list[i].second].color == 0) {
								pos_score(list[i].first - 1, list[i].second + 1, 0.75);
								pos_score(list[i].first - 2, list[i].second + 2, 0.75);
								pos_score(list[i].first - 4, list[i].second + 4, 1);
							}

						}

					}// end OXXO
					else if (is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second + 2].color == 0 && stone[list[i].first - 3][list[i].second + 3].color == 0 &&
						stone[list[i].first - 4][list[i].second + 4].dir[direc] == 1 && stone[list[i].first - 4][list[i].second + 4].blocked[direc] == false
						&& stone[list[i].first - 4][list[i].second + 4].color == color) { //OXXXO
						if (is_inboard(list[i].first + 1, list[i].second - 1) && stone[list[i].first + 1][list[i].second - 1].color == 0) {
							pos_score(list[i].first - 1, list[i].second + 1, 0.75);
							pos_score(list[i].first - 2, list[i].second + 2, 0.75);
							pos_score(list[i].first - 3, list[i].second + 3, 0.75);
						}
						else {
							pos_score(list[i].first - 1, list[i].second + 1, 1);
							pos_score(list[i].first - 2, list[i].second + 2, 1);
							pos_score(list[i].first - 3, list[i].second + 3, 1);
						}
						pos_score(list[i].first - 5, list[i].second + 5, 1);


					}// end OXXXO
					else if (is_inboard(list[i].first - 5, list[i].second + 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first - 2][list[i].second + 2].color == 0 && stone[list[i].first - 3][list[i].second + 3].color == 0 &&
						stone[list[i].first - 5][list[i].second + 5].dir[direc] == 1 && stone[list[i].first - 5][list[i].second + 5].blocked[(direc + 4) % 8] == false
						&& stone[list[i].first - 5][list[i].second + 5].color == color) { //OXXXXO
						pos_score(list[i].first - 1, list[i].second + 1, 0.75);
						pos_score(list[i].first - 2, list[i].second + 2, 0.75);
						pos_score(list[i].first - 3, list[i].second + 3, 0.75);
						pos_score(list[i].first - 4, list[i].second + 4, 0.75);
					}


					break;

				case 4:

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second + 4) && stone[list[i].first][list[i].second + 2].color == 0
							&& stone[list[i].first][list[i].second + 3].color == 0 && stone[list[i].first][list[i].second + 4].color == 0) {

							if (is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second + 5].color == 0) { //○○xxxx 
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && is_inboard(list[i].first, list[i].second - 3)
									&& stone[list[i].first][list[i].second - 2].color == 0 && stone[list[i].first][list[i].second - 3].color == 0) { //xxx○○★xxx 좌 3 free
									pos_score(list[i].first, list[i].second + 2, 2);
									pos_score(list[i].first, list[i].second + 3, 1.5);
								}
								else {
									pos_score(list[i].first, list[i].second + 2, 1);
									pos_score(list[i].first, list[i].second + 3, 1);
								}

								pos_score(list[i].first, list[i].second + 4, 1);
								pos_score(list[i].first, list[i].second + 5, 1);


							}//inboard 5 
							if (stone[list[i].first][list[i].second + 5].color != 0) { //x○○xxx● 
								if (stone[list[i].first][list[i].second + 5].color != 0 && stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {

									pos_score(list[i].first, list[i].second + 2, 1);
									pos_score(list[i].first, list[i].second + 3, 1);
									pos_score(list[i].first, list[i].second + 4, 1);
									if (stone[list[i].first][list[i].second - 2].color != 0) {
										pos_score(list[i].first, list[i].second - 1, 1);
									}



								}

							}//end //○○xxx● 

						}//ooxxx if  x free 
						else if (is_inboard(list[i].first, list[i].second + 3) && stone[list[i].first][list[i].second + 2].color == 0
							&& stone[list[i].first][list[i].second + 3].color == 0) {
							if (is_inboard(list[i].first, list[i].second - 2) && stone[list[i].first][list[i].second - 1].color == 0
								&& stone[list[i].first][list[i].second - 2].color == 0) {
								pos_score(list[i].first, list[i].second + 2, 1);
								pos_score(list[i].first, list[i].second + 3, 1);

							}

						}//ooxx  if x free

					}//2
					else if (is_inboard(list[i].first, list[i].second + 2) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first][list[i].second + 2].dir[direc] == 1 && stone[list[i].first][list[i].second + 2].blocked[direc] == false
						&& stone[list[i].first][list[i].second + 2].color == color) { //OXOxxx
						if (is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second + 4].color == 0 && stone[list[i].first][list[i].second + 5].color == 0) {
							if (is_inboard(list[i].first, list[i].second - 2) && stone[list[i].first][list[i].second - 1].color == 0 && stone[list[i].first][list[i].second - 2].color == 0) {
								pos_score(list[i].first, list[i].second + 1, 1);

							}
							else {
								if (is_inboard(list[i].first, list[i].second - 1) && stone[list[i].first][list[i].second - 1].color == 0)
									pos_score(list[i].first, list[i].second - 1, 1);
								else
								{
									if (is_inboard(list[i].first, list[i].second + 6) && stone[list[i].first][list[i].second + 6].color == color)
										break; // 2에서 7나오는 길 점수 부여안함. 
								}
								pos_score(list[i].first, list[i].second + 1, 2);
							}
							pos_score(list[i].first, list[i].second + 3, 1);
							pos_score(list[i].first, list[i].second + 4, 1);
							pos_score(list[i].first, list[i].second + 5, 1);

						}
						else if (is_inboard(list[i].first, list[i].second + 4) && stone[list[i].first][list[i].second + 4].color == 0
							&& is_inboard(list[i].first, list[i].second - 1) && stone[list[i].first][list[i].second - 1].color == 0) { //xoxoxx
							if (is_inboard(list[i].first, list[i].second - 2) && stone[list[i].first][list[i].second - 2].color == 0) {
								pos_score(list[i].first, list[i].second + 1, 1);

							}
							else {
								pos_score(list[i].first, list[i].second - 1, 1);
								pos_score(list[i].first, list[i].second + 1, 2);
							}
							pos_score(list[i].first, list[i].second + 3, 1);
							pos_score(list[i].first, list[i].second + 4, 1);

						}

					}//end OXO 
					else if (is_inboard(list[i].first, list[i].second + 3) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first][list[i].second + 2].color == 0 && stone[list[i].first][list[i].second + 3].dir[direc] == 1 && stone[list[i].first][list[i].second + 3].blocked[direc] == false
						&& stone[list[i].first][list[i].second + 3].color == color) { //OXXO
						if (is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second + 5].color == 0) {//OXXOXX
							if (is_inboard(list[i].first, list[i].second - 1) && stone[list[i].first][list[i].second - 1].color == 0) {
								pos_score(list[i].first, list[i].second + 1, 0.75);
								pos_score(list[i].first, list[i].second + 2, 0.75);

							}
							else {

								pos_score(list[i].first, list[i].second + 1, 1.5);
								pos_score(list[i].first, list[i].second + 2, 1.5);
							}
							pos_score(list[i].first, list[i].second + 4, 1);
							pos_score(list[i].first, list[i].second + 5, 1);

						}
						else {
							if (is_inboard(list[i].first, list[i].second - 1) && stone[list[i].first][list[i].second].color == 0) {
								pos_score(list[i].first, list[i].second + 1, 0.75);
								pos_score(list[i].first, list[i].second + 2, 0.75);
								pos_score(list[i].first, list[i].second + 4, 1);
							}

						}

					}// end OXXO
					else if (is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first][list[i].second + 2].color == 0 && stone[list[i].first][list[i].second + 3].color == 0 &&
						stone[list[i].first][list[i].second + 4].dir[direc] == 1 && stone[list[i].first][list[i].second + 4].blocked[direc] == false
						&& stone[list[i].first][list[i].second + 4].color == color) { //OXXXO
						if (is_inboard(list[i].first, list[i].second - 1) && stone[list[i].first][list[i].second - 1].color == 0) {
							pos_score(list[i].first, list[i].second + 1, 0.75);
							pos_score(list[i].first, list[i].second + 2, 0.75);
							pos_score(list[i].first, list[i].second + 3, 0.75);
						}
						else {
							pos_score(list[i].first, list[i].second + 1, 1);
							pos_score(list[i].first, list[i].second + 2, 1);
							pos_score(list[i].first, list[i].second + 3, 1);
						}
						pos_score(list[i].first, list[i].second + 5, 1);


					}// end OXXXO
					else if (is_inboard(list[i].first, list[i].second + 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first][list[i].second + 2].color == 0 && stone[list[i].first][list[i].second + 3].color == 0 &&
						stone[list[i].first][list[i].second + 5].dir[direc] == 1 && stone[list[i].first][list[i].second + 5].blocked[(direc + 4) % 8] == false
						&& stone[list[i].first][list[i].second + 5].color == color) { //OXXXXO
						pos_score(list[i].first, list[i].second + 1, 0.75);
						pos_score(list[i].first, list[i].second + 2, 0.75);
						pos_score(list[i].first, list[i].second + 3, 0.75);
						pos_score(list[i].first, list[i].second + 4, 0.75);
					}


					break;


				case 5:

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 4, list[i].second + 4) && stone[list[i].first + 2][list[i].second + 2].color == 0
							&& stone[list[i].first + 3][list[i].second + 3].color == 0 && stone[list[i].first + 4][list[i].second + 4].color == 0) {

							if (is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first + 5][list[i].second + 5].color == 0) { //○○xxxx 
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && is_inboard(list[i].first - 3, list[i].second - 3)
									&& stone[list[i].first - 2][list[i].second - 2].color == 0 && stone[list[i].first - 3][list[i].second - 3].color == 0) { //xxx○○★xxx 좌 3 free
									pos_score(list[i].first + 2, list[i].second + 2, 2);
									pos_score(list[i].first + 3, list[i].second + 3, 1.5);
								}
								else {
									pos_score(list[i].first + 2, list[i].second + 2, 1);
									pos_score(list[i].first + 3, list[i].second + 3, 1);
								}

								pos_score(list[i].first + 4, list[i].second + 4, 1);
								pos_score(list[i].first + 5, list[i].second + 5, 1);


							}//inboard 5 
							if (stone[list[i].first + 5][list[i].second + 5].color != 0) { //x○○xxx● 
								if (stone[list[i].first + 5][list[i].second + 5].color != 0 && stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {

									pos_score(list[i].first + 2, list[i].second + 2, 1);
									pos_score(list[i].first + 3, list[i].second + 3, 1);
									pos_score(list[i].first + 4, list[i].second + 4, 1);
									if (stone[list[i].first - 2][list[i].second - 2].color != 0) {
										pos_score(list[i].first - 1, list[i].second - 1, 1);
									}



								}

							}//end //○○xxx● 

						}//ooxxx if  x free 
						else if (is_inboard(list[i].first + 3, list[i].second + 3) && stone[list[i].first + 2][list[i].second + 2].color == 0
							&& stone[list[i].first + 3][list[i].second + 3].color == 0) {
							if (is_inboard(list[i].first - 2, list[i].second - 2) && stone[list[i].first - 1][list[i].second - 1].color == 0
								&& stone[list[i].first - 2][list[i].second - 2].color == 0) {
								pos_score(list[i].first + 2, list[i].second + 2, 1);
								pos_score(list[i].first + 3, list[i].second + 3, 1);

							}

						}//ooxx  if x free

					}//2
					else if (is_inboard(list[i].first + 2, list[i].second + 2) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second + 2].dir[direc] == 1 && stone[list[i].first + 2][list[i].second + 2].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second + 2].color == color) { //OXOxxx
						if (is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first + 4][list[i].second + 4].color == 0 && stone[list[i].first + 5][list[i].second + 5].color == 0) {
							if (is_inboard(list[i].first - 2, list[i].second - 2) && stone[list[i].first - 1][list[i].second - 1].color == 0 && stone[list[i].first - 2][list[i].second - 2].color == 0) {
								pos_score(list[i].first + 1, list[i].second + 1, 1);

							}
							else {
								if (is_inboard(list[i].first - 1, list[i].second - 1) && stone[list[i].first - 1][list[i].second - 1].color == 0)
									pos_score(list[i].first - 1, list[i].second - 1, 1);
								else
								{
									if (is_inboard(list[i].first + 6, list[i].second + 6) && stone[list[i].first + 6][list[i].second + 6].color == color)
										break; // 2에서 7나오는 길 점수 부여안함. 
								}
								pos_score(list[i].first + 1, list[i].second + 1, 2);
							}
							pos_score(list[i].first + 3, list[i].second + 3, 1);
							pos_score(list[i].first + 4, list[i].second + 4, 1);
							pos_score(list[i].first + 5, list[i].second + 5, 1);

						}
						else if (is_inboard(list[i].first + 4, list[i].second + 4) && stone[list[i].first + 4][list[i].second + 4].color == 0
							&& is_inboard(list[i].first - 1, list[i].second - 1) && stone[list[i].first - 1][list[i].second - 1].color == 0) { //xoxoxx
							if (is_inboard(list[i].first - 2, list[i].second - 2) && stone[list[i].first - 2][list[i].second - 2].color == 0) {
								pos_score(list[i].first + 1, list[i].second + 1, 1);

							}
							else {
								pos_score(list[i].first - 1, list[i].second - 1, 1);
								pos_score(list[i].first + 1, list[i].second + 1, 2);
							}
							pos_score(list[i].first + 3, list[i].second + 3, 1);
							pos_score(list[i].first + 4, list[i].second + 4, 1);

						}

					}//end OXO 
					else if (is_inboard(list[i].first + 3, list[i].second + 3) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second + 2].color == 0 && stone[list[i].first + 3][list[i].second + 3].dir[direc] == 1 && stone[list[i].first + 3][list[i].second + 3].blocked[direc] == false
						&& stone[list[i].first + 3][list[i].second + 3].color == color) { //OXXO
						if (is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first + 5][list[i].second + 5].color == 0) {//OXXOXX
							if (is_inboard(list[i].first - 1, list[i].second - 1) && stone[list[i].first - 1][list[i].second - 1].color == 0) {
								pos_score(list[i].first + 1, list[i].second + 1, 0.75);
								pos_score(list[i].first + 2, list[i].second + 2, 0.75);

							}
							else {

								pos_score(list[i].first + 1, list[i].second + 1, 1.5);
								pos_score(list[i].first + 2, list[i].second + 2, 1.5);
							}
							pos_score(list[i].first + 4, list[i].second + 4, 1);
							pos_score(list[i].first + 5, list[i].second + 5, 1);

						}
						else {
							if (is_inboard(list[i].first - 1, list[i].second - 1) && stone[list[i].first][list[i].second].color == 0) {
								pos_score(list[i].first + 1, list[i].second + 1, 0.75);
								pos_score(list[i].first + 2, list[i].second + 2, 0.75);
								pos_score(list[i].first + 4, list[i].second + 4, 1);
							}

						}

					}// end OXXO
					else if (is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second + 2].color == 0 && stone[list[i].first + 3][list[i].second + 3].color == 0 &&
						stone[list[i].first + 4][list[i].second + 4].dir[direc] == 1 && stone[list[i].first + 4][list[i].second + 4].blocked[direc] == false
						&& stone[list[i].first + 4][list[i].second + 4].color == color) { //OXXXO
						if (is_inboard(list[i].first - 1, list[i].second - 1) && stone[list[i].first - 1][list[i].second - 1].color == 0) {
							pos_score(list[i].first + 1, list[i].second + 1, 0.75);
							pos_score(list[i].first + 2, list[i].second + 2, 0.75);
							pos_score(list[i].first + 3, list[i].second + 3, 0.75);
						}
						else {
							pos_score(list[i].first + 1, list[i].second + 1, 1);
							pos_score(list[i].first + 2, list[i].second + 2, 1);
							pos_score(list[i].first + 3, list[i].second + 3, 1);
						}
						pos_score(list[i].first + 5, list[i].second + 5, 1);


					}// end OXXXO
					else if (is_inboard(list[i].first + 5, list[i].second + 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second + 2].color == 0 && stone[list[i].first + 3][list[i].second + 3].color == 0 &&
						stone[list[i].first + 5][list[i].second + 5].dir[direc] == 1 && stone[list[i].first + 5][list[i].second + 5].blocked[(direc + 4) % 8] == false
						&& stone[list[i].first + 5][list[i].second + 5].color == color) { //OXXXXO
						pos_score(list[i].first + 1, list[i].second + 1, 0.75);
						pos_score(list[i].first + 2, list[i].second + 2, 0.75);
						pos_score(list[i].first + 3, list[i].second + 3, 0.75);
						pos_score(list[i].first + 4, list[i].second + 4, 0.75);
					}


					break;

				case 6:

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 4, list[i].second) && stone[list[i].first + 2][list[i].second].color == 0
							&& stone[list[i].first + 3][list[i].second].color == 0 && stone[list[i].first + 4][list[i].second].color == 0) {

							if (is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first + 5][list[i].second].color == 0) { //○○xxxx 
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && is_inboard(list[i].first - 3, list[i].second)
									&& stone[list[i].first - 2][list[i].second].color == 0 && stone[list[i].first - 3][list[i].second].color == 0) { //xxx○○★xxx 좌 3 free
									pos_score(list[i].first + 2, list[i].second, 2);
									pos_score(list[i].first + 3, list[i].second, 1.5);
								}
								else {
									pos_score(list[i].first + 2, list[i].second, 1);
									pos_score(list[i].first + 3, list[i].second, 1);
								}

								pos_score(list[i].first + 4, list[i].second, 1);
								pos_score(list[i].first + 5, list[i].second, 1);


							}//inboard 5 
							if (stone[list[i].first + 5][list[i].second].color != 0) { //x○○xxx● 
								if (stone[list[i].first + 5][list[i].second].color != 0 && stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {

									pos_score(list[i].first + 2, list[i].second, 1);
									pos_score(list[i].first + 3, list[i].second, 1);
									pos_score(list[i].first + 4, list[i].second, 1);
									if (stone[list[i].first - 2][list[i].second].color != 0) {
										pos_score(list[i].first - 1, list[i].second, 1);
									}



								}

							}//end //○○xxx● 

						}//ooxxx if  x free 
						else if (is_inboard(list[i].first + 3, list[i].second) && stone[list[i].first + 2][list[i].second].color == 0
							&& stone[list[i].first + 3][list[i].second].color == 0) {
							if (is_inboard(list[i].first - 2, list[i].second) && stone[list[i].first - 1][list[i].second].color == 0
								&& stone[list[i].first - 2][list[i].second].color == 0) {
								pos_score(list[i].first + 2, list[i].second, 1);
								pos_score(list[i].first + 3, list[i].second, 1);

							}

						}//ooxx  if x free

					}//2
					else if (is_inboard(list[i].first + 2, list[i].second) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second].dir[direc] == 1 && stone[list[i].first + 2][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second].color == color) { //OXOxxx
						if (is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first + 4][list[i].second].color == 0 && stone[list[i].first + 5][list[i].second + 5].color == 0) {
							if (is_inboard(list[i].first - 2, list[i].second) && stone[list[i].first - 1][list[i].second].color == 0 && stone[list[i].first - 2][list[i].second - 2].color == 0) {
								pos_score(list[i].first + 1, list[i].second, 1);

							}
							else {
								if (is_inboard(list[i].first - 1, list[i].second) && stone[list[i].first - 1][list[i].second].color == 0)
									pos_score(list[i].first - 1, list[i].second, 1);
								else
								{
									if (is_inboard(list[i].first + 6, list[i].second) && stone[list[i].first + 6][list[i].second].color == color)
										break; // 2에서 7나오는 길 점수 부여안함. 
								}
								pos_score(list[i].first + 1, list[i].second, 2);
							}
							pos_score(list[i].first + 3, list[i].second, 1);
							pos_score(list[i].first + 4, list[i].second, 1);
							pos_score(list[i].first + 5, list[i].second, 1);

						}
						else if (is_inboard(list[i].first + 4, list[i].second) && stone[list[i].first + 4][list[i].second].color == 0
							&& is_inboard(list[i].first - 1, list[i].second) && stone[list[i].first - 1][list[i].second].color == 0) { //xoxoxx
							if (is_inboard(list[i].first - 2, list[i].second) && stone[list[i].first - 2][list[i].second].color == 0) {
								pos_score(list[i].first + 1, list[i].second, 1);

							}
							else {
								pos_score(list[i].first - 1, list[i].second, 1);
								pos_score(list[i].first + 1, list[i].second, 2);
							}
							pos_score(list[i].first + 3, list[i].second, 1);
							pos_score(list[i].first + 4, list[i].second, 1);

						}

					}//end OXO 
					else if (is_inboard(list[i].first + 3, list[i].second) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second].color == 0 && stone[list[i].first + 3][list[i].second].dir[direc] == 1 && stone[list[i].first + 3][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 3][list[i].second].color == color) { //OXXO
						if (is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first + 5][list[i].second].color == 0) {//OXXOXX
							if (is_inboard(list[i].first - 1, list[i].second) && stone[list[i].first - 1][list[i].second].color == 0) {
								pos_score(list[i].first + 1, list[i].second, 0.75);
								pos_score(list[i].first + 2, list[i].second, 0.75);

							}
							else {

								pos_score(list[i].first + 1, list[i].second, 1.5);
								pos_score(list[i].first + 2, list[i].second, 1.5);
							}
							pos_score(list[i].first + 4, list[i].second, 1);
							pos_score(list[i].first + 5, list[i].second, 1);

						}
						else {
							if (is_inboard(list[i].first - 1, list[i].second) && stone[list[i].first][list[i].second].color == 0) {
								pos_score(list[i].first + 1, list[i].second, 0.75);
								pos_score(list[i].first + 2, list[i].second, 0.75);
								pos_score(list[i].first + 4, list[i].second, 1);
							}

						}

					}// end OXXO
					else if (is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second].color == 0 && stone[list[i].first + 3][list[i].second].color == 0 &&
						stone[list[i].first + 4][list[i].second].dir[direc] == 1 && stone[list[i].first + 4][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 4][list[i].second].color == color) { //OXXXO
						if (is_inboard(list[i].first - 1, list[i].second - 1) && stone[list[i].first - 1][list[i].second].color == 0) {
							pos_score(list[i].first + 1, list[i].second, 0.75);
							pos_score(list[i].first + 2, list[i].second, 0.75);
							pos_score(list[i].first + 3, list[i].second, 0.75);
						}
						else {
							pos_score(list[i].first + 1, list[i].second, 1);
							pos_score(list[i].first + 2, list[i].second, 1);
							pos_score(list[i].first + 3, list[i].second, 1);
						}
						pos_score(list[i].first + 5, list[i].second, 1);


					}// end OXXXO
					else if (is_inboard(list[i].first + 5, list[i].second) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second].color == 0 && stone[list[i].first + 3][list[i].second].color == 0 &&
						stone[list[i].first + 5][list[i].second].dir[direc] == 1 && stone[list[i].first + 5][list[i].second].blocked[(direc + 4) % 8] == false
						&& stone[list[i].first + 5][list[i].second].color == color) { //OXXXXO
						pos_score(list[i].first + 1, list[i].second, 0.75);
						pos_score(list[i].first + 2, list[i].second, 0.75);
						pos_score(list[i].first + 3, list[i].second, 0.75);
						pos_score(list[i].first + 4, list[i].second, 0.75);
					}


					break;

				case 7:

					if (stone[list[i].first][list[i].second].dir[direc] == 2 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 4, list[i].second - 4) && stone[list[i].first + 2][list[i].second - 2].color == 0
							&& stone[list[i].first + 3][list[i].second - 3].color == 0 && stone[list[i].first + 4][list[i].second - 4].color == 0) {

							if (is_inboard(list[i].first + 5, list[i].second - 5) && stone[list[i].first + 5][list[i].second - 5].color == 0) { //○○xxxx 
								if (stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false && is_inboard(list[i].first - 3, list[i].second + 3)
									&& stone[list[i].first - 2][list[i].second + 2].color == 0 && stone[list[i].first - 3][list[i].second + 3].color == 0) { //xxx○○★xxx 좌 3 free
									pos_score(list[i].first + 2, list[i].second - 2, 2);
									pos_score(list[i].first + 3, list[i].second - 3, 1.5);
								}
								else {
									pos_score(list[i].first + 2, list[i].second - 2, 1);
									pos_score(list[i].first + 3, list[i].second - 3, 1);
								}

								pos_score(list[i].first + 4, list[i].second - 4, 1);
								pos_score(list[i].first + 5, list[i].second - 5, 1);


							}//inboard 5 
							if (stone[list[i].first + 5][list[i].second - 5].color != 0) { //x○○xxx● 
								if (stone[list[i].first + 5][list[i].second - 5].color != 0 && stone[list[i].first][list[i].second].blocked[(direc + 4) % 8] == false) {

									pos_score(list[i].first + 2, list[i].second - 2, 1);
									pos_score(list[i].first + 3, list[i].second - 3, 1);
									pos_score(list[i].first + 4, list[i].second - 4, 1);
									if (stone[list[i].first - 2][list[i].second + 2].color != 0) {
										pos_score(list[i].first - 1, list[i].second + 1, 1);
									}



								}

							}//end //○○xxx● 

						}//ooxxx if  x free 
						else if (is_inboard(list[i].first + 3, list[i].second - 3) && stone[list[i].first + 2][list[i].second - 2].color == 0
							&& stone[list[i].first + 3][list[i].second - 3].color == 0) {
							if (is_inboard(list[i].first - 2, list[i].second + 2) && stone[list[i].first - 1][list[i].second + 1].color == 0
								&& stone[list[i].first - 2][list[i].second + 2].color == 0) {
								pos_score(list[i].first + 2, list[i].second - 2, 1);
								pos_score(list[i].first + 3, list[i].second - 3, 1);

							}

						}//ooxx  if x free

					}//2
					else if (is_inboard(list[i].first + 2, list[i].second - 2) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second - 2].dir[direc] == 1 && stone[list[i].first + 2][list[i].second - 2].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second - 2].color == color) { //OXOxxx
						if (is_inboard(list[i].first - 5, list[i].second - 5) && stone[list[i].first + 4][list[i].second - 4].color == 0 && stone[list[i].first + 5][list[i].second - 5].color == 0) {
							if (is_inboard(list[i].first - 2, list[i].second + 2) && stone[list[i].first - 1][list[i].second + 1].color == 0 && stone[list[i].first - 2][list[i].second + 2].color == 0) {
								pos_score(list[i].first + 1, list[i].second - 1, 1);

							}
							else {
								if (is_inboard(list[i].first - 1, list[i].second + 1) && stone[list[i].first - 1][list[i].second + 1].color == 0)
									pos_score(list[i].first - 1, list[i].second + 1, 1);
								else
								{
									if (is_inboard(list[i].first + 6, list[i].second - 6) && stone[list[i].first + 6][list[i].second - 6].color == color)
										break; // 2에서 7나오는 길 점수 부여안함. 
								}
								pos_score(list[i].first + 1, list[i].second - 1, 2);
							}
							pos_score(list[i].first + 3, list[i].second - 3, 1);
							pos_score(list[i].first + 4, list[i].second - 4, 1);
							pos_score(list[i].first + 5, list[i].second - 5, 1);

						}
						else if (is_inboard(list[i].first + 4, list[i].second - 4) && stone[list[i].first + 4][list[i].second - 4].color == 0
							&& is_inboard(list[i].first - 1, list[i].second + 1) && stone[list[i].first - 1][list[i].second + 1].color == 0) { //xoxoxx
							if (is_inboard(list[i].first - 2, list[i].second + 2) && stone[list[i].first - 2][list[i].second + 2].color == 0) {
								pos_score(list[i].first + 1, list[i].second - 1, 1);

							}
							else {
								pos_score(list[i].first - 1, list[i].second + 1, 1);
								pos_score(list[i].first + 1, list[i].second - 1, 2);
							}
							pos_score(list[i].first + 3, list[i].second - 3, 1);
							pos_score(list[i].first + 4, list[i].second - 4, 1);

						}

					}//end OXO 
					else if (is_inboard(list[i].first + 3, list[i].second - 3) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second - 2].color == 0 && stone[list[i].first + 3][list[i].second - 3].dir[direc] == 1 && stone[list[i].first + 3][list[i].second - 3].blocked[direc] == false
						&& stone[list[i].first + 3][list[i].second - 3].color == color) { //OXXO
						if (is_inboard(list[i].first + 5, list[i].second - 5) && stone[list[i].first + 5][list[i].second - 5].color == 0) {//OXXOXX
							if (is_inboard(list[i].first - 1, list[i].second + 1) && stone[list[i].first - 1][list[i].second + 1].color == 0) {
								pos_score(list[i].first + 1, list[i].second - 1, 0.75);
								pos_score(list[i].first + 2, list[i].second - 2, 0.75);

							}
							else {

								pos_score(list[i].first + 1, list[i].second - 1, 1.5);
								pos_score(list[i].first + 2, list[i].second - 2, 1.5);
							}
							pos_score(list[i].first + 4, list[i].second - 4, 1);
							pos_score(list[i].first + 5, list[i].second - 5, 1);

						}
						else {
							if (is_inboard(list[i].first - 1, list[i].second + 1) && stone[list[i].first][list[i].second].color == 0) {
								pos_score(list[i].first + 1, list[i].second - 1, 0.75);
								pos_score(list[i].first + 2, list[i].second - 2, 0.75);
								pos_score(list[i].first + 4, list[i].second - 4, 1);
							}

						}

					}// end OXXO
					else if (is_inboard(list[i].first + 5, list[i].second - 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second - 2].color == 0 && stone[list[i].first + 3][list[i].second - 3].color == 0 &&
						stone[list[i].first + 4][list[i].second - 4].dir[direc] == 1 && stone[list[i].first + 4][list[i].second - 4].blocked[direc] == false
						&& stone[list[i].first + 4][list[i].second - 4].color == color) { //OXXXO
						if (is_inboard(list[i].first - 1, list[i].second + 1) && stone[list[i].first - 1][list[i].second + 1].color == 0) {
							pos_score(list[i].first + 1, list[i].second - 1, 0.75);
							pos_score(list[i].first + 2, list[i].second - 2, 0.75);
							pos_score(list[i].first + 3, list[i].second - 3, 0.75);
						}
						else {
							pos_score(list[i].first + 1, list[i].second - 1, 1);
							pos_score(list[i].first + 2, list[i].second - 2, 1);
							pos_score(list[i].first + 3, list[i].second - 3, 1);
						}
						pos_score(list[i].first + 5, list[i].second - 5, 1);


					}// end OXXXO
					else if (is_inboard(list[i].first + 5, list[i].second - 5) && stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false
						&& stone[list[i].first + 2][list[i].second - 2].color == 0 && stone[list[i].first + 3][list[i].second - 3].color == 0 &&
						stone[list[i].first + 5][list[i].second - 5].dir[direc] == 1 && stone[list[i].first + 5][list[i].second - 5].blocked[(direc + 4) % 8] == false
						&& stone[list[i].first + 5][list[i].second - 5].color == color) { //OXXXXO
						pos_score(list[i].first + 1, list[i].second - 1, 0.75);
						pos_score(list[i].first + 2, list[i].second - 2, 0.75);
						pos_score(list[i].first + 3, list[i].second - 3, 0.75);
						pos_score(list[i].first + 4, list[i].second - 4, 0.75);
					}


					break;


				}//switch




				 //	return false;

			}//if
		}//direc
	}//i
	
	if (mode) {
		bool loop = true;
		int max_index = 0;
		if (!score.empty()) {

			while (loop) {

				max_index = return_max_index();
				if (optimal_x != score[max_index].second.first || optimal_y != score[max_index].second.second) {
					loop = false;
				}
				optimal_x = score[max_index].second.first;
				optimal_y = score[max_index].second.second;

				score.erase(score.begin() + max_index);
			}
			/*if(max_index>=3.5 || color==1)
			return true;*/
			return true;
		}
		else {
			return false;
		}
	}
	else
		return false;
}
void find_two_x4(vector<PAIR> list) {

	int two_count = 0;
	score.clear();
	int color = 0;
	int check = 0;
	if (!list.empty()) {
		color = stone[list[0].first][list[0].second].color;
		check = (color % 2) + 1;
	}
	if (!block_list.empty()) {
		if (color != stone[block_list.front().first][block_list.front().second].color) {
			rotation_block();
		}
	}
	//for (int i = 0; i < list.size(); i++) {
	for (int i = list.size() - 1; i >= 0; i--) {
		for (int direc = 0; direc < 8; direc++) {
			if (stone[list[i].first][list[i].second].dir[(direc + 4) % 8] == 1) {
				if (stone[list[i].first][list[i].second].dir[direc] == 2)
					two_count++;





				//	return false;

			}//if
		}//direc
	}//i


	if (two_count >= 4)
		start_attack = true;
}
bool find_one(vector<PAIR> * list2) {
	if (mode)
		score.clear();
	int color = 0;

	vector<PAIR> list = *list2;

	if (!list2->empty()) {
		if (&list2[0] == &my_list) {
			color = 1;
		}
		else
			color = 2;


		

	}
	int opcolor;
	if (color == 1)
		opcolor = 2;
	else
		opcolor = 1;
	if (!block_list.empty()) {
		if (color != stone[block_list.front().first][block_list.front().second].color) {
			rotation_block();
		}
	}
	//for (int i = 0; i < list.size(); i++) {
	for (int i = list.size() - 1; i >= 0; i--) {
		for (int direc = 0; direc < 8; direc++) {

			if (!start_attack) {
				if (list[i].first < 3 || list[i].first >15 || list[i].second < 3 || list[i].second >15)
					break;
			}
			if (stone[list[i].first][list[i].second].dir[(direc + 4) % 8] == 1) {
				switch (direc) {
				case 0:
					if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second - 6) && stone[list[i].first][list[i].second - 6].color != color) {
							if (stone[list[i].first][list[i].second - 1].color != opcolor&&stone[list[i].first][list[i].second - 2].color != opcolor&&
								stone[list[i].first][list[i].second - 3].color != opcolor&&stone[list[i].first][list[i].second - 4].color != opcolor&&
								stone[list[i].first][list[i].second - 5].color != opcolor) {
								if (stone[list[i].first][list[i].second - 6].color == 0) {
									if (stone[list[i].first][list[i].second - 1].color == 0)
										pos_score(list[i].first, list[i].second - 1, 1.05);
									if (stone[list[i].first][list[i].second - 2].color == 0)
										pos_score(list[i].first, list[i].second - 2, 1.04);
									if (stone[list[i].first][list[i].second - 3].color == 0)
										pos_score(list[i].first, list[i].second - 3, 1.03);
									if (stone[list[i].first][list[i].second - 4].color == 0)
										pos_score(list[i].first, list[i].second - 4, 1.02);
									if (stone[list[i].first][list[i].second - 5].color == 0)
										pos_score(list[i].first, list[i].second - 5, 1.01);
								}

							}
						}
					}
					break;
				case 1:
					if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 6, list[i].second - 6) && stone[list[i].first - 6][list[i].second - 6].color != color) {
							if (stone[list[i].first - 1][list[i].second - 1].color != opcolor&&stone[list[i].first - 2][list[i].second - 2].color != opcolor&&
								stone[list[i].first - 3][list[i].second - 3].color != opcolor&&stone[list[i].first - 4][list[i].second - 4].color != opcolor&&
								stone[list[i].first - 5][list[i].second - 5].color != opcolor) {
								if (stone[list[i].first - 6][list[i].second - 6].color == 0) {
									if (stone[list[i].first - 1][list[i].second - 1].color == 0)
										pos_score(list[i].first - 1, list[i].second - 1, 1.05);
									if (stone[list[i].first - 2][list[i].second - 2].color == 0)
										pos_score(list[i].first - 2, list[i].second - 2, 1.04);
									if (stone[list[i].first - 3][list[i].second - 3].color == 0)
										pos_score(list[i].first - 3, list[i].second - 3, 1.03);
									if (stone[list[i].first - 4][list[i].second - 4].color == 0)
										pos_score(list[i].first - 4, list[i].second - 4, 1.02);
									if (stone[list[i].first - 5][list[i].second - 5].color == 0)
										pos_score(list[i].first - 5, list[i].second - 5, 1.01);
								}

							}
						}
					}
					break;
				case 2:
					if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 6, list[i].second) && stone[list[i].first - 6][list[i].second].color != color) {
							if (stone[list[i].first - 1][list[i].second].color != opcolor&&stone[list[i].first - 2][list[i].second].color != opcolor&&
								stone[list[i].first - 3][list[i].second].color != opcolor&&stone[list[i].first - 4][list[i].second].color != opcolor&&
								stone[list[i].first - 5][list[i].second].color != opcolor) {
								if (stone[list[i].first - 6][list[i].second].color == 0) {
									if (stone[list[i].first - 1][list[i].second].color == 0)
										pos_score(list[i].first - 1, list[i].second, 1.05);
									if (stone[list[i].first - 2][list[i].second].color == 0)
										pos_score(list[i].first - 2, list[i].second, 1.04);
									if (stone[list[i].first - 3][list[i].second].color == 0)
										pos_score(list[i].first - 3, list[i].second, 1.03);
									if (stone[list[i].first - 4][list[i].second].color == 0)
										pos_score(list[i].first - 4, list[i].second, 1.02);
									if (stone[list[i].first - 5][list[i].second].color == 0)
										pos_score(list[i].first - 5, list[i].second, 1.01);
								}

							}
						}
					}
					break;
				case 3:
					if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first - 6, list[i].second + 6) && stone[list[i].first - 6][list[i].second + 6].color != color) {
							if (stone[list[i].first - 1][list[i].second + 1].color != opcolor&&stone[list[i].first - 2][list[i].second + 2].color != opcolor&&
								stone[list[i].first - 3][list[i].second + 3].color != opcolor&&stone[list[i].first - 4][list[i].second + 4].color != opcolor&&
								stone[list[i].first - 5][list[i].second + 5].color != opcolor) {
								if (stone[list[i].first - 6][list[i].second + 6].color == 0) {
									if (stone[list[i].first - 1][list[i].second + 1].color == 0)
										pos_score(list[i].first - 1, list[i].second + 1, 1.05);
									if (stone[list[i].first - 2][list[i].second + 2].color == 0)
										pos_score(list[i].first - 2, list[i].second + 2, 1.04);
									if (stone[list[i].first - 3][list[i].second + 3].color == 0)
										pos_score(list[i].first - 3, list[i].second + 3, 1.03);
									if (stone[list[i].first - 4][list[i].second + 4].color == 0)
										pos_score(list[i].first - 4, list[i].second + 4, 1.02);
									if (stone[list[i].first - 5][list[i].second + 5].color == 0)
										pos_score(list[i].first - 5, list[i].second + 5, 1.01);
								}

							}
						}
					}
					break;
				case 4:
					if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first, list[i].second + 6) && stone[list[i].first][list[i].second + 6].color != color) {
							if (stone[list[i].first][list[i].second + 1].color != opcolor&&stone[list[i].first][list[i].second + 2].color != opcolor&&
								stone[list[i].first][list[i].second + 3].color != opcolor&&stone[list[i].first][list[i].second + 4].color != opcolor&&
								stone[list[i].first][list[i].second + 5].color != opcolor) {
								if (stone[list[i].first][list[i].second + 6].color == 0) {
									if (stone[list[i].first][list[i].second + 1].color == 0)
										pos_score(list[i].first, list[i].second + 1, 1.05);
									if (stone[list[i].first][list[i].second + 2].color == 0)
										pos_score(list[i].first, list[i].second + 2, 1.04);
									if (stone[list[i].first][list[i].second + 3].color == 0)
										pos_score(list[i].first, list[i].second + 3, 1.03);
									if (stone[list[i].first][list[i].second + 4].color == 0)
										pos_score(list[i].first, list[i].second + 4, 1.02);
									if (stone[list[i].first][list[i].second + 5].color == 0)
										pos_score(list[i].first, list[i].second + 5, 1.01);
								}

							}
						}
					}
					break;
				case 5:
					if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 6, list[i].second + 6) && stone[list[i].first + 6][list[i].second + 6].color != color) {
							if (stone[list[i].first + 1][list[i].second + 1].color != opcolor&&stone[list[i].first + 2][list[i].second + 2].color != opcolor&&
								stone[list[i].first + 3][list[i].second + 3].color != opcolor&&stone[list[i].first + 4][list[i].second + 4].color != opcolor&&
								stone[list[i].first + 5][list[i].second + 5].color != opcolor) {
								if (stone[list[i].first + 6][list[i].second + 6].color == 0) {
									if (stone[list[i].first + 1][list[i].second + 1].color == 0)
										pos_score(list[i].first + 1, list[i].second + 1, 1.05);
									if (stone[list[i].first + 2][list[i].second + 2].color == 0)
										pos_score(list[i].first + 2, list[i].second + 2, 1.04);
									if (stone[list[i].first + 3][list[i].second + 3].color == 0)
										pos_score(list[i].first + 3, list[i].second + 3, 1.03);
									if (stone[list[i].first + 4][list[i].second + 4].color == 0)
										pos_score(list[i].first + 4, list[i].second + 4, 1.02);
									if (stone[list[i].first + 5][list[i].second + 5].color == 0)
										pos_score(list[i].first + 5, list[i].second + 5, 1.01);
								}

							}
						}
					}
					break;
				case 6:
					if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 6, list[i].second) && stone[list[i].first + 6][list[i].second].color != color) {
							if (stone[list[i].first + 1][list[i].second].color != opcolor&&stone[list[i].first + 2][list[i].second].color != opcolor&&
								stone[list[i].first + 3][list[i].second].color != opcolor&&stone[list[i].first + 4][list[i].second].color != opcolor&&
								stone[list[i].first + 5][list[i].second].color != opcolor) {
								if (stone[list[i].first + 6][list[i].second].color == 0) {
									if (stone[list[i].first + 1][list[i].second].color == 0)
										pos_score(list[i].first + 1, list[i].second, 1.05);
									if (stone[list[i].first + 2][list[i].second].color == 0)
										pos_score(list[i].first + 2, list[i].second, 1.04);
									if (stone[list[i].first + 3][list[i].second].color == 0)
										pos_score(list[i].first + 3, list[i].second, 1.03);
									if (stone[list[i].first + 4][list[i].second].color == 0)
										pos_score(list[i].first + 4, list[i].second, 1.02);
									if (stone[list[i].first + 5][list[i].second].color == 0)
										pos_score(list[i].first + 5, list[i].second, 1.01);
								}

							}
						}
					}
					break;
				case 7:
					if (stone[list[i].first][list[i].second].dir[direc] == 1 && stone[list[i].first][list[i].second].blocked[direc] == false) {
						if (is_inboard(list[i].first + 6, list[i].second - 6) && stone[list[i].first + 6][list[i].second - 6].color != color) {
							if (stone[list[i].first + 1][list[i].second - 1].color != opcolor&&stone[list[i].first + 2][list[i].second - 2].color != opcolor&&
								stone[list[i].first + 3][list[i].second - 3].color != opcolor&&stone[list[i].first + 4][list[i].second - 4].color != opcolor&&
								stone[list[i].first + 5][list[i].second - 5].color != opcolor) {
								if (stone[list[i].first + 6][list[i].second - 6].color == 0) {
									if (stone[list[i].first + 1][list[i].second - 1].color == 0)
										pos_score(list[i].first + 1, list[i].second - 1, 1.05);
									if (stone[list[i].first + 2][list[i].second - 2].color == 0)
										pos_score(list[i].first + 2, list[i].second - 2, 1.04);
									if (stone[list[i].first + 3][list[i].second - 3].color == 0)
										pos_score(list[i].first + 3, list[i].second - 3, 1.03);
									if (stone[list[i].first + 4][list[i].second - 4].color == 0)
										pos_score(list[i].first + 4, list[i].second - 4, 1.02);
									if (stone[list[i].first + 5][list[i].second - 5].color == 0)
										pos_score(list[i].first + 5, list[i].second - 5, 1.01);
								}

							}
						}
					}
					break;
				}//switch
			}//if
		}//direc
	}//i
	if (mode) {
		bool loop = true;
		int max_index = 0;
		if (!score.empty()) {

			while (loop) {

				max_index = return_max_index();
				if (optimal_x != score[max_index].second.first || optimal_y != score[max_index].second.second) {
					loop = false;
				}
				optimal_x = score[max_index].second.first;
				optimal_y = score[max_index].second.second;

				score.erase(score.begin() + max_index);
			}
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}