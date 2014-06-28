#ifndef __OTHELLO_H__
#define __OTHELLO_H__

#include <cstring>
#include <string>
#include <vector>
using namespace std;

class othello16 {
	static const int direction[8][2];
	static const int MAXN;
	int map[16][16];
	void set(int color, int x, int y);
public:
	int mycolor;
	
	void init();
	
	void init(int color, string s);
	//初始化局面

	bool canmove(int color, int x, int y);
	//返回color是否能落子在(x,y)
	
	bool is(int color, int x, int y);
	//判断(x,y)是否有颜色为color的子.color=0则为询问该格是否为空
	
	bool canmove(int color);
	//判断当前执color色的玩家是否有落子点
	
	int count(int color);
	//检查当前棋盘有多少color颜色的点.color=0则为询问有多少空格
	
	vector<pair<int, int> > allmove(int color);
	//返回能color颜色的落子点的列表.
	
	bool play(int color, int &x, int &y);
	//修改局面,在(x,y)放下color颜色的棋子.color不能为0
	
	string tostring();
	//返回当前局面的string表示
};

#endif
