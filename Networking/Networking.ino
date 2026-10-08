
#include <WiFiS3.h>  //import the library with all the server functions and stuff
char ssid[] = "Test";
char pass[] = "wordpass";
// set two character arrays with the Wifi and password you want to host the server on

WiFiServer Buggy(5200);  // establish an instance of class WiFiServer called Buggy

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, pass);  //give the arduino's wifi thing the network name and password

  Buggy.begin();  //Tells the server to begin listening for incoming connections.
}


void loop() {
  WiFiClient laptop = Buggy.available();  //initialize a WifiClient called laptop

  sendUpdate(laptop);  //function that regularly sends data to laptop

  checkInput(laptop);  //function that checks for updates from the laptop and updates global vars accordingly

  if (laptop.connected()) {  // let client (laptop) know that the connection has been established
    laptop.write("T");
  }//honestly only left this here as a remnant of the buggy code as a demo to us of available member functions for wifiClient, prolly not useful
}
