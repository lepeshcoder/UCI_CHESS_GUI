#pragma once
#include "framework.h"
class Resources
{
public:
	Gdiplus::Image* PiecesImages[2][6];
	Gdiplus::Image* BoardImage;
	Gdiplus::Image* MoveImage;
	Gdiplus::Image* WhiteCellImage;
	Gdiplus::Image* BlackCellImage;
	int CellSize;
	int BoardSize;
	int PieceSize;

public:

	Resources();

};

