#include <IRremote.hpp>
#include <string>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <LEAmDNS.h>
 
#ifndef STASSID
#define STASSID "ASUS_D8"
#define STAPSK "kindle_2672"
#endif

#define RepeatPeriodMillis 23
#define numberOfRepeats 2

const char *ssid = STASSID;
const char *password = STAPSK;
WebServer server(80);

int SEND_LED_PIN = 15;
int C_LED_PIN = 25;

//---------------------------------------
String tempBtn = "off";
String tempVal = "none";


void handleRoot() {
  String html ="<!DOCTYPE html><html lang=\"ja\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><script src=\"http://code.jquery.com/jquery.js\"></script><script>$(function() {$(\"button\").click(function() {var text = $(this).text();if (text === \"暗く\") {$.ajax({url: \"lightctl?mode=4\",type: \"GET\",async: true});} else if (text === \"明るく\") {$.ajax({url: \"lightctl?mode=3\",type: \"GET\",async: true});} else if (text === \"全灯\") {$.ajax({url: \"lightctl?mode=0\",type: \"GET\",async: true});} else if (text === \"消灯\") {$.ajax({url: \"lightctl?mode=1\",type: \"GET\",async: true});} else if (text === \"常夜灯\") {$.ajax({url: \"lightctl?mode=2\",type: \"GET\",async: true});}});});</script><title>Document</title><style>body {display: flex;justify-content: center;align-items: center;height: 100vh;}.button-container {display: flex;flex-wrap: wrap;align-content: center;align-items: center;flex-direction: column;gap: 10px;}.split-buttons {display: flex;width: 48%;gap: 10px;}button {width: 80vw;padding: 10px 20px;border: none;border-radius: 10px;background-color: #b9b9b9;color: black;font-size: 16px;cursor: pointer;}button:hover {background-color: #d2e8ff;}button:active {background-color: #518ac3;}</style></head><body><div class=\"button-container\"><div class=\"split-buttons\"><button>暗く</button><button>明るく</button></div><button>全灯</button><button>消灯</button><button>常夜灯</button></div></body></html>";
  server.send(200, "text/HTML", html);
}

//---------------------------------------
void Temperature(){
  tempVal = String(analogReadTemp());
  if(tempBtn == "off"){
    tempBtn = "on";    
  }else{
    tempBtn = "off";
  }
  handleRoot();
}
//--------------------------------------- 
void Lightctl(){
  int mode;
  if(server.args() > 0 && server.argName(0) == "mode"){
    mode = (int)(server.arg(0).toInt());
    switch(mode){
      case 0:
         IrSender.sendPulseDistanceWidth(38, 3150, 1550, 400, 1150, 400, 350, 0x6FA306, 32, PROTOCOL_IS_LSB_FIRST, RepeatPeriodMillis, numberOfRepeats);
        server.send(200,"text/plain","light is ON");
        break;
      case 1:
        IrSender.sendPulseDistanceWidth(38, 3100, 1550, 400, 1150, 400, 400, 0x806FA306, 32, PROTOCOL_IS_LSB_FIRST, RepeatPeriodMillis, numberOfRepeats);
        server.send(200,"text/plain","light is OFF");
        break;
      case 2:
        IrSender.sendPulseDistanceWidth(38, 3150, 1500, 450, 1150, 450, 350, 0x206FA306, 32, PROTOCOL_IS_LSB_FIRST, RepeatPeriodMillis, numberOfRepeats);
        server.send(200,"text/plain","light is nightlight");
        break;
      case 3:
        IrSender.sendPulseDistanceWidth(38, 3150, 1550, 400, 1150, 400, 350, 0xC06FA306, 32, PROTOCOL_IS_LSB_FIRST, RepeatPeriodMillis, numberOfRepeats);
        server.send(200,"text/plain","light is bright");
        break;
      case 4:
        IrSender.sendPulseDistanceWidth(38, 3100, 1550, 400, 1150, 400, 400, 0x406FA306, 32, PROTOCOL_IS_LSB_FIRST, RepeatPeriodMillis, numberOfRepeats);
        server.send(200,"text/plain","light is dark");
        break;
      default:
        server.send(400,"text/plain","nothing this mode");
    }
  }else{
    server.send(400,"text/plain","Please select the mode");
  }
}
//--------------------------------------- 
void setup(void) {
  pinMode(C_LED_PIN, OUTPUT);
  IrSender.begin(SEND_LED_PIN,true,C_LED_PIN);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("picow0")) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.on("/Temperature", Temperature);
  server.on("/lightctl", Lightctl);
  
  server.begin();
  Serial.println("HTTP server started");
  
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}