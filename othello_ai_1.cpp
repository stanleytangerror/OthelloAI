#include <iostream>
#include <stack>
#include "gamti.h"
#include "othello16.h"

// 棋盘边长
#define MAXEDGE 16
// 可能出现的估价函数最大值
#define MAXVALUE 100000
// 遍历深度
#define DEPTH 3
//棋子颜色
#define WHITE 1
#define BLACK 2
#define BLANK 0

class othello_ai {
	othello16 o;//实例化othello16类
public:
	int valuemap[MAXEDGE][MAXEDGE];
	//std::stack< std::pair<int, int> > steps;
	std::pair<int, int> next;
	void init(int color, std::string s);
	void init_valuemap();
	void adjust_valuemap();
	void move(int color, int x, int y);
	std::pair<int, int> get();
	int turn(othello16 o_upper, int player, int backpoint, int depth);
	int evaluation(othello16 o_t, int player);
	void string2map(std::string str);
	std::pair<int, int> seize_peak(std::vector< std::pair<int, int> > v);
};

//初始化阶段,告知你所执子的颜色,和当前棋盘落子情况s
void othello_ai::init(int color, std::string s){
	o.init(color, s);
	init_valuemap();
	std::cerr<<"My color is "<<o.mycolor<<std::endl;
}

// 初始化估计值
void othello_ai::init_valuemap() {
	int i, j;
	int value_peak = 500, value01 = -1000, value02 = -1000, value_edge = 10, value_diag = -5;
	for (i = 0; i < MAXEDGE; i ++) {
		for (j = 0; j < MAXEDGE; j ++) {
			valuemap[i][j] = ( i * (MAXEDGE - 1 - i) + j * (MAXEDGE - 1 - j) - 60 ) / 16;
			// 横边
			if ( i * (i - MAXEDGE + 1) == 0 ) {
				valuemap[i][j] = 60 - j * (MAXEDGE - 1 - j);
				continue;
			}
			// 竖边
			if ( j * (j - MAXEDGE + 1) == 0 ) {
				valuemap[i][j] = 60 - i * (MAXEDGE - 1 - i);
				continue;
			}
			// 边内
			if ( (i == 1 || i == MAXEDGE - 2) || (j == 1 || j == MAXEDGE - 2) ) {
				valuemap[i][j] = 0;
				continue;
			}
			// 对角线
			//if ( (i == j) || (i + j == MAXEDGE - 1) ) {
			//	valuemap[i][j] = value_diag;
			//	continue;
			//}
		}
	}

	// 顶点
	valuemap[0][0] = value_peak;
	valuemap[0][MAXEDGE - 1] = value_peak;
	valuemap[MAXEDGE - 1][0] = value_peak;
	valuemap[MAXEDGE - 1][MAXEDGE - 1] = value_peak;
	// 顶点周围
	valuemap[0][1] = value01;
	valuemap[0][MAXEDGE - 2] = value01;
	valuemap[MAXEDGE - 1][1] = value01;
	valuemap[MAXEDGE - 1][MAXEDGE - 2] = value01;
	valuemap[1][0] = value01;
	valuemap[MAXEDGE - 2][0] = value01;
	valuemap[1][MAXEDGE - 1] = value01;
	valuemap[MAXEDGE - 2][MAXEDGE - 1] = value01;
	valuemap[1][1] = value02;
	valuemap[MAXEDGE - 2][1] = value02;
	valuemap[1][MAXEDGE - 2] = value02;
	valuemap[MAXEDGE - 2][MAXEDGE - 2] = value02;
}

