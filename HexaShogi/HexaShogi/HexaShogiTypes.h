#pragma once

enum HexaDirection {
	HxDir_F_C,
	HxDir_F_L,
	HxDir_F_R,
	HxDir_B_C,
	HxDir_B_L,
	HxDir_B_R
};

class CHexagon;
typedef std::shared_ptr<CHexagon> HexagonPtr;

class CPiece;
typedef std::shared_ptr<CPiece> PiecePtr;
