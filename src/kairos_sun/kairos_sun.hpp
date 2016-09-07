#include <Arduino.h>
#include <RTClib.h>

/**
 * Objeto que controla el Sol en función del tiempo.
 */
class KairosSun {
public:
  KairosSun(const uint8_t relay, const int start_hours, const int start_minute,
            const int interval);
  void SetRTC(RTC_DS1307 rtc);
  bool Update();

private:
  uint8_t relay;
  int start_hour;
  int start_minute;
  int interval;

  RTC_DS1307 rtc;
};
