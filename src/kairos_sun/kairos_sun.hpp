#include <Arduino.h>
#include <NTPClient.h>

/**
 * Objeto que controla el Sol en función del tiempo.
 */
class KairosSun {
public:
  KairosSun(const uint8_t relay, const int start_hours, const int start_minute,
            const int interval);
  void SetNTPClient(NTPClient *timeClient);
  bool Update();

private:
  uint8_t relay;
  int start_hour;
  int start_minute;
  int interval;

  NTPClient *timeClient;
};
