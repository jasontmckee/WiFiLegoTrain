/*
  Libraries:
  Adafruit SSD1306 - https://github.com/adafruit/Adafruit_SSD1306
  WiFiWebServer_RTL8720 - https://github.com/khoih-prog/WiFiWebServer_RTL8720

  Wiring:
  Use     Name    Number
          PA15    9
          PA27    2
  I2C SDA PA26    8
  RFID RX PA8     1
  M Speed PA13    11
  M Dir   PA14    10
          PA30    3
  Buzzer  PA12    12
  Battery PB3     6
  I2C SCL PA25    7
  RFID TX PA7     0
  USB RX  PB2     5
  USB TX  PB1     4

*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// serial port sanity
// rfid is wired to LOG RX/TX for programming and 7941W rfid board
#define rfid Serial1
// console is wired to the onboard USB Serial RX/TX port
#define console Serial

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RFID
String lastUid = "";

// buzzer user timer callback to not block
#include <GTimer.h>
#define BUZZER PA12

// servo control
#define M_SPEED PA13
#define M_DIR LED_BUILTIN

bool motorForward = true;
int motorSpeed = 0;
int motorRaw = 0;

// use the ADC to monitor battery voltage levels
#define BATTERY PB3

// webserver
#define DEBUG_WIFI_WEBSERVER_PORT console
// Debug Level from 0 to 4
#define _WIFI_LOGLEVEL_ 0
#include <WiFiWebServer_RTL8720.h>

// config params
#include <FlashMemory.h>
#define SAVE_TAG_COUNT 10
typedef struct {
  int version = 7;
  char ssid[33]; // 32+null terminator
  char pass[64]; // 63+null terminator
  char name[32]; // display name of train
  int batteryEmpty = 0;
  int batteryFull = 1023;
  int motorMin = 0;
  int motorMax = 255;
  bool activeBrake = true;
  int brakeTimeMS = 500;
  bool displayRaw = true;
  bool dynamicPower = false;
  byte tag[SAVE_TAG_COUNT][9];
  char tagName[SAVE_TAG_COUNT][32];
  bool beep = true;
  int beepMS = 250;
} config_t;
config_t config;

WiFiWebServer server(80);

void setup() {
  console.begin(115200);

  pinMode(BUZZER,OUTPUT);

  // pull operating params up from flash
  int currentConfigVersion = config.version;
  loadConfig();
  if(currentConfigVersion > config.version) {
    // more parameters have been added, save config to update version
    console.print("Config version (");
    console.print(currentConfigVersion);
    console.print(") is newer than stored version (");
    console.print(config.version);
    console.println("), upgrading.");
    config_t defaults;
    if(config.version<1) {
      console.println("Invalid stored data, resetting to defaults");
      strcpy(config.name,"Train");
      config.batteryEmpty = defaults.batteryEmpty;
      config.batteryFull = defaults.batteryFull;
      config.motorMin = defaults.motorMin;
      config.motorMax = defaults.motorMax;
      config.activeBrake = defaults.activeBrake;
      config.brakeTimeMS = defaults.brakeTimeMS;
    }
    
    if(config.version < 5) {
      console.println("Upgrading to config v5");
      // initialize tag names to ""
      for(int i=0;i<SAVE_TAG_COUNT;i++) {
        String name = "tag";
        name += (i+1);
        strcpy(config.tagName[i],name.c_str());
        // init tag length to 0;
        config.tag[i][0] = 0;
        console.println(String("Initializing tagName[") + i + "] to " + config.tagName[i]);
      }
    }
    if(config.version < 6) {
      console.println("Upgrading to config v6");
      config.beep = true;
      config.beepMS = 250;
    }    
    if(config.version < 7) {
      console.println("Upgrading to config v7");
      config_t v7;
      console.println("TODO: test v6 to v7 upgrade");
      // copy current config object into temp config shifting everything by the size of the inserted fields
      memcpy((&v7)+sizeof(config.ssid)+sizeof(config.pass),&config,sizeof(config)-sizeof(config.ssid)-sizeof(config.pass));
      memcpy(&config,&v7,sizeof(config));
      // initialize default values
      strcpy(config.ssid,"default ssid");
      strcpy(config.pass,"default password");
    }    
    // update config version to current
    config.version = currentConfigVersion;

    saveConfig();
    console.println("Done!");
  }
  // reset version for testing
  //config.version--;
  //saveConfig();

  // OLED display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    console.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();

  // motor setup
  pinMode(M_SPEED,OUTPUT);
  pinMode(M_DIR,OUTPUT);

  // wifi setup
  int status = WL_IDLE_STATUS;
  console.print("Attempting to connect to SSID: "); 
  console.print(config.ssid);
  display.setTextSize(1);
  display.println("Connecting to");
  display.println(config.ssid);
  display.display();

  int timeoutSec = 0;
  while (status != WL_CONNECTED) 
  {
    // if we haven't connected after the timeout, drop to console wifi config
    if(timeoutSec > 10) {
      display.println("Failed");
      display.println("Configure ssid/pass");
      display.println("on serial console");
      display.display();
      wifiConfig();
      // TODO: restart!
      console.println("reset device");
      NVIC_SystemReset();
      console.println("if you see this, something went wrong");
      while(true);
    }
    status = WiFi.begin(config.ssid, config.pass);
    delay(1000);
    console.print(".");
    timeoutSec++;
  }
  console.println("");
  console.println("Connected");
  display.println(WiFi.localIP());
  display.display();
  
  // web setup
  server.on(F("/"), handleRoot);
  server.on(F("/api/control"), handleControl);
  server.on(F("/api/config"), handleConfig);
  server.onNotFound(handleFiles);
  //server.onNotFound(handleNotFound);

  server.begin();
  console.print(F("HTTP server started @ "));
  console.println(WiFi.localIP());
  display.println("HTTP started");
  display.display();

  // RFID setup
  rfid.begin(115200);

  console.println("Ready!");
  updateDisplay();
}

void loop() {
  server.handleClient();
}

void drawSignal(int x, int y, int px) {
  // wifi signal strength
  int fill = map(WiFi.RSSI(),-80,-44,0,px);
  display.drawTriangle(x,y+px-1,
                       x+px,y+px-1,
                       x+px,y,SSD1306_WHITE);
  display.fillTriangle(x,y+px-1,
                       x+fill,y+px-1,
                       x+fill,y+px-fill,SSD1306_WHITE);
}

void drawBattery(int x, int y, int w, int h) {
  int tip = h/4;
  display.fillRect(x,y+(h/4),tip,y+h/2, SSD1306_WHITE);
  
  display.drawRect(tip+x,y,w-tip,h, SSD1306_WHITE);
  int battW = map(getBatteryLevel(),0,100,0,w-4-tip);
  display.fillRect(x+w-2-battW,y+2,battW,h-4,SSD1306_WHITE);
}

void drawMotor(int x, int y, int w, int h) {
  int speed = motorSpeed;
  bool forward = motorForward;
  //int speed = 1;
  //bool forward = false;
  
  // draw vertical line in center
  display.drawLine(x+w/2,y,x+w/2,y+h,SSD1306_WHITE);
  // calulate arrow height, width
  int arrowH = h;
  int arrowW = h/2;
  // draw frame
  // L arrow frame
  display.drawLine(x,y+h/2,x+arrowW,y,SSD1306_WHITE);
  display.drawLine(x,y+h/2,x+arrowW,y+h,SSD1306_WHITE);
  // R arrow frame
  display.drawLine(x+w-arrowW-1,y,x+w-1,y+h/2,SSD1306_WHITE);
  display.drawLine(x+w-arrowW-1,y+h,x+w-1,y+h/2,SSD1306_WHITE);
  // top/bottom frame lines
  display.drawLine(x+arrowW,y,x+w-arrowW-1,y,SSD1306_WHITE);
  display.drawLine(x+arrowW,y+h-1,x+w-arrowW-1,y+h-1,SSD1306_WHITE);
  
  // calculate bar height & length (minus arrow)
  int barH = h/2;
  int barW = map(speed,1,100,1,w/2-arrowW);
  if(speed > 0) {
    // draw bar & arrows from center, right for forward, left for backwards
    if(forward) {
      display.fillRect(x+w/2,y+h/2-barH/2,barW,barH,SSD1306_WHITE);
      display.fillTriangle(x+w/2+barW,y,
                           x+w/2+barW,y+h-1,
                           x+w/2+barW+arrowW,y+h/2,SSD1306_WHITE);
    } else {
      display.fillRect(x+w/2-barW,y+h/2-barH/2,barW,barH,SSD1306_WHITE);
      display.fillTriangle(x+w/2-arrowW-barW,y+h/2,
                           x+w/2-barW,y,
                           x+w/2-barW,y+h-1,SSD1306_WHITE);
  
    }
  }
}

void updateDisplay() {
  display.clearDisplay();

  drawBattery(0,0,62,16);
  display.setCursor(64,0);
  display.setTextSize(2);
  display.print(getBatteryLevel());
  display.print("%");

  drawSignal(127-16,0,16);

  display.setTextSize(1);
  display.setCursor(0,16);
  display.print("IP: ");
  display.println(WiFi.localIP());

  display.setTextSize(1);
  display.setCursor(0,24);
  display.print("Tag: ");
  display.println(lastUid);

  display.setTextSize(1);
  display.setCursor(0,32);
  display.print("Motor: ");
  if(config.displayRaw) {
    display.print(motorForward ? "Forward" : "Reverse");
    display.print(" ");
  }
  display.print(motorSpeed);
  display.print("%");

  if(config.displayRaw) {
    display.setCursor(0,display.height()-24);
    display.setTextSize(1);
    display.print("Motor raw: ");
    display.println(motorRaw);
    display.print("Batt raw: ");
    display.println(analogRead(BATTERY));
    display.setTextSize(1);
    display.print("WiFi RSSI: ");
    display.print(WiFi.RSSI());
    display.println(" dbm");  
  } else {
    drawMotor(0,display.height()-24,display.width(),24);
  }
  // show it all
  display.display();  
}

int getBatteryLevel() {
  int b = analogRead(BATTERY);
  return map(b,config.batteryEmpty,config.batteryFull,0,100);
}

void motor(int speed, bool forward) {
  motorForward = forward;
  motorSpeed = speed;
  if(forward) {
    digitalWrite(M_DIR,0);
  } else {
    digitalWrite(M_DIR,1);
  }

  // because there's a gap between 0/off and the minimum value to move the train, treat 0 as off always before mapping below
  if(speed==0) {
    motorRaw = 0;
  } else {
    if(config.dynamicPower) {
      /*
        as battery drops, motorRaw needs to be adjusted up as battery volts drop to maintain same speed
        for example, without boosting requested power:
        battery @ 100% 913 (~8.7V)
        motorMin = 91 (91/255*8.7 = 3.1V)
        motorMax = 171 (171/255*8.7 = 5.8V)
  
        battery @ 71% 729 (~7.4V)
        motorMin = 91 (91/255*7.4 = 2.6V
        motorMax = 171 (171/255*7.4 = 4.9V)
  
        battery @ 0% 227 (~3.3V)
        motorMin = 91 (91/255*3.3 = 1.1V
        motorMax = 171 (171/255*3.3 = 2.2V)  

        as battery voltage drops closer to minimum voltage reqiured to move the train (~3.1V), 
        increase the lowest raw value being sent until min value is 255. Similarly, adjust the 
        max value range upwards to try to maintain a consistent voltage. Using the example of
        a raw battery value of 913=8.7V above, we can keep supplying a full 5.8V to the train
        until the battery drops below 67%
      */
      int motorRawMin = map(analogRead(BATTERY),config.batteryFull,config.batteryEmpty,config.motorMin,255);
      int motorRawMax = map(analogRead(BATTERY),config.batteryFull,config.batteryEmpty,config.motorMax,255);
      motorRaw = map(speed, 1, 100, motorRawMin, motorRawMax);
    } else { 
      motorRaw = map(speed, 1, 100, config.motorMin, config.motorMax);
    }
  }
  analogWrite(M_SPEED,motorRaw);
  updateDisplay();
}

