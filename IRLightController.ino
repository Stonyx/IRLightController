// Debug related definitions
// #define DEBUG
#ifdef DEBUG
  #define DEBUG_SERIAL_BEGIN() Serial.begin(9600)
  #define DEBUG_LOG(string) Serial.print(string)
  #define DEBUG_LOG_LN(string) Serial.println(string)
#else
  #define DEBUG_SERIAL_BEGIN()
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
#include <IRremote.h>

// Define settings
#define SD_CARD_PIN 4
#define ETHERNET_PIN 10
#define NTP_MAX_POLLS 10
#define NTP_POLL_INTERVAL 150 // In milliseconds
#define TIMEZONE_OFFSET (4 * SECS_PER_HOUR) // In seconds
#define DST_START_MONTH 3
#define DST_START_DAY (14 - ((1 + current.Year * 5 / 4) % 7))
#define DST_START_HOUR 2
#define DST_OFFSET (1 * SECS_PER_HOUR) // In seconds
#define DST_END_MONTH 11
#define DST_END_DAY (7 - ((1 + current.Year * 5 / 4) % 7))
#define DST_END_HOUR 2
#define URL_MAX_LENGTH 50

// Define IR codes for Ecoxotic E-Series fixtures
#define SET_CLOCK_CODE 0x20DF3AC5
#define ON_TIME_CODE 0x20DFBA45
#define OFF_TIME_CODE 0x20DF827D
#define POWER_CODE 0x20DF02FD
#define HOUR_UP_CODE 0x20DF1AE5
#define MINUTE_DOWN_CODE 0x20DF9A65
#define ENTER_CODE 0x20DFA25D
#define RESUME_CODE 0x20DF22DD
#define SUNLIGHT_CODE 0x20DF2AD5
#define FULL_SPECTRUM_CODE 0x20DFAA55
#define CRISP_BLUE_CODE 0x20DF926D
#define DEEP_WATER_CODE 0x20DF12ED
#define RED_UP_CODE 0x20DF0AF5
#define GREEN_UP_CODE 0x20DF8A75
#define BLUE_UP_CODE 0x20DFB24D
#define WHITE_UP_CODE 0x20DF32CD
#define RED_DOWN_CODE 0x20DF38C7
#define GREEN_DOWN_CODE 0x20DFB847
#define BLUE_DOWN_CODE 0x20DF7887
#define WHITE_DOWN_CODE 0x20DFF807
#define M1_CODE 0x20DF18E7
#define M2_CODE 0x20DF9867
#define DAYLIGHT_CODE 0x20DF58A7
#define MOONLIGHT_CODE 0x20DFD827
#define DYNAMIC_MOON_CODE 0x20DF28D7
#define DYNAMIC_LIGHTNING_CODE 0x20DFA857
#define DYNAMIC_CLOUD_CODE 0x20DF6897
#define DYNAMIC_FADE_CODE 0x20DFE817

// Define IR codes for Current USA Satelite Plus & Plus Pro fixtures

// Define schedule weekdays
#define NEVER 0
#define SUNDAY 1
#define MONDAY 2
#define TUESDAY 3
#define WEDNESDAY 4
#define THURSDAY 5
#define FRIDAY 6
#define SATURDAY 7
#define EVERYDAY 8
#define MON_TO_FRI 9
#define SUN_AND_SAT 10

// Define EEPROM locations
#define COLOR_VALUES_COUNT 5 // Power, Red, Green, Blue, and White
#define MEMORY_SCHEDULE_COUNT 50 // Must be multiples of 2
#define TIMER_SCHEDULE_COUNT 50 // Must be multiples of 5
#define COLOR_VALUES_LOCATION_BEGIN 0
#define COLOR_VALUES_LOCATION_END (COLOR_VALUES_LOCATION_BEGIN + sizeof(ColorValues) * COLOR_VALUES_COUNT)
#define MEMORY_SCHEDULE_LOCATION_BEGIN COLOR_VALUES_LOCATION_END
#define MEMORY_SCHEDULE_LOCATION_END (MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * MEMORY_SCHEDULE_COUNT)
#define TIMER_SCHEDULE_LOCATION_BEGIN MEMORY_SCHEDULE_LOCATION_END
#define TIMER_SCHEDULE_LOCATION_END (TIMER_SCHEDULE_LOCATION_BEGIN + sizeof(TimerSchedule) * TIMER_SCHEDULE_COUNT)

