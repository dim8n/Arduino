#include <DS1302RTC.h>
 
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)
#define DAYS_PER_WEEK (7UL)
#define SECS_PER_WEEK (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_WEEK * 52UL)
#define SECS_YR_2000  (946684800UL) // the time at the start of y2k
 
// leap year calulator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )
 
#define DS1302_SCLK_PIN        A6    // Arduino pin for the Serial Clock
#define DS1302_IO_PIN          A7    // Arduino pin for the Data I/O
#define DS1302_CE_PIN          A8    // Arduino pin for the Chip Enable
 
static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
DS1302 rtc(DS1302_SCLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN); // Real Time Clock
 
String timestamp;
 
void setup (void)
{
  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
//  rtc.writeProtect(false);
 
  // The following lines can be commented out to use the values already stored in the DS1302
//  rtc.setDOW(FRIDAY);        // Set Day-of-Week to FRIDAY
//  rtc.setTime(9, 56, 50);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(21, 2, 2014);   // Set the date to August 6th, 2010
}
 
void loop(void)
{
  EcoSensors es;
 
  es.rtc = getEpochTime(rtc.getTime());  
  timestamp = (String) es.rtc;
  Serial.println("Timestamp: " + timestamp);
}
 
// Modified from Time.h to work with time from DS1302 library
uint32_t getEpochTime(Time tm){   
  int i;
  uint32_t seconds;
  int year = tm.year - 1970;
 
  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds= year*(SECS_PER_DAY * 365);
  for (i = 0; i < year; i++) {
    if (LEAP_YEAR(i)) {
      seconds +=  SECS_PER_DAY;   // add extra days for leap years
    }
  }
 
  // add days for this year, months start from 1
  for (i = 1; i < tm.mon; i++) {
    if ( (i == 2) && LEAP_YEAR(year)) { 
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
    }
  }
  seconds+= (tm.date-1) * SECS_PER_DAY;
  seconds+= tm.hour * SECS_PER_HOUR;
  seconds+= tm.min * SECS_PER_MIN;
  seconds+= tm.sec;
  return seconds; 
}
