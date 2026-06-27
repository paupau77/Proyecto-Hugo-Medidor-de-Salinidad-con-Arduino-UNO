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
#include <math.h>

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= PINES =================
const int sensorPin = A0;
const int pinTemperatura = A1;
const int buttonPin = 2;

// ================= CONFIGURACIÓN =================
float maxConductividad = 50.0;

// ================= ESTADO =================
bool medirActivo = true;
bool botonPresionado = false;

// ================= FÓRMULAS =================

// Lineal
float a1 = 0.5;
float b1 = 0.8;

// Cuadrática
float a2 = 0.02;
float b2 = 0.4;
float c2 = 0.7;

// Cúbica
float a3 = 0.001;
float b3 = -0.02;
float c3 = 0.5;
float d3 = 0.6;

// Selección de fórmula
int tipoFormula = 3;

unsigned long ultimaLectura = 0;
const unsigned long intervaloLectura = 300;

//==================================================

void setup() {

  pinMode(sensorPin, INPUT);
  pinMode(pinTemperatura, INPUT);
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
}

//==================================================

void loop() {

  leerBoton();

  if (medirActivo && millis() - ultimaLectura >= intervaloLectura) {

    int adc = analogRead(sensorPin);
    int adcTemp = analogRead(pinTemperatura);

    float voltaje = adc * (5.0 / 1023.0);

    float conductividad = (adc / 1023.0) * maxConductividad;

    float voltTemp = adcTemp * (5.0 / 1023.0);

    float temperatura = voltTemp * 100.0;

    float salinidad = 0;

    switch (tipoFormula) {

      case 1:

        salinidad = a1 * conductividad + b1;
        break;

      case 2:

        salinidad =
          a2 * pow(conductividad, 2) +
          b2 * conductividad +
          c2;
        break;

      case 3:

        salinidad =
          a3 * pow(conductividad, 3) +
          b3 * pow(conductividad, 2) +
          c3 * conductividad +
          d3;
        break;
    }

    mostrarLectura(voltaje, conductividad, salinidad, temperatura);

    Serial.print("Voltaje: ");
    Serial.print(voltaje, 2);

    Serial.print(" V   Conductividad: ");
    Serial.print(conductividad, 2);

    Serial.print(" mS/cm   Salinidad: ");
    Serial.print(salinidad, 2);

    Serial.print(" g/L   Temp: ");
    Serial.print(temperatura, 1);

    Serial.println(" C");

    ultimaLectura = millis();
  }

  if (!medirActivo) {

    static bool pantallaMostrada = false;

    if (!pantallaMostrada) {

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("== PAUSADO ==");

      lcd.setCursor(0, 1);
      lcd.print("Presiona boton");

      pantallaMostrada = true;
    }

  } else {

    static bool pantallaMostrada = false;
    pantallaMostrada = false;

  }
}

//==================================================

void leerBoton() {

  static unsigned long ultimoCambio = 0;

  const unsigned long debounce = 50;

  bool estado = digitalRead(buttonPin) == LOW;

  if (estado &&
      !botonPresionado &&
      millis() - ultimoCambio > debounce) {

    botonPresionado = true;

    medirActivo = !medirActivo;

    Serial.println(medirActivo ? "MIDIENDO" : "PAUSADO");

    lcd.clear();

    ultimoCambio = millis();
  }

  if (!estado && botonPresionado) {

    botonPresionado = false;

    ultimoCambio = millis();
  }
}

//==================================================

void mostrarLectura(float voltaje,
                    float conductividad,
                    float salinidad,
                    float temperatura) {

  lcd.clear();

  lcd.setCursor(0, 0);

  lcd.print("V:");
  lcd.print(voltaje, 1);

  lcd.print(" C:");
  lcd.print(conductividad, 1);

  lcd.setCursor(0, 1);

  lcd.print("S:");
  lcd.print(salinidad, 1);

  lcd.print(" T:");
  lcd.print(temperatura, 0);
}
