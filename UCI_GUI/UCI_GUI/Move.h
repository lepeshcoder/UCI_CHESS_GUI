#pragma once
#include"framework.h"
class Move
{
public:
	// Откуда и куда пошла фигура доска нумеруется с левого верхнего угла
	int From;
	int To;
	// Тип фигуры
	int PieceType;
	// Тип хода
	int MoveType;
	int OpPieceType;
public:
	Move();

	Move(int from, int to, int piecetype, int movetype,int opPieceType);


	// Метод для общения с движком формат ( E2E4 )
	string ToString();


};

