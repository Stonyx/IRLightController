// IR Light Controller
// Copyright (C) 2015 Stonyx
//
// This software is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0
// International License.
//
// You can redistribute and/or modify this software for non-commerical purposes under the terms
// of the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
//
// This software is provided "as is" without express or implied warranty.

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

// Define SEND_NEC so that only the NEC sections of the IRRemote library are included
#define SEND_NEC

// Include header files
#include <avr/wdt.h>
#include "Flash.h"
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <Time.h>
#include <TimeLib.h>
#include <IRremote.h>

// Define settings
#define SD_CARD_PIN 4
#define ETHERNET_PIN 10
#define NTP_MAX_POLLS 10
#define NTP_POLL_INTERVAL 1000 // In milliseconds
#define URL_MAX_LENGTH 50
#define TCP_BUFFER_SIZE 128

// Define IR code prefix
#define CODE_PREFIX 0x20DF

// Define IR codes for the Ecoxotic E-Series fixtures
#define ECO_SET_CLOCK_CODE 0x3AC5
#define ECO_ON_TIME_CODE 0xBA45
#define ECO_OFF_TIME_CODE 0x827D
#define ECO_POWER_CODE 0x02FD
#define ECO_HOUR_UP_CODE 0x1AE5
#define ECO_MINUTE_DOWN_CODE 0x9A65
#define ECO_ENTER_CODE 0xA25D
#define ECO_RESUME_CODE 0x22DD
#define ECO_SUNLIGHT_CODE 0x2AD5
#define ECO_FULL_SPECTRUM_CODE 0xAA55
#define ECO_CRISP_BLUE_CODE 0x926D
#define ECO_DEEP_WATER_CODE 0x12ED
#define ECO_RED_UP_CODE 0x0AF5
#define ECO_GREEN_UP_CODE 0x8A75
#define ECO_BLUE_UP_CODE 0xB24D
#define ECO_WHITE_UP_CODE 0x32CD
#define ECO_RED_DOWN_CODE 0x38C7
#define ECO_GREEN_DOWN_CODE 0xB847
#define ECO_BLUE_DOWN_CODE 0x7887
#define ECO_WHITE_DOWN_CODE 0xF807
#define ECO_M1_CODE 0x18E7
#define ECO_M2_CODE 0x9867
#define ECO_DAYLIGHT_CODE 0x58A7
#define ECO_MOONLIGHT_CODE 0xD827
#define ECO_DYNAMIC_MOON_CODE 0x28D7
#define ECO_DYNAMIC_LIGHTNING_CODE 0xA857
#define ECO_DYNAMIC_CLOUD_CODE 0x6897
#define ECO_DYNAMIC_DAWN_DUSK_CODE 0xE817

// Define IR codes for the Current USA Satellite Plus Pro fixture
#define SATPP_SET_CLOCK_CODE 0x3AC5
#define SATPP_ON_TIME_CODE 0xBA45
#define SATPP_OFF_TIME_CODE 0x827D
#define SATPP_POWER_CODE 0x02FD
#define SATPP_HOUR_UP_CODE 0x1AE5
#define SATPP_MINUTE_DOWN_CODE 0x9A65
#define SATPP_ENTER_CODE 0xA25D
#define SATPP_RESUME_CODE 0x22DD
#define SATPP_SUNLIGHT_CODE 0x2AD5
#define SATPP_FULL_SPECTRUM_CODE 0xAA55
#define SATPP_CRISP_BLUE_CODE 0x926D
#define SATPP_DEEP_WATER_CODE 0x12ED
#define SATPP_RED_UP_CODE 0x0AF5
#define SATPP_GREEN_UP_CODE 0x8A75
#define SATPP_BLUE_UP_CODE 0xB24D
#define SATPP_WHITE_UP_CODE 0x32CD
#define SATPP_RED_DOWN_CODE 0x38C7
#define SATPP_GREEN_DOWN_CODE 0xB847
#define SATPP_BLUE_DOWN_CODE 0x7887
#define SATPP_WHITE_DOWN_CODE 0xF807
#define SATPP_M1_CODE 0x18E7
#define SATPP_M2_CODE 0x9867
#define SATPP_DAYLIGHT_CODE 0x58A7
#define SATPP_MOONLIGHT_CODE 0xD827
#define SATPP_DYNAMIC_MOON_1_CODE 0x28D7
#define SATPP_DYNAMIC_MOON_2_CODE 0xA857
#define SATPP_DYNAMIC_CLOUD_CODE 0x6897
#define SATPP_DYNAMIC_DAWN_DUSK_CODE 0xE817
#define SATPP_DYNAMIC_STORM_1_CODE 0x08F7
#define SATPP_DYNAMIC_STORM_2_CODE 0x8877
#define SATPP_DYNAMIC_STORM_3_CODE 0x48B7
#define SATPP_DYNAMIC_STORM_4_CODE 0xC837

