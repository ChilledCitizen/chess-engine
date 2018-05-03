#pragma once

#include "state.h"
#include "eValues.h"
#include <thread>
#include <vector>
#include <future>

Move* chooseAIMove(State*, int depth);

int alphaBetaMax(State*, int, int, int);
int alphaBetaMin(State*, int, int, int);
int evaluate(State*);

int getMaterialState(State*);

int overAlpha = -100000;
int overBeta = 100000;

int materialList[12] =	{
						 100, 500, 320, 330, 900, 20000, 
						-100,-500,-320,-330,-900,-20000
						};

int midList(int piece, int index) {
	switch (piece) {
	case 0:
		return posWPawnMid[index];
	case 1:
		return posWRookMid[index];
	case 2:
		return posWKnightMid[index];
	case 3:
		return posWBishopMid[index];
	case 4:
		return posWQueenMid[index];
	case 5:
		return posWKingMid[index];
	case 6:
		return posBPawnMid[index];
	case 7:
		return posBRookMid[index];
	case 8:
		return posBKnightMid[index];
	case 9:
		return posBBishopMid[index];
	case 10:
		return posBQueenMid[index];
	case 11:
		return posBKingMid[index];
	default:
		return 0;
	}
}
int lateList(int piece, int index) {
	switch (piece) {
	case 0:
		return posWPawnLate[index];
	case 1:
		return posWRookLate[index];
	case 2:
		return posWKnightLate[index];
	case 3:
		return posWBishopLate[index];
	case 4:
		return posWQueenLate[index];
	case 5:
		return posWKingLate[index];
	case 6:
		return posBPawnLate[index];
	case 7:
		return posBRookLate[index];
	case 8:
		return posBKnightLate[index];
	case 9:
		return posBBishopLate[index];
	case 10:
		return posBQueenLate[index];
	case 11:
		return posBKingLate[index];
	default:
		return 0;
	}
}


int alphaBetaMax(State* currentStateMa, int alpha, int beta, int depth) {
	if (depth == 0) {
		return evaluate(currentStateMa);
	}
	currentStateMa->setAllLegalMoves(false);
	int score = 0;
	State* aNewMa = nullptr;
	for (Move m : *(currentStateMa->moveArray)) {
		aNewMa = currentStateMa->createNew(&m);
		score = alphaBetaMin(aNewMa, alpha, beta, depth - 1);
		delete aNewMa;
		aNewMa = nullptr;
		if (score >= beta) {
			return beta;
		}
		if (score > alpha) {
			alpha = score;
		}
	}
	if (currentStateMa->moveArray->size() == 0) {
		return beta;
	}
	return alpha;
}


int alphaBetaMin(State* currentStateMi, int alpha, int beta, int depth) {
	if (depth == 0) {
		return evaluate(currentStateMi);
	}
	currentStateMi->setAllLegalMoves(false);
	int score = 0;
	State* aNewMi = nullptr;
	for (Move m : *(currentStateMi->moveArray)) {
		aNewMi = currentStateMi->createNew(&m);
		score = alphaBetaMax(aNewMi, alpha, beta, depth - 1);
		delete aNewMi;
		aNewMi = nullptr;
		if (score <= alpha) {
			return alpha;
		}
		if (score < beta) {
			beta = score;
		}
	}
	if (currentStateMi->moveArray->size() == 0) {
		return alpha;
	}
	return beta;
}


int evaluate(State* currentState) {
	int mat = getMaterialState(currentState);
	return mat;
}

int getMaterialState(State* currentState) {
	uint64_t mask = 0;
	int mat = 0;
	int mobilityOverall = 0;
	uint64_t pieceStorage = 0;
	unsigned long idx = 0;
	for (int i = 0; i < 12; i++) {
		pieceStorage = currentState->_pieces[i];
		while (pieceStorage) {
			BitScanForward64(&idx, pieceStorage);
			mask = 1ULL << idx;
			pieceStorage ^= mask;
			if (currentState->lateGame) {
				mat += lateList(i, idx);
			}
			else {
				mat += midList(i, idx);
			}
			mat += materialList[i];
		}
	}
	return mat;
}


Move* chooseAIMove(State* currentState, int depth) {
	currentState->setAllLegalMoves(true);
	if (currentState->moveArray->size() == 0) {
		return nullptr;
	}
	Move* chosenMove = nullptr;
	int chosenScore = 0;
	int moveScore = 0;
	State* movedState = nullptr;
	int INDEXof = 0;
	int counter = 0;
	//depth -= 1;
	if (currentState->getPlayerTurn()) { // black
		chosenScore = 20000;
		list<future<int>> futures;
		vector<uint64_t> moves;
		vector<State*> states;
		for (Move m : *(currentState->moveArray)) {
			movedState = currentState->createNew(&m);
			moves.push_back(m.getTheMove());
			futures.push_back(std::async(alphaBetaMax,movedState, -100000, 100000, depth));
			states.push_back(movedState);
		}
		for (auto &&i : futures) {
			int getted = i.get();
			if (chosenScore > getted) {
				chosenScore = getted;
				INDEXof = counter;
			}
			counter++;
			wcout << counter << " and score " << getted << endl;
		}
		for (State* st : states) {
			delete st;
		}
		uint64_t daMove = moves[INDEXof];
		chosenMove = new Move(daMove);
	}
	else { // white
		chosenScore = 20000;
		list<future<int>> futures;
		vector<uint64_t> moves;
		vector<State*> states;
		for (Move m : *(currentState->moveArray)) {
			movedState = currentState->createNew(&m);
			moves.push_back(m.getTheMove());
			futures.push_back(std::async(alphaBetaMin, movedState, -100000, 100000, depth));
			states.push_back(movedState);
		}
		for (auto &&i : futures) {
			int getted = i.get();
			if (chosenScore < getted) {
				chosenScore = getted;
				INDEXof = counter;
			}
			counter++;
			wcout << counter << endl;
		}
		for (State* st : states) {
			delete st;
		}
		uint64_t daMove = moves[INDEXof];
		chosenMove = new Move(daMove);
	}

	return chosenMove;
}