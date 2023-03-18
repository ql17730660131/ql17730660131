#include "Man.h"
#include<iostream>
void Man::init(Chess * chess)
{
	this->chess = chess;
}

void Man::go()
{
	MOUSEMSG msg;
	ChessPos pos;
	//获取鼠标信息
	while (1)
	{
		msg = GetMouseMsg();
		//通过chess对象判断是否有效
		if (msg.uMsg == WM_LBUTTONDOWN && chess->clickBoard(msg.x, msg.y, &pos))
		{
			std::cout << pos.row <<","<< pos.col<<std::endl;
			break;
		}
	}
	chess->chessDown(&pos, CHESS_BLACK);
}
