

#include "state.h"

// TODO: Combine legality check to one since everyone uses it

int soldierList[2] = { -1, 1 };
int knightDirlist1[8] = { 10, -6, -10, 6,  17, 15, -17, -15 }; // 1 row changes first
int knightDirlist2[8] = { 1,1,1,1,2,2,2,2 };
int towerDirlist1[4] = { 8, -8 , -1, 1 };
int towerDirlist2[4] = { 1,1,0,0 };
int bishopDirlist[4] = { -9,-7,9,7 };
int kingList[8] = { 8, -8,-1, 1,-9,-7,9,7 };
int kingListCheck[8] = { 1,1,0,0,1,1,1,1 };

// prevents infiniloop with attackMap creation
void kingVunerability(bool color, Move* mov, State* boardState, list<Move> &array, bool king) {
	if (king) {
		boardState->kingVunerability(color, mov, array);
	}
	else {
		array.push_back(*mov);
	}
	delete mov;
}

bool inBounds(int pos) {
	if (pos < 64 && pos >= 0) {
		return true;
	}
	else {
		return false;
	}
}

// also works for knight checking
bool inDiagonal(int start, int end, int cycle) {
	int result = abs(start / 8 - end / 8);
	if (result == cycle) {
		return true;
	}
	else {
		return false;
	}
}

bool inRow(int start, int end) {
	if (start / 8 == end / 8) {
		return true;
	}
	else {
		return false;
	}
}


void LegitMoves::getLegitMovesByIndex(int pieceIndex, int posIndex, State *boardState, list<Move> &array, bool kingVunerability) {
	switch (pieceIndex) {
	case 0:
		soldierMoves(posIndex, false, boardState, array, kingVunerability, pieceIndex);
		break;
	case 1:
		towerMoves(posIndex, false, boardState, array, kingVunerability, pieceIndex);
		break;
	case 2:
		knightMoves(posIndex, false, boardState, array, kingVunerability, pieceIndex);
		break;
	case 3:
		bishopMoves(posIndex, false, boardState, array, kingVunerability, pieceIndex);
		break;
	case 4:
		queenMoves(posIndex, false, boardState, array, kingVunerability, pieceIndex);
		break;
	case 5:
		kingMoves(posIndex, false, boardState, array, kingVunerability, pieceIndex);
		break;
	case 6:
		soldierMoves(posIndex, true, boardState, array, kingVunerability, pieceIndex);
		break;
	case 7:
		towerMoves(posIndex, true, boardState, array, kingVunerability, pieceIndex);
		break;
	case 8:
		knightMoves(posIndex, true, boardState, array, kingVunerability, pieceIndex);
		break;
	case 9:
		bishopMoves(posIndex, true, boardState, array, kingVunerability, pieceIndex);
		break;
	case 10:
		queenMoves(posIndex, true, boardState, array, kingVunerability, pieceIndex);
		break;
	case 11:
		kingMoves(posIndex, true, boardState, array, kingVunerability, pieceIndex);
		break;
	}
}

// May be able to omit checks for steps
void LegitMoves::soldierMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kingVn, int pi) {
	int startRow = -1;
	int endRow = -1;
	int multiplier = 0;
	int pieceIndex = -1;
	if (!color) {
		startRow = 1;
		endRow = 7;
		multiplier = 8;
		pieceIndex = 0;
	} else {
		startRow = 6;
		endRow = 0;
		multiplier = -8;
		pieceIndex = 6;
	}

	int firstStep = posIndex + multiplier;
	if (inBounds(firstStep)) {
		if (!(boardState->isOccupied(firstStep))) {
			soldierBasicMoveCreator(posIndex, firstStep, color, boardState, array, kingVn, pi); // first step
		}
	}
	
	if (posIndex / 8 == startRow) {
		int secondStep = firstStep + multiplier;
		if (inBounds(secondStep)) {
			if (!(boardState->isOccupied(secondStep)) && !(boardState->isOccupied(firstStep))) {
				soldierBasicMoveCreator(posIndex, secondStep, color, boardState, array, kingVn, pi); // secondStep
			}
		}
	}

	//attacks & side Kill
	for (int i = 0; i < 2; i++) {
		int newIndex = firstStep + soldierList[i];
		int sideIndex = posIndex + soldierList[i];
		if (inDiagonal(posIndex, newIndex, 1) && inBounds(newIndex)) {
			if (boardState->isOccupied(newIndex) && boardState->getColor(newIndex) != color) {
				soldierBasicMoveCreator(posIndex, newIndex, color, boardState, array, kingVn, pi);
			}
		}
		if (inDiagonal(posIndex, sideIndex, 0)) {
			if (boardState->getSideKillIndex() == sideIndex) {
				Move* mov = new Move(color, posIndex, newIndex, pieceIndex);
				mov->setSideKill();
				kingVunerability(color, mov, boardState, array, kingVn);
			}
		}
	}
}



