#include "utils.h"


int64_t utils::getMilliseconds()
{
	timeb t;
	ftime(&t);
	return t.time * 1000 + t.millitm;
	return int64_t();
}
