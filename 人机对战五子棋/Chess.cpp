#include "Chess.h"
#include <mmsystem.h>
#include <math.h>
#include <conio.h>
//自定义棋子背景渲染接口（否则使用默认的棋子边缘不透明）
void putimagePNG(int x, int y, IMAGE* picture) //x为载入图片的X坐标，y为Y坐标
{
	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + x) + (iy + y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}
#pragma comment(lib,"winmm.lib")//提供音乐播放
Chess::Chess(int gradeSize, int marginX, int marginY, float chessSize)
{
	this->grateSize = gradeSize;
	this->margin_x = marginX;
	this->margin_y = marginY;
	this->chessSize = chessSize;
	playerFlag = CHESS_BLACK;
	for (int i = 0; i < gradeSize; i++)
	{
		std::vector<int> row;
		for (int j = 0; j < gradeSize; j++) {
			row.push_back(0);
		}
		chessMap.push_back(row);
	}
}

void Chess::init()
{
	//创建游戏窗口
	initgraph(545, 542,EW_SHOWCONSOLE);
	//显示棋盘文件
	loadimage(0, "res/棋盘.jpg", 545, 542);
	//播放开始提示音
	mciSendString("play res/start.wav", 0, 0, 0);
	//加载黑子和白子的图片
	loadimage(&chessBlackImg, "res/black.png",chessSize,chessSize,true);
	loadimage(&chessWhiteImg, "res/white.png",chessSize,chessSize,true);

	//棋盘清零
	for(int i=0;i< grateSize;i++)
		for (int j = 0; j < grateSize; j++)
		{
			chessMap[i][j] = 0;
		}
	playerFlag = true;
}

bool Chess::clickBoard(int x, int y, ChessPos * pos)
{
	int row = (y - margin_y) / chessSize;
	int col = (x - margin_x) / chessSize;
	double offset = chessSize * 0.4;

	bool ret = false;

	double len;
	do
	{
		int leftTopPosX = margin_x + chessSize * col;
		int leftTopPosY = margin_y + chessSize * row;
		len = sqrt((x - leftTopPosX)*(x - leftTopPosX) + (y - leftTopPosY)*(y - leftTopPosY));
		//左上角的判断
		if (len < offset)
		{
			pos->row = row;
			pos->col = col;
			if (chessMap[pos->row][pos->col] == 0)
			{
				ret = true;
			}
			break;
		}
		//右上角的判断
		int rightTopPosX = margin_x + chessSize * (col + 1);
		int rightTopPosY = margin_y + chessSize * row;
		len = sqrt((x - rightTopPosX)*(x - rightTopPosX) + (y - rightTopPosY)*(y - rightTopPosY));
		if (len < offset)
		{
			pos->row = row;
			pos->col = col+1;
			if (chessMap[pos->row][pos->col] == 0)
			{
				ret = true;
			}
			break;
		}
		//左下角的判断
		int leftBotPosX = margin_x + chessSize * col;
		int leftBotPosY = margin_y + chessSize * (row+1);
		len = sqrt((x - leftBotPosX)*(x - leftBotPosX) + (y - leftBotPosY)*(y - leftBotPosY));
		//左下角的判断
		if (len < offset)
		{
			pos->row = row+1;
			pos->col = col;
			if (chessMap[pos->row][pos->col] == 0)
			{
				ret = true;
			}
			break;
		}
		//右下角的判断
		int rightBotPosX = margin_x + chessSize * (col + 1);
		int rightBotPosY = margin_y + chessSize * (row + 1);
		len = sqrt((x - rightBotPosX)*(x - rightBotPosX) + (y - rightBotPosY)*(y - rightBotPosY));
		if (len < offset)
		{
			pos->row = row + 1;
			pos->col = col + 1;
			if (chessMap[pos->row][pos->col] == 0)
			{
				ret = true;
			}
			break;
		}

	} while (0);
	
	return ret;
}

