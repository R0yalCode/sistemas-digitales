
/*
  PRACTICAS 5 Y 6 - SISTEMAS DIGITALES
  Parte A: Lectura del LM35 y filtro de media movil
  ESP32 DevKit V1

  Sensor LM35:
  Vout = 10 mV por cada grado Celsius

  Pines:
  LM35 Vout -> GPIO34

  Muestreo: 100 ms
  Filtro: media movil
*/

// ================= CONFIGURACION =================

const int PIN_LM35 = 34;

// Cambiar N por 1, 10 o 30 para realizar las pruebas
const int N = 10;

const unsigned long INTERVALO_MUESTREO = 100;

// ================= FILTRO =================

float buffer[N];
float suma = 0.0;

int indice = 0;
bool bufferLleno = false;

float temperaturaFiltrada = 0.0;

// ================= TIEMPO =================

unsigned long tiempoAnterior = 0;

// ================= SETUP =================

void setup() {
  Serial.begin(115200);

  analogSetAttenuation(ADC_0db);

  // Inicializar el buffer
  for (int i = 0; i < N; i++) {
    buffer[i] = 0.0;
  }

  Serial.println("Sistema LM35 iniciado");
  Serial.println("Filtro de media movil");
}

// ================= LOOP =================

void loop() {

  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= INTERVALO_MUESTREO) {

    tiempoAnterior = tiempoActual;

    // Leer voltaje calibrado en milivoltios
    uint32_t milivoltios = analogReadMilliVolts(PIN_LM35);

    // Convertir milivoltios a grados Celsius
    float temperaturaCruda = milivoltios / 10.0;

    // ================= MEDIA MOVIL =================

    if (!bufferLleno) {

      buffer[indice] = temperaturaCruda;
      suma += temperaturaCruda;

      indice++;

      if (indice >= N) {
        indice = 0;
        bufferLleno = true;
      }

      // Durante el llenado se promedian las muestras disponibles
      int muestrasActuales = bufferLleno ? N : indice;

      if (muestrasActuales == 0) {
        muestrasActuales = N;
      }

      temperaturaFiltrada = suma / muestrasActuales;

    } else {

      // Restar la muestra antigua
      suma -= buffer[indice];

      // Guardar la nueva muestra
      buffer[indice] = temperaturaCruda;

      // Sumar la nueva muestra
      suma += temperaturaCruda;

      // Calcular media
      temperaturaFiltrada = suma / N;

      // Avanzar en el buffer circular
      indice++;

      if (indice >= N) {
        indice = 0;
      }
    }

    // ================= SERIAL PLOTTER =================

    Serial.print("T_cruda:");
    Serial.print(temperaturaCruda, 1);

    Serial.print(",T_filtrada:");
    Serial.println(temperaturaFiltrada, 1);
  }
}
