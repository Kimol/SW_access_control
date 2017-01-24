#include <iostream>
#include "radio.h"

int main()
{
    std::cout << "START" << std::endl;
 	Radio radio(std::string("Raspb"), std::string("1Ardu"), 25, 0);
	radio.start();
	radio.join();
	return 0;
}