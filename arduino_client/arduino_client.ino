#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ESP8266HTTPClient.h>  // Include the HTTP client library: 
#include <WiFiClient.h>         // Include the WiFi client library: 
#include <ArduinoJson.h>        // Include the JSON formatting library
#include <RunningMedian.h>
#include <Thread.h>
#include <Fetch.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

// Declare all the global constants:

// Declare the HTTP client:
HTTPClient http;
WiFiClient client;

// Define the pins for sensors and actuators: 
const int TEMPERATURE_PIN = D1;

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(TEMPERATURE_PIN);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

// Create running median filters for the two sensors
RunningMedian buffer_temp = RunningMedian(10);

// Threads for actuation and data transmission: 
Thread* data_transmission_thread = new Thread();

// Declare variables for the events: 
String wifi_ssid;

// Server names and URL's:
// String SERVER_BASE_URL     = "https://plum-cockroach-gown.cyclic.app";    // Deployment server//
String SERVER_BASE_URL   = "https://192.168.86.38:3000";                  // Local testing server//


// Callback function to setup the wifi connection: 
bool setupWifi(const char* ssid, const char* password) {

  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Trying to connect to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i < 30) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  Serial.println("  ");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println('\n');
    Serial.println("Connection established!");  
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

    if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
      Serial.println("mDNS responder started");
    } else {
      Serial.println("Error setting up MDNS responder!");
    }

    wifi_ssid = ssid;
    return true;
  }
  else
  {
    return false;
  }
}

float readTemperature() 
{
  // Send the command to get temperatures
  sensors.requestTemperatures(); 

  return sensors.getTempCByIndex(0);

  //print the temperature in Celsius
//  Serial.print("Temperature: ");
//  Serial.print(sensors.getTempCByIndex(0));
//  Serial.print((char)176);//shows degrees character
//  Serial.print("C  |  ");
//  
//  //print the temperature in Fahrenheit
//  Serial.print((sensors.getTempCByIndex(0) * 9.0) / 5.0 + 32.0);
//  Serial.print((char)176);//shows degrees character
//  Serial.println("F");
}



void transmissionCallback() {

    if (WiFi.status() == WL_CONNECTED) { 

      http.begin(client, SERVER_BASE_URL + "/api/esp8266data");
      http.addHeader("Content-Type", "application/json");
      Serial.println(" Data transmission callback");
      
      const int capacity = JSON_OBJECT_SIZE(5);
      StaticJsonDocument<capacity> esp_data;
  
      // Fill in the data: 
      esp_data["temperature"] = buffer_temp.getMedian();
      esp_data["wifi_ssid"] = wifi_ssid;
  
      String esp_data_str;
      serializeJson(esp_data, esp_data_str);

      int esp_data_res_code = http.POST(esp_data_str);

      if (esp_data_res_code > 0){
        Serial.println("Successful data transmission");
      }
      else {
        Serial.println("Failed to transmit");
        Serial.println(esp_data_res_code);
      }

      //Todo: Check the response code:
      http.end();  // Close connection

//      RequestOptions options;
//      options.method = "POST";
//      options.headers["Content-Type"] = "application/json";
//      options.headers["Content-Length"] = strlen(esp_data_str.c_str());
//      options.fingerprint = SHA1_FINGERPRINT;
//      options.body = esp_data_str;

      // Making the request
//      String DATA_POST_URL = SERVER_BASE_URL + "/api/esp8266data";
//      Response response = fetch(DATA_POST_URL.c_str(), options);
//      Serial.println(response);/
    
   }

}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(10);
  Serial.println('\n');

  // Setup the wifi connection: 
  bool wifi_res_1 = setupWifi(ssid, password);

//  if (wifi_res_1 == false)
//  {
//    bool wifi_res_2 = setupWifi(ssid_2, password_2);
//  }




  // Initialize the actuation pins:
  pinMode(TEMPERATURE_PIN, INPUT);

  // Start the temperature sensing library: 
  sensors.begin();

  data_transmission_thread->onRun(transmissionCallback);
  data_transmission_thread->setInterval(1000);  // milliseconds

}

void loop() {

  if (WiFi.status() != WL_CONNECTED)
  {
    bool wifi_res_1 = setupWifi(ssid, password);

//    if (wifi_res_1 == false)
//    {
//      bool wifi_res_2 = setupWifi(ssid_2, password_2);
//    }
  }

  // put your main code here, to run repeatedly:
  const float temperature = readTemperature();

  // Add sensor data to the buffers:
  buffer_temp.add(temperature);

  // Transmit the current data to the server
  if(data_transmission_thread->shouldRun() && WiFi.status() == WL_CONNECTED) 
    data_transmission_thread->run();  
  
  // Set sample rate to ~100Hz: 
  delay(500);

  // Prints for debugging
  Serial.print("Temperature is: ");
  Serial.println(temperature);
}
