#include "MoveGen.h"

MoveGen::MoveGen()
{
	Rays = new Bitboard * [8];
	for (int i = 0; i < 8; i++)
	{
		Rays[i] = new Bitboard[64];
	}
	InitRays();

	InitRookMasks();
	InitBishopMasks();
	InitQueenMasks();

	BishopCache = new Bitboard * [64];
	for (int i = 0; i < 64; i++)
	{
		BishopCache[i] = new Bitboard[Bitboard(1) << bishop_bits[i]];
	}
	RookCache = new Bitboard * [64];
	for (int i = 0; i < 64; i++)
	{
		RookCache[i] = new Bitboard[Bitboard(1) << rook_bits[i]];
	}
	QueenCache = new Bitboard * [64];
	for (int i = 0; i < 64; i++)
	{
		QueenCache[i] = new Bitboard[Bitboard(1) << queen_bits[i]];
	}
	KingCache = new Bitboard[64];
	KnightCache = new Bitboard[64];
	WPawnCache = new Bitboard[64];
	BPawnCache = new Bitboard[64];
	WPawnMoveCache = new Bitboard[64];
	BPawnMoveCache = new Bitboard[64];
	InitPawnMoveCache();
	InitPawnCache();
	InitKnightCache();
	InitKingCache();
	InitBishopCache();
	InitRookCache();
	InitQueenCache();
}


Bitboard MoveGen::calc_pawn_moves(PieceColors color, Bitboard pawns)
{
	switch (color)
	{
	case WHITE:
		return ((pawns << 8) | ((pawns & RANK_2) << 16));
	case BLACK:
		return ((pawns >> 8) | ((pawns & RANK_7) >> 16));
	}
}

Bitboard MoveGen::calc_pawn_captures(PieceColors color, Bitboard pawns)
{
	switch (color)
	{
	case WHITE:
		return (((pawns << 9) & ~FILE_H) | ((pawns << 7) & ~FILE_A));
	case BLACK:
		return (((pawns >> 9) & ~FILE_A) | ((pawns >> 7) & ~FILE_H));
	}
}

Bitboard MoveGen::calc_knight_attack(Bitboard knights)
{
	Bitboard r1 = ((knights >> 1) & ~FILE_A);
	Bitboard l1 = ((knights << 1) & ~FILE_H);
	Bitboard r2 = ((knights >> 2) & ~(FILE_A | FILE_B));
	Bitboard l2 = ((knights << 2) & ~(FILE_H | FILE_G));
	return (((l1 | r1) << 16) | ((l1 | r1) >> 16) | ((l2 | r2) << 8) | ((l2 | r2) >> 8));
}

Bitboard MoveGen::calc_king_attack(Bitboard king)
{
	return (((king << 7) | (king >> 9) | (king >> 1)) & ~FILE_A) |
		(((king << 9) | (king >> 7) | (king << 1)) & ~FILE_H) |
		((king >> 8) | (king << 8));
} 

Bitboard MoveGen::get_rook_attack(int sq, Bitboard blockers)
{
	blockers &= RookMasks[sq];
	Bitboard key = _pext_u64(blockers, RookMasks[sq]);
	return RookCache[sq][key];
}

Bitboard MoveGen::get_bishop_attack(int sq, Bitboard blockers)
{
	blockers &= BishopMasks[sq];
	Bitboard key = _pext_u64(blockers, BishopMasks[sq]);
	return BishopCache[sq][key];
}

Bitboard MoveGen::get_queen_attack(int sq, Bitboard blockers)
{
	blockers &= QueenMasks[sq];
	Bitboard key = _pext_u64(blockers, QueenMasks[sq]);
	return QueenCache[sq][key];
}

Bitboard MoveGen::get_knight_attack(int sq)
{
	return KnightCache[sq];
}

Bitboard MoveGen::get_king_attack(int sq)
{
	return KingCache[sq];
}

Bitboard MoveGen::get_pawn_attack(int sq, PieceColors color)
{
	if (color == WHITE) return WPawnCache[sq];
	else return BPawnCache[sq];
}

Bitboard MoveGen::get_pawn_moves(int sq, PieceColors color)
{
	if (color == WHITE) return WPawnMoveCache[sq];
	else return BPawnMoveCache[sq];
}


