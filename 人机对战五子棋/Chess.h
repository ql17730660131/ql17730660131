#pragma once
#include<graphics.h>
#include<vector>

struct ChessPos
{
	int row;
	int col;

	ChessPos(int r=0,int c=0):row(r),col(c){}
};

typedef enum {
	CHESS_WHITE = -1,//白棋
	CHESS_BLACK = 1//黑棋
}chess_kind_t;

class Chess
{ 
public:
	Chess(int gradeSize, int marginX, int marginY, float chessSize);
	void init();//初始化

	//判断指定坐标是否有效点击
	bool clickBoard(int x, int y, ChessPos *pos);

	void chessDown(ChessPos* pos, chess_kind_t kind);//落子

	int getGradeSize();//获取棋盘的大小(13,15,17,19)

	//获取指定位置是黑棋还是白棋
	int getChessData(int row,int col);
	int getChessData(ChessPos *pos);

	bool checkOver();//检查棋局是否结束
private:
	IMAGE chessBlackImg;//黑子
	IMAGE chessWhiteImg;//白子

	int grateSize;//棋盘的大小（13，15，17，19）
	int margin_x;//棋盘的左侧边界
	int margin_y;//棋盘的顶部边界
	float chessSize;//棋子的大小

	//存储当前棋局的棋子分配数据 
	//chessMap[3][5]表示棋盘的第三行第五列的落子情况 0：空白  -1：白子 1：黑子
	std::vector<std::vector<int>> chessMap;

	//表示现在该谁下子
	bool playerFlag=false;//

	void updateGameMap(ChessPos *pos);

	ChessPos lastPos; //最近落子位置, Chess的private数据成员
	bool checkWin();//若胜负已分返回true
};