// Define Structures
struct ColorValues
{
  byte red;
  byte green;
  byte blue;
  byte white;
};
struct MemorySchedule
{
  byte button : 5;
  byte weekday : 4;
  unsigned long timeSinceMidnight : 17;
  unsigned long duration : 17;
};
struct TimerSchedule
{
  byte button : 5;
  byte weekday : 4;
  unsigned long timeSinceMidnight : 17;
};

// Define global variables
static EthernetServer gServer(80);
static ColorValues gCurrentColorValues;
static byte gMemoryScheduleCounters[MEMORY_SCHEDULE_COUNT / 2]; // Behaves like a struct with two 4 bit fields
static unsigned short int gTimerScheduleCounters[TIMER_SCHEDULE_COUNT / 5]; // Behaves like a struct with five 3 bit fields
static unsigned long gClearScheduleCountersTime;

// Setup code
void setup()
{
  // Initialize the serial communication for debugging
  DEBUG_SERIAL_BEGIN();
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
  #ifdef DEBUG
  Ethernet.begin(mac, IPAddress(192, 168, 0, 254));
  #else
  if (!Ethernet.begin(mac))
    DEBUG_LOG_LN(F("Failed to start ethernet"));
  #endif

  // Start the server
  DEBUG_LOG_LN(F("Starting server ..."));
  gServer.begin();
  
  // Start the time sync
  DEBUG_LOG_LN(F("Synching time with NTP server ..."));
  setSyncProvider(&getNTPTime);
  setSyncInterval(3600);
  while (timeStatus() == timeNotSet);

  // Set the time for clearing the schedule counters
  gClearScheduleCountersTime = nextSunday(now());

  // Setup the schedule counters
  setupScheduleCounters();

  // Log free memory
  DEBUG_LOG_FREE_RAM();
}

// Function called to get the time from a NTP server
unsigned long inline getNTPTime()
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
  // TODO: check if the upd.write statement will work in an overflow situation
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

// Function called to setup the schedule counters
void setupScheduleCounters()
{
  // Prepare needed variables
  unsigned long currentTime = now();
  byte day = dayOfWeek(currentTime);
  
  // Loop through the memory schedules
  for (byte i = 0; i < MEMORY_SCHEDULE_COUNT; ++i)
  {
    // Load the schedule
    MemorySchedule schedule = EEPROM.get(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * i, 
      schedule);

    // Check if this schedule is active
    if (schedule.weekday == NEVER)
      continue;

    // Calculate how many times this schedule should have run already this week
    byte count = 0;
    if (schedule.weekday >= SUNDAY && schedule.weekday <= SATURDAY)
    {
      if (day > schedule.weekday)
      {
        count = 2;
      }
      else if (day == schedule.weekday)
      {
        if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight + schedule.duration)
          count = 2;
        else if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight)
          count = 1;
      }
    }
    else if (schedule.weekday == EVERYDAY)
    {
      if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight + schedule.duration)
        count = day * 2;
      else if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight)
        count = day * 2 - 1;
      else 
        count = (day - 1) * 2;
    }
    else if (schedule.weekday == MON_TO_FRI)
    {
      if (day >= MONDAY && day <= FRIDAY)
      {
        if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight + schedule.duration)
          count = (day - 1) * 2;
        else if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight)
          count = (day - 1) * 2 - 1;
        else 
          count = (day - 2) * 2;
      }
      else if (day == SATURDAY)
      {
        count = 10;
      }
    }
    else if (schedule.weekday == SUN_AND_SAT)
    {
      if (day == SUNDAY)
      {
        if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight + schedule.duration) 
          count = 2;
        else if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight)
          count = 1;
      }
      else if (day < SATURDAY)
      {
        count = 2;
      }
      else if (day == SATURDAY)
      {
        if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight + schedule.duration) 
          count = 4;
        else if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight)
          count = 3;
      }
    }

    // Adjust the counter
    gMemoryScheduleCounters[i / 2] = (gMemoryScheduleCounters[i / 2] & ~(0x0F << (i % 2) * 4)) |
        (count << (i % 2) * 4);
  }
  
  // Loop through the timer schedules
  for (byte i = 0; i < TIMER_SCHEDULE_COUNT; ++i)
  {
    // Load the schedule
    TimerSchedule schedule = EEPROM.get(TIMER_SCHEDULE_LOCATION_BEGIN + sizeof(TimerSchedule) * i, 
      schedule);

      // Check if this schedule is active
    if (schedule.weekday == NEVER)
      continue;

    // Calculate how many times this schedule should have run already this week
    byte count = 0;
    if (schedule.weekday >= SUNDAY && schedule.weekday <= SATURDAY)
    {
      if (day > schedule.weekday || (day == schedule.weekday && 
          currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight))
        count = 1;
    }
    else if (schedule.weekday == EVERYDAY)
    {
      if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight)
        count = day;
      else
        count = day - 1;
    }
    else if (schedule.weekday == MON_TO_FRI)
    {
      if (day >= MONDAY && day <= FRIDAY)
      {
        if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight)
          count = day - 1;
        else 
          count = day - 2;
      }
      else if (day == SATURDAY)
      {
        count = 5;
      }
    }
    else if (schedule.weekday == SUN_AND_SAT)
    {
      if ((day == SUNDAY && currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight) ||
          (day > SUNDAY && day < SATURDAY))
      {
        count = 1;
      }
      else if (day == SATURDAY)
      {
        if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight)
          count = 2;
        else 
          count = 1;
      }
    }

    // Adjust the counter
    gTimerScheduleCounters[i / 5] = (gTimerScheduleCounters[i / 5] & ~(0x0007 << (i % 5) * 3)) |
        (count << (i % 5) * 3);
  } 
}

