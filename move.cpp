

#include "move.h"

// first bit is color of move
uint64_t Move::colorMask	=	0b0000000000000000000000000000000000000000000000000000000000000001; // 0
uint64_t Move::startMask	=	0b0000000000000000000000000000000000000000000000000000000001111110; // 1
uint64_t Move::endMask		=	0b0000000000000000000000000000000000000000000000000001111110000000; // 7
uint64_t Move::pieceMask	=	0b0000000000000000000000000000000000000000000000011110000000000000; // 13
uint64_t Move::fSideKill	=	0b0000000000000000000000000000000000000000000000100000000000000000; // 17
uint64_t Move::fCastling	=	0b0000000000000000000000000000000000000000000011000000000000000000;	// 18 & 19		// 00 = no, 01 = short, 10 = long
uint64_t Move::fIsProm		=	0b0000000000000000000000000000000000000000000100000000000000000000; // 20
uint64_t Move::fPromotionM	=	0b0000000000000000000000000000000000000000011000000000000000000000;	// 21		// 00 = tower, 01 = knight, 10 = bishop, 11 = queen




Move::Move(uint64_t gend) {
	theMove = gend;
}

Move::Move(bool color, uint64_t start, uint64_t end, uint64_t piece) {
	start <<= 1;
	end <<= (6 + 1);
	piece <<= (6 + 6 + 1);
	uint64_t colorInt = color;
	theMove = colorInt | start | end | piece;
}

Move::Move(bool color, bool shortC, bool longC) {
	uint64_t mv = color;
	mv |= shortC << 18;
	mv |= longC << 19;
	theMove = mv;
}

uint64_t Move::getStart() {
	uint64_t masked = startMask & theMove;
	masked >>= 1;
	return masked;
}
uint64_t Move::getEnd() {
	uint64_t masked = endMask & theMove;
	masked >>= (6 + 1);
	return masked;
}
uint64_t Move::getPiece() {
	uint64_t masked = pieceMask & theMove;
	masked >>= (6 + 6 + 1);
	return masked;
}
uint64_t Move::getColor() {
	uint64_t masked = colorMask & theMove;
	return masked;
}

uint64_t Move::getCastling() {
	uint64_t masked = fCastling & theMove;
	masked >>= 18;
	return masked;
}

bool Move::getSideKill() {
	uint64_t masked = fSideKill & theMove;
	return masked;
}

bool Move::isPromotion() {
	uint64_t masked = fIsProm & theMove;
	return masked;
}

uint64_t Move::getPromotion() {
	uint64_t masked = fPromotionM & theMove;
	masked >>= 21;
	masked += (getColor() * 6) + 1;
	return masked;
}

void Move::setSideKill() {
	theMove |= 1ULL << 17;
}

void Move::setPromotion(uint64_t prom) {
	theMove |= 1ULL << 20;
	theMove |= prom << 21;
}

uint64_t Move::getTheMove() {
	return theMove;
}

bool Move::operator==(Move other) {
	if (theMove == other.getTheMove()) {
		return true;
	}
	else {
		return false;
	}
}