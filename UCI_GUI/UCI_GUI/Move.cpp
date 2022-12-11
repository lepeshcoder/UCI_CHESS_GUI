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
	string Files = "ABCDEFGH";
	string move = "";
	move += Files[From % 8];
	move += '8' - (char)(From / 8);
	move += Files[To % 8];
	move += '8' - char(To / 8);
	return move;
}
