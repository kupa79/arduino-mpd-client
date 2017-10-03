
#include <DogLcd.h>
#include <EEPROM.h>
#include <IRremote.h>

#include <SPI.h>
#include <Ethernet.h>

#include "Util.h"
#include "Config.h"
#include "MpdClient.h"
#include "LcdHelper.h"

#define LCD_RS_PIN     2
#define LCD_CSB_PIN    3
#define LCD_BACKGROUND_LIGHT_PIN A5

#define LED_RED_PIN    7
#define LED_GREEN_PIN  8

#define RECV_PIN 9

#define _PLAY_BUTTON    0xC2E92AD6
#define _PAUSE_BUTTON   0x22CF7E40
#define _STOP_BUTTON    0xCA9582FA
#define _NEXT_BUTTON    0x1E6BA628
#define _PREV_BUTTON    0xBE14D11A
#define _UP_BUTTON      0x9986485A
#define _DOWN_BUTTON    0x1C6A919C
#define _OK_BUTTON      0x6AF21320
#define _POWER_BUTTON   0xB595064
#define _MENU_BUTTON    0x9BED8042

boolean POWER_ON = true;

MpdClient mpdClient;
Config config;


// initialize the library with the numbers of the interface pins
DogLcd lcd(LCD_RS_PIN, LCD_CSB_PIN);
LcdHelper lcdHelper(&lcd);

IRrecv irrecv(RECV_PIN);
decode_results results;


String readInput(const String text, const String validChars, int maxChars, boolean leerzeichen=true){
  String chars;
  if(leerzeichen){
    chars=" ";
  }else{
    chars="";
  }
  chars.concat(validChars);
  String result = "";
  lcdHelper.printQuestion(&text);
  lcd.blink();
  
  unsigned int vCharIndex=0;
  boolean ende=false;
  long button = 0;
  while(!ende){
    lcdHelper.printAnswer(&result, chars[vCharIndex]);
    
    while(!irrecv.decode(&results));
    
    button = results.value;
    irrecv.resume(); // Receive the next value
    if(button == _UP_BUTTON){
      vCharIndex = (vCharIndex+1) % chars.length();
    }else if(button == _DOWN_BUTTON){
      if(vCharIndex==0){
        vCharIndex=chars.length()-1;
      }else{
        vCharIndex--;
      }
    }else if(button == _OK_BUTTON){
      if(chars[vCharIndex] == ' ' || result.length() == maxChars-1){
        ende = true;
      }else if(result.length() < maxChars-1){
        result.concat(chars[vCharIndex]);
        vCharIndex=0;
      }
    }else if(button == _PREV_BUTTON){
      result = result.substring(0, result.length()-1);
      vCharIndex=0;
    }
  }
  if(chars[vCharIndex] != ' '){
    result.concat(chars[vCharIndex]);
  }
  lcd.noBlink();
  return result;
}

boolean configWizard(const boolean force=false){
  
  if(!force && config.read()){
    String r = readInput("Overwrite existing config?", "YN", 1, false);
    if(r.equals("N")){
      return false;
    }
  }
  
  const String mac = readInput("My MAC address:", "0123456789ABCDEF", 12);
  const String ip = readInput("My IP address:", "0123456789.", 15);
  const String serverIp = readInput("MPD IP address:", "0123456789.", 15);
  const String serverPort = readInput("MPD Port:", "0123456789", 5);
  const String password = readInput("MPD Password:", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 16);
  
  config.setMac(&mac);
  config.setIp(&ip);
  config.setServerIp(&serverIp);
  config.setPort(&serverPort);
  config.setPassword(password);
  config.save();
}


boolean isPowerButtonPressed(){
  if(irrecv.decode(&results)){
    irrecv.resume();
    if(results.value == _POWER_BUTTON){
      return true;
    }
  }
  return false;
}

void powerOn(){
  POWER_ON=true;
  digitalWrite(LED_RED_PIN,  HIGH);
  digitalWrite(LED_GREEN_PIN,  LOW);
  digitalWrite(LCD_BACKGROUND_LIGHT_PIN,  HIGH);
  lcd.display();
  lcd.setCursor(0,0);
  lcd.print("ArduMpdClient");
  lcd.setCursor(0,1);
  lcd.print("Version 0.1");
  delay(1000);
  if(!config.read()){
    configWizard();
  }
  
  #ifdef DEBUG
  config.printConfig();
  #endif
  
  connect(); 
}

