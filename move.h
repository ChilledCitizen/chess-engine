#pragma once

#include <cstdint>
#include <tchar.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;

class Move {
private:
	// masks
	static uint64_t colorMask;
	static uint64_t startMask;	
	static uint64_t endMask;	
	static uint64_t pieceMask;	
	static uint64_t fSideKill;
	static uint64_t fCastling;
	static uint64_t fIsProm;
	static uint64_t fPromotionM;
	
	// int that IS the move
	uint64_t theMove;
public:
	Move(uint64_t);
	Move(bool, uint64_t, uint64_t, uint64_t); // basic move
	Move(bool, bool, bool); // castling constructor
	uint64_t getStart();
	uint64_t getEnd();
	uint64_t getPiece();
	uint64_t getColor();
	uint64_t getCastling();
	bool getSideKill();
	bool isPromotion();
	uint64_t getPromotion(); // returns actual promotion piece

	void setSideKill();
	void setPromotion(uint64_t);

	uint64_t getTheMove();
	bool operator==(Move);
};