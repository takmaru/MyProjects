#pragma once

namespace local {

class CParamPerser {
private:
	struct comparenocase {
		bool operator()(const std::wstring& s1, const std::wstring& s2) const {
			return (_wcsicmp(s1.c_str(), s2.c_str()) < 0);
		}
	};

public:
	typedef std::list<std::wstring> paramlist;
private:
	typedef std::vector<std::wstring> paramvector;
	typedef std::map<std::wstring, paramlist, comparenocase> parammap;
	typedef std::pair<std::wstring, paramlist> parampair;

public:
	CParamPerser(int argc, wchar_t* argv[]);
	~CParamPerser();

public:
	int count() const;
	std::wstring operator[](int idx) const;
	int optionCount() const;
	bool isOptionExist(const wchar_t* option) const;
	paramlist operator[](const wchar_t* option) const;

private:
	paramvector m_params;
	parammap m_options;
};

};