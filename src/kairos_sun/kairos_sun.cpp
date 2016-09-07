#include "kairos_sun.hpp"

#include <Wire.h>

/**
 * El constructor recibe el PIN donde se encuentra el relé, la hora de
 * inicio, los minutos de inicio y el intervalo en minutos que debe estar
 * encendido el sistema.
 */
KairosSun::KairosSun(const uint8_t relay, const int start_hour,
                     const int start_minute, const int interval) {
  this->relay = relay;
  this->start_hour = start_hour;
  this->start_minute = start_minute;
  this->interval = interval;
}

/**
 * Configura un cliente NTP para obtener la hora
 * @param timeClient Cliente NTP
 */
void KairosSun::SetNTPClient(NTPClient *timeClient) {
  this->timeClient = timeClient;
}

/**
 * Debe llamarse periódicamente en la función loop para ir comprobando
 * si se debe apagar o encender. Devuelve true si está encendido y false si
 * apagado.
 */
bool KairosSun::Update() {
  bool on;
  int start_time = this->start_hour * 60 + this->start_minute;
  int currentTime =
      this->timeClient->getHours() * 60 + this->timeClient->getMinutes();
  int endTime = (start_time + this->interval) % (24 * 60);
  bool splited = (start_time + this->interval) / (24 * 60) > 1;

  // Enciendo si estamos entre la hora de encendido y de apagado
  if (!splited && (currentTime >= start_time && currentTime <= endTime) ||
      splited && (currentTime >= start_time || currentTime <= endTime)) {
    digitalWrite(this->relay, LOW);
    on = true;
    Serial.printf("Encendido\n");
  } else {
    digitalWrite(this->relay, HIGH);
    on = false;
    Serial.printf("Apagado\n");
  }

  return on;
}
