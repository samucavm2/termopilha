#include <math.h>
#include <Wire.h>
#include <INA3221.h>


// Endereço do INA3221 usado
#define ADDRESS_INA3221 0x40
#define BAUD_RATE 115200

INA3221 INA_1(ADDRESS_INA3221);

#define alpha 0.00482
#define betta 0.000000676
#define R0 2.016927253141618

const int termistor = A3;
const int termopilha = A0;
const float ResistorBase = 100000.0;
const float VinEntrada = 5.0;
const float AdcMax = 1023.0;

const int controle = 3;

void setup() {
    pinMode(controle, OUTPUT);
    pinMode(2, OUTPUT);
    // Iniciando a comunicação Serial
    Serial.begin(BAUD_RATE);
    Serial.println("Iniciar scan por dispositivos I2C:\n");
    Serial.println("Escaniando...\n");

    // Iniciando a comunicação I2C
    Wire.begin();
    Wire.beginTransmission(ADDRESS_INA3221);

    if (INA_1.begin() == false) {
        Serial.println("Comunicação não estabelecida.");
    } else {
        Serial.println("Comunicação estabelecida");
        Serial.println("Endereço:");
        Serial.println(INA_1.getAddress());
    }

    // Configurações do módulo INA3221
    INA_1.reset();
    INA_1.disableChannel(1);
    INA_1.disableChannel(2);

    // Definindo limite para aviso crítico
    Wire.beginTransmission(ADDRESS_INA3221);
}

void loop() {
    float tensao, corrente, resistencia, temperatura;
    tensao = INA_1.getBusVoltage(0);
    corrente = INA_1.getCurrent_mA(0);
    resistencia = (tensao / corrente * 1000); // cálculo da resistência

    float t = 0;
    digitalWrite(2, HIGH);
    delay(5000);
    t = valor_termo_pilha();

    if (tensao == 0 && corrente == 0) {
        temperatura = 27 + 273;
    } else {
        temperatura = 273 + (((sqrt(pow(alpha, 2) + 4 * betta * (resistencia / R0 - 1))) - alpha) / (2 * betta)); // temperatura em kelvin
    }
    digitalWrite(2, LOW);
    Serial.print(tensao, 5);
    Serial.print(",");
    Serial.print(corrente, 5);
    Serial.print(",");
    Serial.println(t, 5);
    analogWrite(controle, 255);
    delay(30000);
}

float valor_termo_pilha() {
    float t = 0;
    long int termo = 0;
    int i = 0;

    while (i <= 1000) {
        termo += analogRead(termopilha);
        i++;
    }

    t = (VinEntrada / AdcMax) * termo / 1000;
    return t;
}
