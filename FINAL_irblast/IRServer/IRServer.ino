/*
 * IRremoteESP8266: IRServer - demonstrates sending IR codes controlled from a webserver
 * Version 0.3 May, 2019
 * Version 0.2 June, 2017
 * Copyright 2015 Mark Szabo
 * Copyright 2019 David Conran
 *
 * An IR LED circuit *MUST* be connected to the ESP on a pin
 * as specified by kIrLed below.
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */
#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#endif  // ESP8266
#if defined(ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#endif  // ESP32
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>
//#include <ESPAsyncWebServer.h>

const char index_html[] PROGMEM={"<!DOCTYPE html>\n<html>\n<head>\n\t<title>TV Remote</title>\n\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\t<style type=\"text/css\">body {\n\t\tbackground-color: #455a64;\n\t\ttext-align: center;\n\t}\n\t\n\th1 {\n\t\tcolor: white;\n\t}\n\tdiv.grid-container {\n\t\tdisplay: grid;\n\t\tgrid-template-columns: repeat(2, auto);\n\t\tgrid-gap: 1vw;\n\t\tmax-width: 1100px;\n\t\talign-items: center;\n\t\tjustify-items: center;\n\t\tpadding: 1vw;\n\t}\n\tdiv.numberPad{\n\t\tmax-width: 500px;\n\t\tmargin: 1% auto;\n\t\tborder-radius: 5px;\n\t\tdisplay: grid;\n\t\tgrid-template-columns: repeat(3, 80px);\n\t\tgrid-template-rows: repeat(7, 80px);\n\t\talign-items: center;\n\t\tjustify-items: center;\n\t\tpadding: 2vw; /*make the padding the same*/\n\t}\n\tdiv.powerPad{\n\t\tmax-width: 240px;\n\t\tmargin: 1% auto;\n\t\tborder-radius: 5px;\n\t\tdisplay: grid;\n\t\tgrid-template-columns: repeat(2, 100px);\n\t\tgrid-template-rows: repeat(4, 100px);\n\t\talign-items: center;\n\t\tjustify-items: center;\n\t\tpadding: 1vw; /*make the padding the same*/\n\t}\n\tbutton.circle-button {\n\t\twidth: 80px;\n\t\theight: 80px;\n\t\tbackground-color: #ddd;\n\t\tborder-radius: 50%;\n\t\tborder: none;\n\t\tbox-shadow: 2px 2px 2px #aaa;\n\t\tcursor: pointer;\n\t\tdisplay: inline;\n\t\tfont-size: 20px;\n\t}\n\tbutton.square-button {\n\t\twidth: 70px;\n\t\theight: 70px;\n\t\tbackground-color: #ddd;\n\t\tborder: none;\n\t\tbox-shadow: 2px 2px 2px #aaa;\n\t\tcursor: pointer;\n\t\tdisplay: inline-block;\n\t\tfont-size: 25px;\n\t}\n\tbutton.rectangle-button {\n\t\twidth: 60px;\n\t\theight: 40px;\n\t\tbackground-color: #ddd;\n\t\tborder: none;\n\t\tbox-shadow: 2px 2px 2px #aaa;\n\t\tcursor: pointer;\n\t\tdisplay: inline-block;\n\t\tfont-size: 20px;\n\t}\n\t</style>\n</head>\n<body>\n\t<h1>TV Remote</h1>\n\t<div class = \"grid-container\">\n\t\t<div class = \"powerPad\">\n\t\t\t<button class = \"circle-button\" id=\"on\"> power </button>\n\t\t\t<button class = \"rectangle-button\" id=\"menu\"> menu </button>\n\t\t\t \n\t\t\t<button class = \"square-button\" id=\"vol-up\">&#x1f50a </button> \n\t\t\t<button class = \"square-button\" id=\"ch-up\">&#x2B9D </button>\n\t\t\t\n\t\t\t<button class = \"square-button\" id=\"vol-down\">&#x1f508 </button>\n\t\t\t<button class = \"square-button\" id=\"ch-down\">&#x2B9F </button>\n\t\t</div>\n\t\t<div class = \"numberPad\">\n\t\t\t<div></div>\n\t\t\t<button class = \"square-button\" id=\"up\">&#x2B9D</button>\n\t\t\t<div></div>\n\t\t\t\n\t\t\t<button class = \"square-button\" id=\"left\"> &#x2B9C </button>\n\t\t\t<button class = \"square-button\" id=\"enter\">enter</button>\n\t\t\t<button class = \"square-button\" id=\"right\"> &#x27A4 </button>\n\t\t\t\n\t\t\t<button class = \"rectangle-button\" id=\"input\"> input </button>\n\t\t\t<button class = \"square-button\" id=\"down\"> &#x2B9F </button>\n\t\t\t<button class = \"rectangle-button\" id=\"mute\"> mute </button>\n\t\t\t\n\t\t\t\n\t\t\t<button class = \"square-button\" id=\"one\">1</button>\n\t\t\t<button class = \"square-button\" id=\"two\">2</button>\n\t\t\t<button class = \"square-button\" id=\"three\">3</button>\n\t\t\t\n\t\t\t<button class = \"square-button\" id=\"four\">4</button>\n\t\t\t<button class = \"square-button\" id=\"five\">5</button>\n\t\t\t<button class = \"square-button\" id=\"six\">6</button>\n\t\t\t\n\t\t\t<button class = \"square-button\" id=\"seven\">7</button>\n\t\t\t<button class = \"square-button\" id=\"eight\">8</button>\n\t\t\t<button class = \"square-button\" id=\"nine\">9</button>\n\t\t\t\n\t\t\t<div></div>\n\t\t\t<button class = \"square-button\" id=\"zero\">0</button>\n\t\t\t<div></div>\n\t\t</div>\n\t</div>\n\t<script>\n\t\tfunction power() {\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=551489775\";\n\t\t}\n\t\tfunction VolumeUp() {\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=551502015\";\n\t\t}\n\t\tfunction VolumeDown() {\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=551534655\";\n\t\t}\n\t\tfunction ChannelUp() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=551485695\";\n\t\t}\n\t\tfunction ChannelDown() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=551518335\";\n\t\t}\n\t\tfunction Input() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=551538735\";\n\t\t}\n\t\tfunction Mute() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=551522415\";\n\t\t}\n\t\tfunction Num1() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=551520375\";\n\t\t}\n\t\tfunction Num2() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=2170978686\";\n\t\t}\n\t\tfunction Num3() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=2170978686\";\n\t\t}\n\t\tfunction Num4() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=2170978686\";\n\t\t}\n\t\tfunction Num5() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=2170978686\";\n\t\t}\n\t\tfunction Num6() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=2170978686\";\n\t\t}\n\t\tfunction Num7() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=2170978686\";\n\t\t}\n\t\tfunction Num8() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=2170978686\";\n\t\t}\n\t\tfunction Num9() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=2170978686\";\n\t\t}\n\t\tfunction Num0() {\n\t\t\t// add your code here\n\t\t\tlocation.href = \"http://192.168.43.200/ir?code=2170978686\";\n\t\t}\n\t\t\n\n\t\tdocument.getElementById(\"on\").addEventListener(\"click\", power);\n\t\tdocument.getElementById(\"vol-up\").addEventListener(\"click\", VolumeUp);\n\t\tdocument.getElementById(\"vol-down\").addEventListener(\"click\", VolumeDown);\n\t\tdocument.getElementById(\"ch-up\").addEventListener(\"click\", ChannelUp);\n\t\tdocument.getElementById(\"ch-down\").addEventListener(\"click\", ChannelDown);\n\t\tdocument.getElementById(\"mute\").addEventListener(\"click\", Mute);\n\t\tdocument.getElementById(\"input\").addEventListener(\"click\", Input);\n\t\tdocument.getElementById(\"one\").addEventListener(\"click\", Num1);\n\t\tdocument.getElementById(\"two\").addEventListener(\"click\", Num2);\n\t\tdocument.getElementById(\"three\").addEventListener(\"click\", Num3);\n\t\tdocument.getElementById(\"four\").addEventListener(\"click\", Num4);\n\t\tdocument.getElementById(\"five\").addEventListener(\"click\", Num5);\n\t\tdocument.getElementById(\"six\").addEventListener(\"click\", Num6);\n\t\tdocument.getElementById(\"seven\").addEventListener(\"click\", Num7);\n\t\tdocument.getElementById(\"eight\").addEventListener(\"click\", Num8);\n\t\tdocument.getElementById(\"nine\").addEventListener(\"click\", Num9);\n\t\tdocument.getElementById(\"zero\").addEventListener(\"click\", Num0);\n\t\t\n\t\t\n\t</script>\n</body>\n</html>\n"};

