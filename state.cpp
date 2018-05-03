#include "state.h"

//occupied, color
// 0 is white, 1 is black
uint64_t State::colorBase[2] =		{
									0b1111111111111111000000000000000000000000000000001111111111111111,
									0b1111111111111111000000000000000000000000000000000000000000000000
									};

//whites : 0 = soldier, 1 = rook, 2 = knight, 3 = bishop, 4 = queen, 5 = king, 
//blacks : 6 = soldier, 7 = rook, 8 = knight, 9 = bishop, 10 = queen, 11 = king
uint64_t State::piecesBase[12] =	{
									0b0000000000000000000000000000000000000000000000001111111100000000,
									0b0000000000000000000000000000000000000000000000000000000010000001,
									0b0000000000000000000000000000000000000000000000000000000001000010,
									0b0000000000000000000000000000000000000000000000000000000000100100,
									0b0000000000000000000000000000000000000000000000000000000000001000,
									0b0000000000000000000000000000000000000000000000000000000000010000,
									0b0000000011111111000000000000000000000000000000000000000000000000,
									0b1000000100000000000000000000000000000000000000000000000000000000,
									0b0100001000000000000000000000000000000000000000000000000000000000,
									0b0010010000000000000000000000000000000000000000000000000000000000,
									0b0000100000000000000000000000000000000000000000000000000000000000,
									0b0001000000000000000000000000000000000000000000000000000000000000,
									};


uint64_t colorTEST[2] =				{
									0b1001000111111111000000000000000010000001000000001111111110010001,
									0b1001000111111111000000000000000010000000000000000000000000000000
									};

uint64_t piecesTEST[12]	=			{
									0b0000000000000000000000000000000000000000000000001111111100000000,
									0b0000000000000000000000000000000000000000000000000000000010000001,
									0b0000000000000000000000000000000000000000000000000000000000000000,
									0b0000000000000000000000000000000000000000000000000000000000000000,
									0b0000000000000000000000000000000000000001000000000000000000000000,
									0b0000000000000000000000000000000000000000000000000000000000010000,
									0b0000000011111111000000000000000000000000000000000000000000000000,
									0b1000000100000000000000000000000000000000000000000000000000000000,
									0b0000000000000000000000000000000000000000000000000000000000000000,
									0b0000000000000000000000000000000000000000000000000000000000000000,
									0b0000000000000000000000000000000010000000000000000000000000000000,
									0b0001000000000000000000000000000000000000000000000000000000000000,
									};



State::~State() {
	delete moveArray;
}

State::State() {
	for (int i = 0; i < 12; i++) {
		_pieces[i] = piecesBase[i];
	}
	for (int i = 0; i < 2; i++) {
		_color[i] = colorBase[i];
	}
	movesSet = false;
	moveArray = nullptr;
	playerTurn = 0;
	sideKillIndex = -1;
	sideKillPieceInd = -1;
	attackMap = 0;

	enemyAttackMap = 0;
	enemyAttackMapSet = false;

	wKingMoved = false;
	wdsRookMoved = false;
	wksRookMoved = false;
	bKingMoved = false;
	bdsRookMoved = false;
	bksRookMoved = false;
	lateGame = false;
}

State::State(bool hue) {
	for (int i = 0; i < 12; i++) {
		_pieces[i] = piecesTEST[i];
	}
	for (int i = 0; i < 2; i++) {
		_color[i] = colorTEST[i];
	}
	movesSet = false;
	moveArray = nullptr;
	playerTurn = 0;
	sideKillIndex = -1;
	sideKillPieceInd = -1;
	attackMap = 0;

	enemyAttackMap = 0;
	enemyAttackMapSet = false;

	wKingMoved = false;
	wdsRookMoved = false;
	wksRookMoved = false;
	bKingMoved = false;
	bdsRookMoved = false;
	bksRookMoved = false;
	lateGame = false;
}

int State::getPlayerTurn() {
	return playerTurn;
}

void State::changeTurn() {
	if (playerTurn == 0) {
		playerTurn = 1;
	} else {
		playerTurn = 0;
	}
}


bool State::checkMoveLegality(Move* mov) {
	for (Move m : *moveArray) {
		/*
		wcout << m.getStart() << "-" << m.getEnd() << "-" << m.getColor() << "-" << m.getPiece() << "  " 
			<< mov->getStart() << "-" << mov->getEnd() << "-" << mov->getColor() << "-" << mov->getPiece() << endl;
		*/
		if (m == *mov) {
			return true;
		}
	}
	return false;
}


