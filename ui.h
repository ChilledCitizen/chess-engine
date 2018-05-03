#pragma once

#include "state.h"
#include <string>
using namespace std;

class UserInterface {
public: 
	State* board;
	static wstring unicodeArr[13];

public:
	static char acceptedPiece[8];
	static char acceptedPromotion[8];
	static char acceptedLetter[8];
	static char acceptedNumber[8];

	UserInterface();
	Move* receivePlayerInput();
	void drawBoard();
	bool tryMove(Move*);
	wstring moveToString(Move*);
};