String readNextTag() {
  byte buf[500];
  int buflen;

  console.println("Reading next tag");

  String uid = "";

  while(uid=="") {
    rfid.write((byte)0xAB);
    rfid.write((byte)0xBA);
    rfid.write((byte)0x00);  // address
    rfid.write((byte)0x10);  // command
    rfid.write((byte)0x00);  // data length
    rfid.write((byte)0x10);  // XOR checksum
  
    int i = 0;

    delay(100);
    //collect any response into buf[]
    while((rfid.available() > 0) && (i < 500)) {
      buf[i++] = rfid.read();
      delay(50);
    }
    buflen = i;
    /*
      example responses:
      uid8 - CD DC 0 81 8 88 4 60 A6 52 22 52 81 60 
      uid4 - CD DC 0 81 4 91 EF 69 1B 89 
      uid4 - CD DC 0 81 4 43 96 D7 9A 1D 
      fail - CD DC 0 80 0 80 
    */
    if (buflen > 0) {
      if (buf[3] == 0x80) {
        // no card found
        console.print(".");
      } else if (buf[3] == 0x81) {
        // tag found
        beep();
        
        // TODO: for fun, calculate checksum
        
        // buf[4] is UID length
        // buf[5] is start of UID
        // fun pointer math
        uid = arrayToHexString(buf+5,buf[4]);
        console.print("UID: [");
        console.print(uid);
        console.println("]");
        
      } else {
        console.print("Undefined Response  ");
      }
  
    }
  }
  lastUid = uid;
  updateDisplay();
  return uid;
}

// called when fail to connect to current wifi ssid/pass stored in config
void wifiConfig() {
  String ssid = String(config.ssid);
  String pass = String(config.pass);
  // default timeout of 1000ms is too short to type
  console.setTimeout(60000);

  console.println("Couldn't connect:");  
  console.print("ssid =\""); 
  console.print(ssid);
  console.print("\" len: ");
  console.println(ssid.length());
  console.print("pass =\""); 
  console.print(pass);
  console.print("\" len: ");
  console.println(pass.length());
  console.println("");

  console.println("Configure wifi");  
  console.print("ssid: ");
  ssid = console.readStringUntil('\r');
  ssid.replace("\n","");
  ssid.replace("\r","");
  console.println(ssid);

  console.print("pass: ");
  pass = console.readStringUntil('\r');
  pass.replace("\n","");
  pass.replace("\r","");
  console.println(pass);

  // copy back to config and save
  strcpy(config.ssid,ssid.c_str());
  strcpy(config.pass,pass.c_str());
  saveConfig();
}
