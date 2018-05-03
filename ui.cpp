

#include "ui.h"
//#include <stdlib.h>
// empty, bs, br, bk, bb, bq, bk, ws, wr, wk, wb, wq, wk
wstring UserInterface::unicodeArr[13] = 
	{L"\u2659" , L"\u2656", L"\u2658" , L"\u2657" , L"\u2655" , L"\u2654", 
	L"\u265F" ,L"\u265C" ,L"\u265E" ,L"\u265D",L"\u265B",L"\u265A" ,L" " };

char UserInterface::acceptedPiece[8] = { 's','r','k','b','Q','K' };
char UserInterface::acceptedPromotion[8] = {'r','k','b','Q'};
char UserInterface::acceptedLetter[8] = { 'a','b','c','d','e','f','g','h' };
char UserInterface::acceptedNumber[8] = { '1','2','3','4','5','6','7','8' };

// TODO: Move generation from input
// TODO: Better loop at main

UserInterface::UserInterface() {
	board = new State();
	//board = new State(true);
}

void clear() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}

void UserInterface::drawBoard() {
	clear();
	char buffer[] = " ";
	_setmode(_fileno(stdout), _O_U16TEXT);
	for (int i = 7; i>-1; i--) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
		wcout << (i + 1);
		for (int j = 0; j<8; j++) {

			if ((i + j) % 2 == 0)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | BACKGROUND_GREEN);
			}
			else
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
			}
			uint64_t index = (i * 8) + j;
			wstring getP = unicodeArr[board->getPiece(index)];
			wcout << buffer;
			wcout << getP;
			wcout << buffer;
		}
		wcout << endl;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
	wcout << " " << buffer << "a" << buffer << buffer << "b" << buffer << buffer << "c" << buffer << buffer << "d" <<
		buffer << buffer << "e" << buffer << buffer << "f" << buffer << buffer << "g" << buffer << buffer << "h" << buffer << endl;
}



Move* UserInterface::receivePlayerInput() {
	bool succesfulInput = false;
	wchar_t input[8];
	bool pieceAccepted = false;
	int pieceInd = -1;
	int charPos1 = -1;
	int numPos1 = -1;
	int charPos2 = -1;
	int numPos2 = -1;
	int promPos = -1;
	int playerTurn = board->getPlayerTurn();
	wcout << "srkbQK | a1-h8 | (Optional) promotion: rkbQ" << endl;
	while (!succesfulInput){
		pieceAccepted = false;
		pieceInd = -1;
		charPos1 = -1;
		numPos1 = -1;
		charPos2 = -1;
		numPos2 = -1;
		promPos = -1;
		if (playerTurn == 0) {
			wcout << "WHITE, ";
		}
		else {
			wcout << "BLACK, ";
		}
		wcout << "input your move: ";
		wcin >> input;
		if (input == L"FORFEIT") {
			return nullptr;
		}
		for (int i = 0; i < 8; i++) {
			if (input[0] == acceptedPiece[i]) {
				pieceAccepted = true;
				pieceInd = i;
			}
			if (input[1] == acceptedLetter[i]) {
				charPos1 = i;
			}
			if (input[2] == acceptedNumber[i]) {
				numPos1 = i;
			}
			if (input[4] == acceptedLetter[i]) {
				charPos2 = i;
			}
			if (input[5] == acceptedNumber[i]) {
				numPos2 = i;
			}
			if (input[6] == acceptedPromotion[i]) {
				if (i<4 && i>=0) {
					promPos = i;
				}
			}
		}

		if ((charPos1 == -1) || (numPos1 == -1) || (charPos2 == -1) || (numPos2 == -1) || !pieceAccepted) {
			if (input[0] == 'O') {
				if (input[4] == 'O') {
					return new Move(playerTurn, false, true);
				}
				else if (input[2] == 'O') {
					return new Move(playerTurn, true, false);
				}
			}
			wcout << "Input is false" << endl;
		} else {
			succesfulInput = true;
		}
	}
	uint64_t startInd = (numPos1 * 8) + charPos1; 
	uint64_t endInd = (numPos2 * 8) + charPos2; 
	uint64_t piece = (playerTurn*6) + pieceInd;
	Move* theMove = new Move(playerTurn, startInd, endInd, piece);
	if (promPos != -1) {
		theMove->setPromotion(promPos);
	}
	int change = 8 - (playerTurn * 16);
	uint64_t sideKillIndex = board->getSideKillIndex();
	//wcout << "sidekill is " << sideKillIndex << endl;
	if (sideKillIndex == startInd + 1 || sideKillIndex == startInd - 1) {
		if (board->getSideKillIndex() == endInd - change) {
			theMove->setSideKill();
		}
	}
	return theMove;
}

bool UserInterface::tryMove(Move* mov) {
	board->setAllLegalMoves(true);
	if (board->checkMoveLegality(mov)) {
		board->updateBoard(mov);
		delete mov;
		return true;
	}
	else {
		wcout << "Move not legal" << endl;
		delete mov;
		return false;
	}
}


wstring UserInterface::moveToString(Move* mov) {
	wchar_t out[6];
	out[0] = acceptedPiece[mov->getPiece() - (6 * mov->getColor())];
	out[1] = acceptedLetter[mov->getStart() % 8];
	out[2] = acceptedNumber[mov->getStart() / 8];
	out[3] = '-';
	out[4] = acceptedLetter[mov->getEnd() % 8];
	out[5] = acceptedNumber[mov->getEnd() / 8];
	return wstring(out);
}