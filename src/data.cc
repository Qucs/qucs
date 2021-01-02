
#include "data.h"

Data::~Data()
{
	CommonData::attach(nullptr, &_common);
}
