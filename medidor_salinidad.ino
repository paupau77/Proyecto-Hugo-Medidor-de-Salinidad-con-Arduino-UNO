/*
  Medidor de Salinidad Básico con Arduino UNO
  -------------------------------------------
  Dispositivo básico para medir la conductividad eléctrica de una muestra líquida
  (simulada con un potenciómetro), mostrando en pantalla LCD 16x2 I2C el voltaje,
  conductividad, salinidad y temperatura.

  Autora: Paulina Juich

  Licencia:
  Este proyecto fue desarrollado por Paulina Juich y registrado en la DNDA (Argentina)
  bajo el número de expediente EX-2025-78014687- el 18 de Julio de 2025.

  Todo el contenido de este repositorio (código fuente, diseño electrónico, documentación)
  se encuentra protegido por derechos de autor.

  © 2025 Paulina Juich. Todos los derechos reservados.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= PINES =================
const int sensorPin = A0;        // Conductividad
const int pinTemperatura = A1;   // LM35
const int pinPH = A2;            // Módulo de pH PH-4502C
const int buttonPin = 2;         // Botón (PULLUP)

// ================= CONFIGURACIÓN GENERAL =================
float maxConductividad = 50.0;           // mS/cm (escala máxima)
bool compensarTemperatura = true;        // Compensación a 25°C
const float coefTemp = 0.02;             // 2% por °C

// ================= CALIBRACIÓN pH =================
const float phReferencia = 7.0;          // Valor de pH para el punto de referencia
float voltajePH7 = 2.5;                  // Voltaje medido para pH 7.0 (ajustar con buffer)
float pendientePH = -0.18;               // V/pH (negativo). Ajustar con buffer pH4 o pH10

// ================= FILTRO EXPONENCIAL =================
const float alpha = 0.25;                // Suavizado (0..1)
float condFiltrada = 0.0;
float tempFiltrada = 25.0;
float pHFiltrado = 7.0;

// ================= FÓRMULAS DE SALINIDAD =================
float a1 = 0.5, b1 = 0.8;
float a2 = 0.02, b2 = 0.4, c2 = 0.7;
float a3 = 0.001, b3 = -0.02, c3 = 0.5, d3 = 0.6;
int tipoFormula = 3;   // 1=Lineal, 2=Cuadrática, 3=Cúbica

// ================= ESTADO =================
bool medirActivo = true;          // Inicia encendido
bool botonPresionado = false;
unsigned long ultimaLectura = 0;
const unsigned long intervaloLectura = 300; // ms

// ================= PROTOTIPOS =================
void leerBoton();
void mostrarLectura(float conductividad, float salinidad, float temperatura, float pH);
float aplicarFiltro(float nueva, float anterior, float alpha);
float leerTemperatura();
float leerConductividad();
float calcularPH(float voltaje);

//==================================================
void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(pinTemperatura, INPUT);
  pinMode(pinPH, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Medidor");
  lcd.setCursor(0, 1);
  lcd.print("Salinidad");
  delay(2000);
  lcd.clear();

  // Valores por defecto seguros
  tempFiltrada = 25.0;
  condFiltrada = 0.0;
  pHFiltrado = 7.0;

  // Intentar primera lectura válida
  float tempInit = leerTemperatura();
  if (tempInit > -900.0) tempFiltrada = tempInit;

  float condInit = leerConductividad();
  if (condInit >= 0.0 && condInit <= maxConductividad) condFiltrada = condInit;

  // Para pH, leemos el ADC y calculamos
  int adcPH = analogRead(pinPH);
  float voltPH = adcPH * (5.0 / 1023.0);
  float pHInit = calcularPH(voltPH);
  if (pHInit > -900.0) pHFiltrado = pHInit;
}

//==================================================
void loop() {
  leerBoton();

  // Solo medir si el dispositivo está encendido
  if (medirActivo && (millis() - ultimaLectura >= intervaloLectura)) {
    // --- Lectura de sensores (una sola vez por sensor) ---
    float tempRaw = leerTemperatura();
    float condRaw = leerConductividad();

    // pH: leemos el ADC una sola vez
    int adcPH = analogRead(pinPH);
    float voltPH = adcPH * (5.0 / 1023.0);
    float pHRaw = calcularPH(voltPH);

    // --- Depuración en monitor serie ---
    Serial.print("Voltaje pH: ");
    Serial.print(voltPH, 3);
    Serial.print(" V   pH raw: ");
    Serial.println(pHRaw, 2);

    // --- Aplicar filtro solo si la lectura es válida ---
    if (tempRaw > -900.0) tempFiltrada = aplicarFiltro(tempRaw, tempFiltrada, alpha);
    if (condRaw >= 0.0 && condRaw <= maxConductividad) 
      condFiltrada = aplicarFiltro(condRaw, condFiltrada, alpha);
    if (pHRaw > -900.0) 
      pHFiltrado = aplicarFiltro(pHRaw, pHFiltrado, alpha);

    // --- Compensación de temperatura ---
    float conductividad = condFiltrada;
    if (compensarTemperatura && tempFiltrada > -900.0) {
      float denom = 1.0 + coefTemp * (tempFiltrada - 25.0);
      if (denom > 0.0) conductividad = condFiltrada / denom;
    }

    // --- Salinidad ---
    float salinidad = 0.0;
    switch (tipoFormula) {
      case 1:
        salinidad = a1 * conductividad + b1;
        break;
      case 2:
        salinidad = a2 * conductividad * conductividad + b2 * conductividad + c2;
        break;
      case 3:
        salinidad = a3 * conductividad * conductividad * conductividad +
                    b3 * conductividad * conductividad +
                    c3 * conductividad + d3;
        break;
    }

    // --- Mostrar en LCD ---
    mostrarLectura(conductividad, salinidad, tempFiltrada, pHFiltrado);

    // --- Monitor serie completo ---
    Serial.print("Cond: ");
    Serial.print(conductividad, 2);
    Serial.print(" mS/cm   Sal: ");
    Serial.print(salinidad, 2);
    Serial.print(" g/L   Temp: ");
    if (tempFiltrada > -900.0) Serial.print(tempFiltrada, 1);
    else Serial.print("---");
    Serial.print(" C   pH: ");
    if (pHFiltrado > -900.0) Serial.println(pHFiltrado, 2);
    else Serial.println("---");

    ultimaLectura = millis();
  }
}

//==================================================
// LECTURA DE TEMPERATURA (LM35)
float leerTemperatura() {
  int adc = analogRead(pinTemperatura);
  float volt = adc * (5.0 / 1023.0);
  if (volt < 0.01 || volt > 4.99) return -999.0;  // desconexión o corto
  float temp = volt * 100.0;
  if (temp < -5.0 || temp > 70.0) return -999.0;  // fuera de rango
  return temp;
}

// LECTURA DE CONDUCTIVIDAD
float leerConductividad() {
  int adc = analogRead(sensorPin);
  return (adc / 1023.0) * maxConductividad;
}

// CÁLCULO DE pH a partir del voltaje (con validación)
float calcularPH(float volt) {
  if (volt < 0.1 || volt > 4.9) return -999.0;  // inválido (desconexión o corto)
  float pH = phReferencia + (voltajePH7 - volt) / pendientePH;
  if (pH < 0.0) pH = 0.0;
  if (pH > 14.0) pH = 14.0;
  return pH;
}

//==================================================
// FILTRO EXPONENCIAL
float aplicarFiltro(float nueva, float anterior, float alpha) {
  return alpha * nueva + (1.0 - alpha) * anterior;
}

//==================================================
// LECTURA DEL BOTÓN CON ANTI-REBOTE Y ENCENDIDO/APAGADO
void leerBoton() {
  static unsigned long ultimoCambio = 0;
  const unsigned long debounce = 50;

  bool estado = digitalRead(buttonPin) == LOW;

  if (estado && !botonPresionado && millis() - ultimoCambio > debounce) {
    botonPresionado = true;
    medirActivo = !medirActivo;

    if (medirActivo) {
      // Encendido
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Iniciando...");
      delay(1000);
      lcd.clear();
      Serial.println("ENCENDIDO");
    } else {
      // Apagado
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Apagando...");
      delay(1000);
      lcd.clear();
      lcd.noBacklight();
      Serial.println("APAGADO");
    }

    ultimoCambio = millis();
  }

  if (!estado && botonPresionado) {
    botonPresionado = false;
    ultimoCambio = millis();
  }
}

//==================================================
// MOSTRAR EN LCD (SIN PARPADEOS)
void mostrarLectura(float conductividad, float salinidad, float temperatura, float pH) {
  // Línea 1: C:xx.x pH:y.y
  lcd.setCursor(0, 0);
  lcd.print("C:");
  if (conductividad >= 0 && conductividad < 1000) lcd.print(conductividad, 1);
  else lcd.print("---");
  lcd.print("  ");

  lcd.setCursor(9, 0);
  lcd.print("pH:");
  if (pH > -900.0 && pH >= 0 && pH <= 14) lcd.print(pH, 1);
  else lcd.print("---");
  lcd.print("  ");

  // Línea 2: S:xx.xg/L T:xxC
  lcd.setCursor(0, 1);
  lcd.print("S:");
  if (salinidad >= 0 && salinidad < 1000) lcd.print(salinidad, 1);
  else lcd.print("---");
  lcd.print("g/L ");

  lcd.setCursor(10, 1);
  lcd.print("T:");
  if (temperatura > -900.0 && temperatura >= -5 && temperatura <= 70) lcd.print(temperatura, 0);
  else lcd.print("---");
  lcd.print("C  ");
}