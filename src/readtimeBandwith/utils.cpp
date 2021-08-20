#include "utils.h"


int64_t utils::getMilliseconds()
{
	timeb t;
	ftime(&t);
	return t.time * 1000+ t.millitm;
	return int64_t();
}

int64_t utils::getUsec()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}