// Define IR codes for the Current USA Satellite Plus fixture
#define SATP_ORANGE_CODE 0x3AC5
#define SATP_BLUE_CODE 0xBA45
#define SATP_ROSE_CODE 0x827D
#define SATP_POWER_CODE 0x02FD
#define SATP_WHITE_CODE 0x1AE5
#define SATP_FULL_SPEC_CODE 0x9A65
#define SATP_PURPLE_CODE 0xA25D
#define SATP_PLAY_PAUSE_CODE 0x22DD
#define SATP_RED_UP_CODE 0x2AD5
#define SATP_GREEN_UP_CODE 0xAA55
#define SATP_BLUE_UP_CODE 0x926D
#define SATP_WHITE_UP_CODE 0x12ED
#define SATP_RED_DOWN_CODE 0x0AF5
#define SATP_GREEN_DOWN_CODE 0x8A75
#define SATP_BLUE_DOWN_CODE 0xB24D
#define SATP_WHITE_DOWN_CODE 0x32CD
#define SATP_M1_CODE 0x38C7
#define SATP_M2_CODE 0xB847
#define SATP_M3_CODE 0x7887
#define SATP_M4_CODE 0xF807
#define SATP_DYNAMIC_MOON_1_CODE 0x18E7
#define SATP_DYNAMIC_MOON_2_CODE 0x9867
#define SATP_DYNAMIC_MOON_3_CODE 0x58A7
#define SATP_DYNAMIC_DAWN_DUSK_CODE 0xD827
#define SATP_DYNAMIC_CLOUD_1_CODE 0x28D7
#define SATP_DYNAMIC_CLOUD_2_CODE 0xA857
#define SATP_DYNAMIC_CLOUD_3_CODE 0x6897
#define SATP_DYNAMIC_CLOUD_4_CODE 0xE817
#define SATP_DYNAMIC_STORM_1_CODE 0x08F7
#define SATP_DYNAMIC_STORM_2_CODE 0x8877
#define SATP_DYNAMIC_STORM_3_CODE 0x48B7
#define SATP_DYNAMIC_STORM_4_CODE 0xC837

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

// Define counts
#define COLOR_VALUES_COUNT 5
#define MEMORY_SCHEDULE_COUNT 50 // Must be multiples of 2
#define TIMER_SCHEDULE_COUNT 50 // Must be multiples of 5

// Define EEPROM locations
#define ADDRESSES_LOCATION_BEGIN 0
#define ADDRESSES_LOCATION_END (ADDRESSES_LOCATION_BEGIN + 6 + 4 * 4) // Six byte MAC address plus four 4 byte IP addresses
#define TIME_ZONE_LOCATION_BEGIN ADDRESSES_LOCATION_END
#define TIME_ZONE_LOCATION_END (TIME_ZONE_LOCATION_BEGIN + sizeof(TimeZone))
#define COLOR_VALUES_LOCATION_BEGIN TIME_ZONE_LOCATION_END
#define COLOR_VALUES_LOCATION_END (COLOR_VALUES_LOCATION_BEGIN + sizeof(ColorValues) * COLOR_VALUES_COUNT)
#define MEMORY_SCHEDULE_LOCATION_BEGIN COLOR_VALUES_LOCATION_END
#define MEMORY_SCHEDULE_LOCATION_END (MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * MEMORY_SCHEDULE_COUNT)
#define TIMER_SCHEDULE_LOCATION_BEGIN MEMORY_SCHEDULE_LOCATION_END
#define TIMER_SCHEDULE_LOCATION_END (TIMER_SCHEDULE_LOCATION_BEGIN + sizeof(TimerSchedule) * TIMER_SCHEDULE_COUNT)

// Define Structures
struct TimeZone
{
  signed short offset; // In minutes
  signed short dstOffset; // In minutes
  byte dstStartMonth; // 0: Janary ... 11: December
  byte dstEndMonth; // 0: Janary ... 11: December
  byte dstStartWeekdayNumber; // 0: first, 1: second, 2: third, 3: fourth, 4: last
  byte dstEndWeekdayNumber; // 0: first, 1: second, 2: third, 3: fourth, 4: last
  byte dstStartWeekday; // 0: Sunday ... 6: Saturday
  byte dstEndWeekday; // 0: Sunday ... 6: Saturday
  unsigned short dstStartMinutes; // After midnight
  unsigned short dstEndMinutes; // After midnight
};
struct ColorValues
{
  byte red;
  byte green;
  byte blue;
  byte white;
};
struct MemorySchedule
{
  byte button;
  byte weekday;
  unsigned long timeSinceMidnight;
  unsigned long duration;
};
struct TimerSchedule
{
  byte button;
  byte weekday;
  unsigned long timeSinceMidnight;
};

// Define global variables
static EthernetServer gServer(80);
static ColorValues gCurrentColorValues;
static byte gMemoryScheduleCounters[MEMORY_SCHEDULE_COUNT / 2]; // Behaves like a struct with two 4 bit fields
static unsigned short gTimerScheduleCounters[TIMER_SCHEDULE_COUNT / 5]; // Behaves like a struct with five 3 bit fields
static unsigned long gClearScheduleCountersTime;
static bool gReboot = false;

// Declare functions
void wdt_init() __attribute__((naked)) __attribute__((section(".init3")));
unsigned long getNtpTime();
byte calcDSTDay(unsigned short year, byte month, byte weekdayNumber, byte weekday);
void initializeStuff();
ColorValues calcColorValues(unsigned long time, byte day, bool includeFade, byte& enabledSchedules, byte& prevOrCurrentSchedule);
byte calcMemoryScheduleCount(unsigned long time, unsigned long midnight, byte day, MemorySchedule schedule);
byte calcTimerScheduleCount(unsigned long time, unsigned long midnight, byte day, TimerSchedule schedule);
void inline processWebRequest();

// Function called to reset the watchdog timer after a reboot
void wdt_init()
{
  MCUSR = 0;
  wdt_disable();
}

