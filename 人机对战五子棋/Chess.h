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
	CHESS_WHITE = -1,//����
	CHESS_BLACK = 1//����
}chess_kind_t;

class Chess
{ 
public:
	Chess(int gradeSize, int marginX, int marginY, float chessSize);
	void init();//��ʼ��

	//�ж�ָ�������Ƿ���Ч���
	bool clickBoard(int x, int y, ChessPos *pos);

	void chessDown(ChessPos* pos, chess_kind_t kind);//����

	int getGradeSize();//��ȡ���̵Ĵ�С(13,15,17,19)

	//��ȡָ��λ���Ǻ��廹�ǰ���
	int getChessData(int row,int col);
	int getChessData(ChessPos *pos);

	bool checkOver();//�������Ƿ����
private:
	IMAGE chessBlackImg;//����
	IMAGE chessWhiteImg;//����

	int grateSize;//���̵Ĵ�С��13��15��17��19��
	int margin_x;//���̵����߽�
	int margin_y;//���̵Ķ����߽�
	float chessSize;//���ӵĴ�С

	//�洢��ǰ��ֵ����ӷ������� 
	//chessMap[3][5]��ʾ���̵ĵ����е����е�������� 0���հ�  -1������ 1������
	std::vector<std::vector<int>> chessMap;

	//��ʾ���ڸ�˭����
	bool playerFlag=false;//

	void updateGameMap(ChessPos *pos);

	ChessPos lastPos; //�������λ��, Chess��private���ݳ�Ա
	bool checkWin();//��ʤ���ѷַ���true
};

