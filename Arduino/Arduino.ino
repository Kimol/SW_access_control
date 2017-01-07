#include <RF24_config.h>
#include <RF24.h>
#include <printf.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <MFRC522.h>

#define RFID_RST_PIN         9
#define RFID_SS_PIN          10

int radioCEPin = 7;
int radioCSPin = 8;
const uint8_t* raspberryAddress = (const uint8_t*)"Raspb";
String arduinoName = "Ardu";
const uint8_t* arduinoAddress = (const uint8_t*)"1Ardu";
int radioChannel = 100;
rf24_datarate_e radioDataRate = RF24_1MBPS;

byte *lastCardUid = new byte[4];

RF24 radio(radioCEPin, radioCSPin);


MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);   // Create MFRC522 instance.

struct DataSet
{
    int id;
} data;

void setup()
{
    Serial.begin(57600);
    while(!Serial);
    SPI.begin();
    mfrc522.PCD_Init();
    
    printf_begin();

    data.id = 1;

    radio.begin();
    radio.setChannel(radioChannel);
    radio.setAutoAck(1);
    radio.setRetries(15, 15);
    radio.setDataRate(radioDataRate);
    radio.openWritingPipe(arduinoAddress);
    radio.openReadingPipe(1, raspberryAddress);
    radio.startListening();
    radio.printDetails();
    Serial.println("INITIALIZED");
}

void loop()
{
    if ( ! mfrc522.PICC_IsNewCardPresent())
      return;
    
    if ( ! mfrc522.PICC_ReadCardSerial())
      return;

    if (isEqualToLast())
      return;
        
    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();   

    // TODO: HANDLE CHECKING ID AND RESET LastCardUid!!!
                
    //sendDataToPi();
    //delay(600000);
}

void sendDataToPi()
{
    radio.stopListening();

    data.id = 100; // ADD CARD ID    
    if (!radio.write(&data, sizeof(DataSet)))
    {
        printf("Failed!\n");
    }
    else
    {
        printf("Done!\n");
    }
    radio.startListening();
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

bool isEqualToLast()
{
  bool equal = true;
  for (int i = 0 ; i < 4 ; ++i)
  {
    if (lastCardUid[i] != mfrc522.uid.uidByte[i])
    {
        equal = false;
    }
  }

  if (equal)
  {
    return true;
  }
  else
  {
    memcpy(lastCardUid, mfrc522.uid.uidByte, mfrc522.uid.size); 
    return false;
  }
}