// Function called to handle the index page
void inline processWebRequest()
{
  // Check if there's data available to read from a client
  EthernetClient client = gServer.available();
  if (client)
  { 
    // Prepare needed variables
    char character = '\0';
    char prevCharacter = '\0';
    byte spacesFound = 0;
    byte sequencialNewLinesFound = 0;
    char string[URL_MAX_LENGTH];
    memset(&string, 0, URL_MAX_LENGTH);
    byte stringLength = 0;

    // Loop while the client is connected
    while (client.connected()) 
    {
      // Check if there's data available to read
      if (client.available())
      {
        // Read a character
        prevCharacter = character;
        character = client.read();

        // Log details
        DEBUG_LOG(F("Read '"));
        DEBUG_LOG(character);
        DEBUG_LOG_LN(F("' character from request header"));
        
        // Check if we're at a seperation point between the three entries on the first line of
        //   the request header
        if (character == ' ' && prevCharacter != '\0' && prevCharacter != ' ' && spacesFound < 2)
        {
          // Increment the space counter
          ++spacesFound;

          // Check if we've just finished reading the first entry
          if (spacesFound == 1)
          {
            // Make sure this is a GET or POST request
            if (strcasecmp(string, "GET") != 0 && strcasecmp(string, "POST") != 0)
            {
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
            memset(&string, 0, URL_MAX_LENGTH);
            stringLength = 0;
          }
        }
        else if (character == '\n')
        {
          // Increment the sequencial new line counter
          ++sequencialNewLinesFound;
          
          // Check if we've just finished reading all the headers
          if (sequencialNewLinesFound == 2)
            break;
        }
        else if (character != '\r' && character != '\n')
        {
          // Reset the sequencial new line counter
          sequencialNewLinesFound = 0;
        }
          
        // Add the character to the string (if we're reading the first or second entry)
        if (spacesFound < 2 && stringLength < URL_MAX_LENGTH - 1)
        {
          // Add the character to the string and increase the size counter
          string[stringLength] = character;
          ++stringLength;
        }
        
        // Log details
        DEBUG_LOG(F("String content: "));
        DEBUG_LOG_LN(string);
      }
    }
    
    // Check if the root is being requested
    if (strcasecmp(string, "/") == 0)
      strcpy(string, "/index.htm");
    
    // Log details
    DEBUG_LOG(F("URL requested: "));
    DEBUG_LOG_LN(string);
  
    // Check what kind of request this is
    if (strncasecmp(string, "/get?", 5) == 0 && stringLength > 10)
    {
      // Send the headers
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: application/octet-stream"));
      client.println(F("Connection: close"));
    
      // Figure out which data is being requested
      if (strcasecmp((char *)string[stringLength - 7], "?memval") == 0)
      {
        for (unsigned short i = COLOR_VALUES_LOCATION_BEGIN; i < COLOR_VALUES_LOCATION_END; ++i)
        {
          client.write(EEPROM.read(i));
        }
      }
      else if (strcasecmp((char *)string[stringLength - 9], "?memsched") == 0)
      {
        for (unsigned short i = MEMORY_SCHEDULE_LOCATION_BEGIN; i < MEMORY_SCHEDULE_LOCATION_END; ++i)
        {
          client.write(EEPROM.read(i));
        }
      }
      else if (strcasecmp((char *)string[stringLength - 10], "?timesched") == 0)
      {
        for (unsigned short i = TIMER_SCHEDULE_LOCATION_BEGIN; i < TIMER_SCHEDULE_LOCATION_END; ++i)
        {
          client.write(EEPROM.read(i));
        }
      }    
    }
    else if (strcasecmp(string, "/restart") == 0)
    {
      // Do a soft restart
      DEBUG_LOG_LN(F("Restarting ..."));
      asm volatile("jmp 0");
    }
    else 
    {
      // Check if we are saving data first
      if (strncasecmp(string, "/save?", 6) == 0 && stringLength > 10)
      {
        // Figure out which data is being saved
        if (strcasecmp((char *)string[stringLength - 7], "?memval") == 0)
        {
          for (unsigned short i = COLOR_VALUES_LOCATION_BEGIN; i < COLOR_VALUES_LOCATION_END; ++i)
          {
            EEPROM.update(client.read(), i);
          }
        }
        else if (strcasecmp((char *)string[stringLength - 9], "?memsched") == 0)
        {
          for (unsigned short i = MEMORY_SCHEDULE_LOCATION_BEGIN; i < MEMORY_SCHEDULE_LOCATION_END; ++i)
          {
            EEPROM.update(client.read(), i);
          }
        }
        else if (strcasecmp((char *)string[stringLength - 10], "?timesched") == 0)
        {
          for (unsigned short i = TIMER_SCHEDULE_LOCATION_BEGIN; i < TIMER_SCHEDULE_LOCATION_END; ++i)
          {
            EEPROM.update(client.read(), i);
          }
        }
    
        // Change the URL to the index page
        strcpy(string, "/index.htm");
      }
      
      // Open the requsted file
      File file = SD.open(string);
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
      
      // Figure out the content type
      const __FlashStringHelper* type;
      if (stringLength > 3)
      {
        if (strcasecmp((char*)string[stringLength - 3], ".js") == 0)
          type = F("text/javascript");
      }
      if (stringLength > 4)
      {
        if (strcasecmp((char *)string[stringLength - 4], ".htm") == 0)
          type = F("text/html");
        if (strcasecmp((char*)string[stringLength - 4], ".css") == 0)
          type = F("text/css");
        else if (stringLength > 4 && strcasecmp((char *)string[stringLength - 4], ".jpg") == 0)
          type = F("image/jpeg");
        else if (stringLength > 4 && strcasecmp((char *)string[stringLength - 4], ".png") == 0)
          type = F("image/png");      
      }

      // Send the file
      client.println(F("HTTP/1.1 200 OK"));
      client.print(F("Content-Type: "));
      client.println(type);
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

// Main code
void loop()
{
  // Process any web requests
  processWebRequest();

  // Prepare various variables
  unsigned long currentTime = now();
  byte prevSchedule;
  unsigned long prevScheduleStop = 0;
  byte nextSchedule;
  unsigned long nextScheduleStart = -1;
  
  // Loop through the memory schedules
  byte i;
  for (i = 0; i < MEMORY_SCHEDULE_COUNT; ++i)
  {
    // Read the schedule entry
    MemorySchedule schedule = EEPROM.get(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * i, 
        schedule);
    
    // Check if this schedule is active
    if (schedule.weekday == NEVER)
      continue;
    
    // Adjust the schedule's weekday
    byte day = dayOfWeek(currentTime);
    if (schedule.weekday == EVERYDAY || (schedule.weekday == MON_TO_FRI && day >= MONDAY && day <= FRIDAY) ||
        (schedule.weekday == SUN_AND_SAT && (day == SUNDAY || day == SATURDAY)))
      schedule.weekday = day;

    // Cacluate this schedule's start and stop times
    unsigned long start = previousMidnight(currentTime - (dayOfWeek(currentTime) - schedule.weekday) *
        SECS_PER_DAY) + schedule.timeSinceMidnight;
    unsigned long stop = start + schedule.duration;

    // Check if this schedule is currently running
    if (currentTime > start && currentTime < stop)
    {
      break;
    }
    // Check if this schedule has already stopped and if it's the closest previous schedule
    else if (currentTime > stop && stop > prevScheduleStop)
    {
      prevSchedule = i;
      prevScheduleStop = stop;
    }
    else if (currentTime > stop - SECS_PER_WEEK && stop - SECS_PER_WEEK > prevScheduleStop)
    {
      prevSchedule = 1;
      prevScheduleStop = stop - SECS_PER_WEEK;
    }
    // Check if this schedule hasn't started yet and if it's the closest next schedule
    else if (currentTime < start && start < nextScheduleStart)
    {
      nextSchedule = i;
      nextScheduleStart = start;
    }
    else if (currentTime < start + SECS_PER_WEEK && start + SECS_PER_WEEK < nextScheduleStart)
    {
      nextSchedule = i;
      nextScheduleStart = start + SECS_PER_WEEK;
    }   
  }
    
  // Check if we looped through all the schedules which means none of them are currently running
  if (i > MEMORY_SCHEDULE_COUNT)
  {
    // Read the previous and next schedule values
    ColorValues prevValues = EEPROM.get(COLOR_VALUES_LOCATION_BEGIN + sizeof(ColorValues) *
        EEPROM.read(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * prevSchedule), prevValues);
    ColorValues nextValues = EEPROM.get(COLOR_VALUES_LOCATION_BEGIN + sizeof(ColorValues) * 
        EEPROM.read(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * nextSchedule), nextValues);

    // Prepare the IR code array
    FLASH_ARRAY(unsigned long, irCodes, RED_UP_CODE, RED_DOWN_CODE, GREEN_UP_CODE, GREEN_DOWN_CODE, BLUE_UP_CODE,
        BLUE_DOWN_CODE, WHITE_UP_CODE, WHITE_DOWN_CODE);

    // Loop through the colors
    for (byte i = 0; i < 4; ++i)
    {
      // Calculate what the color should be at this point in time
      // Math: previous color value + seconds since previous schedule ended /
      //       (seconds between schedules / difference in color between schedules) +
      //       0.5 for rounding
      byte calcValue = (byte)((float)((byte *)&prevValues)[i] + (float)(currentTime - prevScheduleStop) /
          ((float)(nextScheduleStart - prevScheduleStop) / (float)(((byte *)&nextValues)[i] - ((byte *)&prevValues)[i])) +
          (float)0.5);

      // Check if we have to adjust the colors
      if (calcValue < ((byte*)&gCurrentColorValues)[i])
      {
        // Send the IR code, wait, and adjust the current color value
        IRsend irSend;
        irSend.sendNEC(irCodes[i * 2], 32);
        delay(333);
        ++((byte*)&gCurrentColorValues)[i];
      }
      else if (calcValue > ((byte*)&gCurrentColorValues)[i])
      {
        // Send the IR code, wait, and adjust the current color value
        IRsend irSend;
        irSend.sendNEC(irCodes[i * 2 + 1], 32);
        delay(333);
        --((byte *)&gCurrentColorValues)[i];
      }
    }
  }
  
  // Check if it's time to clear the schedule counters
  if (currentTime > gClearScheduleCountersTime)
  {
    // Clear the counters
    memset(&gMemoryScheduleCounters, 0, sizeof(byte) * MEMORY_SCHEDULE_COUNT / 2);
    memset(&gTimerScheduleCounters, 0, sizeof(unsigned short int) * TIMER_SCHEDULE_COUNT / 5);
    
    // Set the new time for clearing the counters
    gClearScheduleCountersTime = nextSunday(currentTime);
  }

  // Preapre the IR code array
  FLASH_ARRAY(unsigned long, memoryIRCodes, POWER_CODE, M1_CODE, M2_CODE, DAYLIGHT_CODE, 
      MOONLIGHT_CODE);

  // Loop through the memory schedules
  for (byte i = 0; i < MEMORY_SCHEDULE_COUNT; ++i)
  {
    // Load the schedule
    MemorySchedule schedule = EEPROM.get(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * i, 
      schedule);

    // Check if this schedule is active
    if (schedule.weekday == 0)
      continue;
  
    // Load the counter for this shedule
    byte counter = (gMemoryScheduleCounters[i / 2] >> (i % 2) * 4) & 0x0F;

    // Check if this schedule should run today
    byte day = dayOfWeek(currentTime);
    if ((schedule.weekday == day && counter < 1) || (schedule.weekday == EVERYDAY && counter < day) ||
        (schedule.weekday == MON_TO_FRI && day >= MONDAY && day <= FRIDAY && counter < day - 1) || 
        (schedule.weekday == SUN_AND_SAT && ((day == SUNDAY && counter < 1) || (day == SATURDAY && counter < 2))))
    {
      // Check if it's time to run this schedule
      if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight)
      {
        // Send the IR signal
        IRsend irSend;
        irSend.sendNEC(memoryIRCodes[schedule.button], 32);
        delay(333);
     
        // Increment the counter
        gMemoryScheduleCounters[i / 2] = (gMemoryScheduleCounters[i / 2] & ~(0x0F << (i % 2) * 4)) |
            (++counter << (i % 2) * 4);
      }
    }
    else if ((schedule.weekday == day && counter < 2) || (schedule.weekday == EVERYDAY && counter < day * 2) ||
        (schedule.weekday == MON_TO_FRI && day >= MONDAY && day <= FRIDAY && counter < (day - 1) * 2) || 
        (schedule.weekday == SUN_AND_SAT && ((day == SUNDAY && counter < 2) || (day == SATURDAY && counter < 4)))) 
    {
      // Check if it's time to run this schedule
      if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight + schedule.duration)
      {
       // Send the IR signal
        IRsend irSend;
        irSend.sendNEC(memoryIRCodes[schedule.button], 32);
        delay(333);
        
        // Increment the counter
       gMemoryScheduleCounters[i / 2] = (gMemoryScheduleCounters[i / 2] & ~(0x0F << (i % 2) * 4)) |
           (++counter << (i % 2) * 4);
      }
    }
  }

  // Prepare the IR code array
  FLASH_ARRAY(unsigned long, timerIRCodes, SET_CLOCK_CODE, ON_TIME_CODE, OFF_TIME_CODE,
      POWER_CODE, HOUR_UP_CODE, MINUTE_DOWN_CODE, ENTER_CODE, RESUME_CODE, SUNLIGHT_CODE,
      FULL_SPECTRUM_CODE, CRISP_BLUE_CODE, DEEP_WATER_CODE, RED_UP_CODE, GREEN_UP_CODE,
      BLUE_UP_CODE, WHITE_UP_CODE, RED_DOWN_CODE, GREEN_DOWN_CODE, BLUE_DOWN_CODE,
      WHITE_DOWN_CODE, M1_CODE, M2_CODE, DAYLIGHT_CODE, MOONLIGHT_CODE, DYNAMIC_MOON_CODE,
      DYNAMIC_LIGHTNING_CODE, DYNAMIC_CLOUD_CODE, DYNAMIC_FADE_CODE);

  // Loop through the timer schedules
  for (byte i = 0; i < TIMER_SCHEDULE_COUNT; ++i)
  {
    // Load the schedule
    TimerSchedule schedule = EEPROM.get(TIMER_SCHEDULE_LOCATION_BEGIN + sizeof(TimerSchedule) * i, 
      schedule);

    // Check if this schedule is active
    if (schedule.weekday == 0)
      continue;
  
    // Load the counter for this shedule
    byte counter = (gTimerScheduleCounters[i / 5] >> (i % 5) * 3) & 0x0007;

    // Check if this schedule should run today
    byte day = dayOfWeek(currentTime);
    if ((schedule.weekday == day && counter < 1) || (schedule.weekday == EVERYDAY && counter < day) ||
        (schedule.weekday == MON_TO_FRI && day >= MONDAY && day <= FRIDAY && counter < day - 1) || 
        (schedule.weekday == SUN_AND_SAT && ((day == SUNDAY && counter < 1) || (day == SATURDAY && counter < 2))))
    {
      // Check if it's time to run this schedule
      if (currentTime > previousMidnight(currentTime) + schedule.timeSinceMidnight)
      {
        // Send the IR signal
        IRsend irSend;
        irSend.sendNEC(timerIRCodes[schedule.button], 32);
        delay(333);
        
        // Increment the counter   
        gTimerScheduleCounters[i / 5] = (gTimerScheduleCounters[i / 5] & ~(0x0007 << (i % 5) * 3)) |
            (++counter << (i % 5) * 3);
      }
    }  
  }
}
