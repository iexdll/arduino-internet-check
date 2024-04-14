//https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/examples/BasicHttpsClient/BasicHttpsClient.ino
//https://randomnerdtutorials.com/solved-reconnect-esp8266-nodemcu-to-wifi/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#define WIFI_SSID "login"
#define WIFI_PASS "password"
#define USE_SERIAL Serial
#define PING_HOST "www.google.com"

const int pinRelay = 13; //D7

HTTPClient https;

int failedPing;

void setup() {

  USE_SERIAL.begin(115200);
  pinMode(pinRelay, OUTPUT);

  https.setTimeout(5000);

  initWiFi();

}

void loop() {

  if (ping()) {
    failedPing = 0;
  } else {
    failedPing++;
  }

  USE_SERIAL.printf("Failed Ping: %d\n", failedPing);

  if (failedPing >= 5) {
    restartRouter();
    failedPing = 0;
  }

  delay(60*1000);

}

int ping() {

  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();

  USE_SERIAL.print("[HTTPS] begin...\n");

  if (https.begin(*client, PING_HOST, 443)) {

    USE_SERIAL.print("[HTTPS] GET...\n");

    int httpCode = https.GET();

    USE_SERIAL.printf("[HTTPS] GET... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      return true;
    }

    https.end();

  } else {
    USE_SERIAL.printf("[HTTPS] Unable to connect\n");
  }

  return false;

}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    USE_SERIAL.print(".");
  }
  USE_SERIAL.println("Connected");
  USE_SERIAL.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void restartRouter() {

  digitalWrite(pinRelay, HIGH);
  delay(5000);
  digitalWrite(pinRelay, LOW);

  USE_SERIAL.println("Restart router");

}