void MoveGen::InitRays()
{
	for (int i = 0; i < 64; i++)
	{
		Rays[NORTH][i] = (FILE_H & ~RANK_1) << i;
		Rays[SOUTH][i] = (FILE_A & ~RANK_8) >> (63 - i);
		Rays[EAST][i] = (RANK_1 >> (8 - (i & 7))) << ((i >> 3) << 3);
		Rays[WEST][i] = ((RANK_1 << ((i & 7) + 1)) & ~RANK_2) << ((i >> 3) << 3);

		Rays[NORTH_WEST][i] = west_n(DIAG_A8H1 & ~RANK_1, col(i)) << (row(i) << 3);
		Rays[NORTH_EAST][i] = east_n(DIAG_A1H8 & ~RANK_1, 7 - col(i)) << (row(i) << 3);
		Rays[SOUTH_EAST][i] = east_n(DIAG_A8H1 & ~RANK_8, 7 - col(i)) >> ((7 - row(i)) << 3);
		Rays[SOUTH_WEST][i] = west_n(DIAG_A1H8 & ~RANK_8, col(i)) >> ((7 - row(i)) << 3);
	}
}

Bitboard MoveGen::calc_rook_attacks(int sq, Bitboard blockers)
{
	Bitboard attacks = 0;
	attacks |= Rays[NORTH][sq];
	if (Rays[NORTH][sq] & blockers)
	{
		attacks &= ~(Rays[NORTH][MyBitScanReverse(Rays[NORTH][sq] & blockers)]);
	}
	attacks |= Rays[SOUTH][sq];
	if (Rays[SOUTH][sq] & blockers)
	{
		attacks &= ~(Rays[SOUTH][MyBitScanForward(Rays[SOUTH][sq] & blockers)]);
	}
	attacks |= Rays[EAST][sq];
	if (Rays[EAST][sq] & blockers)
	{
		attacks &= ~(Rays[EAST][MyBitScanForward(Rays[EAST][sq] & blockers)]);
	}
	attacks |= Rays[WEST][sq];
	if (Rays[WEST][sq] & blockers)
	{
		attacks &= ~(Rays[WEST][MyBitScanReverse(Rays[WEST][sq] & blockers)]);
	}
	return attacks;
}

Bitboard MoveGen::calc_bishop_attacks(int sq, Bitboard blockers)
{
	Bitboard attacks = 0;
	attacks |= Rays[NORTH_EAST][sq];
	if (Rays[NORTH_EAST][sq] & blockers)
	{
		attacks &= ~(Rays[NORTH_EAST][MyBitScanReverse(Rays[NORTH_EAST][sq] & blockers)]);
	}
	attacks |= Rays[NORTH_WEST][sq];
	if (Rays[NORTH_WEST][sq] & blockers)
	{
		attacks &= ~(Rays[NORTH_WEST][MyBitScanReverse(Rays[NORTH_WEST][sq] & blockers)]);
	}
	attacks |= Rays[SOUTH_EAST][sq];
	if (Rays[SOUTH_EAST][sq] & blockers)
	{
		attacks &= ~(Rays[SOUTH_EAST][MyBitScanForward(Rays[SOUTH_EAST][sq] & blockers)]);
	}
	attacks |= Rays[SOUTH_WEST][sq];
	if (Rays[SOUTH_WEST][sq] & blockers)
	{
		attacks &= ~(Rays[SOUTH_WEST][MyBitScanForward(Rays[SOUTH_WEST][sq] & blockers)]);
	}
	return attacks;
}

Bitboard MoveGen::calc_queen_attacks(int sq, Bitboard blockers)
{
	return (calc_bishop_attacks(sq, blockers) | calc_rook_attacks(sq, blockers));
}

void MoveGen::InitRookMasks()
{
	for (int i = 0; i < 64; i++)
	{
		RookMasks[i] = 0;
		RookMasks[i] |= (Rays[NORTH][i] & (~RANK_8));
		RookMasks[i] |= (Rays[SOUTH][i] & (~RANK_1));
		RookMasks[i] |= (Rays[EAST][i] & (~FILE_H));
		RookMasks[i] |= (Rays[WEST][i] & (~FILE_A));
	}
}

void MoveGen::InitBishopMasks()
{
	Bitboard EDGES = RANK_1 | RANK_8 | FILE_A | FILE_H;
	for (int i = 0; i < 64; i++)
	{
		BishopMasks[i] = 0;
		BishopMasks[i] |= (Rays[NORTH_EAST][i]);
		BishopMasks[i] |= (Rays[NORTH_WEST][i]);
		BishopMasks[i] |= (Rays[SOUTH_EAST][i]);
		BishopMasks[i] |= (Rays[SOUTH_WEST][i]);
		BishopMasks[i] &= ~EDGES;
	}
}