// Setup code
void setup()
{
  // Initialize the serial communication for debugging
  DEBUG_SERIAL_BEGIN();
  DEBUG_LOG_LN(F("Starting IR Light Controller sketch ..."));

  // Set the two SS pins to output mode and set them both to high
  pinMode(SD_CARD_PIN, OUTPUT);
  pinMode(ETHERNET_PIN, OUTPUT);
  digitalWrite(SD_CARD_PIN, HIGH);
  digitalWrite(ETHERNET_PIN, HIGH);

  // Start the SD card
  DEBUG_LOG_LN(F("Starting SD card ..."));
  if (!SD.begin(SD_CARD_PIN))
    DEBUG_LOG_LN(F("Failed to start SD card"));

  // Get the various ethernet addresses
  byte addresses[ADDRESSES_LOCATION_END - ADDRESSES_LOCATION_BEGIN];
  for (unsigned short i = ADDRESSES_LOCATION_BEGIN; i < ADDRESSES_LOCATION_END; ++i)
  {
    addresses[i] = EEPROM.read(i);
  }

  // Start the ethernet
  DEBUG_LOG_LN(F("Starting ethernet ..."));
  Ethernet.begin(addresses, &addresses[6], &addresses[6 + 4], &addresses[6 + 4 * 2], &addresses[6 + 4 * 3]);
  
  // Start the server
  DEBUG_LOG_LN(F("Starting server ..."));
  gServer.begin();

  // Start the time sync
  DEBUG_LOG_LN(F("Synching time with NTP server ..."));
  setSyncProvider(&getNtpTime);
  setSyncInterval(30);
  while (timeStatus() == timeNotSet);
  setSyncInterval(3600);

  // Setup the schedule counters
  DEBUG_LOG_LN("Initializing stuff ...");
  initializeStuff();

  // Log free memory
  DEBUG_LOG_FREE_RAM();
}

// Function called to get the time from a NTP server
unsigned long getNtpTime()
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
  if (!udp.beginPacket(timeServer, 123))
  {
    DEBUG_LOG_LN(F("Failed to send UDP packet"));
    return 0;
  }
  for (byte i = 0; i < 12; ++i)
  {
    if (udp.write((byte *)&packet, 4) != 4)
    {
      DEBUG_LOG_LN(F("Failed to send UDP packet"));
      return 0;
    }
  }
  if (!udp.endPacket())
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

    // Wait before checking again
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

  // Load the time zone data
  TimeZone timeZone = EEPROM.get(TIME_ZONE_LOCATION_BEGIN, timeZone);

  // Adjust the time for the epoch and the time zone
  time = time - 2208988800UL + timeZone.offset * 60;

  // Check if this time zone has DST
  if (timeZone.dstOffset != 0)
  {  
    // Get the time elements
    TimeElements current;
    breakTime(time, current);

    // Calculate the DST start and end days
    byte dstStartDay = calcDSTDay(1970 + current.Year, timeZone.dstStartMonth, timeZone.dstStartWeekdayNumber, timeZone.dstStartWeekday);
    byte dstEndDay = calcDSTDay(1970 + current.Year, timeZone.dstEndMonth, timeZone.dstEndWeekdayNumber, timeZone.dstEndWeekday); 

    // Adjust the time for the DST
    if ((current.Month - 1 > timeZone.dstStartMonth && current.Month - 1 < timeZone.dstEndMonth) ||
        (current.Month - 1 == timeZone.dstStartMonth && current.Day - 1 >= dstStartDay && current.Hour * 60 + current.Minute >= timeZone.dstStartMinutes) ||
        (current.Month - 1== timeZone.dstEndMonth && current.Day - 1 <= dstEndDay && current.Hour * 60 + current.Minute < timeZone.dstEndMinutes - timeZone.dstOffset))
      time = time + timeZone.dstOffset * 60;
  }

  // Log details
  DEBUG_LOG(F("Received/Calculated Unix time: "));
  DEBUG_LOG_LN(time);

  return time;
}

// Function called to calculate the DST day of the month
byte calcDSTDay(unsigned short year, byte month, byte weekdayNumber, byte weekday)
{
  // Get the offset based on the month by first checking if we are dealing with the first, second, third, or fourth weekday of the month ...
  byte offset;
  if (weekdayNumber < 4)
  {
    switch (month)
    {
    case 7: // August
      offset = 0;
      break;
    case 1: // February
    case 2: // March
    case 10: // November
      offset = 1;
      break;
    case 5: // June
      offset = 2;
      break;
    case 8: // September
    default: // December
      offset = 3;
      break;
    case 3: // April
    case 6: // July
      offset = 4;
      break;
    case 0: // January
    case 9: // October
      offset = 5;
      break;
    case 4: // May
      offset = 6;
      break;
    }
  }
  // ... or the last weekday of the month
  else
  {
    switch (month)
    {
    case 6: // July
      offset = 0;
      break;
    case 0: // January
    case 1: // February
    case 9: // October
      offset = 1;
      break;
    case 4: // May
      offset = 2;
      break;
    case 7: // August
    case 10: // November
      offset = 3;
      break;
    case 2: // March
    case 5: // June
      offset = 4;
      break;
    case 8: // September
      offset = 5;
      break;
    case 3: // April
    default: // December
      offset = 6;
    }
  }

  // Get the month length
  byte length;
  switch (month)
  {
  case 1: // February
    length = 28;
    break; 
  case 3: // April
  case 5: // June
  case 8: // September
  case 10: // November
    length = 30;
    break;
  default: // January, March, May, July, August, October, December
    length = 31;
  }
  
  // This formula uses a simplified leap year calculation that is valid until 2099 and is accurate for all situations except 
  //   when the day to calculate falls on February 29
  return (weekdayNumber < 4 ? 7 + weekdayNumber * 7 : length) - (offset + weekday +
      /* six days per year minus one day per leap year -> year * 6 - year / 4 == year * 5 / 4 */
      year * 5 / 4 - (year % 4 == 0 && month < 2 ? 1 : 0)) % 7;
}

