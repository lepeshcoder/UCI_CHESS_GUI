#include "Game.h"

Game::Game(HWND window,HWND* win)
{
	this->window = window;
	res = new Resources();
	pos = new Position();
	board = new Board();
	moveGen = new MoveGen();
	ipc = new IPC();
	this->win = win;

	kal.open("C:\\Users\\user\\Desktop\\debug.txt");
	kal << "NEW_GAME\n";
	kal.close();
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
	pos->InitPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	board->InitBoard(*pos);
	string response = ipc->ReceiveResponse();
	state = GameStates::PLAYER_MOVE;
	IsMessage = true;
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
				if ((Bitboard(1) << (63 - index)) & board->GetAttacks()) 
				{
					for (auto& move : pos->CurrMoves)
					{
						if (move.To == 63-index)
						{
							pos->MakeMove(move);
							board->UpdateBoard(*pos);
							board->SetAttacks(Bitboard(0));
							pos->CurrMoves.clear();
							isFigureChoosen = false;
							if (isWin())
							{
								state = GameStates::END;
								if (pos->ActiveColor == WHITE)
									MessageBox(window, L"Black won", L"Game over", MB_OK);
								else
									MessageBox(window, L"White won ", L"Game over", MB_OK);

								return;
							}
							else
							{
								state = GameStates::COMPUTER_MOVE;
								return;
							}
							return;
						}
					}
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
						pos->CurrMoves = CurrMoves;
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
					pos->CurrMoves = CurrMoves;
				}
			}
			break;
		}
		case GameStates::COMPUTER_MOVE:
		{
			string FEN = GetPositionFen();
			string temp = FEN + '\n';
			
			kal.open("C:\\Users\\user\\Desktop\\debug.txt",std::ios::app);
			kal << temp;
			kal.close();

			ipc->SendRequest("position fen " + FEN + '\n');
			Sleep(100);
			ipc->SendRequest("go\n");
			Sleep(500);
			string searchinfo = ipc->ReceiveResponse();
			Sleep(1);
			ipc->SendRequest("stop\n");
			Sleep(100);
			string bestmove = ipc->ReceiveResponse();	
			int index = bestmove.find("bestmove");
			bestmove = bestmove.substr(index + 9, 4);
			std::vector<Move> moves = moveGen->GetAllMoves(*pos, pos->ActiveColor);
			std::vector<string> MoveStrings;
			for (int i = 0; i < moves.size(); i++)
				MoveStrings.push_back(moves[i].ToString());
			for (auto& move : moves)
			{
				if (move.ToString()._Equal(bestmove))
				{
					pos->MakeMove(move);
					board->UpdateBoard(*pos);
					board->SetAttacks(Bitboard(0));
					pos->CurrMoves.clear();
					if (isWin())
					{
						state = GameStates::END;
						if (pos->ActiveColor == WHITE)
							MessageBox(window, L"Black won", L"Game over", MB_OK);
						else
							MessageBox(window, L"White won ", L"Game over", MB_OK);
						return;
					}
					else
					{
						state = GameStates::PLAYER_MOVE;
						return;
					}
					return;
				}
			}

			break;
		}
	}
}

bool Game::isWin()
{
	if (moveGen->GetAllMoves(*pos, pos->ActiveColor).size() == 0
		&& moveGen->IsKingUnderAttack(*pos, pos->ActiveColor))
		return true;
	return false;
}

Game::~Game()
{
	delete board;
	delete ipc;
	delete moveGen;
	delete pos;
}
