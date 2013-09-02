#pragma once

#include "HexaShogiTypes.h"

struct CompareHexagon {
public:
	bool operator()(const HexagonPtr& lhs, const HexagonPtr& rhs);
};
