
/*
  UNIVERSIDAD NACIONAL DE LOJA
  SISTEMAS DIGITALES - PRACTICAS 5 Y 6

  Sistema inteligente de monitoreo de temperatura
  LM35 + ESP32 + LCD 16x2 I2C + Alarma

  LM35 Vout -> GPIO34
  LED       -> GPIO19
  LCD SDA   -> GPIO21
  LCD SCL   -> GPIO22

  Muestreo: 100 ms
  LCD: 500 ms
  Filtro: media movil N=10

  Alarma:
  ON  >= 28 grados C
  OFF <= 26 grados C
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= PINES =================

const int PIN_LM35 = 34;
const int PIN_LED = 19;
const int PIN_SDA = 21;
const int PIN_SCL = 22;

// ================= LCD =================

// Cambiar 0x27 por 0x3F si el escaner
// I2C detecta esa direccion.
const byte DIRECCION_LCD = 0x27;

LiquidCrystal_I2C lcd(DIRECCION_LCD, 16, 2);

// ================= FILTRO =================

const int N = 10;

float buffer[N];
float suma = 0.0;

int indice = 0;
int muestras = 0;

float temperaturaCruda = 0.0;
float temperaturaFiltrada = 0.0;

// ================= TIEMPOS =================

const unsigned long INTERVALO_MUESTREO = 100;
const unsigned long INTERVALO_LCD = 500;

unsigned long tiempoMuestreo = 0;
unsigned long tiempoLCD = 0;

// ================= ALARMA =================

const float TEMPERATURA_ON = 28.0;
const float TEMPERATURA_OFF = 26.0;

bool alarma = false;

// ================= CARACTER GRADOS =================

byte grado[8] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000
};

// ================= SETUP =================

void setup() {

  Serial.begin(115200);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  analogSetAttenuation(ADC_0db);

  Wire.begin(PIN_SDA, PIN_SCL);

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, grado);

  lcd.setCursor(0, 0);
  lcd.print("Monitor Temp.");

  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");

  // Inicializar buffer
  for (int i = 0; i < N; i++) {
    buffer[i] = 0.0;
  }

  Serial.println("Sistema completo iniciado");
}

// ================= LOOP =================

void loop() {

  unsigned long tiempoActual = millis();

  // ====== LECTURA CADA 100 ms ======

  if (tiempoActual - tiempoMuestreo >=
      INTERVALO_MUESTREO) {

    tiempoMuestreo = tiempoActual;

    uint32_t milivoltios =
      analogReadMilliVolts(PIN_LM35);

    temperaturaCruda = milivoltios / 10.0;

    // ====== FILTRO DE MEDIA MOVIL ======

    if (muestras < N) {

      buffer[indice] = temperaturaCruda;
      suma += temperaturaCruda;

      muestras++;

    } else {

      suma -= buffer[indice];

      buffer[indice] = temperaturaCruda;

      suma += temperaturaCruda;
    }

    indice++;

    if (indice >= N) {
      indice = 0;
    }

    temperaturaFiltrada = suma / muestras;

    // ====== HISTÉRESIS ======

    if (!alarma &&
        temperaturaFiltrada >= TEMPERATURA_ON) {

      alarma = true;
    }

    else if (alarma &&
             temperaturaFiltrada <= TEMPERATURA_OFF) {

      alarma = false;
    }

    // ====== LED ======

    digitalWrite(PIN_LED, alarma ? HIGH : LOW);

    // ====== SERIAL ======

    Serial.print("T_cruda:");
    Serial.print(temperaturaCruda, 1);

    Serial.print(",T_filtrada:");
    Serial.print(temperaturaFiltrada, 1);

    Serial.print(",Alarma:");
    Serial.println(alarma ? "SI" : "NO");
  }

  // ====== ACTUALIZACION LCD CADA 500 ms ======

  if (tiempoActual - tiempoLCD >=
      INTERVALO_LCD) {

    tiempoLCD = tiempoActual;

    // Fila 0: temperatura
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");

    lcd.print(temperaturaFiltrada, 1);

    lcd.write((uint8_t)0);

    lcd.print("C     ");

    // Fila 1: estado alarma
    lcd.setCursor(0, 1);

    lcd.print("Alarma: ");

    if (alarma) {
      lcd.print("SI ");
    } else {
      lcd.print("NO ");
    }

    lcd.print("       ");
  }
}
