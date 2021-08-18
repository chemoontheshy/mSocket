#ifndef  UTILS_H
#define  UTILS_H
#include <sys/timeb.h>
#include <iostream>
#include <ctime>
#include<chrono>

namespace utils
{
	int64_t getMilliseconds();
	int64_t getUsec();
}





#endif