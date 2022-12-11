#include "Cell.h"

Cell::Cell()
{
	PieceColor = nullptr;
	PieceType = nullptr;
}

void Cell::SetCell(int pieceType, int pieceColor)
{
	this->PieceType = new int;
	*PieceType = pieceType;
	this->PieceColor = new int;
	*PieceColor = pieceColor;
}

Cell::~Cell()
{
	if (PieceColor) delete PieceColor;
	if (PieceType) delete PieceType;
}
