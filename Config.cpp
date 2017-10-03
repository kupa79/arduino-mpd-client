#include "Config.h"
#include "Util.h"

Config::Config(){
}
Config::~Config(){
}

#ifdef DEBUG
void Config::printConfig(){
  Serial.print("MAC: ");
  for(int a=0;a<6;a++){
    Serial.print(this->mac[a], HEX);
    if(a!=5){
      Serial.print(" ");
    }
  }
  Serial.println();
  Serial.print("IP: ");
  for(int a=0;a<4;a++){
    Serial.print(this->ip[a]);
    if(a!=3){
      Serial.print(".");
    }
  }
  Serial.println();
  Serial.print("Server IP: ");
  for(int a=0;a<4;a++){
    Serial.print(this->serverIp[a]);
    if(a!=3){
      Serial.print(".");
    }
  }
  Serial.println();
  Serial.print("Server Port: ");
  Serial.println(port);
  Serial.print("Password: ");
  Serial.println(password);
}
#endif

void Config::setMac(const String* mac){
  for(int a=0;a<6;a++){
    byte b;
    char bs[3];
    mac->substring(a*2,a*2+2).toCharArray(bs,3);
    sscanf(bs, "%x", &b);
    this->mac[a]=b;
  }
}
void Config::setIp(const String* ip){
  String ipb = *ip;
  for(int a=0;a<4;a++){
    byte b;
    char bs[4];
    ipb.substring(0, ipb.indexOf('.')).toCharArray(bs, 4);
    ipb=ipb.substring(ipb.indexOf('.')+1, ipb.length());
    sscanf(bs, "%i", &b);
    this->ip[a]=b;
  }
}
void Config::setServerIp(const String* serverIp){
  String ipb = *serverIp;
  for(int a=0;a<4;a++){
    byte b;
    char bs[4];
    ipb.substring(0, ipb.indexOf('.')).toCharArray(bs, 4);
    ipb=ipb.substring(ipb.indexOf('.')+1, ipb.length());
    sscanf(bs, "%i", &b);
    this->serverIp[a]=b;
  }
}
void Config::setPort(const String *port){
  char cport[port->length()+1];
  port->toCharArray(cport, port->length()+1);
  sscanf(cport, "%u", &this->port);
}
  

boolean Config::save(){
  byte i = 0;
  
  //TODO: Bei 0 zu kesen anfangen und nicht bei 1 (++i durch i++  ersetzen)
  //Magic Identifier
  for(int a=0;a<2;a++)
    EEPROM.write(i++,MAGIC[a]);
  
  //MAC
  for(int a=0;a<6;a++)
    EEPROM.write(i++,mac[a]);

  //IP
  for(int a=0;a<4;a++)
    EEPROM.write(i++,ip[a]);
    
  //Server IP
  for(int a=0;a<4;a++)
    EEPROM.write(i++,serverIp[a]);
  
  //Port
  EEPROM.write(i++, 0x00FF & (port >> 8));
  EEPROM.write(i++, 0x00FF & (port));

  //Password
  for(int a=0;a<password.length();a++)
    EEPROM.write(i++,password.charAt(a));
  EEPROM.write(i++,'\0');  
  
  return true;
}

void Config::reset(boolean erase){
  EEPROM.write(0,0);
  for(int i=1; erase && i<100; i++){
    EEPROM.write(i,0);
  }
}

boolean Config::read(){
  byte i = 0;
 
  //Magic Identifier
  for(int a=0;a<2;a++){
    byte m = EEPROM.read(i++);
    if(m!=MAGIC[a]){
      return false;
    }
  }
  
  //MAC
  for(int a=0;a<6;a++)
    this->mac[a]=EEPROM.read(i++);

  //IP
  for(int a=0;a<4;a++)
    this->ip[a] = EEPROM.read(i++);
    
  //Server IP
  for(int a=0;a<4;a++)
    this->serverIp[a] = EEPROM.read(i++);
  
  //Port
  port=EEPROM.read(i++) << 8;
  port=EEPROM.read(i++) | port;

  //Password
  password="";
  for(int a=0;a<100;a++){
    char c = EEPROM.read(i++);
    if(c!='\0'){
      password.concat(c);
    }else{
      break;
    }
  }
  
  return true;
}

