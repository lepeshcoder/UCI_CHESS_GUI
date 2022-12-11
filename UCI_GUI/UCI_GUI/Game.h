#pragma once
#include "framework.h"
#include "Position.h"
#include "MoveGen.h"
#include "Board.h"
#include "IPC.h"
#include "Resources.h"
class Game
{
public:
	// Ресурсы
	Resources* res;
	// Информация для модели
	Position* pos;
	// Генератор ходов 
	MoveGen* moveGen;
	// Информация для графики
	Board* board;
	// Взаимодействие с движком
	IPC* ipc; 
	// Выбрана ли фигура
	bool isFigureChoosen;
	// состояние игры
	GameStates state;

public:

	Game(); 

	int GetCellIndex(int mouse_x, int mouse_y);

	void Init();

	void Draw(Gdiplus::Graphics& gr);

	string GetPositionFen();

	void Update(int mouse_x,int mouse_y);

	~Game();
};