void othello_ai::adjust_valuemap() {
	if ( o.is(o.mycolor, 0, 0) ) {
		valuemap[0][1] *= (valuemap[0][1] < 0) ? -1 : 1;
		valuemap[1][0] *= (valuemap[1][0] < 0) ? -1 : 1;
		valuemap[1][1] *= (valuemap[1][1] < 0) ? -1 : 1;
	}
	if ( o.is(o.mycolor, 0, MAXEDGE - 1) ) {
		valuemap[0][MAXEDGE - 2] *= (valuemap[0][MAXEDGE - 2] < 0) ? -1 : 1;
		valuemap[1][MAXEDGE - 1] *= (valuemap[1][MAXEDGE - 1] < 0) ? -1 : 1;
		valuemap[1][MAXEDGE - 2] *= (valuemap[1][MAXEDGE - 2] < 0) ? -1 : 1;
	}
	if ( o.is(o.mycolor, MAXEDGE - 1, 0) ) {
		valuemap[MAXEDGE - 2][1] *= (valuemap[MAXEDGE - 2][1] < 0) ? -1 : 1;
		valuemap[MAXEDGE - 1][0] *= (valuemap[MAXEDGE - 1][0] < 0) ? -1 : 1;
		valuemap[MAXEDGE - 2][1] *= (valuemap[MAXEDGE - 2][1] < 0) ? -1 : 1;
	}
	if ( o.is(o.mycolor, MAXEDGE - 1, MAXEDGE - 1) ) {
		valuemap[MAXEDGE - 1][MAXEDGE - 2] *= (valuemap[MAXEDGE - 1][MAXEDGE - 2] < 0) ? -1 : 1;
		valuemap[MAXEDGE - 2][MAXEDGE - 1] *= (valuemap[MAXEDGE - 2][MAXEDGE - 1] < 0) ? -1 : 1;
		valuemap[MAXEDGE - 2][MAXEDGE - 2] *= (valuemap[MAXEDGE - 2][MAXEDGE - 2] < 0) ? -1 : 1;
	}
}

//告知所有下子情况(包括你自己的落子情况)
void othello_ai::move(int color, int x, int y){
	o.play(color, x, y);
}

//返回一个你的落子决定
std::pair<int, int> othello_ai::get(){
	adjust_valuemap();
	//获得所有可落子的位置,按先行后列的顺序
	std::vector< std::pair<int, int> > ans = o.allmove(o.mycolor);

	std::cerr<<std::endl<<"starting running ..."<<std::endl;

	next = seize_peak(ans);
	if ( next.first >= 0 && next.second >= 0 ) {
		std::cerr<<std::endl<<"-->> player "<<o.mycolor<<" move ("<<next.first<<", "<<next.second<<")"<<std::endl;
		return next;
	}

	turn(o, o.mycolor, MAXVALUE, DEPTH);
	//while (!steps.empty()) { 
	//	next = steps.top();
	//	steps.pop();
	//}
	if (next.first >= 0 && next.second >= 0) {
		if (next.first == ans[0].first && next.second == ans[1].second)
			std::cerr<<std::endl<<"turn choose first"<<std::endl;
		std::cerr<<std::endl<<"-->> player "<<o.mycolor<<" move ("<<next.first<<", "<<next.second<<")"<<std::endl;
		return next;
	} else {
		std::cerr<<std::endl<<"have to choose first"<<std::endl;
		return ans[0];
	}
}

