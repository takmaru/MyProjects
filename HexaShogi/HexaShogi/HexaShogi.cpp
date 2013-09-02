// HexaShogi.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include "HexaField.h"

int wmain(int argc, wchar_t* argv[]) {
	std::locale::global(std::locale("", std::locale::ctype));

	std::wcout << CHexaField().description() << std::endl;

	return 0;
}

