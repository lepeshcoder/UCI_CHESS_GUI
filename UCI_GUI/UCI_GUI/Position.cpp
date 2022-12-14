#include "Position.h"

Position::Position()
{
	Pieces = new Bitboard * [2]{0};
	for (int i = 0; i < 2; i++)
		Pieces[i] = new Bitboard[6]{0};
}

Position::Position(const Position& other)
{
	ActiveColor = other.ActiveColor;
	AllBlackPeaces = other.AllBlackPeaces;
	AllWhitePeaces = other.AllWhitePeaces;
	BlackLongCastle = other.BlackLongCastle;
	BlackShortCastle = other.BlackShortCastle;
	FiftyMovesRuleCounter = other.FiftyMovesRuleCounter;
	MagicCaptureField = other.MagicCaptureField;
	MoveCounter = other.MoveCounter;
	Pieces = new Bitboard * [2];
	for (int i = 0; i < 2; i++)
	{
		Pieces[i] = new Bitboard[6];
		for (int j = 0; j < 6; j++)
			Pieces[i][j] = other.Pieces[i][j];
	}
	WhiteLongCastle = other.WhiteLongCastle;
	WhiteShortCastle = other.WhiteShortCastle;
	CurrMoves = other.CurrMoves;
}



void Position::MakeMove(Move move)
{
	++MoveCounter; // счетчик ходов увелчичваетсся

	// проверка на правило 50 ходов
	if ((move.PieceType) != PAWN || move.MoveType != CAPTURE
		|| move.MoveType != CAPTURE_TRANSFORM || move.MoveType != MAGIC_CAPTURE) ++FiftyMovesRuleCounter;
	else FiftyMovesRuleCounter = 0;

	this->MagicCaptureField = Bitboard(0); // убираем поле взятия на проходе

	switch (move.MoveType)
	{
	case NORMAL_MOVE:
		Pieces[ActiveColor][move.PieceType] &= ~(Bitboard(1) << move.From); // убираем фигуру с места где она стояла
		Pieces[ActiveColor][move.PieceType] |= (Bitboard(1) << move.To); // ставим фигуру туда куда она пошла
		if (ActiveColor == WHITE)
		{
			AllWhitePeaces &= ~(Bitboard(1) << move.From);
			AllWhitePeaces |= (Bitboard(1) << move.To);
			if (move.PieceType == KING)
				WhiteLongCastle = WhiteShortCastle = false;
		}
		else
		{		
			AllBlackPeaces &= ~(Bitboard(1) << move.From);
			AllBlackPeaces |= (Bitboard(1) << move.To);
			if (move.PieceType == KING)
				BlackLongCastle = BlackShortCastle = false;
		}
		
		break;
	case  CAPTURE:
		Pieces[ActiveColor][move.PieceType] &= ~(Bitboard(1) << move.From); // убираем фигуру с места где она стояла
		Pieces[ActiveColor][move.PieceType] |= (Bitboard(1) << move.To); // ставим фигуру туда куда она пошла
		if (ActiveColor == WHITE)
		{
			AllWhitePeaces &= ~(Bitboard(1) << move.From);
			AllWhitePeaces |= (Bitboard(1) << move.To);
			AllBlackPeaces &= ~(Bitboard(1) << move.To);
			Pieces[BLACK][move.OpPieceType] &= ~(Bitboard(1) << move.To);
			if (move.PieceType == KING)
				WhiteLongCastle = WhiteShortCastle = false;
		}
		else
		{
			AllBlackPeaces &= ~(Bitboard(1) << move.From);
			AllBlackPeaces |= (Bitboard(1) << move.To);
			AllWhitePeaces &= ~(Bitboard(1) << move.To);
			Pieces[WHITE][move.OpPieceType] &= ~(Bitboard(1) << move.To);
			if (move.PieceType == KING)
				BlackLongCastle = BlackShortCastle = false;
		}
		break;
	case FIRST_PAWN_MOVE:
		Pieces[ActiveColor][PAWN] &= ~(Bitboard(1) << move.From);
		Pieces[ActiveColor][PAWN] |= (Bitboard(1) << move.To);
		if (ActiveColor == WHITE)
		{
			MagicCaptureField = (Bitboard(1) << (move.To - 8));
			AllWhitePeaces &= ~(Bitboard(1) << move.From);
			AllWhitePeaces |= (Bitboard(1) << move.To);
		}
		else
		{
			MagicCaptureField = (Bitboard(1) << (move.To + 8));
			AllBlackPeaces &= ~(Bitboard(1) << move.From);
			AllBlackPeaces |= (Bitboard(1) << move.To);
		}
		break;
	case MAGIC_CAPTURE:
		Pieces[ActiveColor][PAWN] &= ~(Bitboard(1) << move.From);
		Pieces[ActiveColor][PAWN] |= (Bitboard(1) << move.To);
		if (ActiveColor == WHITE)
		{
			Pieces[BLACK][PAWN] &= ~(Bitboard(1) << (move.To - 8));
			AllBlackPeaces &= ~(Bitboard(1) << (move.To - 8));
			AllWhitePeaces &= ~(Bitboard(1) << move.From);
			AllWhitePeaces |= (Bitboard(1) << move.To);
		}
		else
		{
			Pieces[WHITE][PAWN] &= ~(Bitboard(1) << (move.To + 8));
			AllWhitePeaces &= ~(Bitboard(1) << (move.To + 8));
			AllBlackPeaces &= ~(Bitboard(1) << move.From);
			AllBlackPeaces |= (Bitboard(1) << move.To);
		}
		break;
	case LONG_CASTLE:
		if (ActiveColor == WHITE)
		{		
			WhiteLongCastle = false;
			WhiteShortCastle = false;
			Pieces[WHITE][KING] = 0X20;
			Pieces[WHITE][ROOK] &= ~(0X80);
			Pieces[WHITE][ROOK] |= 0X10;
			AllWhitePeaces &= ~(0X88); 
			AllWhitePeaces |= 0X30;
		}
		else
		{
			BlackLongCastle = false;
			BlackShortCastle = false;
			Pieces[BLACK][KING] = Bitboard(1) << 61;
			Pieces[BLACK][ROOK] &= ~(Bitboard(1) << 63);
			Pieces[BLACK][ROOK] |= (Bitboard(1) << 60);
			AllBlackPeaces &= ~0X8800000000000000;
			AllBlackPeaces |= 0X3000000000000000;
		}
		break;
	case SHORT_CASTLE:
		if (ActiveColor == WHITE)
		{
			WhiteLongCastle = false;
			WhiteShortCastle = false;
			Pieces[WHITE][KING] = 0X2;
			Pieces[WHITE][ROOK] &= ~0X1;
			Pieces[WHITE][ROOK] |= 0X4;
			AllWhitePeaces &= ~(0X9);
			AllWhitePeaces |= 0X6;
		}
		else
		{
			BlackLongCastle = false;
			BlackShortCastle = false;
			Pieces[BLACK][KING] = 0X0200000000000000;
			Pieces[BLACK][ROOK] &= ~(0X0100000000000000);
			Pieces[BLACK][ROOK] |= 0X0400000000000000;
			AllBlackPeaces &= ~0X0900000000000000;
			AllBlackPeaces |= 0X0600000000000000;
		}
		break;
	case TRANSFORM:
		if (ActiveColor == WHITE)
		{
			Pieces[WHITE][PAWN] &= ~(Bitboard(1) << move.From);
			Pieces[WHITE][move.OpPieceType] |= (Bitboard(1) << move.To);
			AllWhitePeaces &= ~(Bitboard(1) << move.From);
			AllWhitePeaces |= (Bitboard(1) << move.To);
		}
		else
		{
			Pieces[BLACK][PAWN] &= ~(Bitboard(1) << move.From);
			Pieces[BLACK][move.OpPieceType] |= (Bitboard(1) << move.To);
			AllBlackPeaces &= ~(Bitboard(1) << move.From);
			AllBlackPeaces |= (Bitboard(1) << move.To);
		}
		break;
	case CAPTURE_TRANSFORM:
		if (ActiveColor == WHITE)
		{
			Pieces[BLACK][move.OpPieceType] &= ~(Bitboard(1) << move.To);
			AllBlackPeaces &= ~(Bitboard(1) << move.To);
			Pieces[WHITE][PAWN] &= ~(Bitboard(1) << move.From);
			Pieces[WHITE][QUEEN] |= (Bitboard(1) << move.To);
			AllWhitePeaces &= ~(Bitboard(1) << move.From);
			AllWhitePeaces |= (Bitboard(1) << move.To);
		}
		else
		{
			Pieces[WHITE][move.OpPieceType] &= ~(Bitboard(1) << move.To);
			AllWhitePeaces &= ~(Bitboard(1) << move.To);
			Pieces[BLACK][PAWN] &= ~(Bitboard(1) << move.From);
			Pieces[BLACK][QUEEN] |= (Bitboard(1) << move.To);
			AllBlackPeaces &= ~(Bitboard(1) << move.From);
			AllBlackPeaces |= (Bitboard(1) << move.To);
		}
		break;
	}
	// смена активного цвета
	if (ActiveColor == WHITE) ActiveColor = BLACK;
	else ActiveColor = WHITE;


}

