#pragma once

#include "HexaShogiTypes.h"

class CHexagon {
public:
	CHexagon();
	CHexagon(int x, int y);
	~CHexagon();

public:
	void setPiece(PiecePtr piece);
	void removePiece();
	bool existPiece() const;
	PiecePtr getPiece();

public:
	std::wstring description() const;

public:
	int x() const {
		return m_x;
	};
	int y() const {
		return m_y;
	};

private:
	int m_x;
	int m_y;

	PiecePtr m_piece;
};

