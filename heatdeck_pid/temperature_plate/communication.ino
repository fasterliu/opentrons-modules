
#define MAX_BUF (64)
#define VERSION "opentrons temperature plate; version 0.1"

int sofar; 
char buffer[MAX_BUF];

void sendAck() {
  Serial.print("\r\nok\r\nok\r\n");
}

void ready() {
  sofar = 0; 
}

void help() {
  Serial.println(VERSION);
  Serial.println(F("Commands:"));
  Serial.println(F("M104 [S(temperature)]; - set temperature and return"));
  Serial.println(F("M105; - get temperature"));
  Serial.println(F("M155 [S(1 or 0)]; - print temperatures ever second"));
  Serial.println(F("M100; - this help message"));
}

//TODO jmg 11/29/17: Make handle negative values!
int parsenumber(char key, double defVal) {
  String cmd = String(buffer);
  int bufIndex;
  char buf[MAX_BUF];
  int i = cmd.lastIndexOf(key);
  
  while(isDigit(cmd.charAt(++i))){
    buf[bufIndex++] = cmd.charAt(i);
  }
  buf[bufIndex] = '\0';
  if(sizeof(buf)) { 
    return String(buf).toInt();
  }
  else {
    return defVal;
  }
}

void processCommand() {
  // look for commands that start with 'M'
  double cmd=parsenumber('M',-1);

  switch(int(cmd)) {
  case 104: //set temp
    setPoint = parsenumber('S',-1);
    sendAck();
    break;
  case 105: //get temp
    Serial.print("temperature reading: ");
    Serial.println(temperature);
    sendAck();
    break;
  case 155:
    if (parsenumber('S', !tempSending)){
      tempSending = true;  
    }
    else {
      tempSending = false;  
    }
    sendAck();
    break;
  case 100: 
    help(); 
    sendAck();
    break;
  default: 
    break;
  }

  if (String("version\n").equals(buffer)){
    Serial.println(VERSION);
    sendAck();
  }

  // if the string has no valid commands it will get here and be silently ignored
}

void checkCommand() {
  // listen for commands
  if( Serial.available() ) { // if something is available
    char c = Serial.read(); // get it
//    Serial.print(c); // optional: repeat back what I got for debugging

    // store the byte as long as there's room in the buffer.
    // if the buffer is full some data might get lost
    if(sofar < MAX_BUF) buffer[sofar++]=c;
    // if we got a return character (\n) the message is done.
    if(c=='\n') { 
      // strings must end with a \0.
      buffer[sofar]=0;
      processCommand(); // do something with the command
      ready();
    }
  }
}

