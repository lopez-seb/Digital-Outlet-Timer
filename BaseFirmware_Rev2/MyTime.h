

#ifndef MyTime_h
#define MyTime_h
#include <avr/interrupt.h>

class RTC {
  
  public:
      RTC();
      void updateTime();
      void setTime(int hours, int tens, int minutes);
      void timerinit();
      int hours();
      int tens();
      int mins();
      int sec();

  private:
    int _hours = 0;
    int _tens = 0;
    int _minutes = 0;
    int _seconds = 0;

    //void timerinit();

  };
#endif
