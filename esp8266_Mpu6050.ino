#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include<ESP8266WebServer.h>
#include <WiFiManager.h>    

#include "Wire.h"       //For communicate
#include "I2Cdev.h"     //For communicate with MPU6050
#include "MPU6050.h"    //The main library of the MPU6050


MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
const uint8_t scl =14;
const uint8_t sda =12;

int rgb[3]={2,1,0};

int data=16;
int clk=13;
int latch=4;
int arr[7][3]={{255,0,0 },{0,255,0},{0,0,255},{255,255,0},{0,255,255},{255,255,255},{0,0,0}};

float refresh=5;
int glowVar=0;
int chaseVar=0;
int del=200;
String Ax,Ay,Az;
int gc=0;
ESP8266WebServer server;
bool otaFlag=0;
unsigned long timeElapsed=0;

//can use the rawliteral format alternately
String htmlPage="HTTP/1.1 200 OK Content-type:text/html Connection: close <head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>html { font-family: Calibri, Arial, sans-serif; display: inline-block; margin: 0px auto; text-align: center;} body { justify-content: center; align-items: center; background: #031321;} input[type=text]{ width: 10%; height: 12px; padding: 12px 20px; margin: 8px 0; box-sizing: border-box; border: 2px solid #FFD4D4; -webkit-transition: 0.5s; transition: 0.5s; outline: none;} input[type=text]:focus {border: 2px solid #FFAAAA} input[type=submit]{color: #2196f3; padding: 8px 15px;text-decoration: none; letter-spacing: 4px; font-size: 12px; margin: 2px; cursor: pointer; overflow: hidden; transition: 0.1s;} input[type=submit]:hover{color: #255784; background:#2196f3; box-shadow: 0 0 10px #2196f3, 0 0 40px #2196f3, 0 0 80px #2196f3;} a{color: #2196f3; padding: 15px 30px;text-decoration: none; letter-spacing: 4px; font-size: 24px; margin: 2px; cursor: pointer; overflow: hidden; transition: 0.1s;} a:hover { color: #255784; background: #2196f3; box-shadow: 0 0 10px #2196f3, 0 0 40px #2196f3, 0 0 80px #2196f3; } .redButton:hover { color: #255784; background: #FF5555; box-shadow: 0 0 10px #FF5555, 0 0 40px #FF5555, 0 0 80px #FF5555; } .greenButton:hover { color: #255784; background: #2AFF55; box-shadow: 0 0 10px #2AFF55, 0 0 40px #2AFF55, 0 0 80px #2AFF55; } .blueButton:hover { color: #255784; background: #00D4FF; box-shadow: 0 0 10px #00D4FF, 0 0 40px #00D4FF, 0 0 80px #00D4FF; } .orangeButton:hover { color: #255784; background: #FF7F2A; box-shadow: 0 0 10px #FF7F2A, 0 0 40px #FF7F2A, 0 0 80px #FF7F2A; } .skyButton:hover { color: #255784; background: #46F2E9; box-shadow: 0 0 10px #46F2E9, 0 0 40px #46F2E9, 0 0 80px #46F2E9; } .purpleButton:hover { color: #255784; background: #D42AFF; box-shadow: 0 0 10px #D42AFF, 0 0 40px #D42AFF, 0 0 80px #D42AFF; } .chase0:hover { color: #255784; background: #FFFFFF; box-shadow: 0 0 10px #FFFFFF, 0 0 40px #FFFFFF, 0 0 80px #FFFFFF; } .off:hover { color: #255784; background: #000000; box-shadow: 0 0 10px #000000, 0 0 40px #000000, 0 0 80px #000000; } .flash:hover { color: #255784; background: #FFFFFF; box-shadow: 0 0 10px #FFFFFF, 0 0 40px #FFFFFF, 0 0 80px #FFFFFF; } p {color= white; -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none;   } h1,h2,h3 {color: white;} </style></head> <body><h1>RGB ARC BIG ONE</h1>";
String ending=" <p><a class=\"redButton\" href=\"/color?val=0\">red</button></a></p> <p><a class=\"greenButton\" href=\"/color?val=1\">green</button></a></p> <p><a class=\"blueButton\" href=\"color?val=2\"><span></span>blue</button></a></p> <p><a class=\"orangeButton\" href=\"color?val=3\">orange</button></a></p> <p><a class=\"skyButton\" href=\"color?val=4\">sky</button></a></p> <p><a class=\"purpleButton\" href=\"color?val=5\">purple</button></a></p> <p><a class=\"chase0\" href=\"chase?val=0\">chaser</button></a></p> <p><a class=\"off\" href=\"/OFFit\"\">OFF</button></a></p> <p><a href=\"/reset\"\">restart</button></a></p> <p><a class=\"flash\" href=\"/flash\"\">Flash</button></a></p> <form action=\"/actionPage\"><h3> RGB:</h3> <input type=\"text\" name=\"red\" value=""> <input type=\"text\" name=\"green\" value=""> <input type=\"text\" name=\"blue\" value=""><br><input type=\"submit\" value=\"Submit\"> </form></body></html>";

