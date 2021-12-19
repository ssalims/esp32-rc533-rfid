/* WITH LIVE READING FROM DHT11 - SZS
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

*/

////*** boleh biarkn part ni sbb ada option utk guna esp32 @ NodeMCU
#ifdef ESP32
  // this will compile for ESP32 board
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  // this will compile for ESP8266, NodeMCU boards
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

///***RFID initialization
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10 //21
#define RST_PIN 9 //22
//pin assignment 
//RC522 esp32 uno
//SCK    18   13
//MISO   19   12
//SDA/SS 21   10
//RST    22    9
//MOSI   23   11
//
//GND   GND
//3v3   3v3


MFRC522 rfid(SS_PIN, RST_PIN);  
MFRC522::MIFARE_Key key; 
// Init array that will store new NUID 
byte nuidPICC[4];
///utk ubah HEX uid ke String
char str[32] = "";
String StrUID;
///***

// Replace with your network credentials
const char* ssid     = "JTMK";
const char* password = "Cisco2020";

// REPLACE with your Domain name and URL path or server's IP address with path
const char* serverName = "ipadd latop + file php kamu";

///***terpulang nk letak atau x... lokasi kelas Class Room 223
String lokasiReader = "CR223";



void setup() {
  Serial.begin(115200);
  ///*** rfid
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  ///***
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Approach your card to the reader...");

}

void loop() {
//////////////////////////PROSESS KAD///////////////
// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  ///***Utk elak kad scan berturut...
  ///***kalau bacaan lama x sama dgn bacaan baru
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
      array_to_string(nuidPICC, 4, str); //function call utk ubah HEX ke String
      StrUID = str; 
    }
    Serial.print(F("The NUID tag is: "));
    Serial.println(StrUID); // UID HEX dlm bentuk String
  } ///***utk elak kad discan 2 kali
  else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
//////////////////////////END of PROSESS KAD///////////////
//////////////////////////Bahagian WiFi///////////////

  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    ///////String httpRequestData = "uid_card=" + StrUID + "&rlokasi=" + lokasiReader;  
    String httpRequestData = "uid_card=" + StrUID;                       
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
       
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  delay(600000);  //10 minit --- 1000 = 1saat, jadi 60,0000 = 60s*10 = 10 min
}

//----------------------------------------Procedure to change the result of reading an array UID into a string------------------------------------------------------------------------------//
void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
