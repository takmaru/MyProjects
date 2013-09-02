#include "stdafx.h"
#include "Piece.h"

CPiece::CPiece() {
}

CPiece::~CPiece() {
}

std::wstring CPiece::description() const {
	std::wostringstream oss;
	return oss.str();
}