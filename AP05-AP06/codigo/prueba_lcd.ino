
/*
  SISTEMAS DIGITALES
  Prueba independiente de LCD 16x2 I2C
  ESP32 DevKit V1

  SDA -> GPIO21
  SCL -> GPIO22
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Cambiar por la direccion encontrada en el escaner I2C
const byte DIRECCION_LCD = 0x27;

LiquidCrystal_I2C lcd(DIRECCION_LCD, 16, 2);

void setup() {

  Serial.begin(115200);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  // Mensaje en la primera fila
  lcd.setCursor(0, 0);
  lcd.print("LCD funcionando");

  // Mensaje en la segunda fila
  lcd.setCursor(0, 1);
  lcd.print("Prueba ESP32");

  Serial.println("Prueba de LCD iniciada");
  Serial.println("Texto enviado a la pantalla");
}

void loop() {

  // No es necesario repetir el mensaje.
}