void State::updateBoard(Move* move) {
	uint64_t cast = move->getCastling();
	uint64_t color = move->getColor();
	if (!cast) {
		uint64_t start = move->getStart();
		uint64_t end = move->getEnd();
		uint64_t piece = move->getPiece();
		//wcout << color << " " << start << " " << end << " " << piece << endl;
		if (isOccupied(end)) {
			uint64_t pieceInd = getPiece(end);
			_pieces[pieceInd] &= ~(1ULL << end);
		}
		_color[0] &= ~(1ULL << start);
		_color[0] |= 1ULL << end;
		_color[1] |= color << end;
		_pieces[piece] &= ~(1ULL << start);
		if (move->getSideKill()) {
			_pieces[sideKillPieceInd] &= ~(1ULL << sideKillIndex);
		} else if (piece == 0) {
			if (((start / 8) == 1) && ((end / 8) == 3)) {
				sideKillIndex = end;
				sideKillPieceInd = piece;
			}
		} else if (piece == 6) {
			if (((start / 8) == 6) && ((end / 8) == 4)) {
				sideKillIndex = end;
				sideKillPieceInd = piece;
			}
		} else {
			sideKillIndex = (uint64_t)-1;
			if (piece == 1) {
				if (start == 0) {
					wdsRookMoved = true;
				}
				else if (start == 8) {
					wksRookMoved = true;
				}
			}
			else if (piece == 7) {
				if (start == 56) {
					bdsRookMoved = true;
				}
				else if (start == 63) {
					bksRookMoved = true;
				}
			}
			else if (piece == 5) {
				wKingMoved = true;
			}
			else if (piece == 11) {
				bKingMoved = true;
			}
		}
		if (move->isPromotion()) {
			uint64_t promInd = move->getPromotion();
			_pieces[promInd] |= 1ULL << end;
		}
		else {
			_pieces[piece] |= 1ULL << end;
		}
	} else {
		switch (color) {
		case 0:
			if (cast == 1) {
				_pieces[5] |= 1ULL << 6;
				_pieces[1] |= 1ULL << 5;
				_pieces[1] &= ~(1ULL << 7);
				_color[0] &= ~(1ULL << 7);
				_color[0] |= 1ULL << 6;
				_color[0] |= 1ULL << 5;
				_color[1] |= color << 6;
				_color[1] |= color << 5;
			}
			else if (cast == 2) {
				_pieces[5] |= 1ULL << 2;
				_pieces[1] |= 1ULL << 3;
				_pieces[1] &= ~(1ULL << 0);
				_color[0] &= ~(1ULL << 0);
				_color[0] |= 1ULL << 2;
				_color[0] |= 1ULL << 3;
				_color[1] |= color << 2;
				_color[1] |= color << 3;
			}
			_pieces[5] &= ~(1ULL << 4);
			_color[0] &= ~(1ULL << 4);
			wKingMoved = true;
			break;
		case 1:
			if (cast == 1) {
				_pieces[11] |= 1ULL << 62;
				_pieces[7] |= 1ULL << 61;
				_pieces[7] &= ~(1ULL << 63);
				_color[0] &= ~(1ULL << 63);
				_color[0] |= 1ULL << 62;
				_color[0] |= 1ULL << 61;
				_color[1] |= color << 62;
				_color[1] |= color << 61;
			}
			else if (cast == 2) {
				_pieces[11] |= 1ULL << 58;
				_pieces[7] |= 1ULL << 59;
				_pieces[7] &= ~(1ULL << 56);
				_color[0] &= ~(1ULL << 56);
				_color[0] |= 1ULL << 58;
				_color[0] |= 1ULL << 59;
				_color[1] |= color << 58;
				_color[1] |= color << 59;
			}
			_pieces[11] &= ~(1ULL << 60);
			_color[0] &= ~(1ULL << 60);
			bKingMoved = true;
			break;
		}
	}
	delete moveArray;
	moveArray = nullptr;
	movesSet = false;
	changeTurn();
}



