/*
  use buildheader.sh to copy html folder assets into html.h
*/
#include "html.h"

void handleControl() {
  console.println("Serving /api/control");
  String dir = "";
  int speed = 0;
  String rfid;
  
  if(server.hasArg("dir")) {
    dir = server.arg("dir");  
  }
  if(server.hasArg("speed")) {
    speed = server.arg("speed").toInt();
  }
  if(server.hasArg("rfid")) {
    rfid = server.arg("rfid");  
  }

  // turn motor on
  motor(speed,dir!="R");

  if(server.hasArg("rfid")) {
    // if rfid mode, start looking for tags
    String uid = "";
    if(rfid=="next") {
      String last = lastUid;
      uid = lastUid;
      while(uid==last) {
        uid = readNextTag();
      }
    } else {
      while(rfid!=uid) {
        uid = readNextTag();
      }
    }
    rfid = uid;
    
    // stop when found
    if(config.activeBrake) {
      // reverse direction briefly to stop rolling
      motor(speed,dir=="R");
      delay(config.brakeTimeMS);
    }
    // now stop for real
    motor(0,true);
  }

  // build result
  String o = F("{\"dir\":\"");
  o += dir;
  o += F("\",\"speed\":");
  o += speed;
  if(server.hasArg("rfid")) {
    o += F(",\"rfid\":\"");
    o += rfid;
    o += F("\"");    
  }
  o += F(",\"battery\":");
  o += getBatteryLevel();
  o += F(" }");
  server.send(200, F("application/json"), o);
  console.println(200);
}

void handleConfig() {
  console.println("Serving /api/config");
  /*
  for(int i=0; i<server.headers(); i++) {
    console.print(server.headerName(i));
    console.print(" : ");
    console.println(server.header(i));
  }
  */
  
  // if POST, process args & save
  if(server.method() == HTTP_POST) {
    console.println("Updating config parameters");
    if(server.hasArg("name")) {
      strcpy(config.name,server.arg("name").c_str());  
    }
    if(server.hasArg("batteryEmpty")) {
      config.batteryEmpty = server.arg("batteryEmpty").toInt();  
    }
    if(server.hasArg("batteryFull")) {
      config.batteryFull = server.arg("batteryFull").toInt();  
    }
    if(server.hasArg("motorMin")) {
      config.motorMin = server.arg("motorMin").toInt();  
    }
    if(server.hasArg("motorMax")) {
      config.motorMax = server.arg("motorMax").toInt();  
    }
    if(server.hasArg("activeBrake")) {
      config.activeBrake = server.arg("activeBrake")=="1" || server.arg("activeBrake").equalsIgnoreCase("true");
    }
    if(server.hasArg("brakeTimeMS")) {
      config.brakeTimeMS = server.arg("brakeTimeMS").toInt();  
    }
    if(server.hasArg("displayRaw")) {
      config.displayRaw = server.arg("displayRaw")=="1" || server.arg("displayRaw").equalsIgnoreCase("true");
    }
    if(server.hasArg("dynamicPower")) {
      config.dynamicPower = server.arg("dynamicPower")=="1" || server.arg("dynamicPower").equalsIgnoreCase("true");
    }
    if(server.hasArg("beep")) {
      config.beep = server.arg("beep")=="1" || server.arg("beep").equalsIgnoreCase("true");
    }
    if(server.hasArg("beepMS")) {
      config.beepMS = server.arg("beepMS").toInt();  
    }

    int tagNameC = 0;
    int tagC = 0;
    for(int i=0; i<server.args(); i++) {

      // tagName
      if(server.argName(i)=="tagName") {
        console.print("tagname: ");
        console.println(server.arg(i));          
        strcpy(config.tagName[tagNameC],server.arg(i).c_str());
        tagNameC++;
      }
      // tag needs to be parsed before being copied into config
      if(server.argName(i)=="tag") {
        console.print("tag: ");
        console.println(server.arg(i));
        String s = server.arg(i);
        // remove [ :-] and leading 0x from string to get length
        s.replace(" ","");
        s.replace(":","");
        s.replace("-","");
        s.replace("0x","");
        int len = s.length()/2;
        // first byte is length, current max UID is 8 bytes
        // prevent buffer overrun by truncating to 8
        config.tag[tagC][0] = len<=8 ? len : 8;
        hexStringToByteArray(s,config.tag[tagC]+1,config.tag[tagC][0]);
        tagC++;
      }
      
    }
    
    // write to flash
    saveConfig();
  }
  
  // POST or GET return config values
  String o = F("{\"configVersion\":");
  o += config.version;
  o += ",\"buildTimestamp\":\"";
  o += __DATE__;
  o += " ";
  o += __TIME__;
  o += "\",\"name\":\"";
  o += config.name;
  o += "\",\"batteryEmpty\":";
  o += config.batteryEmpty;
  o += ",\"batteryFull\":";
  o += config.batteryFull;
  o += ",\"motorMin\":";
  o += config.motorMin;
  o += ",\"motorMax\":";
  o += config.motorMax;
  o += ",\"activeBrake\":";
  o += config.activeBrake ? "true" : "false";
  o += ",\"brakeTimeMS\":";
  o += config.brakeTimeMS;
  o += ",\"beep\":";
  o += config.beep ? "true" : "false";
  o += ",\"beepMS\":";
  o += config.beepMS;
  o += ",\"displayRaw\":";
  o += config.displayRaw ? "true" : "false";
  o += ",\"dynamicPower\":";
  o += config.dynamicPower ? "true" : "false";
  o += ",\"tags\":[";
  for(int i=0; i<SAVE_TAG_COUNT; i++) {
    o += "{\"tagName\":\"";
    o += config.tagName[i];
    o += "\",\"tag\":\"";
    o += arrayToHexString(config.tag[i]+1,config.tag[i][0]);
    o += "\"}";
    if(i+1<SAVE_TAG_COUNT) {
      o += ",";
    }
  }
  o += "]";
  o += F(" }");
  
  //console.println(o);
  
  server.send(200, F("application/json"), o);
  console.println(200);
}

void handleRoot() {
  console.println("Serving /");
  server.send(200, F("text/html"), String(mapFile("/index.html")));
  console.println(200);
}

void handleFiles() {
    String file = server.uri();
    String mimeType;
    String body;
    const char *doc = mapFile(file);
    
    // if we don't have a mapping, 404
    if(doc==NULL) {
      handleNotFound();
    }
    console.print("Serving ");
    console.println(file);
    
    console.print("length: ");
    console.println(sizeof(doc));

    // simple mime type mapping
    if(file.endsWith(".html")) mimeType = F("text/html");
    else if(file.endsWith(".css")) mimeType = F("text/css");
    else if(file.endsWith(".js")) mimeType = F("application/javascript");
    else mimeType = F("text/plain");

    server.send(200, mimeType, doc);
    
    console.println(200);
}

void handleNotFound() {
  console.println("Handling file not found");
  String message = F("File Not Found\n\n");
  
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");
  
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, F("text/plain"), message);
  console.println(message);
  console.println(404);
}