void MoveGen::InitQueenMasks()
{
	for (int i = 0; i < 64; i++) QueenMasks[i] = RookMasks[i] | BishopMasks[i];
}

void MoveGen::InitRookCache()
{
	for (int sq = 0; sq < 64; sq++)
	{
		for (int blocker_index = 0; blocker_index < (1 << rook_bits[sq]); blocker_index++)
		{
			Bitboard blockers = _pdep_u64(blocker_index, RookMasks[sq]);
			Bitboard key = _pext_u64(blockers, RookMasks[sq]);
			RookCache[sq][key] = calc_rook_attacks(sq, blockers);
		}
	}
}

void MoveGen::InitBishopCache()
{
	for (int sq = 0; sq < 64; sq++)
	{
		for (int blocker_index = 0; blocker_index < (1 << bishop_bits[sq]); blocker_index++)
		{
			Bitboard blockers = _pdep_u64(blocker_index, BishopMasks[sq]);
			Bitboard key = _pext_u64(blockers, BishopMasks[sq]);
			BishopCache[sq][key] = calc_bishop_attacks(sq, blockers);
		}
	}
}

void MoveGen::InitQueenCache()
{
	for (int sq = 0; sq < 64; sq++)
	{
		for (int blocker_index = 0; blocker_index < (1 << queen_bits[sq]); blocker_index++)
		{
			Bitboard blockers = _pdep_u64(blocker_index, QueenMasks[sq]);
			Bitboard key = _pext_u64(blockers, QueenMasks[sq]);
			QueenCache[sq][key] = calc_queen_attacks(sq, blockers);
		}
	}
}

void MoveGen::InitKingCache()
{
	for (int sq = 0; sq < 64; sq++)
	{
		KingCache[sq] = calc_king_attack(Bitboard(1) << sq);
	}
}

void MoveGen::InitKnightCache()
{
	for (int sq = 0; sq < 64; sq++)
	{
		KnightCache[sq] = calc_knight_attack(Bitboard(1) << sq);
	}
}

void MoveGen::InitPawnCache()
{
	for (int i = 0; i < 64; i++)
	{
		WPawnCache[i] = calc_pawn_captures(WHITE, (Bitboard(1) << i));
		BPawnCache[i] = calc_pawn_captures(BLACK, (Bitboard(1) << i));
	}
}

void MoveGen::InitPawnMoveCache()
{
	for (int sq = 0; sq < 64; sq++)
	{
		WPawnMoveCache[sq] = calc_pawn_moves(WHITE, Bitboard(1) << sq);
		BPawnMoveCache[sq] = calc_pawn_moves(BLACK, Bitboard(1) << sq);
	}
}

bool MoveGen::is_under_attack(int sq, const Position& position, int color)
{
	Bitboard square = Bitboard(1) << sq;
	Bitboard all_peaces = position.AllBlackPeaces| position.AllWhitePeaces;
	if (color == WHITE)
	{
		Bitboard queens = position.Pieces[BLACK][QUEEN];
		while (queens)
		{
			int queen_sq = MyBitScanReverse(queens);
			Bitboard queen_attack = get_queen_attack(queen_sq, all_peaces);
			if (queen_attack & square) return true;
			queens &= (queens - 1);
		}
		Bitboard rooks = position.Pieces[BLACK][ROOK];
		while (rooks)
		{
			int rook_sq = MyBitScanReverse(rooks);
			Bitboard rook_attack = get_rook_attack(rook_sq, all_peaces);
			if (rook_attack & square) return true;
			rooks &= (rooks - 1);
		}
		Bitboard bishops = position.Pieces[BLACK][BISHOP];
		while (bishops)
		{
			int bishop_sq = MyBitScanReverse(bishops);
			Bitboard bishop_attack = get_bishop_attack(bishop_sq, all_peaces);
			if (bishop_attack & square) return true;
			bishops &= (bishops - 1);
		}
		Bitboard knights = position.Pieces[BLACK][KNIGHT];
		Bitboard knights_capture = calc_knight_attack(knights);
		if (knights_capture & square) return true;
		Bitboard pawns = position.Pieces[BLACK][PAWN];
		Bitboard pawns_captures = calc_pawn_captures(BLACK, pawns);
		if (pawns_captures & square) return true;
	}
	else
	{
		Bitboard queens = position.Pieces[WHITE][QUEEN];
		while (queens)
		{
			int queen_sq = MyBitScanReverse(queens);
			Bitboard queen_attack = get_queen_attack(queen_sq, all_peaces);
			if (queen_attack & square) return true;
			queens &= (queens - 1);
		}
		Bitboard rooks = position.Pieces[WHITE][ROOK];
		while (rooks)
		{
			int rook_sq = MyBitScanReverse(rooks);
			Bitboard rook_attack = get_rook_attack(rook_sq, all_peaces);
			if (rook_attack & square) return true;
			rooks &= (rooks - 1);
		}
		Bitboard bishops = position.Pieces[WHITE][BISHOP];
		while (bishops)
		{
			int bishop_sq = MyBitScanReverse(bishops);
			Bitboard bishop_attack = get_bishop_attack(bishop_sq, all_peaces);
			if (bishop_attack & square) return true;
			bishops &= (bishops - 1);
		}
		Bitboard knights = position.Pieces[WHITE][KNIGHT];
		Bitboard knights_capture = calc_knight_attack(knights);
		if (knights_capture & square) return true;
		Bitboard pawns = position.Pieces[WHITE][PAWN];
		Bitboard pawns_captures = calc_pawn_captures(WHITE, pawns);
		if (pawns_captures & square) return true;
	}
	return false;
}

