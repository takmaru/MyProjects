#pragma once

#include "../tstring/tstring.h"

namespace MyLib {
namespace File {

std::tstring getIniFileString(const std::tstring& iniPath, const std::tstring& section, const std::tstring& key,
							  const std::tstring& defaultValue);
int getIniFileInteger(const std::tstring& iniPath, const std::tstring& section, const std::tstring& key,
					  int defaultValue);

}
}
