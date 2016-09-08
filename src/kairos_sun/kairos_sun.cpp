#include "kairos_sun.hpp"

/**
 * El constructor recibe el PIN donde se encuentra el relé, la hora de
 * inicio, los minutos de inicio y el intervalo en minutos que debe estar
 * encendido el sistema.
 */
KairosSun::KairosSun(const uint8_t relay, const int start_hour,
                     const int start_minute, const int interval,
                     const int update_interval) {
  this->relay = relay;
  this->start_hour = start_hour;
  this->start_minute = start_minute;
  this->interval = interval;
  this->update_interval = update_interval;
}

/**
 * Configura un cliente NTP para obtener la hora
 * @param timeClient Cliente NTP
 */
void KairosSun::SetRTC(RTC_DS1307 rtc) { this->rtc = rtc; }

void KairosSun::SetMode(Mode mode) { this->mode = mode; }

bool KairosSun::State() { return this->state; }

bool KairosSun::check() {
  DateTime now = this->rtc.now();

  int start_time = this->start_hour * 60 + this->start_minute;
  int currentTime = now.hour() * 60 + now.minute();
  int endTime = (start_time + this->interval) % (24 * 60);
  bool splited = ((start_time + this->interval) / (24.0 * 60.0)) > 1.0;

  return (!splited && (currentTime >= start_time && currentTime <= endTime) ||
          (splited && (currentTime >= start_time || currentTime <= endTime)));
}

bool KairosSun::turn_on() {
  bool changed = false;
  digitalWrite(this->relay, LOW);

  if (!this->state || this->last_update == 0) {
    changed = true;
  }

  // Light is on
  this->state = true;

  return changed;
}

bool KairosSun::turn_off() {
  bool changed = false;
  digitalWrite(this->relay, HIGH);

  if (this->state || this->last_update == 0) {
    changed = true;
  }

  // Light is of
  this->state = false;

  return changed;
}

/**
 * Debe llamarse periódicamente en la función loop para ir comprobando
 * si se debe apagar o encender. Devuelve true si se ha modificado el estado,
 * false en caso contrario.
 */
bool KairosSun::Update() {
  bool changed = false;

  if ((millis() - this->last_update < this->update_interval)) {
    return changed;
  }

  switch (this->mode) {
  case on:
    changed = turn_on();
    break;
  case off:
    changed = turn_off();
    break;
  case timer:
    if (check()) {
      changed = turn_on();
    } else {
      changed = turn_off();
    }
    break;
  }

  this->last_update = millis();

  return changed;
}
