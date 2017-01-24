#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <RF24/RF24.h>
#include "data_set.h"

class Radio
{
public:
	Radio(std::string raspberryName,
		std::string adruinoName,
		const int radioCEPin,
		const int radioCSPin,
		size_t radioChannel = 100,
		rf24_datarate_e radioDataRate = RF24_1MBPS);
	void start();
	void stop();
	void join() const;
	bool isAlive() const;
private:
	void run();
	std::string raspberryName;
	std::string arduinosName;
	const int radioCEPin;
	const int radioCSPin;
	size_t radioChannel = 100;
	rf24_datarate_e radioDataRate = RF24_1MBPS;
	std::thread * radioThread;
	bool alive;
};