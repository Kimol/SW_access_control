#include "radio.h"

Radio::Radio(std::string raspberryName,
	std::string arduinoName,
	size_t arduinoNumber,
	const int radioCEPin,
	const int radioCSPin,
	size_t radioChannel,
	rf24_datarate_e radioDataRate) :
	raspberryName(raspberryName),
	arduinosNames(),
	radioCEPin(radioCEPin),
	radioCSPin(radioCSPin),
	radioChannel(radioChannel),
	radioDataRate(radioDataRate),
	alive(false)
{
	for (size_t i = 0; i < arduinoNumber; ++i)
	{
		std::cout << std::string(std::to_string(i + 1) + arduinoName) << std::endl;
        printf("Tu sie wywalam");
		arduinosNames.push_back(std::string(std::to_string(i + 1) + arduinoName));
        printf("Poof");
	}
}

void Radio::start()
{
	alive = true;
	radioThread = new std::thread(&Radio::run, this);
}

void Radio::stop()
{
	alive = false;
}


bool Radio::isAlive() const
{
	return alive;
}

void Radio::join() const
{
	if (radioThread != nullptr)
	{
		radioThread->join();
	}
}

void Radio::run()
{
    printf("Run began");
	RF24 radio(radioCEPin, radioCSPin);
	radio.begin();
	radio.setChannel(radioChannel);
	radio.setAutoAck(1);
	radio.setRetries(15, 15);
	radio.setDataRate(radioDataRate);
	radio.openWritingPipe((const uint8_t *)raspberryName.c_str());
	for (size_t i = 0; i < arduinosNames.size(); ++i)
	{
		std::cout << i+1 << ":" << (const uint8_t *)arduinosNames[i].c_str() << std::endl;
		radio.openReadingPipe(i + 1, (const uint8_t *)arduinosNames[i].c_str());
	}
	radio.startListening();
	radio.printDetails();

	while (alive)
	{
		if (radio.available())
		{
			DataSet data;
			while (radio.available())
			{
				radio.read(&data, sizeof(DataSet));
			}
		}
	}
}