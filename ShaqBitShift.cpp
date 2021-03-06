// ShaqBitShift.cpp : Defines the entry point for the console application.
//

#include "ui.h"
#include "ai.h"

int AIColor = 1;
int depth = 4;


void returnPrompt(State* currentState) {
	int color = -1;
	int kingIndex = -1;
	if (currentState->getPlayerTurn() == 0) {
		color = 1;
		kingIndex = 5;
	}
	else {
		color = 0;
		kingIndex = 11;
	}
	currentState->mapEnemyAttackMoves(color);
	if (currentState->enemyAttackMap & currentState->_pieces[kingIndex]) {
		if (color == 0) {
			wcout << "SHAQ! Black side loses!" << endl;
		}
		else {
			wcout << "SHAQ! White side loses!" << endl;
		}
	}
	else {
		wcout << "MAT: DRAW" << endl;
	}
	wcout << "Game ending, type OK to exit" << endl;
	while (true) {
		wstring input;
		wcin >> input;
		if (input == L"OK") {
			return;
		}
	}
}


int main() {
	UserInterface* ui = new UserInterface();
	ui->drawBoard();
	while (true) {
		if (AIColor == ui->board->getPlayerTurn()) {
			Move* aiMove = chooseAIMove(ui->board, depth);
			if (aiMove == nullptr) {
				returnPrompt(ui->board);
				return 0;
			}
			wstring herb = ui->moveToString(aiMove);
			ui->tryMove(aiMove);
			ui->drawBoard();
			wcout << "Bot move: " << herb << endl;
		}
		else {
			while (true) {
				Move* testMove = ui->receivePlayerInput();
				if (testMove == nullptr) {
					returnPrompt(ui->board);
					return 0;
				}
				if (ui->tryMove(testMove)) {
					ui->drawBoard();
					break;
				}
			}
		}
	}
	
	return 0;
}

