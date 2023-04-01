#include <Arduino.h>

const uint8_t _sensorTinaco0    = 23;
const uint8_t _sensorTinaco100  = 13;
const uint8_t _releDeBomba      = 2;
const uint8_t botonInterruptor  = 12;

uint32_t tiempoDeFalso = 800;
bool estadoBomba       = false; //False = OFF | True = ON

//TIMERS
uint64_t tiempoAnterior               = 0;
uint64_t tiempoBomba                 = 3000000; // 50min  = 3 000000 mseg
uint64_t tiempoBomba_previo          = 0;
uint64_t tiempoBombaMargen           = 300000;  // 5min   = 300000  mseg
uint64_t tiempoBombaMargen_previo    = 0;
uint64_t tiempoBombaReceso           = 7200000; // 2hrs   = 7200000 mseg
uint64_t tiempoBombaReceso_previo    = 0;

uint64_t tiempolecturaSensores        = 10000; // 10seg = 10,000 mseg
uint64_t tiempolecturaSensores_previo = 0;

// Falta considerar el caso donde no haya agua en la toma general y la bomba se quede encendida sin pasar agua

void apagarBomba(){
  estadoBomba = false;
  digitalWrite(_releDeBomba,LOW);
  
  tiempoBombaReceso_previo = millis();
}

void encenderBomba(){
  estadoBomba = true;
  digitalWrite(_releDeBomba,HIGH);
  tiempoBomba_previo = millis();
  tiempoBombaMargen_previo = millis();
}

void controlTinaco() {

  bool sensorTinaco0    = digitalRead(_sensorTinaco0);
  // bool sensorTinaco100  = digitalRead(_sensorTinaco100);

  // Si la bomba está encendida
  if (estadoBomba) {

    // Si pasa el tiempoBomba, la bomba se apaga
    if (millis() - tiempoBomba_previo > tiempoBomba) {
      Serial.println("Tinaco_N_0 Bomba OFF por tiempo");  
      apagarBomba();

    // Si no pasa el tiempo de envendido, pero si sensor tinaco (Lleno) y pasó el tiempo margen
    } else if (sensorTinaco0 && millis() - tiempoBombaMargen_previo > tiempoBombaMargen) {
        Serial.println("Tinaco_N_0 Bomba OFF por margen"); 
        apagarBomba();
    }

  //Si la bomba está apagada
  } else {
    // Si sensor de tinaco está vacío Y el delta de tiempo es mayor que el tiempoBombaReceso
    if (!sensorTinaco0 && millis() - tiempoBombaReceso_previo > tiempoBombaReceso) {
      Serial.println("Tinaco_N_0 Bomba ON");
      encenderBomba();
    }
  }
  
}

void setup() {
  Serial.begin(115200);

  pinMode(_sensorTinaco0,   INPUT_PULLDOWN);
  pinMode(_sensorTinaco100, INPUT_PULLDOWN);
  pinMode(_releDeBomba,     OUTPUT);

  //Instalar una resistencia Pull-down
  // attachInterrupt(digitalPinToInterrupt(botonInterruptor), cambiarEstadoBomba, FALLING);

}

void loop() {

  if (millis() - tiempolecturaSensores_previo > tiempolecturaSensores) {
    Serial.print("SensorT0: ");
    Serial.print(digitalRead(_sensorTinaco0));
    Serial.print(" EstadoBomba: ");
    Serial.print(estadoBomba);
    Serial.print(" TiempoReceso: ");
    Serial.println(millis() - tiempoBombaReceso_previo);

    controlTinaco();

    tiempolecturaSensores_previo = millis();
  }

}

