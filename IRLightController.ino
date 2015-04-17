// Debug related definitions
#define DEBUG
#ifdef DEBUG
  #define DEBUG_BEGIN() Serial.begin(9600)
  #define DEBUG_LOG(string) Serial.print(string)
  #define DEBUG_LOG_LN(string) Serial.println(string)
#else
  #define DEBUG_BEGIN()
  #define DEBUG_LOG(string)
  #define DEBUG_LOG_LN(string)
#endif
#define DEBUG_LOG_FREE_RAM() DEBUG_LOG(F("Free RAM: ")); DEBUG_LOG_LN(FreeRam())

// Define various pins
#define SD_CARD_PIN 4
#define ETHERNET_PIN 10

// Define NEC so that only the NEC sections of the IRRemote library are included
#define NEC

// Include header files
#include "Flash.h"
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Dhcp.h>
#include <SD.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <IRremote.h>

// Define Structures
struct MemorizedValues
{  
  byte red;
  byte green;
  byte blue;
  byte white;
};
struct TimerEntries
{
  byte code;
  byte weekday;
  byte hour;
  byte minute;
  byte second;
};

// Define global variables stored in ROM
static const PROGMEM unsigned long POWER_CODE = 0x20DF02FD;
static const PROGMEM unsigned long DAYLIGHT_CODE = 0x20DF58A7;
static const PROGMEM unsigned long M1_CODE = 0x20DF18E7;
static const PROGMEM unsigned long M2_CODE = 0x20DF9867;
static const PROGMEM unsigned long MOON_CODE = 0x20DFD827;
static const PROGMEM unsigned long RED_UP_CODE = 0x20DF0AF5;
static const PROGMEM unsigned long RED_DOWN_CODE = 0x20DF38C7;
static const PROGMEM unsigned long GREEN_UP_CODE = 0x20DF8A75;
static const PROGMEM unsigned long GREEN_DOWN_CODE = 0x20DFB847;
static const PROGMEM unsigned long BLUE_UP_CODE = 0x20DFB24D;
static const PROGMEM unsigned long BLUE_DOWN_CODE = 0x20DF7887;
static const PROGMEM unsigned long WHITE_UP_CODE = 0x20DF32CD;
static const PROGMEM unsigned long WHITE_DOWN_CODE = 0x20DFF807;

// Define global variables
EthernetServer server(80);

// Setup code
void setup()
{
  // Initialize the serial communication for debugging
  DEBUG_BEGIN();
  DEBUG_LOG_LN(F("Starting IR Light Controller sketch ..."));
  DEBUG_LOG_FREE_RAM();
  
  // Set the two SS pins to output mode and set them both to high
  pinMode(SD_CARD_PIN, OUTPUT);
  pinMode(ETHERNET_PIN, OUTPUT);
  digitalWrite(SD_CARD_PIN, HIGH);
  digitalWrite(ETHERNET_PIN, HIGH);

  // Start the SD card
  DEBUG_LOG_LN(F("Starting SD card ..."));
  if (!SD.begin(SD_CARD_PIN))
    DEBUG_LOG_LN(F("Failed to start SD card"));

  // Start the ethernet  
  DEBUG_LOG_LN(F("Starting ethernet ..."));
  uint8_t mac[] = { 0xDE, 0x12, 0x34, 0x56, 0x78, 0x90 };
  if (!Ethernet.begin(mac))
    DEBUG_LOG_LN(F("Failed to start ethernet"));

  // Log free memory
  DEBUG_LOG_FREE_RAM();
}

// Function called to handle the index page
void processWebRequest()
{
  // Check if there's data available to read from a client
  EthernetClient client = server.available();
  if (client)
  { 
    // Prepare needed variables
    char url[100];
    memset(&url, 0, 100);
    byte urlSize = 0;
    byte spacesFound = 0;

    // Loop while the client is connected
    while (client.connected()) 
    {
      // Check if there's data available to read
      if (client.available())
      {
        // Read a character
        char character = client.read();
        
        // Check if we've read a space character (the character that seperates the three entries 
        //   on the first line of the request header)
        if (character == ' ' && spacesFound < 2)
          ++spacesFound;
          
        // Add the character to the string (if we're reading the second entry which is found
        //   after the first space)
        if (spacesFound == 1 && urlSize < 100)
        {
          url[urlSize] = character;
          ++urlSize;
        }
      }
    }
    
    // Check if the root is being requested
    if (strcmp(url, "/") == 0)
      strcpy(url, "/index.html");
    
    // Log details
    DEBUG_LOG(F("URL requested: "));
    DEBUG_LOG_LN(url);
  
    // Check what kind of request this is
    if (strncmp(url, "/get?", 5) == 0)
    {
      // Get the data
      getData(client, url);
    }
    else 
    {
      // Check if we are saving data first
      if (strncmp(url, "/save?", 6) == 0)
      {
        // Save the data
        saveData(url);

        // Change the URL to the index page
        strcpy(url, "/index.html");
      }
      
      // Open the requsted file
      File file = SD.open(url);
      if (!file)
      {
        // Send an error message
        client.println(F("HTTP/1.1 404 File Not Found"));
        client.println(F("Content-Type: text/html"));
        client.println(F("Connection: close"));
        
        // Close the connection
        delay(1);
        client.stop();
        
        return;
      }
      
      // Send the file
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: "));
      client.println(F("Connection: close"));
      client.println(); 
      while (file.available())
      {
        client.write(file.read());
      }
      file.close();      
    }

    // Close the connection
    delay(1);
    client.stop();
  }
}

// Function called to get data
void getData(EthernetClient client, char url[])
{  
}

// Function called to save data
void saveData(char url[])
{
}

// Main code
void loop()
{
  // Process any web requests
  processWebRequest();
}