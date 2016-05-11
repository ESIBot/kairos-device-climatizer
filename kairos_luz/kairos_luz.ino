/*
Título: RTC_Arduino, Interrupciones por tiempo y uso de relé para encendido de bombillas.
Descripción: Uso básico del reloj RTC DS1307 (RTCLIB.h), uso de interrupciones (TimerOne.h) y de relé.
Autor: frankamente
Fecha: 11/05/2016
URL: https://github.com/frankamente
email: fran29400@gmail.com
Referencia: http://www.tuelectronica.es/tutoriales/arduino/reloj-rtc-i2c-con-arduino.html  http://www.prometec.net/timers/
*/

#include <Wire.h> 
#include "RTClib.h"
#include <TimerOne.h>

const int rele1 = 11;  // Pin asociado a la entrada 1 del Relé
const int rele2 = 12;  // Pin asociado a la entrada 2 del Relé
int estadoLuz = LOW;    // La luz empieza apagado

int contador=0; //Variables usadas en la interrupción
int contadorAnterior=0;

int hora =17; //Hora a la que se enciende
int minutos =13; //Minuto al que queremos que se encienda

int horasEncendida =0;//Horas que queremos que esté encendida
int minutosEncendida = 1; //Minutos que queremos que esté encendida
double convertirASegundos = 1000000;
int segundos = 2; // Dispara cada 2 segundos la interrupción
int encendido =0; // a 0 luz apagada, 1 encendida
RTC_DS1307 RTC;

void setup () {
   pinMode(rele1, OUTPUT); //Declaramos el pin asociado al rele1 como salida
   pinMode(rele2, OUTPUT); //Declaramos el pin asociado al rele2 como salida
   
   Timer1.initialize(segundos*convertirASegundos); //Inicializamos un timer.
   Timer1.attachInterrupt(interrupcion); // Activa la interrupcion y la asocia a ISR_Blink
   
   Wire.begin(); // Inicia el puerto I2C
   RTC.begin(); // Inicia la comunicación con el RTC
   //RTC.adjust(DateTime(__DATE__, __TIME__)); // Establece la fecha y hora (Comentar una vez establecida la hora)
   
   Serial.begin(9600); // Establece la velocidad de datos del puerto serie
}

void interrupcion()
   {    
       
       contador++    ;     // Contador veces se enciende el LED
        
       
   }

void loop(){

  //if (blinkCount%2 == 0){ //Cada 2 Segundos
    if (contador!=contadorAnterior){
      contadorAnterior=contador;
    DateTime now; //Declaramos una variable tipo DateTime llamada now
    now = RTC.now(); //  Leemos la hora del reloj 
      /* 
       *  Serial.print(now.day(), DEC); // Dia
       *  Serial.print(' ');
       *  Serial.print(now.hour(), DEC); // Horas
       *  Serial.print(':');
       *  Serial.print(now.minute(), DEC); // Minutos
       *  Serial.print(':');
       *  Serial.print(now.second(), DEC); // Segundos
       *   Serial.println(); 
       */
       
    if (now.hour()==hora && now.minute()==minutos && estadoLuz == LOW){
      
      estadoLuz=HIGH;
      Serial.println("Encendida");
      Serial.println(estadoLuz);
      
    }
    if (now.hour()==(hora+horasEncendida) && now.minute()==(minutos+minutosEncendida) && estadoLuz == HIGH){
      
      estadoLuz=LOW;
      Serial.println("Apagada");
      Serial.println(estadoLuz);
      
    }
  }
digitalWrite(rele1,estadoLuz);
       //delay(100);

}

