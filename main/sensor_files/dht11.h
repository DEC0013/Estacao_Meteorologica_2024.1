#ifndef DHT11_H
#define DHT11_H

#include "../sensor.h"
#include <DHT.h>
#include <WiFi.h>

const char* ssid = "Cowork-Extensao"; // "SUA_REDE_WIFI"
const char* password = "extensaocts"; // "SUA_SENHA"


// DHT11 sensor class
class DhtSensor : public Sensor {
  private:
    DHT dht;
    float temperature = 0;
    float humidity = 0;

  public: 
    DhtSensor(uint8_t pin, uint8_t type);
    void begin();
    void getData();
    String getSensorData() override;
    ~DhtSensor();
};

DhtSensor::DhtSensor(uint8_t pin, uint8_t type) : dht(pin, type) {}

void DhtSensor::begin() {
  dht.begin();
}

void DhtSensor::getData() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
}

void read_dht_temp(char* temp_str) {
    float temp = dht.readTemperature();
    dtostrf(temp, 4, 2, temp_str);
}

void read_dht_humidity(char* humidity_str) {
    float humidity = dht.readHumidity();
    dtostrf(humidity, 4, 2, humidity_str);
}



String DhtSensor::getSensorData() {
  //getData();

  

  char temp_str[6];  // "xx.xx" + null terminator
  char humidity_str[6];  // "xx.xx" + null terminator

  read_dht_temp(temp_str);
  read_dht_humidity(humidity_str);

  
  char buffer[100];
  int idStation = 0;

  snprintf(buffer, sizeof(buffer), "{\"idStation\": \"%d\", \"temperatura\": \"%s\", \"umidade\": \"%s\"}",  idStation, temp_str, humidity_str);

  //--------------------------------------------------------------------------------------

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  
  Serial.println("Conectado ao WiFi");

  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://estacao-meteorologica.vercel.app/dht");  
    http.addHeader("Content-Type", "application/json");

   
    Serial.println(buffer);
    
    
    int httpResponseCode = http.POST(buffer);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Erro no envio, código: ");
      Serial.println(httpResponseCode);
    }

    http.end();


  }

  else {
    Serial.println("Não há conexão Wi-Fi disponível. Tentando reconectar...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
}

  // --------------------------------------

  return String(buffer);
}

DhtSensor::~DhtSensor() {}

#endif