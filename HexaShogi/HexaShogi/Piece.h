#pragma once

#include "HexaShogiTypes.h"

class CPiece {
public:
	CPiece();
	virtual ~CPiece();

public:
	virtual std::wstring description() const;

private:
	HexaDirection m_dir;
};

