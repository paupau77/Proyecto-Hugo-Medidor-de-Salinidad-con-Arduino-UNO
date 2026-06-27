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

  ⚠️ El incumplimiento de estas condiciones podrá derivar en acciones legales
  conforme a la Ley 11.723 de Propiedad Intelectual.

  © 2025 Paulina Juich. Todos los derechos reservados.

  🧠 El uso personal, académico o educativo sin fines de lucro está permitido con atribución.
  💰 Cualquier uso comercial, distribución, modificación o integración en productos requiere
     una licencia paga o autorización expresa.

  Contacto para licencias: paulinajuich4@gmail.com

  Mejoras en este código:
  - Comentarios detallados en cada bloque
  - Antirrebote optimizado con temporización
  - Preparado para probar fórmulas lineal, cuadrática o cúbica
  - Sensor LM35 para mostrar temperatura
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

// Crear el objeto lcd con dirección I2C 0x27 y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= PINES =================
const int sensorPin = A0;          // Potenciómetro como sensor de salinidad
const int pinTemperatura = A1;     // Sensor LM35
const int buttonPin = 2;           // Pulsador conectado a GND con INPUT_PULLUP

// ================= CONFIGURACIÓN =================
float maxConductividad = 50.0;     // Valor máximo calibrado en mS/cm

// ================= ESTADO =================
bool medirActivo = true;
bool botonPresionado = false;

// ================= FÓRMULAS =================

// Fórmula LINEAL
float a1 = 0.5, b1 = 0.8;

// Fórmula CUADRÁTICA
float a2 = 0.02, b2 = 0.4, c2 = 0.7;

// Fórmula CÚBICA
float a3 = 0.001, b3 = -0.02, c3 = 0.5, d3 = 0.6;

// Selección de fórmula activa
int tipoFormula = 3;

unsigned long ultimaLectura = 0;
const unsigned long intervaloLectura = 300;

//================== SETUP ==================

void setup() {

  pinMode(sensorPin, INPUT);
  pinMode(pinTemperatura, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
  Serial.println("Iniciando medidor de salinidad...");

  lcd.setCursor(0, 0);
  lcd.print("Medidor Salinidad");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

//================== LOOP ==================

void loop() {

  leerBoton();

  if (medirActivo && (millis() - ultimaLectura >= intervaloLectura)) {

    // Lectura conductividad
    int adc = analogRead(sensorPin);

    // Lectura LM35
    int adcTemp = analogRead(pinTemperatura);

    // Conversión de conductividad
    float voltaje = adc * (5.0 / 1023.0);
    float conductividad = (adc / 1023.0) * maxConductividad;

    // Conversión LM35
    float voltTemp = adcTemp * (5.0 / 1023.0);
    float temperatura = voltTemp * 100.0;

    // --------- Salinidad ---------

    float salinidad = 0.0;

    if (tipoFormula == 1) {

      salinidad = a1 * conductividad + b1;

    } else if (tipoFormula == 2) {

      salinidad = a2 * pow(conductividad, 2)
                + b2 * conductividad
                + c2;

    } else if (tipoFormula == 3) {

      salinidad =
        a3 * pow(conductividad, 3)
        + b3 * pow(conductividad, 2)
        + c3 * conductividad
        + d3;
    }

    mostrarLectura(voltaje, conductividad, salinidad, temperatura);

    // --------- Monitor Serial ---------

    Serial.print("Voltaje: ");
    Serial.print(voltaje, 2);

    Serial.print(" V  Conductividad: ");
    Serial.print(conductividad, 2);

    Serial.print(" mS/cm  Salinidad: ");
    Serial.print(salinidad, 2);

    Serial.print(" g/L  Temperatura: ");
    Serial.print(temperatura, 1);

    Serial.println(" C");

    ultimaLectura = millis();
  }

  // Pantalla de pausa

  if (!medirActivo) {

    static bool pausaMostrada = false;

    if (!pausaMostrada) {

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("== PAUSADO ==");

      lcd.setCursor(0, 1);
      lcd.print("Presiona boton");

      pausaMostrada = true;
    }

  } else {

    static bool pausaMostrada = false;
    pausaMostrada = false;

  }
}

//================== BOTÓN ==================

void leerBoton() {

  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;

  bool estadoBoton = digitalRead(buttonPin) == LOW;

  if (estadoBoton &&
      !botonPresionado &&
      (millis() - lastDebounceTime > debounceDelay)) {

    botonPresionado = true;

    medirActivo = !medirActivo;

    Serial.println(medirActivo ? "MIDIENDO" : "PAUSADO");

    lcd.clear();

    lastDebounceTime = millis();
  }

  if (!estadoBoton && botonPresionado) {

    botonPresionado = false;

    lastDebounceTime = millis();
  }
}

//================== LCD ==================

void mostrarLectura(float voltaje,
                    float cond,
                    float sal,
                    float temp) {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltaje, 1);
  lcd.print(" C:");
  lcd.print(cond, 1);

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(sal, 1);

  lcd.print(" T:");
  lcd.print(temp, 0);
}
