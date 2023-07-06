#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>
#include <WiFi.h>
#include "ThingsBoard.h"
#include "BH1750.h"
#include "DHTesp.h"

// Pin Definitions
#define DHTPIN 4
#define BH1750_ADDRESS 0x23 // I2C address of light sensor
#define BAUD_RATE 9600

#define update_data_interval 5000

// Wi-Fi Network: SSID and Password
const char *ssid = "javier";
const char *password = "javier01112003";

// ThingsBoard credentials and settings
#define ThingsBoard_Access_Token "ohxwJ80GW9xubPTTVBE6"
#define ThingsBoard_Server "demo.thingsboard.io"

WiFiClient espClient;
ThingsBoard tb(espClient);
Ticker timerSendData;
DHTesp dht;
BH1750 lightMeter; // I2C address 0x23

void connectToWiFi();
void sendDataToThingsBoard();
void setup()
{
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize Temperature and Humidity sensor
  dht.setup(DHTPIN, DHTesp::DHT11);

  // Initialize BH1750 sensor
  Wire.begin(21, 22); // SDA: 21, SCL: 22
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE, 0x23, &Wire);
  connectToWiFi(); // Initialize light sensor

  //timerSendData yang dimana memanggil funsi sendDataToThingsBoard secara berulang setiap interval
  timerSendData.attach_ms(update_data_interval, sendDataToThingsBoard);
  Serial.println("Sistem Sudah Berjalan!");
}

// Terhubung ke Website ThingsBoard
void loop()
{
  if (!tb.connected())
  {
    if (tb.connect(ThingsBoard_Server, ThingsBoard_Access_Token))
      Serial.println("Terhubung ke Website ThingsBoard");
    else
    {
      Serial.println("Error connecting to ThingsBoard");
      delay(3000);
    }
  }
  tb.loop();
}

 // Mengirim data-data ke ThingsBoard
void sendDataToThingsBoard()
{
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  float lux = lightMeter.readLightLevel();

  if (dht.getStatus() == DHTesp::ERROR_NONE)
  {
    Serial.printf
    ("Temperature: %.2f °C, Humidity: %.2f %%, Light: %.2f lux\n",
     temperature, humidity, lux
    );
  if (tb.connected())
    {
      tb.sendTelemetryFloat("Temperature", temperature);
       // Temperature: ... °C

      tb.sendTelemetryFloat("Humidity", humidity);
      // Humidity: ... %

      tb.sendTelemetryFloat("Light", lux);
      // Light: ... lux
    }
  }
}

//Connect to Wi-Fi Network
void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Koneksi Gagal! Memulai ulang...");
    delay(5000);
    ESP.restart();
  }

  Serial.print("Sistem terhubung dengan alamat IP: ");
  Serial.println(WiFi.localIP());
  Serial.printf("RSSI: %d\n", WiFi.RSSI());
}