const char* kSsid = "vision";
const char* kPassword = "vision1234";
MDNSResponder mdns;

#if defined(ESP8266)
ESP8266WebServer server(80);
#undef HOSTNAME
#define HOSTNAME "esp8266"
#endif  // ESP8266
#if defined(ESP32)
WebServer server(80);
#undef HOSTNAME
#define HOSTNAME "esp32"
#endif  // ESP32

const uint16_t kIrLed = 4;  // ESP GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.


  

void handleRoot() {
  //server.sendHeader("C:\Users\tony\Downloads\IR_HTML", "/index.html",true);
  server.send_P(200, "text/html", index_html);
  //server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
   //request->send(SPIFFS, "/index.html", "text/html");
  //});
}

void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      uint32_t code = strtoul(server.arg(i).c_str(), NULL, 10);
#if SEND_NEC
      irsend.sendNEC(code, 32);
#endif  // SEND_NEC
    }
  }
  handleRoot();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();

  Serial.begin(115200);
  WiFi.begin(kSsid, kPassword);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(kSsid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP().toString());

#if defined(ESP8266)
  if (mdns.begin(HOSTNAME, WiFi.localIP())) {
#else  // ESP8266
  if (mdns.begin(HOSTNAME)) {
#endif  // ESP8266
    Serial.println("MDNS responder started");
    // Announce http tcp service on port 80
    mdns.addService("http", "tcp", 80);
  }

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
#if defined(ESP8266)
  mdns.update();
#endif
  server.handleClient();
}
