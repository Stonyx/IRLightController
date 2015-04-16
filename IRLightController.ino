// Define NEC so that only the NEC sections of the IRRemote library are included
#define NEC

// Include header files
#include <IRremote.h>

// Debug related definitions
#define DEBUG
#ifdef DEBUG
  #define DEBUG_LOG(string) Serial.println(string);
#else
  #define DEBUG_LOG(string)
#endif

// Global variables
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

// Global variables stored in ROM

// Setup code
void setup()
{
#ifdef DEBUG
  // Initialize the serial communication
  Serial.begin(9600);
  Serial.println("Starting IR Light Controller sketch ...");
#endif
}

// Main code
void loop()
{

}