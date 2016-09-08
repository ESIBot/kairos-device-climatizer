#include <Arduino.h>
#include <RTClib.h>

enum Mode { on, off, timer };

/**
 * Objeto que controla el Sol en función del tiempo.
 */
class KairosSun {
public:
  KairosSun(const uint8_t relay, const int start_hours, const int start_minute,
            const int interval, const int update_interval);
  void SetRTC(RTC_DS1307 rtc);
  void SetMode(Mode);
  bool Update();
  bool State();

private:
  bool check();
  bool turn_on();
  bool turn_off();

  uint8_t relay;
  int start_hour;
  int start_minute;
  int interval;
  bool state;
  Mode mode;
  unsigned int update_interval;
  unsigned long last_update;

  RTC_DS1307 rtc;
};
