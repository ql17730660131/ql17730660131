#include "ChessGame.h"
ChessGame::ChessGame(Man * man, AI * ai, Chess * chess)
{
	this->man = man;
	this->ai = ai;
	this->chess = chess;
	man->init(chess);
	this->ai->init(chess);
}
//对局开始五子棋游戏
void ChessGame::play()
{
	chess->init();
	while (1)
	{
		//先由棋手落子
		man->go();
		if (chess->checkOver())
		{
			chess->init();
			continue;
		}
	
		ai->go();
		if (chess->checkOver())
		{
			chess->init();
			continue;
		}

	}
}
