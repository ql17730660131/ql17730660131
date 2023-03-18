#pragma once
#include "Chess.h"
class AI
{
public:
	void init(Chess *chess);
	void go();
private:
	Chess *chess;
	std::vector<std::vector<int>> scoreMap;
private:
	void calculateScore();
	ChessPos think();//根据评分进行思考返回下一步落子位置
};

