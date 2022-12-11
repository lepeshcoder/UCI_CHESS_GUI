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

	// ���������� ������� ����� �� ���������
	void InitBoard(Position& pos);

	// ���������� ����� �� ������
	void UpdateBoard(Position& pos);

	Cell* GetCell(int index);
	
	Cell* GetBoard(); 

	string GetPositionFEN();

	void SetAttacks(Bitboard attacks);

	Bitboard GetAttacks();

	~Board();

};

