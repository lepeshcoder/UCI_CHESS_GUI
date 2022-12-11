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
	// �������
	Resources* res;
	// ���������� ��� ������
	Position* pos;
	// ��������� ����� 
	MoveGen* moveGen;
	// ���������� ��� �������
	Board* board;
	// �������������� � �������
	IPC* ipc; 
	// ������� �� ������
	bool isFigureChoosen;
	// ��������� ����
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

