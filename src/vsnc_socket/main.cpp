#include "get_socket_data.h"

int main()
{
	std::unique_ptr<vsnc::vget::VSocket> vsocket;
	vsocket = std::make_unique<vsnc::vget::VSocket>(4000);
	uint8_t* data = new uint8_t[10000];
	size_t num = 0;
	while (num < 1000)
	{
		vsocket->getData(data, 10000);
		num++;
		std::cout << num << std::endl;
	}

	delete[] data;
	// 3.socket部分
	// socket初始化

}