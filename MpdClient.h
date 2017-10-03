#ifndef MPDCLIENT_H
#define MPDCLIENT_H

#include <Arduino.h>
#include <EthernetClient.h>
#include <SPI.h>

#define MAX_RESPONSE_LINES     2 // Bei Bedarf vergrößern
#define MAX_RESPONSE_LINE_SIZE 50 // Bei Bedarf vergrößern  

class MpdResponse{
 public:
 char* errorMsg; 
 char* response[MAX_RESPONSE_LINES][2];
 boolean success;
 
 MpdResponse(){
   success=false;
 }
 ~MpdResponse(){}
 char* getResponseValue(const char* responseKeys);
};

class MpdClient {
private:
  MpdResponse mpdResponse;
  
  EthernetClient client;
  boolean connected;

  MpdResponse* sendCmd(const char* cmd);
  MpdResponse* sendCmd(const char* cmd, const char **params, const char **recognizedResponseKeys);
  boolean contains(const char **recognizedResponseKeys, const char *name);

public:
  MpdClient();
  ~MpdClient();
  MpdResponse* play();
  MpdResponse* next();
  MpdResponse* prev();
  MpdResponse* pause();
  MpdResponse* stop();
  MpdResponse* currentSong();
  MpdResponse* status();
  MpdResponse* password(const char* password);
  boolean connect(byte* serverIp, unsigned int port);
  void disconnect();
  boolean isConnected();
};

#endif




