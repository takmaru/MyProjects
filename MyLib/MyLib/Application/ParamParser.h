#pragma once

#include <Windows.h>
#include <tchar.h>
#include <vector>
#include <list>
#include <map>
#include "../tstring/tstring.h"

namespace MyLib {
namespace App {

class CParamPerser {
private:
	struct comparenocase {
		bool operator()(const std::tstring& s1, const std::tstring& s2) const {
			return (_tcsicmp(s1.c_str(), s2.c_str()) < 0);
		}
	};

public:
	typedef std::list<std::tstring> paramlist;
private:
	typedef std::vector<std::tstring> paramvector;
	typedef std::map<std::tstring, paramlist, comparenocase> parammap;
	typedef std::pair<std::tstring, paramlist> parampair;

public:
	CParamPerser(int argc, TCHAR* argv[]);
	~CParamPerser();

public:
	int count() const;
	std::tstring operator[](int idx) const;
	int optionCount() const;
	bool isOptionExist(LPCTSTR option) const;
	bool isOptionExist(const std::tstring& option) const {
		return isOptionExist(option.c_str());
	}
	paramlist operator[](LPCTSTR option) const;
	paramlist operator[](const std::tstring& option) const {
		return (*this)[option.c_str()];
	}
	bool isOptionValueExist(LPCTSTR option) const;
	bool isOptionValueExist(const std::tstring& option) const {
		return isOptionValueExist(option.c_str());
	}

private:
	paramvector m_params;
	parammap m_options;
};

}
}