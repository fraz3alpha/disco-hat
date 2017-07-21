#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

// NeoPixel stick DIN pin
#define DIN_PIN D4

// How many NeoPixels on the stick?
#define NUM_PIXELS 1

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, DIN_PIN, NEO_GRB + NEO_KHZ800);

enum { ON, OFF };
int led_state = OFF;

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>Laura's Hat</title></head><body>"
  "<h1>Laura's Hat is Online!</h1>"
  "<p>What kind of jazzy pattern would you like to display?</p>"
  "<p><a href='/on'>on</a></p>"
  "<p><a href='/off'>off</a></p>"
  "</body></html>";

void handleRoot() {
  Serial.println("handleRoot");
  webServer.send(200, "text/html", responseHTML);
}

void handleLedOn() {
  Serial.println("handleOn");
  led_state = ON;
  strip.setPixelColor(0, strip.Color(255, 255, 255));
  strip.show();
  webServer.send(200, "text/html", responseHTML);
}

void handleLedOff() {
  Serial.println("handleOff");
  led_state = OFF;
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
  webServer.send(200, "text/html", responseHTML);
}

void setup() {
  Serial.begin(115200);
  strip.begin();
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
  // Android captive portal. Maybe not needed. Might be handled by notFound handler.
  webServer.on("/generate_204", handleRoot);
  // Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  webServer.on("/fwlink", handleRoot);
  webServer.onNotFound(handleRoot);

  Serial.println("Starting Webserver");
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  Serial.println(led_state);
}
