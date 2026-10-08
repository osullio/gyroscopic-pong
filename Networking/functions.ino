
/* 
  The way the networking works is that Proccesing's Loop is constantly checking to see if a char was sent from the buggy
 if a char was sent it checks what that char is
 if the char == u, it expects to receive averagCount, obstacle distance (range), buggyV
 if the char == f the GUI displays a Light to say we are in follow mode
 if the char == n the GUI light turns off
  Proccesing also expects to receive the averagCount whenever it asked the buggy to stop
*/

void sendUpdate(WiFiClient laptop) {
  //this shows how I handled networking in the buggy project. I imagine this kind of format is how we will handle it for this module also

  if (laptop.connected()) {  // if the Laptop is connected to the receiver,

    laptop.write("u");  // this warns the laptop that we are about to send an update

    // send the laptop, averagCount, obstacle distance (range), buggyV
    /*
      laptop.write(averageCount());
      laptop.write(obstacleDist());
      updateVelos();  //update buggyV and targetV before sending
      laptop.write(buggyV);
      laptop.write(targetV);
    */
  }
}

void checkInput(WiFiClient laptop) {

  if (laptop.available()) {  //if the laptop sent something
    /*
      speed = laptop.read();   //change our value for speed to be what the laptop sent
      Serial.println(speed);
    
      if (speed == 0) { // if the laptop asked the buggy to stop, it expects to receive average count so that distance travelled can be update on the gui
        laptop.write(averageCount());
      }
    
    */
  }
}
