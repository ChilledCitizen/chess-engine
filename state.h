#pragma once

#include "move.h"
#include <cstdint>
#include <list>

class State {
private:

	static uint64_t piecesBase[12];
	static uint64_t colorBase[2];


public:
	uint64_t _pieces[12] = {};
	uint64_t _color[2] = {};
	int playerTurn;
	bool movesSet;
	uint64_t sideKillIndex;
	uint64_t sideKillPieceInd;
	uint64_t attackMap;

	uint64_t enemyAttackMap;
	bool enemyAttackMapSet;

	bool wKingMoved;
	bool wdsRookMoved;
	bool wksRookMoved;
	bool bKingMoved;
	bool bdsRookMoved;
	bool bksRookMoved;

	bool lateGame;

public:
	list<Move>* moveArray = nullptr;
	State();
	State(bool);
	~State();
	void updateBoard(Move*);
	uint64_t getPiece(uint64_t);
	bool isOccupied(uint64_t index);
	bool isOccupiedMulti(uint64_t map);
	bool getColor(uint64_t); 
	uint64_t getSideKillIndex();
	void setAllLegalMoves(bool);
	bool checkMoveLegality(Move *mov);
	void setAttackMap();
	bool checkAttackMap(uint64_t index);

	void mapEnemyAttackMoves(int color);
	

	int getPlayerTurn();
	void changeTurn();


	void kingVunerability(bool color, Move* mov, list<Move> &array);
	State* createNew(Move* mov);
	void setFromState(State*);
};

class LegitMoves {
public:
	static void getLegitMovesByIndex(int pieceIndex, int posIndex, State *boardState, list<Move> &array, bool kingVunerability);

private:
	static void soldierMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kvn, int pi);
	static void knightMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kvn, int pi);
	static void kingMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kvn, int pi);
	static void towerMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kvn, int pi);
	static void bishopMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kvn, int pi);
	static void queenMoves(int posIndex, bool color, State *boardState, list<Move> &array, bool kvn, int pi);

	static void soldierBasicMoveCreator(int, int, bool color, State* boardState, list<Move> &array, bool kvn, int pi);
	static bool tbqInsideMoves(int , int , bool color, State *boardState, list<Move> &array, bool kvn, int pi);
};