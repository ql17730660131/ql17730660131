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
	//��ȡ�����Ϣ
	while (1)
	{
		msg = GetMouseMsg();
		//ͨ��chess�����ж��Ƿ���Ч
		if (msg.uMsg == WM_LBUTTONDOWN && chess->clickBoard(msg.x, msg.y, &pos))
		{
			std::cout << pos.row <<","<< pos.col<<std::endl;
			break;
		}
	}
	chess->chessDown(&pos, CHESS_BLACK);
}
