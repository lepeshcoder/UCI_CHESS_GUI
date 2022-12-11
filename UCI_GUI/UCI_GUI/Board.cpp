#include "Board.h"

Board::Board()
{
	this->board = new Cell[size];
	this->Attacks = Bitboard(0);
}


void Board::InitBoard(Position& pos)
{
	for (int color = PieceColors::WHITE; color <= PieceColors::BLACK; color++)
	{
		for (int piece = PieceTypes::KING; piece <= PieceTypes::PAWN; piece++)
		{
			Bitboard temp = pos.Pieces[color][piece];
			while (temp)
			{
				DWORD* index = new DWORD;
				BitScanForward64(index, temp);
				board[63 - *index].SetCell(piece, color);
				temp &= (temp - 1);
			}
		}
	}
}

void Board::UpdateBoard(Position& pos)
{
	for (int i = 0; i < 64; i++)
		board[i].PieceColor = board[i].PieceType = nullptr;
	
	for (int color = PieceColors::WHITE; color <= PieceColors::BLACK; color++)
	{
		for (int piece = PieceTypes::KING; piece <= PieceTypes::PAWN; piece++)
		{
			Bitboard temp = pos.Pieces[color][piece];
			while (temp)
			{
				DWORD* index = new DWORD;
				BitScanForward64(index, temp);
				board[63 - *index].SetCell(piece, color);
				temp &= (temp - 1);
			}
		}
	}
}

Cell* Board::GetCell(int index)
{
	return &board[index];
}

Cell* Board::GetBoard()
{
	return this->board;
}

string Board::GetPositionFEN()
{
	std::string dictionary = "KQRBNPkqrbnp";
	string FEN;
	for (int i = 0; i < 8; i++)
	{
		int counter = 0;
		for (int j = 0; j < 8; j++)
		{
			
			if (board[8 * i + j].PieceColor != nullptr)
			{
				if (counter > 0)
				{
					FEN += char(counter) + '0';
					counter = 0;
				}
				int index = (*board[8 * i + j].PieceColor) * 6 + *board[8 * i + j].PieceType;
				FEN += dictionary[index];
			}
			else ++counter;
			if (counter == 8) FEN += '8';
			else if (j == 7 && counter != 0) FEN += char(counter) + '0';
		}
		FEN += '/';
	}
	FEN.erase(FEN.end() - 1);
	return FEN;
}

void Board::SetAttacks(Bitboard attacks)
{
	this->Attacks = attacks;
}

Bitboard Board::GetAttacks()
{
	return this->Attacks;
}


Board::~Board()
{
	delete[] this->board;
}
