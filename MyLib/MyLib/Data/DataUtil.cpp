#include "stdafx.h"
#include "DataUtil.h"

#include <time.h>
#include <algorithm>

MyLib::Data::BinaryData MyLib::Data::randomData(int datasize) {
	srand((unsigned int)time(NULL));
	MyLib::Data::BinaryData random(datasize, 0);
	std::generate(random.begin(), random.end(), rand);
	return random;
}
