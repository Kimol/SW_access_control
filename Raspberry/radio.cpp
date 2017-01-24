#include "radio.h"

Radio::Radio(std::string raspberryName,
	std::string arduinoName,
	const int radioCEPin,
	const int radioCSPin,
	size_t radioChannel,
	rf24_datarate_e radioDataRate) :
	raspberryName(raspberryName),
	arduinosName(arduinoName),
	radioCEPin(radioCEPin),
	radioCSPin(radioCSPin),
	radioChannel(radioChannel),
	radioDataRate(radioDataRate),
	alive(false)
{
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
    printf("Run began\n");
	RF24 radio(radioCEPin, radioCSPin);
	radio.begin();
	radio.setChannel(radioChannel);
	radio.setAutoAck(1);
	radio.setRetries(15, 15);
	radio.setDataRate(radioDataRate);

	radio.openWritingPipe((const uint8_t *)raspberryName.c_str());
    radio.openReadingPipe(1, (const uint8_t *)arduinosName.c_str());

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
                printf("Data received!\n");
                printf("Data: %02x %02x %02x %02x\n", data.id[0], data.id[1], data.id[2], data.id[3]);
                
                radio.stopListening();
                radio.openWritingPipe((const uint8_t *)arduinosName.c_str());
                radio.openReadingPipe(1, (const uint8_t *)raspberryName.c_str());

                char command[50];
                sprintf(command, "python getHttp.py %02x%02x%02x%02x", data.id[0], data.id[1], data.id[2], data.id[3]);
                int code = system(command);

                data.approval = (bool) code; // TODO: ADD APPROVALS;
                if (radio.write(&data, sizeof(DataSet)))
                {
                    printf("Sucess");
                }
                else
                {
                    printf("Failure");
                }
                
                radio.openWritingPipe((const uint8_t *)raspberryName.c_str());
                radio.openReadingPipe(1, (const uint8_t *)arduinosName.c_str());
                radio.startListening();
			}
		}
	}
}