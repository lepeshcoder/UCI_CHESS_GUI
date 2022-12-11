#pragma once
#include "framework.h"
#include "Cell.h"
#include"Position.h"
class Board
{
private:
	Cell* board;
	const int size = 64;
	Bitboard Attacks;
public:
	
	Board();

	// Заполнение массива фигур по битбордам
	void InitBoard(Position& pos);

	// Обновление доски по модели
	void UpdateBoard(Position& pos);

	Cell* GetCell(int index);
	
	Cell* GetBoard(); 

	string GetPositionFEN();

	void SetAttacks(Bitboard attacks);

	Bitboard GetAttacks();

	~Board();

};

