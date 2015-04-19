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

// Define settings
#define SD_CARD_PIN 4
#define ETHERNET_PIN 10
#define NTP_MAX_POLLS 10
#define NTP_POLL_INTERVAL 150 // Specified in milliseconds
#define URL_MAX_LENGTH 100 // Must be evenly divisible by 5 and at least 60 characters
#define TIMEZONE_OFFSET (4 * SECS_PER_HOUR) // Specified in seconds
#define DST_START_MONTH 3
#define DST_START_DAY (14 - ((1 + current.Year * 5 / 4) % 7))
#define DST_START_HOUR 2
#define DST_OFFSET (1 * SECS_PER_HOUR) // Specified in seconds
#define DST_END_MONTH 11
#define DST_END_DAY (7 - ((1 + current.Year * 5 / 4) % 7))
#define DST_END_HOUR 2

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
static EthernetServer server(80);

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

  // Start the server
  DEBUG_LOG_LN(F("Starting server ..."));
  server.begin();
  
  // Start the time sync
  DEBUG_LOG_LN(F("Synching time with NTP server ..."));
  setSyncProvider(&getNTPTime);
  setSyncInterval(3600);
  while (timeStatus() == timeNotSet);
  
  // Log free memory
  DEBUG_LOG_FREE_RAM();
}

// Function called to get the time from a NTP server
unsigned long getNTPTime()
{
  // Open a UDP socket
  DEBUG_LOG_LN(F("Opening UDP port ..."));
  EthernetUDP udp;
  if (!udp.begin(8888))
  {
    DEBUG_LOG_LN(F("Failed to open UDP port"));
    return 0;
  }

  // Clear any previously received data
  udp.flush();
    
  // Create the packet
  // Note: only the first four bytes of an outgoing NTP packet need to be set
  const unsigned long packet = 0xEC0600E3;
  
  // Send the NTP request
  DEBUG_LOG_LN(F("Sending UDP packet ..."));
  const char timeServer[] = "pool.ntp.org";
  if (!(udp.beginPacket(timeServer, 123) && udp.write((byte *)&packet, 48) == 48 && udp.endPacket()))
  {
    DEBUG_LOG_LN(F("Failed to send UDP packet"));
    return 0;
  }

  // Check for a response
  DEBUG_LOG_LN(F("Checking for response ..."));
  int size;
  for (byte i = 0; i < NTP_MAX_POLLS; ++i)
  {
    // Check if we have a full packet waiting to be read
    if ((size = udp.parsePacket()) >= 48)
      break;
    
    // Delay 150ms
    delay(NTP_POLL_INTERVAL);
  }
  
  // Log details
  DEBUG_LOG(F("Received "));
  DEBUG_LOG(size);
  DEBUG_LOG_LN(F(" byte(s)"));
  
  // Make sure we receive a full packet
  if (size < 48)
    return 0;    
    
  // Read and discard the first 40 bytes
  for (byte i = 0; i < 40; ++i)
    udp.read();

  // Read the next 4 bytes
  time_t time = udp.read();
  for (byte i = 1; i < 4; ++i)
    time = time << 8 | udp.read();

  // Discard the rest of the packet
  udp.flush();
  
  // Close the UPD connection
  udp.stop();

  // Adjust the time
  time = time - 2208988800UL + TIMEZONE_OFFSET;
  
  // Adjust for DST
  TimeElements current;
  breakTime(time, current);
  if ((current.Month > DST_START_MONTH && current.Month < DST_END_MONTH) || 
      (current.Month == DST_START_MONTH && current.Day >= DST_START_DAY && current.Hour >= DST_START_HOUR) ||
      (current.Month == DST_END_MONTH && current.Day <= DST_END_DAY && current.Hour < DST_END_HOUR - 
      (DST_OFFSET / SECS_PER_HOUR)))
    time = time + DST_OFFSET;

  // Log details
  DEBUG_LOG(F("Received/Calculated Unix time: "));
  DEBUG_LOG_LN(time);
  
  return time;
}

// Function called to handle the index page
void inline processWebRequest()
{
  // Check if there's data available to read from a client
  EthernetClient client = server.available();
  if (client)
  { 
    // Prepare needed variables
    char* string = (char*)malloc(URL_MAX_LENGTH / 5);
    memset(&string, 0, URL_MAX_LENGTH / 5);
    byte stringSize = 0;
    byte stringMaxSize = URL_MAX_LENGTH / 5;
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
        {
          // Increment the space counter
          ++spacesFound;

          // Check if we've just finished reading the first entry
          if (spacesFound == 1)
          {
            // Make sure this is a GET request
            if (strcasecmp(string, "GET") != 0)
            {
              // Free memory
              free(string);
            
              // Send an error message
              client.println(F("HTTP/1.1 405 Method Not Allowed"));
              client.println(F("Content-Type: text/html"));
              client.println(F("Connection: close"));
              
              // Close the connection
              delay(1);
              client.stop();
              
              return;
            }
            
            // Clear the string to start reading the next entry
            memset(&string, 0, stringMaxSize);     
          }
        }
          
        // Add the character to the string (if we're reading the first or second entry)
        if (spacesFound < 2 && stringSize < URL_MAX_LENGTH - 1)
        {
          // Add the character to the string and increase the size counter
          string[stringSize] = character;
          ++stringSize;
          
          // Check if we have to increase the size of the string
          if (stringSize == stringMaxSize)
          {
            // Allocate more memory for the string, zero out the new memory, and adjust the max string size
            realloc(string, URL_MAX_LENGTH / 5);
            memset(string + stringSize, 0, URL_MAX_LENGTH / 5);
            stringMaxSize += URL_MAX_LENGTH / 5;
          }
        }
      }
    }
    
    // Check if the root is being requested
    if (strcmp(string, "/") == 0)
      strcpy(string, "/index.html");
    
    // Log details
    DEBUG_LOG(F("URL requested: "));
    DEBUG_LOG_LN(string);
  
    // Check what kind of request this is
    if (strncasecmp(string, "/get?", 5) == 0)
    {
      // Get the data
      getData(client, string);
    }
    else 
    {
      // Check if we are saving data first
      if (strncasecmp(string, "/save?", 6) == 0)
      {
        // Save the data
        saveData(client, string);

        // Change the URL to the index page
        strcpy(string, "/index.html");
      }
      
      // Open the requsted file
      File file = SD.open(string);
      if (!file)
      {
        // Free memory
        free(string);
      
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
  
    // Free memory
    free(string);

    // Close the connection
    delay(1);
    client.stop();
  }
}

// Function called to get data
void inline getData(EthernetClient client, char url[])
{  
}

// Function called to save data
void inline saveData(EthernetClient client, char url[])
{
}

// Main code
void loop()
{
  // Process any web requests
  processWebRequest();
  
  // Process any alarms
  Alarm.delay(0);
}
