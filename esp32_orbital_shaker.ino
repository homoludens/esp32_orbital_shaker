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

void handleRoot() {
   //Robojax.com ESP32 Relay Motor Control
 String HTML ="<!DOCTYPE html>\
  <html>\
  <head>\
  \t\n<title>LoginEKO Orbital Shaker</title>\
  \t\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  \n<style>\
 \nhtml,body{\t\nwidth:100%\;\nheight:100%\;\nmargin:0}\n*{box-sizing:border-box}\n.colorAll{\n\tbackground-color:#90ee90}\n.colorBtn{\n\tbackground-color:#add8e6}\n.angleButtdon,a{\n\tfont-size:30px\;\nborder:1px solid #ccc\;\ndisplay:table-caption\;\npadding:7px 10px\;\ntext-decoration:none\;\ncursor:pointer\;\npadding:5px 6px 7px 10px}a{\n\tdisplay:block}\n.btn{\n\tmargin:5px\;\nborder:none\;\ndisplay:inline-block\;\nvertical-align:middle\;\ntext-align:center\;\nwhite-space:nowrap}\n";
   
  HTML +="</style>\n\n</head>\n\n<body>\n<h1> LoginEKO Orbital Shaker </h1>\n";

  HTML +="\n\t<h3> customDelayMapped: ";
  HTML += customDelayMapped;
  HTML += "</h3>\n";

  time_since_start = millis();
  int minutes = (time_since_start / 1000) / 60;
  int seconds = (time_since_start / 1000) % 60;
  HTML +="\n\t<h3> Time since start: ";
  HTML += int(minutes) ;
  HTML += ":" ;
  HTML += seconds ;
  HTML += "</h3>\n";
  


  HTML +="\t<div class=\"btn\">\n\t\t<a href=\"/speed?speed=add100\">+100";  
  HTML +="</a>\t\n\t</div>\n\n";

  HTML +="\t<div class=\"btn\">\n\t\t<a href=\"/speed?speed=sub100\">-100";  
  HTML +="</a>\t\n\t</div>\n\n";  

  HTML +="\t<div class=\"btn\">\n\t\t<a href=\"/speed?speed=add10\">+10";  
  HTML +="</a>\t\n\t</div>\n\n";

  HTML +="\t<div class=\"btn\">\n\t\t<a href=\"/speed?speed=sub10\">-10";  
  HTML +="</a>\t\n\t</div>\n\n";  


  HTML +="\t<div class=\"btn\">\n\t\t<a href=\"/speed?speed=add1\">+1";  
  HTML +="</a>\t\n\t</div>\n\n";

  HTML +="\t<div class=\"btn\">\n\t\t<a href=\"/speed?speed=sub1\">-1";  
  HTML +="</a>\t\n\t</div>\n\n";  

   if(dirStatus == 2){
    HTML +="\n\t<h2><span style=\"background-color: #FFFF00\">Motor Running in CW</span></h2>\n";    
   }else if(dirStatus ==1){
    HTML +="\n\t<h2><span style=\"background-color: #FFFF00\">Motor Running in CCW</span></h2>\n";      
   }else{
    HTML +="\n\t<h2><span style=\"background-color: #FFFF00\">Motor OFF</span></h2>\n";    
   }
      if(dirStatus ==1){
        HTML +="\t<div class=\"btn\">\n\t\t<a class=\"angleButton\" style=\"background-color:#f56464\"  href=\"/motor?";
        HTML += argId[0];
        HTML += "=off\">";
        HTML +=buttonTitle1[0]; //motor ON title
      }else{
        HTML +="\t<div class=\"btn\">\n\t\t<a class=\"angleButton \" style=\"background-color:#90ee90\"  href=\"/motor?";  
         HTML += argId[0];
        HTML += "=on\">";       
        HTML +=buttonTitle2[0];//motor OFF title   
      }   
     HTML +="</a>\t\n\t</div>\n\n";  
           
      if(dirStatus ==2){
        HTML +="\t<div class=\"btn\">\n\t\t<a class=\"angleButton\" style=\"background-color:#f56464\"  href=\"/motor?";
        HTML += argId[1];
        HTML += "=off\">";
        HTML +=buttonTitle1[1]; //motor ON title
      }else{
        HTML +="\t<div class=\"btn\">\n\t\t<a class=\"angleButton \" style=\"background-color:#90ee90\"  href=\"/motor?";  
         HTML += argId[1];
        HTML += "=on\">";       
        HTML +=buttonTitle2[1];//motor OFF title   
      }   
     HTML +="</a>\t\n\t</div>\n\n";

  HTML +="\t\n</body>\n</html>\n";
  server.send(200, "text/html", HTML);  
}//handleRoot()

void handleNotFound() {
  //Robojax.com 28BYJ-48 Steper Motor Control
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  //Robojax.com 28BYJ-48 Steper Motor Control
}//end of handleNotFound()



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

/*
 * motorControl()
 * updates the value of "dirStatus" varible to 1, 2 or 3
 * returns nothing
 * written by Ahmad Shamshiri
 * on Wednesday April 19, 2020 at 18:03 in Ajax, Ontario, Canada
 * www.robojax.com
 */
void motorControl() {

    if(server.arg(argId[0]) == "on") {
      dirStatus = 1;// CCW         
    } else if(server.arg(argId[0]) == "off") {
      dirStatus = 3;  // motor OFF 
    } else if(server.arg(argId[1]) == "on") {
      dirStatus = 2;  // CW  
    } else if(server.arg(argId[1]) == "off") {
      dirStatus = 3;  // motor OFF
    }  
  handleRoot();
}//motorControl end


void speedControl() {
    if(server.arg(argId[2]) == "add1") {
      customDelayMapped = customDelayMapped + 1;      
    } else if(server.arg(argId[2]) == "sub1") {
      customDelayMapped = customDelayMapped - 1;
    } else if(server.arg(argId[2]) == "add10") {
      customDelayMapped = customDelayMapped + 10;
    } else if(server.arg(argId[2]) == "sub10") {
      customDelayMapped = customDelayMapped - 10;
    } else if(server.arg(argId[2]) == "add100") {
      customDelayMapped = customDelayMapped + 100;
    } else if(server.arg(argId[2]) == "sub100") {
      customDelayMapped = customDelayMapped - 100;
    }
    Serial.print("customDelayMapped: ");
    Serial.print(customDelayMapped);
    Serial.println();
  handleRoot();
}


// Function for reading the Potentiometer
int speedUp() {
  int customDelay = analogRead(POT); // Reads the potentiometer
  int newCustom = map(customDelay, 100, 4096, 1, 4000); // Convrests the read values of the potentiometer from 0 to 1023 into desireded delay values (300 to 4000)
  return newCustom;
}



void time(long val){  
//  int days = elapsedDays(val);
//  int hours = numberOfHours(val);
 int minutes = (val / 1000) % 60;
 int seconds = val % 1000;

  // digital clock display of current time
  Serial.printf("Time = %d:%d\n",minutes, seconds);
  Serial.println(); 
  Serial.printf("Time = %d:%d\n",minutes, seconds);
}

