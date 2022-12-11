#pragma once
#include "framework.h"
class Cell
{
public:
	int* PieceType;
	int* PieceColor;

public:
	Cell();

	void SetCell(int pieceType, int pieceColor);
	
	~Cell();
};

