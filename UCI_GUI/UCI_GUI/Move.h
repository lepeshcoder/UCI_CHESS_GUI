#pragma once
#include"framework.h"
class Move
{
public:
	// ������ � ���� ����� ������ ����� ���������� � ������ �������� ����
	int From;
	int To;
	// ��� ������
	int PieceType;
	// ��� ����
	int MoveType;
	int OpPieceType;
public:
	Move();

	Move(int from, int to, int piecetype, int movetype,int opPieceType);


	// ����� ��� ������� � ������� ������ ( E2E4 )
	string ToString();


};

