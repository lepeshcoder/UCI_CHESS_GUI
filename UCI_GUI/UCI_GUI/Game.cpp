#include "Game.h"

Game::Game()
{
	res = new Resources();
	pos = new Position();
	board = new Board();
	moveGen = new MoveGen();
	//ipc = new IPC();*/
	
}

int Game::GetCellIndex(int mouse_x, int mouse_y)
{
	int offtop = 36, x, y;
	if (mouse_x >= offtop && mouse_x <= res->BoardSize - offtop &&
		mouse_y >= offtop && mouse_y <= res->BoardSize - offtop)
	{
		y = int((double(mouse_y) - offtop) / (res->BoardSize - 2 * offtop) * 8);
		x = int((double(mouse_x) - offtop) / (res->BoardSize - 2 * offtop) * 8);
		return y * 8 + x;
	}
	else return -1;
}

void Game::Init()
{
	pos->InitPosition("rnbqk2r/ppp1ppbp/3p1np1/8/2PPP3/2N2P2/PP4PP/R1BQKBNR b KQkq - 0 5");
	board->InitBoard(*pos);
	state = GameStates::PLAYER_MOVE; 
	isFigureChoosen = false;
}

void Game::Draw(Gdiplus::Graphics& gr)
{
	gr.DrawImage(res->BoardImage,0,0,res->BoardSize,res->BoardSize);
	
	Cell* temp = this->board->GetBoard();
	Bitboard attacks = board->GetAttacks();
	Bitboard mask = Bitboard(1)<<63;

	int x = 35; int y = 35;
	
	for (int i = 0; i < 64; i++)
	{
		if (temp[i].PieceColor)
		{
			gr.DrawImage(res->PiecesImages[*temp[i].PieceColor][*temp[i].PieceType], x, y, res->PieceSize, res->PieceSize);
		}
		if (attacks & mask)
		{
			gr.DrawImage(res->MoveImage, x, y, res->PieceSize, res->PieceSize);
		}
		x += 86;
		if (x == 35+8*86)
		{
			x = 35;
			y += 85; 
		}
		mask >>= 1;
	}
}

string Game::GetPositionFen()
{
	return board->GetPositionFEN() + pos->GetPositionFEN();
}

void Game::Update(int mouse_x, int mouse_y)
{
	int index = GetCellIndex(mouse_x, mouse_y);  
	switch (state)
	{
		case GameStates::PLAYER_MOVE:
		{
			// Если никуда не нажали выходим из update
			if (index == -1) return;
			Cell* currCell = board->GetCell(index);
			// Если фигура уже выбрана
			if (isFigureChoosen)
			{
				// Если после выбора фигуры нажали на один из ее ходов
				if ((Bitboard(1) << index) & board->GetAttacks())
				{

				}
				// Если выбрали другую фигуру
				else
				{
					if (currCell->PieceColor != nullptr && *currCell->PieceColor == pos->ActiveColor)
					{
						isFigureChoosen = true;
						std::vector<Move> AllMoves = moveGen->GetAllMoves(*pos, pos->ActiveColor);
						std::vector<Move> CurrMoves;
						Bitboard bbCurrMoves = 0;
						for (auto& move : AllMoves)
						{
							if (move.From == 63 - index)
							{
								CurrMoves.push_back(move);
								bbCurrMoves |= Bitboard(1) << move.To;
							}
						}
						board->SetAttacks(bbCurrMoves);
					}
					else return;
				}
			}
			else
			{
				// Если фигурка есть на клетке и цвета игрока то выбираем ее и генерирум ее ходы
				if (currCell->PieceColor != nullptr && *currCell->PieceColor == pos->ActiveColor)
				{
					isFigureChoosen = true;
					std::vector<Move> AllMoves = moveGen->GetAllMoves(*pos, pos->ActiveColor);
					std::vector<Move> CurrMoves;
					Bitboard bbCurrMoves = 0;
					for (auto& move : AllMoves)
					{
						if (move.From == 63 - index)
						{
							CurrMoves.push_back(move);
							bbCurrMoves |= Bitboard(1) << move.To;
						}
					}
					board->SetAttacks(bbCurrMoves);
				}
			}
			break;
		}
		case GameStates::COMPUTER_MOVE:
		{
			
			break;
		}
		case GameStates::END:
		{

			break;
		}
	}
}


Game::~Game()
{
	delete board;
	// delete ipc;
	delete moveGen;
	delete pos;
}
