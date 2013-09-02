#include "stdafx.h"

#include "CompareHexagon.h"
#include "Hexagon.h"

bool CompareHexagon::operator()(const HexagonPtr& lhs, const HexagonPtr& rhs) {
	bool result = false;
	if(lhs->x() != rhs->x()) {
		result = (lhs->x() < rhs->x());
	} else {
		result = (lhs->y() < rhs->y());
	}
	return result;
}

