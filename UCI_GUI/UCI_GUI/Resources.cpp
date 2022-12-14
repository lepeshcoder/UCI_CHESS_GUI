#include "Resources.h"


Resources::Resources()
{
	this->CellSize = 100;
	this->PieceSize = 80;
	this->BoardSize = 750;
	BoardImage = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\chess_board.png");
	MoveImage = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\move.png");
	WhiteCellImage = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\white_cell.png");
	BlackCellImage = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\black_cell.png");
	PiecesImages[WHITE][KING] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\white_king.png");
	PiecesImages[WHITE][QUEEN] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\white_queen.png");
	PiecesImages[WHITE][ROOK] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\white_rook.png");
	PiecesImages[WHITE][BISHOP] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\white_bishop.png");
	PiecesImages[WHITE][KNIGHT] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\white_knight.png");
	PiecesImages[WHITE][PAWN] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\white_pawn.png");
	PiecesImages[BLACK][KING] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\black_king.png");
	PiecesImages[BLACK][QUEEN] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\black_queen.png");
	PiecesImages[BLACK][ROOK] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\black_rook.png");
	PiecesImages[BLACK][BISHOP] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\black_bishop.png");
	PiecesImages[BLACK][KNIGHT] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\black_knight.png");
	PiecesImages[BLACK][PAWN] = new Gdiplus::Image(L"C:\\Users\\user\\desktop\\Курсач\\res\\peaces\\black_pawn.png");
}


