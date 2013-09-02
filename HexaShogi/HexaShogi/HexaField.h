#pragma once

#include "HexaShogiTypes.h"
#include "CompareHexagon.h"

class CHexaField {
private:
	typedef std::set<HexagonPtr, CompareHexagon> HexagonSet;

public:
	explicit CHexaField(int size = 3);
	~CHexaField();

public:
	std::wstring description() const;

private:
	HexagonSet m_hexagonSet;
};