int othello_ai::turn(othello16 o_upper, int player, int backpoint, int depth) {
	// 检查超时
	int runtime = get_time();
	if (runtime > 1800) {
		std::cerr<<"time "<<runtime<<" overflow"<<std::endl;
		return (player == o.mycolor) ? MAXVALUE : -MAXVALUE;
	}
	-- depth;

	// std::cerr<<"player "<<player<<" my color "<<o.mycolor<<std::endl;
	othello16 o_t;
	int value_t, value_mark;
	std::pair<int, int> step_mark (-1, -1);
	// 初始化value_mark
	if (player == o.mycolor) {
		value_mark = -MAXVALUE;
	} else {
		value_mark = MAXVALUE;
	}

	std::vector< std::pair<int, int> > nextlist = o_upper.allmove(player);
	std::string map = o_upper.tostring();

	if ( nextlist.size() == 0 ) {
		// 无可下点
		std::cerr<<"nowhere to move ";
		return (player == o.mycolor) ? -MAXVALUE : MAXVALUE;
	} else if (depth <= 0) {

		// 到达最深层，计算估价函数值
		// std::cerr<<"tree hight "<<depth<<std::endl;
		std::vector< std::pair<int, int> >::iterator it;
		for (it = nextlist.begin() ; it != nextlist.end(); ++ it) {
			// 初始化当前棋盘
			o_t.init(o.mycolor, map);
			// 落子
			o_t.play(player, (*it).first, (*it).second);
			// string2map(o_t.tostring());
			value_t = evaluation(o_t, player);
			std::cerr<<std::endl<<"depth "<<DEPTH - depth<<":";
			for (int k = 0; k < DEPTH - depth; k ++) 
				std::cerr<<"    ";
			std::cerr<<"leaf ("<<(*it).first<<", "<<(*it).second<<") value "<<value_t<<", ";
			if ( (player == o.mycolor && value_t >= value_mark)
				|| (player != o.mycolor && value_t <= value_mark) ) {
					value_mark = value_t;
			}
			// 剪枝
			if (player == o.mycolor && value_mark >= backpoint) {
				std::cerr<<"cut beta parent value "<<backpoint<<" current value "<<value_mark<<std::endl;
				return value_mark;
			} else if (player != o.mycolor && value_mark <= backpoint) {
				std::cerr<<"cut alpha parent value "<<backpoint<<" current value "<<value_mark<<std::endl;
				return value_mark;
			}
		}
		return value_mark;
	} else {

		// 普通情况
		std::vector< std::pair<int, int> >::iterator it;
		for (it = nextlist.begin() ; it != nextlist.end(); ++ it) {
			// 初始化当前棋盘
			o_t.init(o.mycolor, map);
			// 落子
			o_t.play(player, (*it).first, (*it).second);
			// string2map(o_t.tostring());
			value_t = turn(o_t, 3 - player, value_mark, depth);
			if (value_t == MAXVALUE || value_t == -MAXVALUE) {
				// 子问题超时
				break;
			}
			std::cerr<<std::endl<<"depth "<<DEPTH - depth<<":";
			for (int k = 0; k < DEPTH - depth; k ++) 
				std::cerr<<"    ";
			std::cerr<<"node ("<<(*it).first<<", "<<(*it).second<<") value "<<value_t<<", ";
			if ( (player == o.mycolor && value_t >= value_mark)// 己方落子
				|| (player != o.mycolor && value_t <= value_mark) ) {//对方落子
					value_mark = value_t;
					step_mark.first = (*it).first;
					step_mark.second = (*it).second;
			}
			// 剪枝
			if (player == o.mycolor && value_mark >= backpoint) {
				std::cerr<<"cut beta parent value "<<backpoint<<" current value "<<value_mark<<std::endl;
				break;
			} else if (player != o.mycolor && value_mark <= backpoint) {
				std::cerr<<"cut alpha parent value "<<backpoint<<" current value "<<value_mark<<std::endl;
				break;
			}
		}
		if (depth = DEPTH - 1) {
			next.first = step_mark.first;
			next.second = step_mark.second;
		}
		return value_mark;
	}
}

int othello_ai::evaluation(othello16 o_t, int player) {
	int total_black = o_t.count(BLACK);
	int total_white = o_t.count(WHITE);
	std::string map = o_t.tostring();
	int value = 0, i, j, num = 0, liberty_value = 0;
	
	if (total_black + total_white <= 64 ) {

		// 前期
		liberty_value += 4 * o_t.canmove(player) * ((player == o_t.mycolor) ? 1 : -1);
		// std::cerr<<" evaluation "<<value<<" ";
		//string2map(map);
		return value + liberty_value;
	} else {

		// 中期
		for (i = 0; i < MAXEDGE; i ++) {
			for (j = 0; j < MAXEDGE; j ++) {
				num = i * MAXEDGE + j;
				if (map[num] == BLANK + '0') {
					continue; 
				}
				value += ( ( o_t.mycolor == map[num] - '0' ) ? 1 : -1 ) * valuemap[i][j];
			}
		}
		liberty_value += 4 * o_t.canmove(player) * ((player == o_t.mycolor) ? 1 : -1);
		return value + liberty_value;
	}
}

void othello_ai::string2map(std::string str) {
	std::cerr<<std::endl;
	for (int i = 0; i < MAXEDGE; i ++) {
		for (int j = 0; j < MAXEDGE; j ++) {
			std::cerr<<str[i * 16 + j];
		}
		std::cerr<<std::endl;
	}
}

std::pair<int, int> othello_ai::seize_peak(std::vector< std::pair<int, int> > v) {
	std::vector< std::pair<int, int> >::iterator it;
	for (it = v.begin() ; it != v.end(); ++ it) {
		if ( ( (*it).first == 0 && (*it).second == 0 )
			|| ( (*it).first == 0 && (*it).second == MAXEDGE - 1 )
			|| ( (*it).first == MAXEDGE - 1 && (*it).second == 0 )
			|| ( (*it).first == MAXEDGE - 1 && (*it).second == MAXEDGE - 1 ) ) {
				std::cerr<<std::endl<<"seize peak ("<<(*it).first<<", "<<(*it).second<<")"<<std::endl;
				return std::pair<int, int> ((*it).first, (*it).second);
		}
	}
	return std::pair<int, int> (-1, -1);
}