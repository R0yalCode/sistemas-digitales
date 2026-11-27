
/*
  PRACTICAS 5 Y 6 - SISTEMAS DIGITALES
  Parte B: Escaner I2C
  ESP32 DevKit V1

  SDA -> GPIO21
  SCL -> GPIO22
*/

#include <Wire.h>

const int PIN_SDA = 21;
const int PIN_SCL = 22;

void setup() {

  Serial.begin(115200);

  Wire.begin(PIN_SDA, PIN_SCL);

  Serial.println();
  Serial.println("================================");
  Serial.println("   ESCANER DE DISPOSITIVOS I2C");
  Serial.println("================================");

  delay(1000);
}

void loop() {

  byte direccion;
  byte error;

  int dispositivosEncontrados = 0;

  Serial.println();
  Serial.println("Buscando dispositivos I2C...");

  for (direccion = 1; direccion <= 126; direccion++) {

    Wire.beginTransmission(direccion);
    error = Wire.endTransmission();

    if (error == 0) {

      Serial.print("Dispositivo encontrado en 0x");

      if (direccion < 16) {
        Serial.print("0");
      }

      Serial.println(direccion, HEX);

      dispositivosEncontrados++;
    }
  }

  if (dispositivosEncontrados == 0) {

    Serial.println("No se encontraron dispositivos.");
  }

  else {

    Serial.print("Total de dispositivos encontrados: ");
    Serial.println(dispositivosEncontrados);
  }

  Serial.println("Escaneo finalizado.");

  delay(5000);
}
