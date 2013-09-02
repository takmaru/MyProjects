#include "stdafx.h"
#include "HexaField.h"

#include "Hexagon.h"
#include "CompareHexagon.h"

CHexaField::CHexaField(int size/*= 3*/) {
	for(int x = -1 * (size - 1); x <= (size - 1); x++) {
		for(int y = -1 * ((size - 1) + ((size -  1) - std::abs(x))); y <= (size - 1) + ((size -  1) - std::abs(x)); y += 2) {
			m_hexagonSet.insert((HexagonPtr)new CHexagon(x, y));
		}
	}
}

CHexaField::~CHexaField() {
}

std::wstring CHexaField::description() const {
	std::wostringstream oss;
	HexagonSet::const_iterator it;
	for(it = m_hexagonSet.begin(); it != m_hexagonSet.end(); ++it) {
		oss << (*it)->description() << std::endl;
	}
	return oss.str();
}