bool MoveGen::IsKingUnderAttack(const Position& position, int color)
{
	return is_under_attack(MyBitScanReverse(position.Pieces[color][KING]), position, color);
}

bool MoveGen::IsCastlePossible(const Position& position, int color, int CastleType)
{
	Bitboard all_peaces = position.AllBlackPeaces| position.AllWhitePeaces;
	switch (CastleType)
	{
	case SHORT_CASTLE:
		if (color == WHITE)
		{
			if (position.WhiteShortCastle)
			{
				if (!(all_peaces & WHITE_SHORT_CASTLE & ~position.Pieces[WHITE][KING]))
				{
					for (int i = 1; i <= 3; i++)
						if (is_under_attack(i, position, WHITE)) return false;
				}
				else return false;
			}
			else return false;
		}
		else
		{
			if (position.BlackShortCastle)
			{
				if (!(all_peaces & BLACK_SHORT_CASTLE & ~position.Pieces[BLACK][KING]))
				{
					for (int i = 57; i <= 59; ++i)
						if (is_under_attack(i, position, BLACK)) return false;
				}
				else return false;
			}
			else return false;
		}
		break;
	case LONG_CASTLE:
		if (color == WHITE)
		{
			if (position.WhiteLongCastle)
			{
				if (!(all_peaces & WHITE_LONG_CASTLE & ~position.Pieces[WHITE][KING]))
				{
					for (int i = 3; i <= 5; ++i)
						if (is_under_attack(i, position, WHITE)) return false;
				}
				else return false;
			}
			else return false;
		}
		else
		{
			if (position.BlackLongCastle)
			{
				if (!(all_peaces & BLACK_LONG_CASTLE & ~position.Pieces[BLACK][KING]))
				{
					for (int i = 59; i <= 61; ++i)
						if (is_under_attack(i, position, BLACK)) return false;
				}
				else return false;
			}
			else return false;
		}
		break;
	}
	return true;
}

