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
	// Конструктор по умолчанию
	Position();

	Position(const Position& other);

	// Сделать ход
	void MakeMove(Move move);

	void UnMakeMove(Move move);

	// Инициализировать начальную позицию
	void InitPosition();

	// Инициализировать позицию по FEN
	void InitPosition(string FEN);

	string GetPositionFEN();

	// Деструктор
	~Position();

	// выводит битборд в нативном виде
	static string ViewBitboard(Bitboard a);
	

};

