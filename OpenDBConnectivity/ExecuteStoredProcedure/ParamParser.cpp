#include "stdafx.h"
#include "ParamParser.h"

local::CParamPerser::CParamPerser(int argc, wchar_t* argv[]) :
	m_params(), m_options() {

	std::wstring option;
	for(int i = 0; i < argc; i++) {

		m_params.push_back(std::wstring(argv[i]));

		if(	(argv[i][0] == L'-') ||
			(argv[i][0] == L'/')	) {
			option = (wchar_t*)(&argv[i][1]);
		} else {
			parammap::iterator find = m_options.find(option);
			if(find != m_options.end()) {
				find->second.push_back(std::wstring(argv[i]));
			} else {
				paramlist params;
				params.push_back(std::wstring(argv[i]));
				m_options.insert(parampair(option, params));
			}
		}
	}
}

local::CParamPerser::~CParamPerser() {
}

int local::CParamPerser::count() const {
	return m_params.size();
}
std::wstring local::CParamPerser::operator[](int idx) const {
	return m_params[idx];
}

int local::CParamPerser::optionCount() const {
	return m_options.size();
}
bool local::CParamPerser::isOptionExist(const wchar_t* option) const {
	return (m_options.find(option) != m_options.end());
}
local::CParamPerser::paramlist local::CParamPerser::operator[](const wchar_t* option) const {
	paramlist params;
	parammap::const_iterator find = m_options.find(option);
	if(find != m_options.end()) {
		params = find->second;
	}
	return params;
}