void connect(){
  
  Ethernet.begin(config.getMac(), config.getIp());
  delay(1000);
  
  boolean connectionEstablished = false;
  while(!connectionEstablished){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Connect MPD=");
    if (mpdClient.connect(config.getServerIp(), config.getPort())) {
      debug("Connection to MPD-Server established");
      lcd.print("ok");

      if(config.getPassword().length()>0){
        lcd.setCursor(0,1);
        lcd.print("Password   =");
        const int pwLength = config.getPassword().length();
        char pw[pwLength+1];
        config.getPassword().toCharArray(pw, pwLength+1);
        MpdResponse* res = mpdClient.password(pw);
        if(res->success){
          lcd.print("ok");
          connectionEstablished = true;
        }else{
          mpdClient.disconnect();
          lcd.print("fail");
        }
        #ifdef DEBUG
        if(res->success){
          debug("Authentication successful");
        }else{
          debug("Authentication failed");
        }
        #endif
        
        
      }else{
        // No password needed
        connectionEstablished = true;
      }
    } else {
      lcd.print("fail");
      debug("Connection to MPD-Server failed");
    }
    if(!connectionEstablished){
      debug("Retry in 5 seconds...");
      
      for(int i=50;i>0;i--){
        lcd.setCursor(0,2);
        lcd.print("Retry in ");
        lcd.print(i/10+1);
        lcd.print(" s ...");
        delay(100);
        if(isPowerButtonPressed()){
          powerOff();
          return;
        }
      }
    }
  }
}

void powerOff(){
  POWER_ON=false;
  
  mpdClient.disconnect();
  byte invalidMac[] = {
    0,0,0,0,0,0  };
  byte invalidIp[] = {
    0,0,0,0  };
  Ethernet.begin(invalidMac, invalidIp);
  
  lcd.clear();
  lcd.noDisplay();
  digitalWrite(LED_RED_PIN,  LOW);
  digitalWrite(LED_GREEN_PIN,  LOW);
  digitalWrite(LCD_BACKGROUND_LIGHT_PIN,  LOW);
}



void setup() {
  
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  
  setupPin(LED_RED_PIN, OUTPUT, LOW);
  setupPin(LED_GREEN_PIN, OUTPUT, LOW);
  setupPin(LCD_BACKGROUND_LIGHT_PIN, OUTPUT, LOW);
  
  //Disable SD Card Reader
  setupPin(4, OUTPUT, HIGH);
  
  setupPin(RECV_PIN, OUTPUT, LOW);
  irrecv.enableIRIn();
  lcdHelper.initLcd();
  
  if(POWER_ON){
    powerOn();
  }
}

unsigned long counter=0;

void loop() {
  
  if(!POWER_ON){
    while(true){
      if(isPowerButtonPressed()){
        powerOn();
        return;
      }
    }
  }
  
  if(!mpdClient.isConnected()){
    powerOff();
    powerOn();
    return;
  }
  
  long button = 0;
  if (irrecv.decode(&results)) {
    button = results.value;
    irrecv.resume(); // Receive the next value
    
    #ifdef DEBUG
      Serial.print("IR Recv: ");
      Serial.println(button, HEX);
    #endif
    
    if(button == _POWER_BUTTON){
      powerOff();
      return;
    }else if(button == _MENU_BUTTON){
      if(configWizard()){
        powerOff();
        powerOn();
        return;
      }
    }else if(button == _PLAY_BUTTON){
      mpdClient.play();
    }else if(button == _PAUSE_BUTTON){
      mpdClient.pause();
    }else if(button == _STOP_BUTTON){
      mpdClient.stop();
    }else if(button == _NEXT_BUTTON){
      mpdClient.next();
    }else if(button == _PREV_BUTTON){
      mpdClient.prev();
    } else {
      button = 0;
    }
  }
  
  if(counter%250==0 || button != 0){
    
    MpdResponse* response = mpdClient.status();
    const char* state = response->getResponseValue("state");
    const String time = response->getResponseValue("time");
    
    lcdHelper.printMpdPlaybackStatus(state);
    
    if(strcmp(state, "play")==0){
      digitalWrite(LED_GREEN_PIN, HIGH);
      digitalWrite(LED_RED_PIN,  LOW);
    }else if(strcmp(state, "stop")==0) {
      digitalWrite(LED_GREEN_PIN, LOW);
      digitalWrite(LED_RED_PIN, HIGH);
    }else{
      digitalWrite(LED_GREEN_PIN, !digitalRead(LED_GREEN_PIN));
      digitalWrite(LED_RED_PIN, LOW);
    }
    
    char t[10];
    const String currentTime = time.substring(0, time.indexOf(":"));
    currentTime.toCharArray(t,10);
    const int iCurrentTime = atoi(t);
    
    const String totalTime = time.substring(time.indexOf(":")+1, time.length());
    totalTime.toCharArray(t,10);
    const int iTotalTime = atoi(t);
    
    ///////////////////////////////////////////////////////////////// 
    response = mpdClient.currentSong();
    const char *artist = response->getResponseValue("Artist");
    const char *title = response->getResponseValue("Title");
    
    lcdHelper.printSong(artist,title,iCurrentTime,iTotalTime);
  }
  
  delay(1);
  counter = (counter + 1) % 10000;
}



