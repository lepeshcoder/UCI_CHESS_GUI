// header.h: включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows
#include <windows.h>
// Файлы заголовков среды выполнения C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <objidl.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
#include <string>
#include <string.h>
#include <immintrin.h>
#include <algorithm>
#include <sstream>
#pragma comment (lib,"Gdiplus.lib")

typedef unsigned long long Bitboard;

using std::string;
using std::cout;
using std::cin;
using std::endl;

enum PieceColors { WHITE = 0, BLACK };
enum PieceTypes { KING = 0, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };
enum MoveTypes { NORMAL_MOVE, CAPTURE, MAGIC_CAPTURE, TRANSFORM, SHORT_CASTLE, LONG_CASTLE, FIRST_PAWN_MOVE, CAPTURE_TRANSFORM };
enum GameStates { PLAYER_MOVE, COMPUTER_MOVE, END };

// возврашает индекс первого ненулевого бита начиная с младшего разряда
int MyBitScanReverse(Bitboard bb);

// Возвращает индекс первого ненулевого бита начиная с старшего разряда 
int MyBitScanForward(Bitboard bb);