void Position::UnMakeMove(Move move)
{

}

void Position::InitPosition()
{
	InitPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

// rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2
void Position::InitPosition(string FEN)
{
	std::string dictionary = "KQRBNPkqrbnp";
	char arr[64];
	int i = 0, index = 0;
	while (FEN[i] != ' ')
	{
		if (FEN[i] == '/') i++;
		else
		{
			if (((FEN[i] - '0') <= 9) && ((FEN[i] - '0') >= 0))
			{
				for (int j = 0; j < FEN[i] - '0'; j++) arr[index++] = '0';
			}
			else
			{
				arr[index++] = FEN[i];
			}
			i++;
		}
	}
	for (int j = 0; j < dictionary.length(); j++)
	{
		Bitboard temp = 0;
		for (int k = 0; k < 64; k++)
		{
			if (arr[k] == dictionary[j])
			{
				temp |= (Bitboard(1) << (63 - k));
			}
		}
		Pieces[j / 6][j % 6] = temp;
	}
	
	ActiveColor = (FEN[++i] == 'w') ? PieceColors::WHITE : PieceColors::BLACK;
	i += 2;
	BlackShortCastle = BlackLongCastle = WhiteLongCastle = WhiteShortCastle = false;
	while (FEN[i] != ' ')
	{
		if (FEN[i] == 'K') WhiteShortCastle = true;
		else if (FEN[i] == 'k') BlackShortCastle = true;
		else if (FEN[i] == 'q') BlackLongCastle = true;
		else if (FEN[i] == 'Q') WhiteLongCastle = true;
		i++;
	}
	i++;
	int sq = 0;
	if (FEN[i] != '-')
	{
		sq = 'h' - FEN[i] + ((FEN[i + 1] - '0' - 1) << 3);
		MagicCaptureField = Bitboard(1) << sq;
		i++;
	}
	else MagicCaptureField = Bitboard(0);
	i += 2;
	this->FiftyMovesRuleCounter= FEN[i] - '0';
	i += 2;
	MoveCounter = FEN[i] - '0';
	this->AllBlackPeaces= AllWhitePeaces = Bitboard(0);
	for (int i = 0; i < 6; i++)
	{
		AllWhitePeaces |= Pieces[WHITE][i];
		AllBlackPeaces |= Pieces[BLACK][i];
	}
}

string Position::GetPositionFEN()
{
	// w KQkq - 0 1
	string FEN = " ";
	FEN += (ActiveColor == PieceColors::WHITE) ? "w " : "b ";
	if (WhiteShortCastle) FEN += 'K';
	if (WhiteLongCastle) FEN += 'Q';
	if (BlackShortCastle) FEN += 'k';
	if (BlackLongCastle) FEN += 'q';
	if (!WhiteShortCastle && !WhiteLongCastle && !BlackShortCastle && !BlackLongCastle) FEN += '-';
	FEN += ' ';
	if (MagicCaptureField)
	{
		DWORD index;
		BitScanReverse64(&index, MagicCaptureField);
		int rank = index / 8;
		int file = index % 8;
		FEN += 'h' - char(file);
		FEN += char(rank) + '1';
		FEN += ' ';
	}
	else FEN += "- ";
	std::stringstream a;
	a << FiftyMovesRuleCounter;
	string k; a >> k;
	FEN += '0' + ' ';
	std::stringstream b;
	b << MoveCounter;
	string l; b >> l;
	FEN += l;
	return FEN;
}

Position::~Position()
{
	for (int i = 0; i < 2; i++)
		delete[] Pieces[i];
	delete[] Pieces;
}

string Position::ViewBitboard(Bitboard a)
{
	string res;
		for (int i = 0; i < 64; i++)
		{
			if (a & 0x8000000000000000) res += "1 ";
			else res += "0 ";
			a <<= 1;
			if (((i + 1) & 7) == 0) res += '\n';
		}
	return res;
	
}
