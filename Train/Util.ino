// format len bytes of b[] into a String
String arrayToHexString(byte* b, int len) {
    String s = "";
    for(byte i = 0; i < len; i++) {
      s += (b[i] < 0x10 ? "0" : "");
      s += String(b[i],HEX);
    }
    s.trim();
  return s;
}

// parse string into byte array
// len should be max size of byte array or badness happens
void hexStringToByteArray(String s, byte b[], int len) {
  // remove [ :-] and leading 0x from string to get length
  s.replace(" ","");
  s.replace(":","");
  s.replace("-","");
  s.replace("0x","");
  
  char *c = (char *)s.c_str();
  for(int i=0; i<len*2; i++) {
    byte nibble = 0xFF;
    if(c[i]>='0' && c[i]<='9') nibble = c[i]-'0';
    if(c[i]>='A' && c[i]<='F') nibble = c[i]-'A'+10;
    if(c[i]>='a' && c[i]<='f') nibble = c[i]-'a'+10;
    if(i%2) {
      b[i/2] += nibble;
    } else {
      b[i/2] = nibble*16;
    }
  }
}

// non-blocking beep call
void beep() {
  if(config.beep) {
    digitalWrite(BUZZER,HIGH);
    // timer 0, delay config.beepMS, run once, user data is ignored by stopBeep
    GTimer.begin(0, config.beepMS * 1000, stopBeep, false, 0);  
  }
}

// callback for GTimer to turn off buzzer
void stopBeep(uint32_t data) {
  digitalWrite(BUZZER,LOW);    
}

void loadConfig() {
  console.println("Loading config from flash");
  FlashMemory.read();
  memcpy(&config,FlashMemory.buf,sizeof(config_t));
}

void saveConfig() {
  console.println("Storing config to flash");
  memcpy(FlashMemory.buf,&config,sizeof(config_t));
  FlashMemory.update();
}
