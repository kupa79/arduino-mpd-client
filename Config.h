#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Util.h"


const byte MAGIC[] = {
    0x74, 0xA4  };

class Config{
private:
  byte mac[6];
  byte ip[4];
  byte serverIp[4];
  unsigned int port;
  String password;
  
public:
  Config();
  ~Config();
  
  byte* getMac(){
    return mac;
  }
  
  byte* getIp(){
    return ip;
  }
  
  byte* getServerIp(){
    return serverIp;
  }
  
  unsigned int getPort(){
    return port;
  }
  
  String getPassword(){
    return password;
  }
  
  void setMac(byte* mac){
    for(byte b=0;b<6;b++){
      this->mac[b]=mac[b];
    }
  }
  
  void setIp(byte* ip){
    for(byte b=0;b<4;b++){
      this->ip[b]=ip[b];
    }
  }
  
  void setServerIp(byte* serverIp){
    for(byte b=0;b<4;b++){
      this->serverIp[b]=serverIp[b];
    }
  }
  
  void setPort(unsigned int port){
    this->port=port;
  }
  
  void setPassword(const String password){
    this->password=password;
  }
  
  
  void setMac(const String *mac);
  void setIp(const String *ip);
  void setServerIp(const String *serverIp);
  void setPort(const String *port);
  
  boolean save();
  void reset(boolean erase=false);
  boolean read();
#ifdef DEBUG
  void printConfig();
#endif
};

#endif