std::vector<Move> MoveGen::GetAllMoves(const Position& position, int PieceColor)
{
	std::vector<Move> moves;
	Bitboard AllPieces = position.AllBlackPeaces | position.AllWhitePeaces;
	if (position.ActiveColor == WHITE)
	{
		Bitboard Queens = position.Pieces[WHITE][QUEEN];
		while (Queens)
		{
			int queen = MyBitScanReverse(Queens);
			Bitboard UnLegalMoves = get_queen_attack(queen, AllPieces) & ~position.AllWhitePeaces;
			string kal = Position::ViewBitboard(UnLegalMoves);
			while (UnLegalMoves)
			{
				int to = MyBitScanReverse(UnLegalMoves);
				Move move(queen, to, QUEEN, NORMAL_MOVE,KING);
				Bitboard NewQueenPosition = Bitboard(1) << to;
				string kal1 = Position::ViewBitboard(NewQueenPosition);
				for (int i = 0; i < 6; i++)
				{
					if (position.Pieces[BLACK][i] & NewQueenPosition)
					{
						move.OpPieceType = i;
						move.MoveType = CAPTURE;
					}
				}
				Position NewPos(position);
				NewPos.MakeMove(move);
				if (!IsKingUnderAttack(NewPos, WHITE))
				{
					moves.push_back(move);
				}
				UnLegalMoves &= UnLegalMoves - 1;
			}
			Queens &= Queens - 1;
		}
		Bitboard Rooks = position.Pieces[WHITE][ROOK];
		while (Rooks)
		{
			int rook = MyBitScanReverse(Rooks);
			Bitboard UnLegalMoves = get_rook_attack(rook, AllPieces) & ~position.AllWhitePeaces;
			string kal = Position::ViewBitboard(UnLegalMoves);
			while (UnLegalMoves)
			{
				int to = MyBitScanReverse(UnLegalMoves);
				Move move(rook, to, ROOK, NORMAL_MOVE, KING);
				Bitboard NewRookPosition = Bitboard(1) << to;
				string kal1 = Position::ViewBitboard(NewRookPosition);
				for (int i = 0; i < 6; i++)
				{
					if (position.Pieces[BLACK][i] & NewRookPosition)
					{
						move.OpPieceType = i;
						move.MoveType = CAPTURE;
					}
				}
				Position NewPos(position);
				NewPos.MakeMove(move);
				if (!IsKingUnderAttack(NewPos, WHITE))
				{
					moves.push_back(move);
				}
				UnLegalMoves &= UnLegalMoves - 1;
			}
			Rooks &= Rooks - 1;
		}
		Bitboard Bishops = position.Pieces[WHITE][BISHOP];
		while (Bishops)
		{
			int bishop = MyBitScanReverse(Bishops);
			Bitboard UnLegalMoves = get_bishop_attack(bishop, AllPieces) & ~position.AllWhitePeaces;
			string kal = Position::ViewBitboard(UnLegalMoves);
			while (UnLegalMoves)
			{
				int to = MyBitScanReverse(UnLegalMoves);
				Move move(bishop, to, BISHOP, NORMAL_MOVE, KING);
				Bitboard NewBishopPosition = Bitboard(1) << to;
				string kal1 = Position::ViewBitboard(NewBishopPosition);
				for (int i = 0; i < 6; i++)
				{
					if (position.Pieces[BLACK][i] & NewBishopPosition)
					{
						move.OpPieceType = i;
						move.MoveType = CAPTURE;
					}
				}
				Position NewPos(position);
				NewPos.MakeMove(move);
				if (!IsKingUnderAttack(NewPos, WHITE))
				{
					moves.push_back(move);
				}
				UnLegalMoves &= UnLegalMoves - 1;
			}
			Bishops &= Bishops - 1;
		}
		Bitboard Knigths = position.Pieces[WHITE][KNIGHT];
		while (Knigths)
		{
			int knight = MyBitScanReverse(Knigths);
			Bitboard UnLegalMoves = get_knight_attack(knight) & ~position.AllWhitePeaces;
			string kal = Position::ViewBitboard(UnLegalMoves);
			while (UnLegalMoves)
			{
				int to = MyBitScanReverse(UnLegalMoves);
				Move move(knight, to, KNIGHT, NORMAL_MOVE, KING);
				Bitboard NewKnightPosition = Bitboard(1) << to;
				string kal1 = Position::ViewBitboard(NewKnightPosition);
				for (int i = 0; i < 6; i++)
				{
					if (position.Pieces[BLACK][i] & NewKnightPosition)
					{
						move.OpPieceType = i;
						move.MoveType = CAPTURE;
					}
				}
				Position NewPos(position);
				NewPos.MakeMove(move);
				if (!IsKingUnderAttack(NewPos, WHITE))
				{
					moves.push_back(move);
				}
				UnLegalMoves &= UnLegalMoves - 1;
			}
			Knigths &= Knigths - 1;
		}
		Bitboard King = position.Pieces[WHITE][KING];
		int king = MyBitScanReverse(King);
		Bitboard UnLegalMoves = get_king_attack(king) & (~position.AllWhitePeaces);
		string kal = Position::ViewBitboard(UnLegalMoves);
		while (UnLegalMoves)
		{
			int to = MyBitScanReverse(UnLegalMoves);
			Move move(king, to, KING, NORMAL_MOVE, KING);
			Bitboard NewKingPosition = Bitboard(1) << to;
			for (int i = 0; i < 6; i++)
			{
				if (position.Pieces[BLACK][i] & NewKingPosition)
				{
					move.OpPieceType = i;
					move.MoveType = CAPTURE;
				}
			}
			Position NewPos(position);
			NewPos.MakeMove(move);
			if (!IsKingUnderAttack(NewPos, WHITE))
			{
				moves.push_back(move);
			}
			UnLegalMoves &= UnLegalMoves - 1;
		}
		if (IsCastlePossible(position, PieceColor, MoveTypes::SHORT_CASTLE))
		{
			moves.push_back(Move(3, 1, KING, SHORT_CASTLE, KING));
		}
		if (IsCastlePossible(position, PieceColor, MoveTypes::LONG_CASTLE))
		{
			moves.push_back(Move(3, 5, KING, LONG_CASTLE, KING));
		}
		Bitboard Pawns = position.Pieces[WHITE][PAWN];
		while (Pawns)
		{
			// генераци€ NORMAL_MOVE,FIRST_PAWN_MOVE,TRANSFORM
			int pawn = MyBitScanReverse(Pawns);
			Bitboard UnLegalMoves = get_pawn_moves(pawn, WHITE) & ~AllPieces;
			// ѕроверка на невозможность двойного хода если перел пешкой стоит фигура
			if (UnLegalMoves >> 16 == Bitboard(1) << pawn)
				UnLegalMoves = Bitboard(0);
			while (UnLegalMoves)
			{
				int to = MyBitScanReverse(UnLegalMoves);
				Move move(pawn, to, PAWN, NORMAL_MOVE, QUEEN);
				Bitboard NewPawnPosition = Bitboard(1) << to;
				if (NewPawnPosition >> 16 == Bitboard(1) << pawn)
					move.MoveType = FIRST_PAWN_MOVE;
				if (NewPawnPosition & RANK_8)
					move.MoveType = TRANSFORM;
				Position NewPos(position);
				NewPos.MakeMove(move);
				if (!IsKingUnderAttack(NewPos, WHITE))
					moves.push_back(move);
				UnLegalMoves &= UnLegalMoves - 1;
			}
			// √енераци€ Capture,CAPTURE_TRANSFORM
			UnLegalMoves = get_pawn_attack(pawn, WHITE) & position.AllBlackPeaces;
			while (UnLegalMoves)
			{
				int to = MyBitScanReverse(UnLegalMoves);
				Move move(pawn, to, PAWN, CAPTURE, KING);
				Bitboard NewPawnPosition = Bitboard(1) << to;
				for (int i = 0; i < 6; i++)
				{
					if (position.Pieces[BLACK][i] & NewPawnPosition)
					{
						move.OpPieceType = i;
						if (NewPawnPosition & RANK_8) move.MoveType = CAPTURE_TRANSFORM;
						else move.MoveType = CAPTURE;
					}
				}
				Position NewPos(position);
				NewPos.MakeMove(move);
				if (!IsKingUnderAttack(NewPos, WHITE))
					moves.push_back(move);
				UnLegalMoves &= UnLegalMoves - 1;
			}
			UnLegalMoves = get_pawn_attack(pawn, WHITE) & ~position.AllWhitePeaces;
			while (UnLegalMoves)
			{
				int to = MyBitScanReverse(UnLegalMoves);
				Move move(pawn, to, PAWN, MAGIC_CAPTURE, PAWN);
				Bitboard NewPawnPosition = Bitboard(1) << to;
				if (position.MagicCaptureField == NewPawnPosition)
				{
					Position NewPos(position);
					NewPos.MakeMove(move);
					if (!IsKingUnderAttack(NewPos, WHITE))
						moves.push_back(move);
					break;
				}
				UnLegalMoves &= UnLegalMoves - 1;
			}
			Pawns &= Pawns - 1;
		}
	}
	else
	{
	Bitboard Queens = position.Pieces[BLACK][QUEEN];
	while (Queens)
	{
		int queen = MyBitScanReverse(Queens);
		Bitboard UnLegalMoves = get_queen_attack(queen, AllPieces) & ~position.AllBlackPeaces;
		string kal = Position::ViewBitboard(UnLegalMoves);
		while (UnLegalMoves)
		{
			int to = MyBitScanReverse(UnLegalMoves);
			Move move(queen, to, QUEEN, NORMAL_MOVE, KING);
			Bitboard NewQueenPosition = Bitboard(1) << to;
			string kal1 = Position::ViewBitboard(NewQueenPosition);
			for (int i = 0; i < 6; i++)
			{
				if (position.Pieces[WHITE][i] & NewQueenPosition)
				{
					move.OpPieceType = i;
					move.MoveType = CAPTURE;
				}
			}
			Position NewPos(position);
			NewPos.MakeMove(move);
			if (!IsKingUnderAttack(NewPos, BLACK))
			{
				moves.push_back(move);
			}
			UnLegalMoves &= UnLegalMoves - 1;
		}
		Queens &= Queens - 1;
	}
	Bitboard Rooks = position.Pieces[BLACK][ROOK];
	while (Rooks)
	{
		int rook = MyBitScanReverse(Rooks);
		Bitboard UnLegalMoves = get_rook_attack(rook, AllPieces) & ~position.AllBlackPeaces;
		string kal = Position::ViewBitboard(UnLegalMoves);
		while (UnLegalMoves)
		{
			int to = MyBitScanReverse(UnLegalMoves);
			Move move(rook, to, ROOK, NORMAL_MOVE, KING);
			Bitboard NewRookPosition = Bitboard(1) << to;
			string kal1 = Position::ViewBitboard(NewRookPosition);
			for (int i = 0; i < 6; i++)
			{
				if (position.Pieces[WHITE][i] & NewRookPosition)
				{
					move.OpPieceType = i;
					move.MoveType = CAPTURE;
				}
			}
			Position NewPos(position);
			NewPos.MakeMove(move);
			if (!IsKingUnderAttack(NewPos, BLACK))
			{
				moves.push_back(move);
			}
			UnLegalMoves &= UnLegalMoves - 1;
		}
		Rooks &= Rooks - 1;
	}
	Bitboard Bishops = position.Pieces[BLACK][BISHOP];
	while (Bishops)
	{
		int bishop = MyBitScanReverse(Bishops);
		Bitboard UnLegalMoves = get_bishop_attack(bishop, AllPieces) & ~position.AllBlackPeaces;
		string kal = Position::ViewBitboard(UnLegalMoves);
		while (UnLegalMoves)
		{
			int to = MyBitScanReverse(UnLegalMoves);
			Move move(bishop, to, BISHOP, NORMAL_MOVE, KING);
			Bitboard NewBishopPosition = Bitboard(1) << to;
			string kal1 = Position::ViewBitboard(NewBishopPosition);
			for (int i = 0; i < 6; i++)
			{
				if (position.Pieces[WHITE][i] & NewBishopPosition)
				{
					move.OpPieceType = i;
					move.MoveType = CAPTURE;
				}
			}
			Position NewPos(position);
			NewPos.MakeMove(move);
			if (!IsKingUnderAttack(NewPos, BLACK))
			{
				moves.push_back(move);
			}
			UnLegalMoves &= UnLegalMoves - 1;
		}
		Bishops &= Bishops - 1;
	}
	Bitboard Knigths = position.Pieces[BLACK][KNIGHT];
	while (Knigths)
	{
		int knight = MyBitScanReverse(Knigths);
		Bitboard UnLegalMoves = get_knight_attack(knight) & ~position.AllBlackPeaces;
		string kal = Position::ViewBitboard(UnLegalMoves);
		while (UnLegalMoves)
		{
			int to = MyBitScanReverse(UnLegalMoves);
			Move move(knight, to, KNIGHT, NORMAL_MOVE, KING);
			Bitboard NewKnightPosition = Bitboard(1) << to;
			string kal1 = Position::ViewBitboard(NewKnightPosition);
			for (int i = 0; i < 6; i++)
			{
				if (position.Pieces[WHITE][i] & NewKnightPosition)
				{
					move.OpPieceType = i;
					move.MoveType = CAPTURE;
				}
			}
			Position NewPos(position);
			NewPos.MakeMove(move);
			if (!IsKingUnderAttack(NewPos, BLACK))
			{
				moves.push_back(move);
			}
			UnLegalMoves &= UnLegalMoves - 1;
		}
		Knigths &= Knigths - 1;
	}
	Bitboard King = position.Pieces[BLACK][KING];
	int king = MyBitScanReverse(King);
	Bitboard UnLegalMoves = get_king_attack(king) & (~position.AllBlackPeaces);
	string kal = Position::ViewBitboard(UnLegalMoves);
	while (UnLegalMoves)
	{
		int to = MyBitScanReverse(UnLegalMoves);
		Move move(king, to, KING, NORMAL_MOVE, KING);
		Bitboard NewKingPosition = Bitboard(1) << to;
		for (int i = 0; i < 6; i++)
		{
			if (position.Pieces[WHITE][i] & NewKingPosition)
			{
				move.OpPieceType = i;
				move.MoveType = CAPTURE;
			}
		}
		Position NewPos(position);
		NewPos.MakeMove(move);
		if (!IsKingUnderAttack(NewPos, BLACK))
		{
			moves.push_back(move);
		}
		UnLegalMoves &= UnLegalMoves - 1;
	}
	if (IsCastlePossible(position, PieceColor, MoveTypes::SHORT_CASTLE))
	{
		moves.push_back(Move(59, 57, KING, SHORT_CASTLE, KING));
	}
	if (IsCastlePossible(position, PieceColor, MoveTypes::LONG_CASTLE))
	{
		moves.push_back(Move(59, 61, KING, LONG_CASTLE, KING));
	}
	Bitboard Pawns = position.Pieces[BLACK][PAWN];
	while (Pawns)
	{
		// генераци€ NORMAL_MOVE,FIRST_PAWN_MOVE,TRANSFORM
		int pawn = MyBitScanReverse(Pawns);
		Bitboard UnLegalMoves = get_pawn_moves(pawn, BLACK) & ~AllPieces;
		// ѕроверка на невозможность двойного хода если перел пешкой стоит фигура
		if (UnLegalMoves << 16 == Bitboard(1) << pawn)
			UnLegalMoves = Bitboard(0);
		while (UnLegalMoves)
		{
			int to = MyBitScanReverse(UnLegalMoves);
			Move move(pawn, to, PAWN, NORMAL_MOVE, QUEEN);
			Bitboard NewPawnPosition = Bitboard(1) << to;
			if (NewPawnPosition << 16 == Bitboard(1) << pawn)
				move.MoveType = FIRST_PAWN_MOVE;
			if (NewPawnPosition & RANK_8)
				move.MoveType = TRANSFORM;
			Position NewPos(position);
			NewPos.MakeMove(move);
			if (!IsKingUnderAttack(NewPos, BLACK))
				moves.push_back(move);
			UnLegalMoves &= UnLegalMoves - 1;
		}
		// √енераци€ Capture,CAPTURE_TRANSFORM
		UnLegalMoves = get_pawn_attack(pawn, BLACK) & position.AllWhitePeaces;
		while (UnLegalMoves)
		{
			int to = MyBitScanReverse(UnLegalMoves);
			Move move(pawn, to, PAWN, CAPTURE, KING);
			Bitboard NewPawnPosition = Bitboard(1) << to;
			for (int i = 0; i < 6; i++)
			{
				if (position.Pieces[WHITE][i] & NewPawnPosition)
				{
					move.OpPieceType = i;
					if (NewPawnPosition & RANK_1) move.MoveType = CAPTURE_TRANSFORM;
					else move.MoveType = CAPTURE;
				}
			}
			Position NewPos(position);
			NewPos.MakeMove(move);
			if (!IsKingUnderAttack(NewPos, BLACK))
				moves.push_back(move);
			UnLegalMoves &= UnLegalMoves - 1;
		}
		UnLegalMoves = get_pawn_attack(pawn, BLACK) & ~position.AllBlackPeaces;
		while (UnLegalMoves)
		{
			int to = MyBitScanReverse(UnLegalMoves);
			Move move(pawn, to, PAWN, MAGIC_CAPTURE, PAWN);
			Bitboard NewPawnPosition = Bitboard(1) << to;
			if (position.MagicCaptureField == NewPawnPosition)
			{
				Position NewPos(position);
				NewPos.MakeMove(move);
				if (!IsKingUnderAttack(NewPos, WHITE))
					moves.push_back(move);
				break;
			}
			UnLegalMoves &= UnLegalMoves - 1;
		}
		Pawns &= Pawns - 1;
	}
	}
	return moves;
}

inline Bitboard MoveGen::east_n(Bitboard board, int n)
{
	Bitboard new_board = board;
	for (int i = 0; i < n; i++) {
		new_board = (new_board >> 1) & (~FILE_A);
	}
	return new_board;
}

inline Bitboard MoveGen::west_n(Bitboard board, int n)
{
	Bitboard new_board = board;
	for (int i = 0; i < n; i++) {
		new_board = (new_board << 1) & (~FILE_H);
	}
	return new_board;
}

inline int MoveGen::col(int sq)
{
	return sq & 7;
}

inline int MoveGen::row(int sq)
{
	return sq >> 3;
}




