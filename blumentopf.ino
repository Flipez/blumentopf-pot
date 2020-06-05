#include <WiFi.h>
#include <WebServer.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

WebServer server(80);

const int AirBaseValue = 3300;
const int WaterBaseValue = 1450;
int soilMoistureValue = 0;
int soilMoistureValue2 = 0;


void setup() {
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.print("Connected (");
  Serial.print(WiFi.localIP());
  Serial.println(")");

  server.on("/", handle_OnConnect);
  server.on("/metrics", handle_Metrics);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  server.send(200, "text/plain", "blumentopf"); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

void handle_Metrics(){
  unsigned long StartTime = millis();  
  
  soilMoistureValue = analogRead(32);
  soilMoistureValue2 = analogRead(33);

  unsigned long CurrentTime = millis();

  String response = "";
  response += "blumentopf_moisture_value{gpio=\"32\"} " + String(soilMoistureValue);
  
  response += "\nblumentopf_moisture_value{gpio=\"33\"} " + String(soilMoistureValue2);

  response += "\nblumentopf_water_base{gpio=\"32\"} " + String(WaterBaseValue);

  response += "\nblumentopf_water_base{gpio=\"33\"} " + String(WaterBaseValue);

  response += "\nblumentopf_air_base{gpio=\"32\"} " + String(AirBaseValue);

  response += "\nblumentopf_air_base{gpio=\"33\"} " + String(AirBaseValue);

  response += "\nblumentopf_scrape_duration_milliseconds " + String(CurrentTime - StartTime);

  response += "\nblumentopf_uptime_milliseconds " + String(StartTime);

  response += "\nblumentopf_onboard_temperature_celsius " + String((temprature_sens_read() - 32) / 1.8);

  response += "\nblumentopf_up 1";

  server.send(200, "text/plain", response);
}
