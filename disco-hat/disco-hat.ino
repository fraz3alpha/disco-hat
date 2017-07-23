#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

// NeoPixel stick DIN pin
#define DIN_PIN D4

// How many NeoPixels on the stick?
#define NUM_PIXELS 30

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, DIN_PIN, NEO_GRB + NEO_KHZ800);

enum { ON, OFF };
int led_state = ON;

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>Laura's Hat</title></head><body>"
  "<h1>Laura's Hat is Online!</h1>"
  "<p>What kind of jazzy pattern would you like to display?</p>"
  "<p><a href='/on'>on</a></p>"
  "<p><a href='/off'>off</a></p>"
  "<p><a href='/dim'>dim</a></p>"
  "<p><a href='/bright'>bright</a></p>"
  "<p><a href='/full'>full</a></p>"
  "<p><a href='/max'>max</a></p>"
  "</body></html>";

void handleRoot() {
  Serial.println("handleRoot");
  webServer.send(200, "text/html", responseHTML);
}

void handleLedOn() {
  Serial.println("handleOn");
  led_state = ON;
  webServer.send(200, "text/html", responseHTML);
}

void handleLedOff() {
  Serial.println("handleOff");
  led_state = OFF;
  webServer.send(200, "text/html", responseHTML);
}

void handleLedDim() {
  Serial.println("handleLedDim");
  strip.setBrightness(20);
  webServer.send(200, "text/html", responseHTML);
}

void handleLedBright() {
  Serial.println("handleLedBright");
  strip.setBrightness(50);
  webServer.send(200, "text/html", responseHTML);
}

void handleLedFull() {
  Serial.println("handleLedFull");
  strip.setBrightness(100);
  webServer.send(200, "text/html", responseHTML);
}

void handleLedMax() {
  Serial.println("handleLedMax");
  strip.setBrightness(255);
  webServer.send(200, "text/html", responseHTML);
}

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Start with all pixels off
  Serial.println("Starting access point");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Laura's Hat");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // replay to all requests with same HTML
  webServer.on("/", handleRoot);
  webServer.on("/on", handleLedOn);
  webServer.on("/off", handleLedOff);
  webServer.on("/dim", handleLedDim);
  webServer.on("/bright", handleLedBright);
  webServer.on("/full", handleLedFull);
  webServer.on("/max", handleLedMax);
  // Android captive portal. Maybe not needed. Might be handled by notFound handler.
  webServer.on("/generate_204", handleRoot);
  // Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  webServer.on("/fwlink", handleRoot);
  webServer.onNotFound(handleRoot);

  Serial.println("Starting Webserver");
  webServer.begin();
}

  uint8_t j;
  uint16_t i;

  unsigned long last_changed = 0;

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();

  if (led_state==ON) {
    if (millis() > last_changed + 20) { 
      last_changed = millis();
      j++;
      for(i=0; i< strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
    }
  } else {
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
    }
    strip.show();
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
