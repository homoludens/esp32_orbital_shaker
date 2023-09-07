// Simple Stepper Motor Control
//
// by Achim pieters, www.studiopieters.nl
//
//

#include <Stepper.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>


// Defines pins numbers

const int POT = 2;
const int LED = 3;
const int dirPin = 4;
const int stepPin = 5;

int  dirStatus = 3;


String buttonTitle1[] ={"CCW", "CW"};
String buttonTitle2[] ={"CCW", "CW"};
String argId[] ={"ccw", "cw", "speed"};

// const char *ssid = "b9a8de";
// const char *password = "281263985";

const char *ssid = "hlu";
const char *password = "0123456789";

 
WebServer server(80);

int customDelay; // Defines variables

int customDelayMapped = 2000;

long int time_since_start = millis();



void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);

  digitalWrite(dirPin,HIGH); //Enables the motor to move in a particular direction

  Serial.begin(115200);

  //set the resolution to 12 bits (0-4096)
  analogReadResolution(12);

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
  Serial.print("IP address: http://");
  Serial.println(WiFi.localIP());

  //multicast DNS  
  if (MDNS.begin("orbshake")) {
    Serial.println("MDNS responder started");
    Serial.println("access via http://orbshake");
  }

  server.on("/", handleRoot);
  server.on("/motor", HTTP_GET, motorControl);
  server.on("/speed", HTTP_GET, speedControl);            
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}
void loop() {

  server.handleClient();

  if(dirStatus ==1){ 
    digitalWrite(dirPin , HIGH);
  } else if(dirStatus == 2) { 
    digitalWrite(dirPin, LOW);
  } else{
    digitalWrite(dirPin, LOW);
  }
  // Serial.print("loop: customDelayMapped: ");  
  // Serial.print(customDelayMapped);   
  // Serial.println();

  // customDelayMapped = speedUp(); // Gets custom delay values from the custom speedUp function
  // Makes pules with custom delay, depending on the Potentiometer, from which the speed of the motor depends
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(customDelayMapped);
}

// Function for reading the Potentiometer
int speedUp() {
  int customDelay = analogRead(POT); // Reads the potentiometer
  int newCustom = map(customDelay, 100, 4096, 1, 4000); // Convrests the read values of the potentiometer from 0 to 1023 into desireded delay values (300 to 4000)
  return newCustom;
}