// Function called to initialize various global variables
void initializeStuff()
{
  // Prepare needed variables
  unsigned long time = now();
  unsigned long midnight = previousMidnight(time);
  byte day = dayOfWeek(time);

  // Loop through the memory schedules
  for (byte i = 0; i < MEMORY_SCHEDULE_COUNT; ++i)
  {
    // Load the schedule
    MemorySchedule schedule = EEPROM.get(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * i,
      schedule);

    // Set the counter
    gMemoryScheduleCounters[i / 2] = (gMemoryScheduleCounters[i / 2] & ~(0x0F << (i % 2) * 4)) |
        (calcMemoryScheduleCount(time, midnight, day, schedule) << (i % 2) * 4);
  }

  // Loop through the timer schedules
  for (byte i = 0; i < TIMER_SCHEDULE_COUNT; ++i)
  {
    // Load the schedule
    TimerSchedule schedule = EEPROM.get(TIMER_SCHEDULE_LOCATION_BEGIN + sizeof(TimerSchedule) * i,
      schedule);

    // Set the counter
    gTimerScheduleCounters[i / 5] = (gTimerScheduleCounters[i / 5] & ~(0x0007 << (i % 5) * 3)) |
        (calcTimerScheduleCount(time, midnight, day, schedule) << (i % 5) * 3);
  }

  // Set the time for clearing the schedule counters
  gClearScheduleCountersTime = nextSunday(time);

  // Set the current color values
  byte enabledSchedules;
  byte prevOrCurrentSchedule;
  gCurrentColorValues = calcColorValues(time, day, false, enabledSchedules, prevOrCurrentSchedule);
  if (enabledSchedules == 0)
    return;

  // Load the previous schedule
  MemorySchedule schedule = EEPROM.get(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * prevOrCurrentSchedule,
      schedule);

  // Preapre the IR code array
  FLASH_ARRAY(unsigned short, memoryIRCodes, ECO_POWER_CODE, ECO_M1_CODE, ECO_M2_CODE, ECO_DAYLIGHT_CODE,
      ECO_MOONLIGHT_CODE, SATPP_POWER_CODE, SATPP_M1_CODE, SATPP_M2_CODE, SATPP_DAYLIGHT_CODE,
      SATPP_MOONLIGHT_CODE, SATP_POWER_CODE, SATP_M1_CODE, SATP_M2_CODE, SATP_M3_CODE, SATP_M4_CODE);

  // Log details
  DEBUG_LOG_LN(F("Sending IR signal for previous or current schedule"));

  // Send the IR signal
  IRsend irSend;
  irSend.sendNEC((((unsigned long)CODE_PREFIX) << 16) | memoryIRCodes[schedule.button], 32);
  delay(333);
}

// Function called to calculate what the schedule count should be for the given memory schedule
byte calcMemoryScheduleCount(unsigned long time, unsigned long midnight, byte day, MemorySchedule schedule)
{
  // Calculate how many times this schedule should have run already this week
  byte count = 0;
  switch (schedule.weekday)
  {
  case NEVER:
    count = 0;
    break;
  case SUNDAY:
  case MONDAY:
  case TUESDAY:
  case WEDNESDAY:
  case THURSDAY:
  case FRIDAY:
  case SATURDAY:
    if (day > schedule.weekday)
    {
      count = 2;
    }
    else if (day == schedule.weekday)
    {
      if (time > midnight + schedule.timeSinceMidnight + schedule.duration)
        count = 2;
      else if (time > midnight + schedule.timeSinceMidnight)
        count = 1;
    }
    break;
  case EVERYDAY:
    if (time > midnight + schedule.timeSinceMidnight + schedule.duration)
      count = day * 2;
    else if (time > midnight + schedule.timeSinceMidnight)
      count = day * 2 - 1;
    else
      count = (day - 1) * 2;
    break;
  case MON_TO_FRI:
    if (day == SATURDAY)
    {
      count = 10;
    }
    else if (day >= MONDAY /* && day <= FRIDAY */)
    {
      if (time > midnight + schedule.timeSinceMidnight + schedule.duration)
        count = (day - 1) * 2;
      else if (time > midnight + schedule.timeSinceMidnight)
        count = (day - 1) * 2 - 1;
      else
        count = (day - 2) * 2;
    }
    break;
  default: // SUN_AND_SAT
    if (day == SATURDAY)
    {
      if (time > midnight + schedule.timeSinceMidnight + schedule.duration)
        count = 4;
      else if (time > midnight + schedule.timeSinceMidnight)
        count = 3;
    }
    else if (day > SUNDAY)
    {
      count = 2;
    }
    else // if (day == SUNDAY)
    {
      if (time > midnight + schedule.timeSinceMidnight + schedule.duration)
        count = 2;
      else if (time > midnight + schedule.timeSinceMidnight)
        count = 1;
    }
  }

  return count;
}

// Function called to calculate what the schedule count should be for the given timer schedule
byte calcTimerScheduleCount(unsigned long time, unsigned long midnight, byte day, TimerSchedule schedule)
{
  // Calculate how many times this schedule should have run already this week
  byte count = 0;
  switch (schedule.weekday)
  {
  case NEVER:
    count = 0;
    break;
  case SUNDAY:
  case MONDAY:
  case TUESDAY:
  case WEDNESDAY:
  case THURSDAY:
  case FRIDAY:
  case SATURDAY:
    if (day > schedule.weekday)
    {
      count = 1;
    }
    else if (day == schedule.weekday)
    {
      if (time > midnight + schedule.timeSinceMidnight)
        count = 1;
    }
    break;
  case EVERYDAY:
    if (time > midnight + schedule.timeSinceMidnight)
      count = day;
    else
      count = day - 1;
    break;
  case MON_TO_FRI:
    if (day == SATURDAY)
    {
      count = 5;
    }
    else if (day >= MONDAY /* && day <= FRIDAY */)
    {
      if (time > midnight + schedule.timeSinceMidnight)
        count = day - 1;
      else
        count = day - 2;
    }
    break;
  default: // SUN_AND_SAT
    if (day == SATURDAY)
    {
      if (time > midnight + schedule.timeSinceMidnight)
        count = 2;
      else
        count = 1;
    }
    else if (day > SUNDAY)
    {
      count = 1;
    }
    else // if (day == SUNDAY)
    {
      if (time > midnight + schedule.timeSinceMidnight)
        count = 1;
    }
  }

  return count;
}

