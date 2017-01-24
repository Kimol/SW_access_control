#include <RF24_config.h>
#include <RF24.h>
#include <printf.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <MFRC522.h>

#define RFID_RST_PIN         9
#define RFID_SS_PIN          10

#define GREEN_LED_PIN        3
#define RED_LED_PIN          2

int radioCEPin = 7;
int radioCSPin = 8;
const uint8_t* raspberryAddress = (const uint8_t*)"Raspb";
String arduinoName = "Ardu";
const uint8_t* arduinoAddress = (const uint8_t*)"1Ardu";
int radioChannel = 100;
rf24_datarate_e radioDataRate = RF24_1MBPS;

RF24 radio(radioCEPin, radioCSPin);


MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);   // Create MFRC522 instance.

struct DataSet
{
    byte id[4];
    bool approval;
} data;

void setup()
{
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT); 
    Serial.begin(57600);
    while(!Serial);
    SPI.begin();
    mfrc522.PCD_Init();
    
    printf_begin();

    for (int i = 0 ; i < 4 ; ++i)
    {
      data.id[i] = 0;
    }

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
    digitalWrite(GREEN_LED_PIN, LOW);    
    digitalWrite(RED_LED_PIN, LOW);  
     
    if ( ! mfrc522.PICC_IsNewCardPresent())
      return;
    
    if ( ! mfrc522.PICC_ReadCardSerial())
      return;

    //if (isEqualToLast())
    //  return;
        
    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();   

    memcpy(data.id, mfrc522.uid.uidByte, mfrc522.uid.size); 
    
    // TODO: HANDLE CHECKING ID AND RESET LastCardUid!!!

    digitalWrite(GREEN_LED_PIN, HIGH);    
    digitalWrite(RED_LED_PIN, HIGH);              
    sendDataToPi();

    radio.stopListening();
    radio.openWritingPipe(raspberryAddress);
    radio.openReadingPipe(1, arduinoAddress);
    radio.startListening();

    while (true)
    {
      if (radio.available())
      {
        DataSet dataR;
        if (radio.available())
        {
          radio.read(&dataR, sizeof(DataSet));
          led(!dataR.approval);

          radio.stopListening();
          radio.openWritingPipe(arduinoAddress);
          radio.openReadingPipe(1, raspberryAddress);   
          radio.startListening();     
          break;
        }
      }
    }
    
    delay(2000);
}

void led(bool isApproved)
{
    if (isApproved)
    {      
      digitalWrite(GREEN_LED_PIN, HIGH);    
      digitalWrite(RED_LED_PIN, LOW);    
    }
    else
    {      
      digitalWrite(GREEN_LED_PIN, LOW);    
      digitalWrite(RED_LED_PIN, HIGH);    
      delay(100);      
      digitalWrite(GREEN_LED_PIN, LOW);    
      digitalWrite(RED_LED_PIN, LOW);        
      delay(100);     
      digitalWrite(GREEN_LED_PIN, LOW);    
      digitalWrite(RED_LED_PIN, HIGH);    
      delay(100);      
      digitalWrite(GREEN_LED_PIN, LOW);    
      digitalWrite(RED_LED_PIN, LOW);        
      delay(100); 
      digitalWrite(GREEN_LED_PIN, LOW);    
      digitalWrite(RED_LED_PIN, HIGH);    
    }
}
void sendDataToPi()
{
    radio.stopListening();

//    data.id[0] = lastCardUid[0];
//    data.id[1] = lastCardUid[1];
//    data.id[2] = lastCardUid[2];
//    data.id[3] = lastCardUid[3];    
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

