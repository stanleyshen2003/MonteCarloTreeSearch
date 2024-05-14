
#include "STcpClient.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <cmath>
#define g_row 12
#define g_col 12
#define sheep_num 16

using namespace std;

/*
    選擇起始位置
    選擇範圍僅限場地邊緣(至少一個方向為牆)
    
    return: init_pos
    init_pos=<x,y>,代表你要選擇的起始位置
    
*/
std::vector<int> InitPos(int mapStat[12][12])
{
	std::vector<int> init_pos;
	init_pos.resize(2);

	/*
		Write your code here
	*/
    int col = g_col;
    int row = g_row;

    int x = ceil(col / 2);
    int y = ceil(row / 2);
    int dir[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
    bool visit[g_row][g_col] = { false };


    int decide_x = x;
    int decide_y = y;
    bool finded = false;
    queue<pair<int, int>> q;
    q.push({ x, y });
    visit[x][y] = true;
    while (!q.empty()) {
        pair<int, int> current;
        current = q.front();
        q.pop();

        for (int i = 0; i < 4; i++) {
            int dx = current.first + dir[i][0];
            int dy = current.second + dir[i][1];

            if (dx > 0 && dx < col && dy > 0 && dy < row) {
                if (mapStat[dx][dy] == -1) {
                    finded = true;
                    decide_x = current.first;
                    decide_y = current.second;

                    break;
                }

                else if (mapStat[dx][dy] == 0 && visit[dx][dy] == false) {
                    q.push({ dx, dy });
                    visit[dx][dy] = true;
                }
            }

            if (finded) break;
        }
    }

    init_pos[0] = decide_x;
    init_pos[1] = decide_y;

    return init_pos;
}

/*
	產出指令
    
    input: 
	playerID: 你在此局遊戲中的角色(1~4)
    mapStat : 棋盤狀態, 為 12*12矩陣, 
					0=可移動區域, -1=障礙, 1~4為玩家1~4佔領區域
    sheepStat : 羊群分布狀態, 範圍在0~16, 為 12*12矩陣

    return Step
    Step : <x,y,m,dir> 
            x, y 表示要進行動作的座標 
            m = 要切割成第二群的羊群數量
            dir = 移動方向(1~9),對應方向如下圖所示
            1 2 3
			4 X 6
			7 8 9
*/
std::vector<int> GetStep(int playerID,int mapStat[12][12], int sheepStat[12][12])
{
	std::vector<int> step;
	step.resize(4);

	/*
		Write your code here
	*/
    // cout << player_turn << '\n';
    int directions8[8][2] = { {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1} };

    int x, y;
    int store_x, store_y;
    int value = -100;
    for (int i = 0; i < g_row; i++) {
        for (int j = 0; j < g_col; j++) {
            int direction = 0;
            if (mapStat[i][j] == playerID && sheepStat[i][j] > 1) {
                for (int k = 0; k < 8; k++) {
                    x = i + directions8[k][0];
                    y = j + directions8[k][1];
                    if (x >= 0 && x < 12 && y >= 0 && y < 12 && mapStat[x][y] == 0) {
                        direction++;
                    }
                }
                if (!direction)
                    continue;
                direction = sheepStat[i][j] - 2 * direction;
                if (direction > value) {
                    value = direction;
                    store_x = i; store_y = j;
                }
            }
        }
    }

    // cout << "x: " << store_x << " y: " << store_y << endl;

    pair<int, int> dir8_value[8];
    for (int i = 0; i < 8; i++) {
        dir8_value[i] = make_pair(-1, 0);
    }
    int dircount = 0;
    for (int i = 0; i < 8; i++) {
        if (mapStat[store_x + directions8[i][0]][store_y + directions8[i][1]] == 0) {
            dircount++;
        }
    }

    for (int i = 0; i < 8; i++) {
        x = store_x, y = store_y;

        while (1) {
            x = x + directions8[i][0], y = y + directions8[i][1];
            if (x < 0 || x >= g_col || y < 0 || y >= g_row || mapStat[x][y] != 0) {
                break;
            }
            dir8_value[i].first++;
        }
        x -= directions8[i][0];
        y -= directions8[i][1];
        for (int j = 0; j < 8; j++) {
            if (mapStat[x + directions8[j][0]][y + directions8[j][1]] == 0) {
                dir8_value[i].second++;
            }
        }
    }

    int max_value = -10000000;
    int out_dir = -1;
    int out_dir_8 = -1;
    for (int i = 0; i < 8; i++) {
        if (dir8_value[i].first == -1) {
            continue;
        }
        if (dir8_value[i].second < 3) {
            dir8_value[i].second = -50;
        }
        if (dir8_value[i].first > 8) {
            dir8_value[i].first = 50;
        }
        if (dir8_value[i].first - dir8_value[i].second > max_value) {
            max_value = dir8_value[i].first - dir8_value[i].second;
            out_dir_8 = (dir8_value[i].second < 0) ? 2 : dir8_value[i].second;
            out_dir = i;
        }
    }

    // cout << "out_dir: " << out_dir << " out_dir_8: " << out_dir_8 << endl;

    double amount = (double)sheepStat[store_x][store_y] * ((double)out_dir_8 / ((double)dircount + (double)out_dir_8));

    int n = (int)amount;
    if (n <= 0) {
        n = 1;
    }
    if (n >= sheepStat[store_x][store_y]) {
        n = sheepStat[store_x][store_y] - 1;
    }

    step[0] = store_x;
    step[1] = store_y;
    step[2] = n;
    step[3] = (out_dir < 4) ? out_dir + 1 : out_dir + 2;;

    return step;
}

int main()
{
	int id_package;
	int playerID;
    int mapStat[12][12];
    int sheepStat[12][12];

	// player initial
	GetMap(id_package,playerID,mapStat);
	std::vector<int> init_pos = InitPos(mapStat);
	SendInitPos(id_package,init_pos);

	while (true)
	{
		if (GetBoard(id_package, mapStat, sheepStat))
			break;
		// hide other player's sheep number start
		for (int i = 0; i < 12; i++)
			for (int j = 0; j < 12; j++)
				if (mapStat[i][j] != playerID)
					sheepStat[i][j] = 0;
		// hide other player's sheep number end 
		std::vector<int> step = GetStep(playerID,mapStat,sheepStat);
		SendStep(id_package, step);
	}
	// DON'T MODIFY ANYTHING IN THIS WHILE LOOP OR YOU WILL GET 0 POINT IN THIS QUESTION
}