// Function called to calculate what the current color values should be
ColorValues calcColorValues(unsigned long time, byte day, bool includeFade, byte& enabledSchedules, byte& prevOrCurrentSchedule)
{
  // Prepare needed variables
  enabledSchedules = 0;
  unsigned long prevScheduleStop = 0;
  byte nextSchedule;
  unsigned long nextScheduleStart = 0xFFFFFFFF;

  // Loop through the memory schedules
  byte i;
  for (i = 0; i < MEMORY_SCHEDULE_COUNT; ++i)
  {
    // Read the schedule
    MemorySchedule schedule = EEPROM.get(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * i,
        schedule);

    // Check if this schedule is active and adjust the schedule's weekday
    if (schedule.weekday == NEVER)
      continue;
    else if (schedule.weekday == EVERYDAY || (schedule.weekday == MON_TO_FRI && day >= MONDAY && day <= FRIDAY) ||
        (schedule.weekday == SUN_AND_SAT && (day == SUNDAY || day == SATURDAY)))
      schedule.weekday = day;
      
    // Add this schedule to the enabled schedules counter
    ++enabledSchedules;

    // Cacluate this schedule's start and stop times
    unsigned long start = previousMidnight((time - (day - schedule.weekday) * SECS_PER_DAY)) + schedule.timeSinceMidnight;
    unsigned long stop = start + schedule.duration;

    // Check if this schedule is currently running
    if (time > start && time < stop)
    {
      prevOrCurrentSchedule = i;
      break;
    }

    // Check if this schedule has already stopped and if it's the closest previous schedule
    if (time > stop && stop > prevScheduleStop)
    {
      prevOrCurrentSchedule = i;
      prevScheduleStop = stop;
    }
    else if (time > stop - SECS_PER_WEEK && stop - SECS_PER_WEEK > prevScheduleStop)
    {
      prevOrCurrentSchedule = i;
      prevScheduleStop = stop - SECS_PER_WEEK;
    }

    // Check if this schedule hasn't started yet and if it's the closest next schedule
    if (time < start && start < nextScheduleStart)
    {
      nextSchedule = i;
      nextScheduleStart = start;
    }
    else if (time < start + SECS_PER_WEEK && start + SECS_PER_WEEK < nextScheduleStart)
    {
      nextSchedule = i;
      nextScheduleStart = start + SECS_PER_WEEK;
    }
  }

  // Check if we have no enabled schedules
  if (enabledSchedules == 0)
    return gCurrentColorValues;

  // Read the previous or current schedule's color values
  ColorValues prevValues = EEPROM.get(COLOR_VALUES_LOCATION_BEGIN + sizeof(ColorValues) *
      EEPROM.read(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * prevOrCurrentSchedule),
      prevValues);

  // Check if we should take the fade into account and if we didn't loop through all the schedules which
  //   means one of them is currently running
  if (includeFade == false || i < MEMORY_SCHEDULE_COUNT)
  {
    return prevValues;
  }
  else
  {
    // Read the next schedule's color values
    ColorValues nextValues = EEPROM.get(COLOR_VALUES_LOCATION_BEGIN + sizeof(ColorValues) *
        EEPROM.read(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * nextSchedule),
        nextValues);

    // Loop through the colors
    ColorValues calcValues;
    for (byte i = 0; i < 4; ++i)
    {
      // Calculate what the color should be at this point in time
      // Math: previous color value + seconds since previous schedule ended /
      //       (seconds between schedules / difference in color between schedules) +
      //       0.5 for rounding
      ((byte*)&calcValues)[i] = (byte)((float)((byte *)&prevValues)[i] + (float)(time - prevScheduleStop) /
          ((float)(nextScheduleStart - prevScheduleStop) / (float)(((byte *)&nextValues)[i] - ((byte *)&prevValues)[i])) +
          (float)0.5);
    }

    return calcValues;
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

    // Log details
    DEBUG_LOG_LN(F("\nReceived the following request: "));

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
        DEBUG_LOG(character);

        // Check if we're at the end of a line
        if (character == '\n')
        {
           // Increment the sequencial new line counter
          ++sequencialNewLinesFound;

          // Check if we've just finished reading all the headers
          if (sequencialNewLinesFound == 2)
            break;
        }
        
        // Check if we're not at the end of a line
        /* else */ if (character != '\n' && character != '\r')
        {
          // Reset the sequencial new line counter
          sequencialNewLinesFound = 0;
        }
          
        // Check if we're at a seperation point between entries ...
        if (character == ' ')
        {
          // Increment the space counter
          if (prevCharacter != '\0' && prevCharacter != ' ')
            ++spacesFound;
        }
        // ... or if we're reading the second entry on the first line
        else if (spacesFound == 1 && stringLength < URL_MAX_LENGTH - 1)
        {
          // Add the character to the string and increase the size counter
          string[stringLength] = character;
          ++stringLength;
        }
      }
    }

    // Check if the root is being requested
    if (strcasecmp(string, "/") == 0)
    {
      strcpy(string, "/index.htm");
      stringLength = 10;
    }

    // Log details
    DEBUG_LOG(F("URL requested: "));
    DEBUG_LOG_LN(string);

    // Check what kind of request this is
    bool responseSent = false;
    if (strcasecmp(string, "/get?st") == 0 || strcasecmp(string, "/get?ip") == 0 || strcasecmp(string, "/get?tz") == 0 ||
        strcasecmp(string, "/get?cv") == 0 || strcasecmp(string, "/get?ms") == 0 ||  strcasecmp(string, "/get?ts") == 0)
    {
      // Send the headers
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: application/octet-stream"));
      client.println(F("Connection: close\n"));

      // Figure out which data is being requested
      if (strcasecmp(string, "/get?st") == 0) // st = status
      {
        // Send the data
        unsigned long currentTime = now();
        client.write((byte *)&currentTime, sizeof(unsigned long));
        client.write((byte *)&gCurrentColorValues, sizeof(ColorValues));
      }
      /* else */ if (strcasecmp(string, "/get?ip") == 0) // ip = IP addresses
      {
        // Send the data
        for (unsigned short i = ADDRESSES_LOCATION_BEGIN; i < ADDRESSES_LOCATION_END; ++i)
        {
          client.write(EEPROM.read(i));
        }
      }
      /* else */ if (strcasecmp(string, "/get?tz") == 0) // tz = time zone
      {
        // Send the data
        for (unsigned short i = TIME_ZONE_LOCATION_BEGIN; i < TIME_ZONE_LOCATION_END; ++i)
        {
          client.write(EEPROM.read(i));
        }
      }
      /* else */ if (strcasecmp(string, "/get?cv") == 0) // cv = color values
      {
        // Send the data
        for (unsigned short i = COLOR_VALUES_LOCATION_BEGIN; i < COLOR_VALUES_LOCATION_END; ++i)
        {
          client.write(EEPROM.read(i));
        }
      }
      /* else */ if (strcasecmp(string, "/get?ms") == 0) // ms = memory schedules
      {
        // Send the data
        for (unsigned short i = MEMORY_SCHEDULE_LOCATION_BEGIN; i < MEMORY_SCHEDULE_LOCATION_END; ++i)
        {
          client.write(EEPROM.read(i));
        }
      }
      /* else */ if (strcasecmp(string, "/get?ts") == 0) // ts = timer schedules
      {
        // Send the data
        for (unsigned short i = TIMER_SCHEDULE_LOCATION_BEGIN; i < TIMER_SCHEDULE_LOCATION_END; ++i)
        {
          client.write(EEPROM.read(i));
        }
      }

      // Set the response sent flag
      responseSent = true;
    }
    else if (strcasecmp(string, "/set?ip") == 0) // ip = IP addresses
    {
      // Save the data
      for (unsigned short i = ADDRESSES_LOCATION_BEGIN; i < ADDRESSES_LOCATION_END; ++i)
      {
        EEPROM.update(i, client.read());
      }

      // Set the reboot flag
      gReboot = true;
    }
    else if (strcasecmp(string, "/set?tz") == 0) // tz = time zone
    {
      // Save the data
      for (unsigned short i = TIME_ZONE_LOCATION_BEGIN; i < TIME_ZONE_LOCATION_END; ++i)
      {
        EEPROM.update(i, client.read());
      }

      // Set the reboot flag
      gReboot = true;
    }
    else if (strcasecmp(string, "/set?cv") == 0) // cv = color values
    {
      // Save the data
      for (unsigned short i = COLOR_VALUES_LOCATION_BEGIN; i < COLOR_VALUES_LOCATION_END; ++i)
      {
        EEPROM.update(i, client.read());
      }

      // Re-initalize stuff
      initializeStuff();
    }
    else if (strcasecmp(string, "/set?ms") == 0) // ms = memory schedules
    {
      // Save the data
      for (unsigned short i = MEMORY_SCHEDULE_LOCATION_BEGIN; i < MEMORY_SCHEDULE_LOCATION_END; ++i)
      {
        EEPROM.update(i, client.read());
      }

      // Re-initalize stuff
      initializeStuff();
    }
    else if (strcasecmp(string, "/set?ts") == 0) // ts = timer schedules
    {
      // Save the data
      for (unsigned short i = TIMER_SCHEDULE_LOCATION_BEGIN; i < TIMER_SCHEDULE_LOCATION_END; ++i)
      {
        EEPROM.update(i, client.read());
      }

      // Re-initalize stuff
      initializeStuff();
    }
    else if (strcasecmp(string, "/reset") == 0)
    {
      // Log details
      DEBUG_LOG_LN(F("Resetting saved settings ..."));

      // Reset all saved settings
      FLASH_ARRAY(byte, defaultSettings, 0xDE, 0x12, 0x34, 0x56, 0x78, 0x90, 192, 168, 1, 254, 192, 168, 1, 1, 192, 168, 1, 1, 255, 255, 255, 0,
          0xD4, 0xFE, 0x3C, 0x00, 2, 10, 1, 0, 0, 0, 0x78, 0x00, 0x78, 0x00);
      for (unsigned short i = ADDRESSES_LOCATION_BEGIN; i < TIME_ZONE_LOCATION_END; ++i)
      {
        EEPROM.update(i, defaultSettings[i - ADDRESSES_LOCATION_BEGIN]);
      }
      for (unsigned short i = COLOR_VALUES_LOCATION_BEGIN; i < TIMER_SCHEDULE_LOCATION_END; ++i)
      {
        EEPROM.update(i, 0);
      }
      
      // Set the reboot falg
      gReboot = true;
    }
    else if (strcasecmp(string, "/reboot") == 0)
    {
      // Set the reboot flag
      gReboot = true;
    }
    else
    {
      // Open the requested file
      File file = SD.open(string);
      if (file)
      {
        // Figure out the content type
        const __FlashStringHelper* type;
        if (strcasecmp((char *)&string[stringLength - 4], ".htm") == 0)
          type = F("text/html");
        /* else */ if (strcasecmp((char*)&string[stringLength - 4], ".css") == 0)
          type = F("text/css");
        /* else */ if (strcasecmp((char*)&string[stringLength - 3], ".js") == 0)
          type = F("text/javascript");
        /* else */ if (strcasecmp((char *)&string[stringLength - 4], ".png") == 0)
          type = F("image/png");

        // Send the header
        client.println(F("HTTP/1.1 200 OK"));
        client.print(F("Content-Type: "));
        client.println(type);
        client.println(F("Cache-Control: max-age=604800"));
        client.println(F("Connection: close\n"));
        
        // Send the file
        byte buffer[TCP_BUFFER_SIZE];
        byte bytesRead = 0;
        while (file.available())
        {
          // Read a byte and increase the counter
          buffer[bytesRead] = file.read();
          ++bytesRead;
          
          // Check if the buffer is full
          if (bytesRead == TCP_BUFFER_SIZE)
          {
            // Send the data
            client.write(buffer, TCP_BUFFER_SIZE);
            
            // Reset the counter
            bytesRead = 0;
          }  
        }
        if (bytesRead > 0)
        {
          // Send any remaining data
          client.write(buffer, bytesRead);
        }
        file.close();

        // Set the response sent flag
        responseSent = true;
      }
      #ifdef DEBUG
      else
      {
        // Log details
        DEBUG_LOG(F("Failed to open file: "));
        DEBUG_LOG_LN(string);
      }
      #endif
    }

    // Check if we still need to send a response
    if (responseSent == false)
    {
      client.println(F("HTTP/1.1 200 OK"));
      client.print(F("Content-Type: text/html"));
      client.println(F("Connection: close\n"));
    }

    // Close the connection
    delay(10);
    client.stop();
  }
}

