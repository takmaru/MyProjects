#include "stdafx.h"
#include "ParamParser.h"

MyLib::App::CParamPerser::CParamPerser(int argc, TCHAR* argv[]) :
	m_params(), m_options() {

	std::tstring option;
	for(int i = 0; i < argc; i++) {

		m_params.push_back(std::tstring(argv[i]));

		if(	(argv[i][0] == L'-') ||
			(argv[i][0] == L'/')	) {
			option = (LPTSTR)(&argv[i][1]);
		} else {
			parammap::iterator find = m_options.find(option);
			if(find != m_options.end()) {
				find->second.push_back(std::tstring(argv[i]));
			} else {
				paramlist params;
				params.push_back(std::tstring(argv[i]));
				m_options.insert(parampair(option, params));
			}
		}
	}
}

MyLib::App::CParamPerser::~CParamPerser() {
}

int MyLib::App::CParamPerser::count() const {
	return m_params.size();
}
std::tstring MyLib::App::CParamPerser::operator[](int idx) const {
	return m_params[idx];
}

int MyLib::App::CParamPerser::optionCount() const {
	return m_options.size();
}
bool MyLib::App::CParamPerser::isOptionExist(LPCTSTR option) const {
	return (m_options.find(option) != m_options.end());
}
MyLib::App::CParamPerser::paramlist MyLib::App::CParamPerser::operator[](LPCTSTR option) const {
	paramlist params;
	parammap::const_iterator find = m_options.find(option);
	if(find != m_options.end()) {
		params = find->second;
	}
	return params;
}
bool MyLib::App::CParamPerser::isOptionValueExist(LPCTSTR option) const {
	parammap::const_iterator find = m_options.find(option);
	return (	(find != m_options.end()) &&
				(find->second.size() > 0)	);
}
