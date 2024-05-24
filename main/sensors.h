#ifndef SENSORS_H
#define SENSORS_H

#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

// Pluviometer constants
#define DIAMETRO 125 // diametro interno do balde
#define RAIO 6.25 // raio interno do balde
#define VOLUME 3.05 // volume da bascula (em cm3) (1cm3 == 1ml) (1ml == 1000mm3)

// Anemometer constants
#define PERIOD 5000 // Measurement period in milliseconds
#define RADIUS 147 // Anemometer radius in mm

class DhtSensor {
  private:
    DHT dht;

  public: 
    DhtSensor(uint8_t pin, uint8_t type) : dht(pin, type) {}

    void begin() {
      dht.begin();
    }

    void getDht() {
      float temp = dht.readTemperature();
      float umidade = dht.readHumidity();

      Serial.print("TEMP C ");
      Serial.print(temp);

      Serial.print(" -- Umidade ");
      Serial.print(umidade);
      Serial.println(" % ");
    }
};

class BmpSensor {
  private:
    Adafruit_BMP280 bmp;

  public:
    bool begin(uint8_t address) {
      return bmp.begin(address); 
    }

    uint8_t sensorID() {
      return bmp.sensorID();
    }
    
    void setSampling(Adafruit_BMP280::sensor_mode mode, Adafruit_BMP280::sensor_sampling tempSampling, Adafruit_BMP280::sensor_sampling pressSampling, Adafruit_BMP280::sensor_filter filter, Adafruit_BMP280::standby_duration duration) {
      bmp.setSampling(mode, tempSampling, pressSampling, filter, duration);
    }

    void getBmp() {
      Serial.print(F("Temperature = "));
      Serial.print(bmp.readTemperature());
      Serial.println(" *C");

      Serial.print(F("Pressure = "));
      Serial.print(bmp.readPressure());
      Serial.println(" Pa");

      Serial.print(F("Approx altitude = "));
      Serial.print(bmp.readAltitude(1011)); 
      Serial.println(" m");

      Serial.println();
    }
};

class PluviometerSensor {
  private:
    int val = 0;
    int old_val = 0;
    float volume_coletado;
    
  public:
    static volatile unsigned long ContactBounce;
    static volatile unsigned long REEDCOUNT;

    void getRain(){
      // float area_recipiente = 3.14159265 * (RAIO * RAIO); // área da seção transversal do recipiente em cm²
      // float volume_por_virada = (VOLUME/area_recipiente);
      volume_coletado = (REEDCOUNT * 0.25) * 10; // volume total coletado em cm³

      Serial.print("Viradas: ");
      Serial.println(REEDCOUNT);

      Serial.print("Chuva: ");
      Serial.print (volume_coletado);
      Serial.println(" mm");
    }
};

class AnemometerSensor {
  private:
    unsigned int sampleNumber = 0;  // Sample number
    unsigned int rpm = 0;  // Revolutions per minute
    float windSpeedMetersPerSecond = 0;  // Wind speed in m/s
    float windSpeedKilometersPerHour = 0;  // Wind speed in km/h

  public:
    static volatile unsigned int counter; // Magnet counter for sensor

    void measureWindSpeed() {
      counter = 0;  
      long startTime = millis();
      while (millis() < startTime + PERIOD) {}
      // Wait for the period to complete
    }

    void calculateRPM() {
      rpm = (counter * 60) / (PERIOD / 1000); // Calculate RPM
    }

    void calculateWindSpeedMetersPerSecond() {
      windSpeedMetersPerSecond = ((4 * PI * RADIUS * rpm) / 60) / 1000; // Calculate wind speed in m/s
    }

    void calculateWindSpeedKilometersPerHour() {
      windSpeedKilometersPerHour = windSpeedMetersPerSecond * 3.6; // Convert m/s to km/h
    }

    void getAnemometer() {
      sampleNumber++;
      Serial.print(sampleNumber);
      Serial.print(": Start measurement...");
      measureWindSpeed();
      Serial.println(" finished.");
      Serial.print("Counter: ");
      Serial.print(counter);
      Serial.print("; RPM: ");
      calculateRPM();
      Serial.print(rpm);
      Serial.print("; Wind speed: ");
      
      // Print wind speed in m/s
      calculateWindSpeedMetersPerSecond();
      Serial.print(windSpeedMetersPerSecond);
      Serial.print(" [m/s] ");              
      
      // Print wind speed in km/h
      calculateWindSpeedKilometersPerHour();
      Serial.print(windSpeedKilometersPerHour);
      Serial.print(" [km/h] ");  
      Serial.println();
    }
};

class WindIndicatorSensor {
  private:
    float valor = 0; // declara a variável inicial em 0
    int Winddir = 0; // Declara o valor inicial em 0
    int pin;

  public:
    WindIndicatorSensor(int win_pin) : pin(win_pin) {}

    void getWindDirection() {
      valor = analogRead(pin) * (5.0 / 4095.0); // Calcula a tensão para ESP32, onde a resolução ADC é 12-bit (0-4095) e a referência é 3.3V

      // Apresenta os valores da tensão de saída no Monitor Serial
      Serial.print("Leitura do sensor: ");
      Serial.print(valor, 2); // imprime com duas casas decimais
      Serial.println(" volt");

      // Determina a direção do vento baseada na tensão
      if (valor <= 2.90) {
        Winddir = 0;
        Serial.println("Direção do Vento: Norte");
      } else if (valor <= 3.05) {
        Winddir = 315;
        Serial.println("Direção do Vento: Noroeste");
      } else if (valor <= 3.25) {
        Winddir = 270;
        Serial.println("Direção do Vento: Oeste");
      } else if (valor <= 3.45) {
        Winddir = 225;
        Serial.println("Direção do Vento: Sudoeste");
      } else if (valor <= 3.75) {
        Winddir = 180;
        Serial.println("Direção do Vento: Sul");
      } else if (valor <= 4.00) {
        Winddir = 135;
        Serial.println("Direção do Vento: Sudeste");
      } else if (valor <= 4.25) {
        Winddir = 90;
        Serial.println("Direção do Vento: Leste");
      } else if (valor <= 4.65) {
        Winddir = 45;
        Serial.println("Direção do Vento: Nordeste");
      } else {
        Winddir = 000;
        Serial.println("Direção do Vento: Indeterminada");
      }

      // Imprime o ângulo de direção do vento
      Serial.print("Ângulo: ");
      Serial.print(Winddir);
      Serial.println(" Graus");
    }
};

// Defining static interruption variables
volatile unsigned int AnemometerSensor::counter = 0;
volatile unsigned long PluviometerSensor::ContactBounce = 0;
volatile unsigned long PluviometerSensor::REEDCOUNT = 0;

// Pluviometer Interruption function
static void IRAM_ATTR isr_rain() { // This is the function that the interrupt calls to increment the turning count
  if ((millis() - PluviometerSensor::ContactBounce) > 50) { // debounce the switch contact.
    PluviometerSensor::REEDCOUNT++; // Incrementa a contagem de pulsos
    PluviometerSensor::ContactBounce = millis();
  }
}

// Anemometer Interruption function
static void countRevolution() {
  AnemometerSensor::counter++; // Increment counter for each revolution detected
}

#endif