// Main code
void loop()
{
  // Check the reboot flag
  if (gReboot == true)
  {
    // Log details
    DEBUG_LOG_LN(F("Rebooting ..."));
    
    // Enable the watchdog timer and wait for the reboot
    wdt_enable(WDTO_250MS);
    while (1) {}
  }

  // Process any web requests
  processWebRequest();

  // Prepare needed variables
  unsigned long time = now();
  unsigned long midnight = previousMidnight(time);
  byte day = dayOfWeek(time);

  // Check if it's time to clear the schedule counters
  if (time > gClearScheduleCountersTime)
  {
    // Clear the counters
    memset(&gMemoryScheduleCounters, 0, sizeof(byte) * MEMORY_SCHEDULE_COUNT / 2);
    memset(&gTimerScheduleCounters, 0, sizeof(unsigned short) * TIMER_SCHEDULE_COUNT / 5);

    // Set the new time for clearing the counters
    gClearScheduleCountersTime = nextSunday(time);
  }

  // Preapre the IR code array
  FLASH_ARRAY(unsigned short, memoryIRCodes, ECO_POWER_CODE, ECO_M1_CODE, ECO_M2_CODE, ECO_DAYLIGHT_CODE,
      ECO_MOONLIGHT_CODE, SATPP_POWER_CODE, SATPP_M1_CODE, SATPP_M2_CODE, SATPP_DAYLIGHT_CODE,
      SATPP_MOONLIGHT_CODE, SATP_POWER_CODE, SATP_M1_CODE, SATP_M2_CODE, SATP_M3_CODE, SATP_M4_CODE);

  // Loop through the memory schedules
  for (byte i = 0; i < MEMORY_SCHEDULE_COUNT; ++i)
  {
    // Load the schedule
    MemorySchedule schedule = EEPROM.get(MEMORY_SCHEDULE_LOCATION_BEGIN + sizeof(MemorySchedule) * i,
      schedule);

    // Load the count for this shedule
    byte count = (gMemoryScheduleCounters[i / 2] >> (i % 2) * 4) & 0x0F;

    // Check if it's time to run this schedule
    if (count < calcMemoryScheduleCount(time, midnight, day, schedule))
    {
      // Log details
      DEBUG_LOG(F("Sending IR signal for schedule: "));
      DEBUG_LOG_LN(i);
      
      // Send the IR signal
      IRsend irSend;
      irSend.sendNEC((((unsigned long)CODE_PREFIX) << 16) | memoryIRCodes[schedule.button], 32);
      delay(333);
   
      // Increment the counter
      gMemoryScheduleCounters[i / 2] = (gMemoryScheduleCounters[i / 2] & ~(0x0F << (i % 2) * 4)) |
          (++count << (i % 2) * 4);
    }
  }

  // Prepare the IR code array
  FLASH_ARRAY(unsigned short, timerIRCodes, ECO_SET_CLOCK_CODE, ECO_ON_TIME_CODE, ECO_OFF_TIME_CODE,
      ECO_POWER_CODE, ECO_HOUR_UP_CODE, ECO_MINUTE_DOWN_CODE, ECO_ENTER_CODE, ECO_RESUME_CODE,
      ECO_SUNLIGHT_CODE, ECO_FULL_SPECTRUM_CODE, ECO_CRISP_BLUE_CODE, ECO_DEEP_WATER_CODE,
      ECO_RED_UP_CODE, ECO_GREEN_UP_CODE, ECO_BLUE_UP_CODE, ECO_WHITE_UP_CODE, ECO_RED_DOWN_CODE,
      ECO_GREEN_DOWN_CODE, ECO_BLUE_DOWN_CODE, ECO_WHITE_DOWN_CODE, ECO_M1_CODE, ECO_M2_CODE,
      ECO_DAYLIGHT_CODE, ECO_MOONLIGHT_CODE, ECO_DYNAMIC_MOON_CODE, ECO_DYNAMIC_LIGHTNING_CODE,
      ECO_DYNAMIC_CLOUD_CODE, ECO_DYNAMIC_DAWN_DUSK_CODE, SATPP_SET_CLOCK_CODE, SATPP_ON_TIME_CODE,
      SATPP_OFF_TIME_CODE, SATPP_POWER_CODE, SATPP_HOUR_UP_CODE, SATPP_MINUTE_DOWN_CODE,
      SATPP_ENTER_CODE, SATPP_RESUME_CODE, SATPP_SUNLIGHT_CODE, SATPP_FULL_SPECTRUM_CODE,
      SATPP_CRISP_BLUE_CODE, SATPP_DEEP_WATER_CODE, SATPP_RED_UP_CODE, SATPP_GREEN_UP_CODE,
      SATPP_BLUE_UP_CODE, SATPP_WHITE_UP_CODE, SATPP_RED_DOWN_CODE, SATPP_GREEN_DOWN_CODE,
      SATPP_BLUE_DOWN_CODE, SATPP_WHITE_DOWN_CODE, SATPP_M1_CODE, SATPP_M2_CODE, SATPP_DAYLIGHT_CODE,
      SATPP_MOONLIGHT_CODE, SATPP_DYNAMIC_MOON_1_CODE, SATPP_DYNAMIC_MOON_2_CODE, SATPP_DYNAMIC_CLOUD_CODE,
      SATPP_DYNAMIC_DAWN_DUSK_CODE, SATPP_DYNAMIC_STORM_1_CODE, SATPP_DYNAMIC_STORM_2_CODE,
      SATPP_DYNAMIC_STORM_3_CODE, SATPP_DYNAMIC_STORM_4_CODE, SATP_ORANGE_CODE, SATP_BLUE_CODE, SATP_ROSE_CODE, 
      SATP_POWER_CODE, SATP_WHITE_CODE, SATP_FULL_SPEC_CODE, SATP_PURPLE_CODE, SATP_PLAY_PAUSE_CODE, SATP_RED_UP_CODE,
      SATP_GREEN_UP_CODE, SATP_BLUE_UP_CODE, SATP_WHITE_UP_CODE, SATP_RED_DOWN_CODE, SATP_GREEN_DOWN_CODE,
      SATP_BLUE_DOWN_CODE, SATP_WHITE_DOWN_CODE, SATP_M1_CODE, SATP_M2_CODE, SATP_M3_CODE, SATP_M4_CODE,
      SATP_DYNAMIC_MOON_1_CODE, SATP_DYNAMIC_MOON_2_CODE, SATP_DYNAMIC_MOON_3_CODE, SATP_DYNAMIC_DAWN_DUSK_CODE, 
      SATP_DYNAMIC_CLOUD_1_CODE, SATP_DYNAMIC_CLOUD_2_CODE, SATP_DYNAMIC_CLOUD_3_CODE, SATP_DYNAMIC_CLOUD_4_CODE,
      SATP_DYNAMIC_STORM_1_CODE, SATP_DYNAMIC_STORM_2_CODE, SATP_DYNAMIC_STORM_3_CODE, SATP_DYNAMIC_STORM_4_CODE);

  // Loop through the timer schedules
  for (byte i = 0; i < TIMER_SCHEDULE_COUNT; ++i)
  {
    // Load the schedule
    TimerSchedule schedule = EEPROM.get(TIMER_SCHEDULE_LOCATION_BEGIN + sizeof(TimerSchedule) * i,
      schedule);

    // Load the count for this shedule
    byte count = (gTimerScheduleCounters[i / 5] >> (i % 5) * 3) & 0x0007;

    // Check if it's time to run this schedule
    if (count < calcTimerScheduleCount(time, midnight, day, schedule))
    {
      // Log details
      DEBUG_LOG(F("Sending IR signal for timer: "));
      DEBUG_LOG_LN(i);
      
      // Send the IR signal
      IRsend irSend;
      irSend.sendNEC((((unsigned long)CODE_PREFIX) << 16) | timerIRCodes[schedule.button], 32);
      delay(333);

      // Increment the counter
      gTimerScheduleCounters[i / 5] = (gTimerScheduleCounters[i / 5] & ~(0x0007 << (i % 5) * 3)) |
          (++count << (i % 5) * 3);
    }
  }

  // Calculate what the current color values should be
  byte enabledSchedules;
  byte prevOrCurrentSchedule;
  ColorValues calcValues = calcColorValues(time, day, true, enabledSchedules, prevOrCurrentSchedule);
  if (enabledSchedules == 0)
    return;

  // Prepare the IR code array
  FLASH_ARRAY(unsigned short, irCodes, ECO_RED_UP_CODE, ECO_RED_DOWN_CODE, ECO_GREEN_UP_CODE, ECO_GREEN_DOWN_CODE,
      ECO_BLUE_UP_CODE, ECO_BLUE_DOWN_CODE, ECO_WHITE_UP_CODE, ECO_WHITE_DOWN_CODE, SATPP_RED_UP_CODE,
      SATPP_RED_DOWN_CODE, SATPP_GREEN_UP_CODE, SATPP_GREEN_DOWN_CODE, SATPP_BLUE_UP_CODE, SATPP_BLUE_DOWN_CODE,
      SATPP_WHITE_UP_CODE, SATPP_WHITE_DOWN_CODE, SATP_RED_UP_CODE, SATP_GREEN_UP_CODE, SATP_BLUE_UP_CODE,
      SATP_WHITE_UP_CODE, SATP_RED_DOWN_CODE, SATP_GREEN_DOWN_CODE, SATP_BLUE_DOWN_CODE, SATP_WHITE_DOWN_CODE);

  // Loop through the colors
  for (byte i = 0; i < 4; ++i)
  {
    // Check if we have to adjust the colors
    if (((byte *)&calcValues)[i] > ((byte *)&gCurrentColorValues)[i])
    {
      // Log details
      DEBUG_LOG(F("Sending IR signal to increase color: "));
      DEBUG_LOG_LN(i);
      
      // Send the IR code, wait, and adjust the current color value
      IRsend irSend;
      irSend.sendNEC((((unsigned long)CODE_PREFIX) << 16) | irCodes[i * 2], 32);
      delay(333);
      ++((byte*)&gCurrentColorValues)[i];
    }
    else if (((byte *)&calcValues)[i] < ((byte*)&gCurrentColorValues)[i])
    {
      // Log details
      DEBUG_LOG(F("Sending IR signal to decrease color: "));
      DEBUG_LOG_LN(i);

      // Send the IR code, wait, and adjust the current color value
      IRsend irSend;
      irSend.sendNEC((((unsigned long)CODE_PREFIX) << 16) | irCodes[i * 2 + 1], 32);
      delay(333);
      --((byte *)&gCurrentColorValues)[i];
    }
  }
}
