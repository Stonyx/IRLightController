// Debug related definitions
#define DEBUG
#ifdef DEBUG
  #define DEBUG_LOG(string) Serial.println(string);
#else
  #define DEBUG_LOG(string)
#endif

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
#include "TinyWebServer.h"
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
TinyWebServer* gWebServer;
boolean gSDCardInitialized;

// Setup code
void setup()
{
#ifdef DEBUG
  // Initialize the serial communication
  Serial.begin(9600);
  Serial << F("Starting IR Light Controller sketch ...\n");
  Serial << F("Free RAM: ") << FreeRam() << F("\n");
#endif

  // Set the two SS pins to output mode and set them both to high
  pinMode(SD_CARD_PIN, OUTPUT);
  pinMode(ETHERNET_PIN, OUTPUT);
  digitalWrite(SD_CARD_PIN, HIGH);
  digitalWrite(ETHERNET_PIN, HIGH);

  // Initialize the SD card
  DEBUG_LOG(F("Starting SD card ..."));
  if (!SD.begin(SD_CARD_PIN))
  {
    DEBUG_LOG(F("Failed to start SD card."));
    gSDCardInitialized = false;
  }
  else
  {
    gSDCardInitialized = true;
  }

  // Initialize the ethernet  
  DEBUG_LOG(F("Starting ethernet ..."));
  uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  if (!Ethernet.begin(mac))
    DEBUG_LOG(F("Failed to start ethernet."));

  // Start the server
  DEBUG_LOG(F("Starting web server ..."));
  TinyWebServer::PathHandler handlers[] = {
    {"/", TinyWebServer::GET, &indexHandler},
    {"/" "*", TinyWebServer::GET, &fileHandler},
    {NULL},
  };
  gWebServer = new TinyWebServer(handlers, NULL, 80);
  gWebServer->begin();

#ifdef DEBUG
  Serial << F("Free RAM: ") << FreeRam() << F("\n");
#endif
}

// Function called to handle the index page
boolean indexHandler(TinyWebServer& webServer)
{

}

// Function called to handle serving files
boolean fileHandler(TinyWebServer& webServer) 
{
  // Check if the SD card was initialized
  if(!gSDCardInitialized)
  {
    webServer.send_error_code(500);
    webServer << F("Internal Server Error");
    return true;
  }

  // Get the request filename
  char* filename = TinyWebServer::get_file_from_path(webServer.get_path());
  if(!filename) 
  {
    webServer.send_error_code(400);
    webServer << F("Bad Request");
    return true;
  }

  // Open the file
  File file = SD.open(filename);
  if (file)
  {
    // Send the file
    webServer.send_error_code(200);
    webServer.send_content_type(TinyWebServer::get_mime_type_from_filename(filename));
    webServer.end_headers();
    // webServer.send_file(file);
    file.close();
  }
  else
  {
    webServer.send_error_code(404);
    webServer << F("404 - File Not Found");
  }

  // Free up memory
  free(filename);

  return true;
}

// Main code
void loop()
{
  // Process any web requests
  gWebServer->process();
}