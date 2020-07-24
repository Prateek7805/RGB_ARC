# RGB_ARC
The idea of this project is to create an IOT RGB ring light using the SMD 5050 RGB leds from the 5M or 4.5M RGB led strip.
The RGB leds have 6 Pins hence we can decide to connect them in common anode or common cathode. I choose common cathode.
The circuit is driven with ESP8266(ESP-01) Modules with carefully soldering wires to GPIO 16,12,13,14, 4(very carefully and then add EPOXY) or just make use of ESP12-E or ESP12-F modules with all the pins broken out. The leds are connected using GPIO pins 2 1 0 which are also the tx, rx pins of the uart hence make sure you hold GPIO 0 to ground on restart.
I have made Use of an IMU, however its optional, Also the website I created is the one I made before I learned Web development so you can choose to replace it with your own if you know HTML and CSS. 
The WiFi Credentials( SSID and PASS) can be easily passed by first connecting to The wifi n/w called "ARC" and then entering 192.168.4.1 in the address bar of a browser and hit enter. Enter the credentials under No Scan option and Reboot the ESP8266.
The code is straight forward and makes use of ESP8266WebServer library for handling the HTTP requests and WiFiManager.h for getting the SSID and PASSWORD.