void LegitMoves::knightMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kingVn, int pi) {
	int newIndex = 0;
	for (int i = 0; i < 8; i++) {
		newIndex = posIndex + knightDirlist1[i];
		if (inDiagonal(posIndex, newIndex, knightDirlist2[i]) && inBounds(newIndex)) {
			tbqInsideMoves(posIndex, newIndex, color, boardState, array, kingVn, pi);
		}
	}
}

void LegitMoves::kingMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kingVn, int pi) {
	int newIndex = 0;
	for (int i = 0; i < 8; i++) {
		newIndex = posIndex + kingList[i];
		if (inDiagonal(posIndex, newIndex, kingListCheck[i]) && inBounds(newIndex)) {
			tbqInsideMoves(posIndex, newIndex, color, boardState, array, kingVn, pi);
		}
	}
}

void LegitMoves::towerMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kingVn, int pi) {
	int newIndex = 0;
	int j = 0;
	bool cont = false;
	for (int i = 0; i < 4; i++) {
		j = 1;
		while (true) {
			newIndex = posIndex + towerDirlist1[i] * j;
			if (inBounds(newIndex) && inDiagonal(posIndex, newIndex, towerDirlist2[i]*j)) {
				cont = tbqInsideMoves(posIndex, newIndex, color, boardState, array, kingVn, pi);
				if (!cont) {
					break;
				}
			}
			else {
				break;
			}
			j++;
		}
	}
}

void LegitMoves::bishopMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kingVn, int pi) {
	int newIndex = 0;
	int j = 0;
	bool cont = false;
	for (int i = 0; i < 4; i++) {
		j = 1;
		while (true) {
			newIndex = posIndex + bishopDirlist[i] * j;
			if (inDiagonal(posIndex, newIndex, j) && inBounds(newIndex)) {
				cont = tbqInsideMoves(posIndex, newIndex, color, boardState, array, kingVn, pi);
				if (!cont) {
					break;
				}
			}
			else {
				break;
			}
			j++;
		}
	}
}

void LegitMoves::queenMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kingVn, int pi) {
	bishopMoves(posIndex, color, boardState, array, kingVn, pi);
	towerMoves(posIndex, color, boardState, array, kingVn, pi);
}


bool LegitMoves::tbqInsideMoves(int startIndex, int checkIndex, bool color, State *boardState, list<Move> &array, bool kingVn, int pi) {
	if (boardState->isOccupied(checkIndex)) {
		if (boardState->getColor(checkIndex) != color) {
			Move* mov = new Move(color, startIndex, checkIndex, pi);
			kingVunerability(color, mov, boardState, array, kingVn);
		}
		return false;
	}
	else {
		Move* mov = new Move(color, startIndex, checkIndex, pi);
		kingVunerability(color, mov, boardState, array, kingVn);
		return true;
	}

}


void LegitMoves::soldierBasicMoveCreator(int startIndex, int endIndex, bool color, State* boardState, list<Move> &array, bool kingVn, int pi) {
	if (!color) {
		if (endIndex / 8 == 7) {
			for (int i = 0; i < 4; i++) {
				Move* mov = new Move(color, startIndex, endIndex, pi);
				mov->setPromotion(i);
				kingVunerability(color, mov, boardState, array, kingVn);
			}
		}
		else {
			Move* mov = new Move(color, startIndex, endIndex, pi);
			kingVunerability(color, mov, boardState, array, kingVn);
		}
	}
	else {
		if (endIndex / 8 == 0) {
			for (int i = 0; i < 4; i++) {
				Move* mov = new Move(color, startIndex, endIndex, pi);
				mov->setPromotion(i);
				kingVunerability(color, mov, boardState, array, kingVn);
			}
		}
		else {
			Move* mov = new Move(color, startIndex, endIndex, pi);
			kingVunerability(color, mov, boardState, array, kingVn);
		}
	}
}


