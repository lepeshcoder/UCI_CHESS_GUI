#pragma once
#include"framework.h"
#include"Move.h"
class Position
{
public:
	Bitboard** Pieces;
	Bitboard AllWhitePeaces;
	Bitboard AllBlackPeaces;
	Bitboard MagicCaptureField;
	int ActiveColor;
	bool WhiteShortCastle;
	bool WhiteLongCastle;
	bool BlackShortCastle;
	bool BlackLongCastle;
	unsigned int MoveCounter;
	unsigned int FiftyMovesRuleCounter;

public:
	// ����������� �� ���������
	Position();

	Position(const Position& other);

	// ������� ���
	void MakeMove(Move move);

	void UnMakeMove(Move move);

	// ���������������� ��������� �������
	void InitPosition();

	// ���������������� ������� �� FEN
	void InitPosition(string FEN);

	string GetPositionFEN();

	// ����������
	~Position();

	// ������� ������� � �������� ����
	static string ViewBitboard(Bitboard a);
	

};

