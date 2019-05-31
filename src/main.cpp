/**
 * A simple Azure IoT example for sending telemetry.
 */
#include "esp_wpa2.h"
#include <WiFi.h>
#include "Esp32MQTTClient.h"
#include <HTTPClient.h>
#include "DHTesp.h"
#include <Arduino.h>

// Please input the SSID and password of WiFi
const char *ssid = "<Your SSID>"; // The SSID
const char *password = "<Your WIFI Password>";

//GPIO 15
#define DHTpin 15

DHTesp dht;

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static const char* connectionString = "<Your IoT Hub Connection String>";

static bool hasIoTHub = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting connecting WiFi.");
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
 


  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED)
  {
      delay(1000);
      Serial.print(".");        
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //Not Connected to IoT Hub
  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;
  //Setup the DHT11 using GPIO15
  dht.setup(DHTpin, DHTesp::DHT11);
}

void loop() {
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  
  Serial.println("start sending events.");
  if (hasIoTHub)
  {
    char buff[256];
    //Send the in JSON format
    String res = "{\"Weather\": {\"Temperature\": \"";
    res.concat(temperature);
    res.concat("\",\"Humidity\": \"");
    res.concat(humidity);
    res.concat("\"}}");

    // Replace the following line with your data sent to Azure IoTHub
    snprintf(buff, 256, res.c_str());
    Serial.println(res);
    if (Esp32MQTTClient_SendEvent(buff))
    {
      Serial.println("Sending data succeed");
    }
    else
    {
      Serial.println("Failure...");
    }
    delay(5000);
  }
}