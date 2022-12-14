#pragma once
#include "framework.h"
#include "Position.h"





enum verticals { A = 0, B, C, D, E, F, G, H };
enum Directions { NORTH = 0, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST };
constexpr Bitboard FILE_H = 0X0101010101010101;
constexpr Bitboard FILE_A = 0X8080808080808080;
constexpr Bitboard FILE_B = 0X4040404040404040;
constexpr Bitboard FILE_G = 0X0202020202020202;
constexpr Bitboard RANK_2 = 0XFF00;
constexpr Bitboard RANK_1 = 0XFF;
constexpr Bitboard RANK_5 = 0XFF00000000;
constexpr Bitboard RANK_4 = 0XFF000000;
constexpr Bitboard RANK_7 = 0XFF000000000000;
constexpr Bitboard RANK_8 = 0XFF00000000000000;
constexpr Bitboard ONE = 0XFFFFFFFFFFFFFFFF;
constexpr Bitboard DIAG_A8H1 = 0X8040201008040201;
constexpr Bitboard DIAG_A1H8 = 0X0102040810204080;
constexpr Bitboard WHITE_SHORT_CASTLE = 0XE;
constexpr Bitboard WHITE_LONG_CASTLE = 0X38;
constexpr Bitboard BLACK_SHORT_CASTLE = 0X0E00000000000000;
constexpr Bitboard BLACK_LONG_CASTLE = 0X3800000000000000;


const int rook_bits[64] = {
	12,11,11,11,11,11,11,12,
	11,10,10,10,10,10,10,11,
	11,10,10,10,10,10,10,11,
	11,10,10,10,10,10,10,11,
	11,10,10,10,10,10,10,11,
	11,10,10,10,10,10,10,11,
	11,10,10,10,10,10,10,11,
	12,11,11,11,11,11,11,12
};

const int bishop_bits[64] = {
	6,5,5,5,5,5,5,6,
	5,5,5,5,5,5,5,5,
	5,5,7,7,7,7,5,5,
	5,5,7,9,9,7,5,5,
	5,5,7,9,9,7,5,5,
	5,5,7,7,7,7,5,5,
	5,5,5,5,5,5,5,5,
	6,5,5,5,5,5,5,6
};

const int queen_bits[64] = {
   18,16,16,16,16,16,16,18,
   16,15,15,15,15,15,15,16,
   16,15,17,17,17,17,15,16,
   16,15,17,19,19,17,15,16,
   16,15,17,19,19,17,15,16,
   16,15,17,17,17,17,15,16,
   16,15,15,15,15,15,15,16,
   18,16,16,16,16,16,16,18,
};




class MoveGen
{
private:
	Bitboard** Rays;
	Bitboard** BishopCache;
	Bitboard** RookCache;
	Bitboard** QueenCache;
	Bitboard* KingCache;
	Bitboard* KnightCache;
	Bitboard* WPawnCache;
	Bitboard* BPawnCache;
	Bitboard* WPawnMoveCache;
	Bitboard* BPawnMoveCache;
	Bitboard RookMasks[64];
	Bitboard BishopMasks[64];
	Bitboard QueenMasks[64];
	

public:

	MoveGen();

	// ���������� ������� �� ����� ������-���������� ������ ����� (��� ������)
	Bitboard calc_pawn_moves(PieceColors color, Bitboard pawns);

	// ���������� ������� �� ����� ������-���������� �������� ����� ( �� �������� �������� � ����� ������� ������� 1 ����( ������� ��� �� 1 ���) )
	Bitboard calc_pawn_captures(PieceColors color, Bitboard pawns);

	// ���������� ������� � ����� ������-���������� ������ ���� 
	Bitboard calc_knight_attack(Bitboard knights);

	// ���������� ������ �� ����� ������-���������� ������ ������
	Bitboard calc_king_attack(Bitboard king);

	// ���������� ������� �� ����� ���������������� ������ ����� � ������ ������ ��� ������� ������ ��������
	Bitboard get_rook_attack(int sq, Bitboard blockers);

	// ���������� ������� �� ����� ���������������� ������ ����� � ������ ������ ��� ������� ������ ��������
	Bitboard get_bishop_attack(int sq, Bitboard blockers);

	//
	Bitboard get_queen_attack(int sq, Bitboard blockers);


	Bitboard get_knight_attack(int sq);

	Bitboard get_king_attack(int sq);

	Bitboard get_pawn_attack(int sq, PieceColors color);

	Bitboard get_pawn_moves(int sq, PieceColors color);



	// ��������� ������ �����
	void InitRays();

	// ����������� ��������� ���� (����) ����� ��� ������ ������ � ������ ����������� ����� 
	Bitboard calc_rook_attacks(int sq, Bitboard blockers);

	// ����������� ��������� ���� (����) ����� ��� ������ ������ � ������ ����������� �����
	Bitboard calc_bishop_attacks(int sq, Bitboard blockers);

	// ����������� ��������� ���� (����) ����� ��� ������ ������ � ������ ����������� �����
	Bitboard calc_queen_attacks(int sq, Bitboard blockers);

	// ������������ ��� ���� ����� �� ������ ������ (�� ������ ���� ����� (��� ���������� �����������) )
	void InitRookMasks();

	// ������������ ��� ���� ����� �� ������ ������ (�� ������ ���� ����� (��� ���������� �����������) )
	void InitBishopMasks();

	// ������������ ��� ���� ����� �� ������ ������ (�� ������ ���� ����� (��� ���������� �����������) )
	void InitQueenMasks();

	// ���������� ���� ����� ����� ��� ������ ������ � ������ ����������� �����
	void InitRookCache();

	// ���������� ���� ����� ����� ��� ������ ������ � ������ ����������� �����
	void InitBishopCache();

	// ���������� ���� ����� ����� ��� ������ ������ � ������ ����������� �����
	void InitQueenCache();

	// ���������� ���� ����� ������ ��� ������ ������ 
	void InitKingCache();

	// ���������� ���� ����� ���� ��� ������ ������ 
	void InitKnightCache();

	void InitPawnCache();

	void InitPawnMoveCache();

	bool is_under_attack(int sq, const Position& position, int color);

	bool IsKingUnderAttack(const Position& position, int color);

	bool IsCastlePossible(const Position& position, int color, int CastleType);

	std::vector<Move> GetAllMoves(const Position& position,int PieceColor);


	// ��������������� �������
	inline Bitboard east_n(Bitboard board, int n);

	inline Bitboard west_n(Bitboard board, int n);

	inline int col(int sq);

	inline int row(int sq);

};