void Chess::chessDown(ChessPos * pos, chess_kind_t kind)
{
	mciSendString("play res/down7.WAV", 0, 0, 0);

	int x = margin_x + chessSize * pos->col - 0.5*chessSize;
	int y = margin_y + chessSize * pos->row - 0.5*chessSize;
	if (kind == CHESS_WHITE)
	{
		putimagePNG(x, y, &chessWhiteImg);
	}
	else if (kind == CHESS_BLACK)
	{
		putimagePNG(x, y, &chessBlackImg);
	}
	updateGameMap(pos);
}
int Chess::getGradeSize()
{
	return grateSize;
}

int Chess::getChessData(int row, int col)
{
	return chessMap[row][col];
}

int Chess::getChessData(ChessPos * pos)
{
	return chessMap[pos->row][pos->col];
}

bool Chess::checkOver()
{
		if (checkWin()) {
			Sleep(1500);
			if (playerFlag == false) {  //黑棋赢（玩家赢）,此时标记已经反转，轮到白棋落子
				mciSendString("play res/不错.mp3", 0, 0, 0);
				loadimage(0, "res/胜利.jpg",545, 542);
			}
			else {
				mciSendString("play res/失败.mp3", 0, 0, 0);
				loadimage(0, "res/失败.jpg", 545, 542);
			}

			_getch();  //暂停
			return true;
		}

		return false;
}

void Chess::updateGameMap(ChessPos * pos)
{
	lastPos = *pos;
	chessMap[pos->row][pos->col] = playerFlag?CHESS_BLACK:CHESS_WHITE;
	playerFlag = !playerFlag;
}

bool Chess::checkWin()
{
	// 横竖斜四种大情况，每种情况都根据当前落子往后遍历5个棋子，有一种符合就算赢
	// 水平方向
	int row = lastPos.row;
	int col = lastPos.col;

	for (int i = 0; i < 5; i++)
	{
		// 往左5个，往右匹配4个子，20种情况
		if (col - i >= 0 && 
			col - i + 4 < grateSize &&
			chessMap[row][col - i] == chessMap[row][col - i + 1] &&
			chessMap[row][col - i] == chessMap[row][col - i + 2] &&
			chessMap[row][col - i] == chessMap[row][col - i + 3] &&
			chessMap[row][col - i] == chessMap[row][col - i + 4])
			return true;
	}

	// 竖直方向(上下延伸4个)
	for (int i = 0; i < 5; i++)
	{
		if (row - i >= 0 &&
			row - i + 4 < grateSize &&
			chessMap[row - i][col] == chessMap[row - i + 1][col] &&
			chessMap[row - i][col] == chessMap[row - i + 2][col] &&
			chessMap[row - i][col] == chessMap[row - i + 3][col] &&
			chessMap[row - i][col] == chessMap[row - i + 4][col])
			return true;
	}

	// “/"方向
	for (int i = 0; i < 5; i++)
	{
		if (row + i < grateSize &&
			row + i - 4 >= 0 &&
			col - i >= 0 &&
			col - i + 4 < grateSize &&
			// 第[row+i]行，第[col-i]的棋子，与右上方连续4个棋子都相同
			chessMap[row + i][col - i] == chessMap[row + i - 1][col - i + 1] &&
			chessMap[row + i][col - i] == chessMap[row + i - 2][col - i + 2] &&
			chessMap[row + i][col - i] == chessMap[row + i - 3][col - i + 3] &&
			chessMap[row + i][col - i] == chessMap[row + i - 4][col - i + 4])
			return true;
	}

	// “\“ 方向
	for (int i = 0; i < 5; i++)
	{
		// 第[row+i]行，第[col-i]的棋子，与右下方连续4个棋子都相同
		if (row - i >= 0 &&
			row - i + 4 < grateSize &&
			col - i >= 0 &&
			col - i + 4 < grateSize &&
			chessMap[row - i][col - i] == chessMap[row - i + 1][col - i + 1] &&
			chessMap[row - i][col - i] == chessMap[row - i + 2][col - i + 2] &&
			chessMap[row - i][col - i] == chessMap[row - i + 3][col - i + 3] &&
			chessMap[row - i][col - i] == chessMap[row - i + 4][col - i + 4])
			return true;
	}

	return false;
}