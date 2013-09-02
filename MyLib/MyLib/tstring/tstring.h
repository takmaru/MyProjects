#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

namespace std {

#ifdef UNICODE

	#define	tcin	wcin
	#define	tcout	wcout
	#define	tcerr	wcerr
	#define	tclog	wclog

	typedef std::wstring		tstring;
	typedef std::wifstream		tifstream;
	typedef std::wofstream		tofstream;
	typedef std::wfstream		tfstream;
	typedef std::wistringstream	tistringstream;
	typedef std::wostringstream	tostringstream;
	typedef std::wstringstream	tstringstream;

#else

	#define	tcin	cin
	#define	tcout	cout
	#define	tcerr	cerr
	#define	tclog	clog

	typedef std::string			tstring;
	typedef std::ifstream		tifstream;
	typedef std::ofstream		tofstream;
	typedef std::fstream		tfstream;
	typedef std::istringstream	tistringstream;
	typedef std::ostringstream	tostringstream;
	typedef std::stringstream	tstringstream;

#endif


}
