#include "Move.h"

Move::Move()
{
}

Move::Move(int from, int to, int piecetype, int movetype, int opPieceType)
{
	From = from;
	To = to;
	PieceType = piecetype;
	MoveType = movetype;
	OpPieceType = opPieceType;
}

string Move::ToString()
{
	string Files = "abcdefgh";
	string move = "";
	move += Files[7 - (From % 8)];
	move += '1' + (char)(From / 8);
	move += Files[7 - (To % 8)];
	move += '1' + (char)(To / 8);
	return move;
}
