/*
   *  ESP8266 JSON Encode Server
   *  -Manoj R. Thkuar
    https://circuits4you.com
   */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

const char* wifiName = "SMC24_EXT";
const char* wifiPass = "asdfghjkl";

const int RELE = 0; //5
const int BUTTON = 2;
const char* URL_PARAM_TIMEOUT = "timeout";
const char* URL_PARAM_VALUE_INFINITE = "infinite";
float releActiveTime = 0;
bool timerCountDownActive = true;

ESP8266WebServer server(80);  //Define server object

//Handles http request
void handleRoot() {

  if (server.arg(URL_PARAM_TIMEOUT) == ""){
    switchRele();
  }else if(server.arg(URL_PARAM_TIMEOUT) == URL_PARAM_VALUE_INFINITE){
    switchReleOn();
  }else{
    if(server.arg(URL_PARAM_TIMEOUT) == "0"){
        setTimerCountDownActive(true);
        setReleActiveTime(server.arg(URL_PARAM_TIMEOUT).toInt());
       
      }else{
        setTimerCountDownActive(true);
        setReleActiveTime(server.arg(URL_PARAM_TIMEOUT).toInt());
        
      }
  }

  String webPage = createJSONObjectFromParams();
  server.send(200, "text/html", webPage);
}

//Handles http request
void handleInfo() {
  String webPage = createJSONObjectFromParams();
  server.send(200, "text/html", webPage);
}

void handleState() {
  String webPage = createJSONObjectFromParams();
  server.send(200, "text/html", webPage);
}

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(RELE, OUTPUT);
  pinMode(BUTTON, INPUT);
  Serial.begin(115200);
  delay(10);
  Serial.println();
  deactivateRele();

  Serial.print("Connecting");

  WiFi.begin(wifiName, wifiPass);   //Connect to WiFi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP

  server.on("/chickenlight", handleRoot);
  server.on("/chickenlightinfo", handleInfo);//Associate handler function to web requests

  server.begin(); //Start web server
  Serial.println("HTTP server started");
}

void loop() {
  //Handle Clinet requests
  server.handleClient();

  if (digitalRead(BUTTON) == LOW) {
    digitalWrite(RELE, abs(digitalRead(RELE) - 1));
    delay(1500);
    setReleActiveTime(5);
    setTimerCountDownActive(true);
    activateRele();
  }

  if(releActiveTime == 0){
    deactivateRele();
  }else{
    activateRele();
  }



  if (timerCountDownActive){
    if (releActiveTime > 0){
      releActiveTime = releActiveTime - 0.5;
    }
      
    delay(500);  
  }

  

}

String createJSONObjectFromParams(){
  String webPage;
  
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  for (int i = 0; i < server.args(); i++) {
    root[server.argName(i)] = server.arg(i);
  }

  root["releActiveTime"] = releActiveTime;
  root["timerCountDownActive"] = timerCountDownActive;
  root["releActive"] = digitalRead(RELE);

  root.printTo(webPage);  
  return webPage;
}

void setReleActiveTime(int activeTime){
  releActiveTime = activeTime;
}

void setTimerCountDownActive(bool isActive){
  timerCountDownActive = isActive;
}

void switchRele(){
  int val = digitalRead(RELE);
  setTimerCountDownActive(false);
    
  if(val == LOW){
    switchReleOff();
  }else{
     switchReleOn();
  }
  
}

void switchReleOff(){
  setTimerCountDownActive(false);
  deactivateRele();
  setReleActiveTime(0);
}

void switchReleOn(){
  setTimerCountDownActive(false);
  activateRele();
  setReleActiveTime(1);
}

void activateRele(){
  digitalWrite(RELE, LOW);
}

void deactivateRele(){
  digitalWrite(RELE, HIGH);
}