// TODO: Set maps as static so they only need to be created once.
void State::setAllLegalMoves(bool kingCheck) {
	if (!movesSet) {
		moveArray = new list<Move>; // initializing from nullptr
		enemyAttackMapSet = false;
		if (!(_pieces[4] & _pieces[10])) {
			lateGame = true;
		}
		// castlings
		switch (playerTurn) {
		case 0:
			if (!wKingMoved) {
				if (!wksRookMoved) {
					uint64_t map = 1ULL << 6; map |= 1ULL << 5;
					if (!isOccupiedMulti(map)) {
						mapEnemyAttackMoves(1);
						if (!(enemyAttackMap & map)) {
							Move* mov = new Move(0, true, false);
							moveArray->push_back(*mov);
							delete mov;
						}
					}
				}
				if (!wdsRookMoved) {
					uint64_t map = 1ULL << 1; map |= 1ULL << 2; map |= 1ULL << 3;
					if (!isOccupiedMulti(map)) {
						mapEnemyAttackMoves(1);
						if (!(enemyAttackMap & map)) {
							Move* mov = new Move(0, false, true);
							moveArray->push_back(*mov);
							delete mov;
						}
					}
				}
			}
			break;
		case 1:
			if (!bKingMoved) {
				if (!bksRookMoved) {
					uint64_t map = 1ULL << 62; map |= 1ULL << 61;
					if (!isOccupiedMulti(map)) {
						mapEnemyAttackMoves(0);
						if (!(enemyAttackMap & map)) {
							Move* mov = new Move(1, true, false);
							moveArray->push_back(*mov);
							delete mov;
						}
					}
				}
				if (!bdsRookMoved) {
					uint64_t map = 1ULL << 57; map |= 1ULL << 58; map |= 1ULL << 59;
					if (!isOccupiedMulti(map)) {
						mapEnemyAttackMoves(0);
						if (!(enemyAttackMap & map)) {
							Move* mov = new Move(1, false, true);
							moveArray->push_back(*mov);
							delete mov;
						}
					}
				}
			}
			break;
		}
		enemyAttackMapSet = false;
		// other moves
		uint64_t pieceStorage = 0;
		unsigned long idx = 0;
		uint64_t mask = 0;
		for (int i = (playerTurn * 6); i < (6 + (playerTurn * 6)); i++) {
			pieceStorage = _pieces[i];
			while (pieceStorage) {
				BitScanForward64(&idx, pieceStorage);
				mask = 1ULL << idx;
				LegitMoves::getLegitMovesByIndex(i, idx, this, *moveArray, kingCheck);
				pieceStorage ^= mask;
			}
		}
		movesSet = true;
	}
}


void State::mapEnemyAttackMoves(int color) {
	if (!enemyAttackMapSet) {
		list<Move>* attackMoves = new list<Move>;
		enemyAttackMap = 0;
		uint64_t pieceStorage = 0;
		unsigned long idx = 0;
		uint64_t mask = 0;
		for (int i = (color * 6); i < (6 + (color * 6)); i++) {
			pieceStorage = _pieces[i];
			while (pieceStorage) {
				BitScanForward64(&idx, pieceStorage);
				mask = 1ULL << idx;
				LegitMoves::getLegitMovesByIndex(i, idx, this, *attackMoves, false);
				pieceStorage ^= mask;
			}
		}
		for (Move m : *attackMoves) {
			enemyAttackMap |= 1ULL << m.getEnd();
		}
		delete attackMoves;
		enemyAttackMapSet = true;
	}
}


uint64_t State::getPiece(uint64_t index) {
	unsigned long ind = 0;
	uint64_t mask = (uint64_t)0;
	uint64_t mask2 = (uint64_t)1 << index;
	for (int i = 0; i < 12; i++) {
		mask = _pieces[i] & mask2;
		if (mask != 0) {
			return (uint64_t)i;
		}
	}
	return 12; // empty index
}

bool State::isOccupied(uint64_t index) {
	uint64_t mask2 = 1ULL << index;
	return _color[0] & mask2;
}

bool State::isOccupiedMulti(uint64_t map) {
	return _color[0] & map;
}

bool State::getColor(uint64_t index) {
	uint64_t mask2 = 1ULL << index;
	return _color[1] & mask2;
	
}

uint64_t State::getSideKillIndex() {
	return sideKillIndex;
}


void State::setAttackMap() {
	attackMap = 0;
	for (Move m : *moveArray) {
		attackMap |= 1ULL << m.getEnd();
	}
}

bool State::checkAttackMap(uint64_t index) {
	if (attackMap & (1ULL << index)) {
		return true;
	}
	else {
		return false;
	}
}

void State::kingVunerability(bool color, Move* mov, list<Move> &array) {

	State* movState = createNew(mov);
	if (!color) {
		movState->mapEnemyAttackMoves(1);
		if (!(movState->enemyAttackMap & movState->_pieces[5])) {
			array.push_back(*mov);
		}
	}
	else {
		movState->mapEnemyAttackMoves(0);
		if (!(movState->enemyAttackMap & movState->_pieces[11])) {
			array.push_back(*mov);
		}
	}
	delete movState;
}

State* State::createNew(Move* mov) {
	State* aNew = new State();
	aNew->setFromState(this);
	aNew->updateBoard(mov);
	return aNew;
}

void State::setFromState(State* old) {
	for (int i = 0; i < 12; i++) {
		_pieces[i] = old->_pieces[i];
	}
	for (int i = 0; i < 2; i++) {
		_color[i] = old->_color[i];
	}
	movesSet = old->movesSet;
	moveArray = nullptr;
	playerTurn = old->playerTurn;
	sideKillIndex = old->sideKillIndex;
	sideKillPieceInd = old->sideKillPieceInd;
	attackMap = old->attackMap;

	enemyAttackMap = 0;
	enemyAttackMapSet = false;

	wKingMoved = old->wKingMoved;
	wdsRookMoved = old->wdsRookMoved;
	wksRookMoved = old->wksRookMoved;
	bKingMoved = old->bKingMoved;
	bdsRookMoved = old->bdsRookMoved;
	bksRookMoved = old->bksRookMoved;
	lateGame = old->lateGame;
}