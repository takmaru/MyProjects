#include "stdafx.h"
#include "Hexagon.h"

#include "Piece.h"

CHexagon::CHexagon() : m_x(0), m_y(0), m_piece() {
}

CHexagon::CHexagon(int x, int y) : m_x(x), m_y(y), m_piece() {
}

CHexagon::~CHexagon() {
}

void CHexagon::setPiece(PiecePtr piece) {
	m_piece = piece;
}
void CHexagon::removePiece() {
	m_piece.reset();
}
bool CHexagon::existPiece() const {
	return m_piece;
}
PiecePtr CHexagon::getPiece() {
	return m_piece;
}

std::wstring CHexagon::description() const {
	std::wostringstream oss;
	oss << L"(" << m_x << L", " << m_y << L")";
	if(m_piece) {
		oss << L" on " << m_piece->description();
	}
	return oss.str();
}
