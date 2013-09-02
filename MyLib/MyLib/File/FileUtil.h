#pragma once

#include "../tstring/tstring.h"

namespace MyLib {
namespace File {

bool copy(std::tstring src, std::tstring dst, bool isOverride);
bool remove(std::tstring path);

}
}
