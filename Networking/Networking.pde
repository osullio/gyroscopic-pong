
int getInteger() {
  //if we try to read in too early it'll save as -1, so keep checking until we get the actual signal
  int NUM = -1;
  while (NUM == -1) { //wait till count != -1
    NUM = buggy.read();
  }
  return NUM;
}

void dataEval(char data) {
  if (data == 'u') {   // u means UPDATE, expect to receive lots of information
    int count = getInteger();
    dist = floor( count * 3.1416 * 6.5);
    range = getInteger();
    vBuggy = getInteger();
    vTarget = getInteger();
  } 
  else if (data == 'f') { // f means we entered follow mode
    follow = true;
  } 
  else if (data == 'n') { // n means we left follow mode
    follow = false;
  }
}

void send() {
  if (lastSpeed != speed) { //if speed changes
  
    buggy.write(speed);//tell buggy to change speed
    
    lastSpeed = speed; //update the last speed after speed changes

    // if we set the buggy speed to 0 expect to receive the wheel count
    if (speed == 0) {
      int count = getInteger();
      dist = floor( count * 3.1416 * 6.5);
    }
  }
}
