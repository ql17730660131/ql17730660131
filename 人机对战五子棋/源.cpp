#include<iostream>
#include"ChessGame.h"

int main() {
	Man man;
	AI ai;
	Chess chess(13,30,30,40);
	ChessGame game(&man, &ai,&chess);

	game.play();

	return 0;
}