void setup() {

  pinMode(rgb[0], FUNCTION_3);
  pinMode(rgb[1], OUTPUT);
  pinMode(rgb[2], OUTPUT);
  pinMode(data,OUTPUT);
  pinMode(clk,OUTPUT);
  pinMode(latch,OUTPUT);
  delay(1);
  shiftOut(data,clk,MSBFIRST,0);
  shiftOut(data,clk,MSBFIRST,~0);
  shiftOut(data,clk,MSBFIRST,~0);
  latchIt();
  glowVar=6;
  Wire.begin(sda,scl);
  mpu.initialize(); 
  
  
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.autoConnect("ARC");
  server.begin();
  delay(10);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  
  server.on("/",[](){
   mpuGetVals();
    String SendPage=htmlPage+"<h2>"+Ax+" "+Ay+" "+Az+" </h2>"+ending;
    server.send(200,"text/html",SendPage);
  });


  
  server.on("/color",[](){
    
 glowVar=server.arg(0).toInt();
 mpuGetVals();
    String SendPage=htmlPage+"<h2>"+Ax+" "+Ay+" "+Az+" </h2>"+ending;
 server.send(200,"text/html",SendPage);
 gc=0;
});

server.on("/chase",[](){
    
 chaseVar=server.arg(0).toInt();
 mpuGetVals();
 String SendPage=htmlPage+"<h2>"+Ax+" "+Ay+" "+Az+" </h2>"+ending;
 server.send(200,"text/html",SendPage);
 gc=1;
 
});

server.on("/reset",[](){
    delay(100);
    ESP.restart();
  });

server.on("/OFFit",[](){
  glowVar=6;
    shiftOut(data,clk,MSBFIRST,0);
  shiftOut(data,clk,MSBFIRST,~0);
  shiftOut(data,clk,MSBFIRST,~0);
  latchIt();
  mpuGetVals();
 String SendPage=htmlPage+"<h2>"+Ax+" "+Ay+" "+Az+" </h2>"+ending;
 server.send(200,"text/html",SendPage);
  gc=0;
  });


server.on("/flash",[](){
  glowVar=6;
    shiftOut(data,clk,MSBFIRST,~0);
  shiftOut(data,clk,MSBFIRST,~0);
  shiftOut(data,clk,MSBFIRST,~0);
  latchIt();
  mpuGetVals();
 String SendPage=htmlPage+"<h2>"+Ax+" "+Ay+" "+Az+" </h2>"+ending;
 server.send(200,"text/html",SendPage);
  gc=3;
  });

  server.on("/actionPage",[](){
    
 
  int r=server.arg("red").toInt();
  int g=server.arg("green").toInt();
  int b=server.arg("blue").toInt();
  
  r=(r>255)?255:((r<0)?0:r);
  g=(g>255)?255:((g<0)?0:g);
  b=(b>255)?255:((b<0)?0:b);

  analogWrite(rgb[0],r); 
  analogWrite(rgb[1],g); 
  analogWrite(rgb[2],b); 
        
  shiftOut(data,clk,MSBFIRST,0);
  shiftOut(data,clk,MSBFIRST,0);
  shiftOut(data,clk,MSBFIRST,0);
  latchIt();
  mpuGetVals();
 String SendPage=htmlPage+"<h2>"+Ax+" "+Ay+" "+Az+" </h2>"+ending;
 server.send(200,"text/html",SendPage);
  gc=3;
  });
}


void loop() {
 
  server.handleClient();
  if(!gc){
    glow();    
  }
  else if(gc==1){
    chase();
  }
  
}

void mpuGetVals(){
 mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
 int xAcc=map(ax,-25000,25000,0,255);
 int yAcc=map(ay,-25000,25000,0,255);
 int zAcc=map(az,-25000,25000,0,255);
 
 
Ax=String(xAcc);
Ay=String(yAcc);
Az=String(zAcc);

 
 if(xAcc<10)
    Ax="00"+Ax;
 else if(xAcc<100)
    Ax="0"+Ax;

    
 if(yAcc<10)
    Ay="00"+Ay;
 else if(yAcc<100)
    Ay="0"+Ay;
    
 if(zAcc<10)
    Az="00"+Az;
 else if(zAcc<100)
    Az="0"+Az;
  
}

void glow(){
  for(int k=0; k<3; k++)
    analogWrite(rgb[k],arr[glowVar][k]*3); 
  shiftOut(data,clk,MSBFIRST,0);
  shiftOut(data,clk,MSBFIRST,0);
  shiftOut(data,clk,MSBFIRST,0);
  latchIt();
  refresh=5;
}


void chase(){
switch(chaseVar){
case 0:{
  int opt=random(1,6);
      for(int k=0; k<3; k++)
        analogWrite(rgb[k],arr[opt][k]*3); 
      for(int i=0; i<8; i++){
        shiftOut(data,clk,MSBFIRST,0);
        shiftOut(data,clk,MSBFIRST,~0);
        shiftOut(data,clk,MSBFIRST,~(1<<i));
        latchIt();
        delay(del);
      }
     for(int i=0; i<8; i++){
        shiftOut(data,clk,MSBFIRST,0);
        shiftOut(data,clk,MSBFIRST,~(1<<i));
        shiftOut(data,clk,MSBFIRST,~0);
        latchIt();
        delay(del);
     }
     break;
}
}
}


void latchIt(){
digitalWrite(latch, HIGH);
delayMicroseconds(10);
digitalWrite(latch, LOW);
delayMicroseconds(10);
}
