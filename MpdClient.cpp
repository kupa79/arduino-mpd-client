
#include "MpdClient.h"
#include "Util.h"

MpdClient::MpdClient(){
  this->connected = false;
  
  mpdResponse.errorMsg=(char*)realloc(mpdResponse.errorMsg, sizeof(char));
  mpdResponse.errorMsg[0]='\0';
  
  for(int i=0;i<MAX_RESPONSE_LINES;i++){
    mpdResponse.response[i][0]=(char*)realloc(mpdResponse.response[i][0], sizeof(char));
    mpdResponse.response[i][1]=(char*)realloc(mpdResponse.response[i][1], sizeof(char));
    mpdResponse.response[i][0][0]='\0';
    mpdResponse.response[i][1][0]='\0';
  }
}

MpdClient::~MpdClient(){
  
  free(mpdResponse.errorMsg);
  
  for(int i=0;i<MAX_RESPONSE_LINES;i++){
    free(mpdResponse.response[i][0]);
    free(mpdResponse.response[i][1]);
  }
}

MpdResponse* MpdClient::play(){
  return sendCmd("play");
}

MpdResponse* MpdClient::next(){
  return sendCmd("next");
}

MpdResponse* MpdClient::prev(){
  return sendCmd("previous");
}

MpdResponse* MpdClient::pause(){
  return sendCmd("pause");
}

MpdResponse* MpdClient::stop(){
  return sendCmd("stop");
}

MpdResponse* MpdClient::currentSong(){
  const char* recognized[] = {
    "Artist","Title",NULL      };
  return sendCmd("currentsong", NULL, recognized);
}

MpdResponse* MpdClient::password(const char *password){
  const char* params[] = {
    password, NULL      };
  return sendCmd("password", params, NULL);
}

MpdResponse* MpdClient::status(){
  const char* recognized[] = {
    "state","time",NULL      };
  return sendCmd("status", NULL, recognized);
}

char* MpdResponse::getResponseValue(const char * name){
  for(int i=0;i<MAX_RESPONSE_LINES;i++){
    if(strcmp(response[i][0], name) == 0){
      return response[i][1];
    }
  }
  return NULL; 
}

boolean MpdClient::contains(const char **recognized, const char *name){
  for(int i=0;recognized!=NULL && i<MAX_RESPONSE_LINES;i++){
    if(recognized[i]!=NULL){
      if(strcmp(name, recognized[i]) == 0){
        return true;
      }
    }
    else{
      break;
    }
  }
  return false;  
}

MpdResponse* MpdClient::sendCmd(const char* cmd){
  return sendCmd(cmd, NULL, NULL);
}

MpdResponse* MpdClient::sendCmd(const char* cmd, const char **params, const char **recognized){
  
  mpdResponse.errorMsg[0]='\0';
  
  for(int i=0;i<MAX_RESPONSE_LINES;i++){
    mpdResponse.response[i][0][0]='\0';
    mpdResponse.response[i][1][0]='\0';
  }

  client.print(cmd);
  for(int a=0;a<3;a++){
    if(params!=NULL && params[a]!=NULL){
      client.print(" ");
      client.print(params[a]);
    }
    else{
      break;
    }
  }
  client.println();

  while(!client.available()){
    debug("Waiting for response...");
    if(!isConnected()){
      mpdResponse.success = false;
      return &mpdResponse;
    }
  }

  unsigned int responseIndex=0;
  char * line = NULL; 
  unsigned int lineIndex=0;
  
  while(client.available())  {
    char c = client.read();
    if(c != '\n'){
      if(lineIndex<MAX_RESPONSE_LINE_SIZE-1){
        line = (char*) realloc(line, (lineIndex+2) * sizeof(char));
        line[lineIndex]=c;
        line[lineIndex+1]='\0';
        lineIndex++;
      }
    }
    else{
      if(strncmp(line, "OK", 2)==0){
        free(line);
        mpdResponse.success = true;
        return &mpdResponse;
      }
      else if(strncmp(line, "ACK", 3)==0){
        char* msg = line + 4;
        mpdResponse.errorMsg = (char*) realloc(mpdResponse.errorMsg, strlen(msg) + 1);
        strcpy(mpdResponse.errorMsg, msg);
        free(line);
        mpdResponse.success = false;
        return &mpdResponse;
      }

      for(int i=0; i<lineIndex; i++){
        if(line[i] == ':'){
          line[i] = '\0';
          char* name = line;
          char* val = line + i + 2;
          if(contains(recognized, name) && responseIndex<MAX_RESPONSE_LINES){
            mpdResponse.response[responseIndex][0]=(char*)realloc(mpdResponse.response[responseIndex][0], (strlen(name)+1)*sizeof(char));
            mpdResponse.response[responseIndex][1]=(char*)realloc(mpdResponse.response[responseIndex][1], (strlen(val)+1)*sizeof(char));
            strncpy(mpdResponse.response[responseIndex][0], name, strlen(name));
            strncpy(mpdResponse.response[responseIndex][1], val, strlen(val));
            mpdResponse.response[responseIndex][0][strlen(name)]='\0';
            mpdResponse.response[responseIndex][1][strlen(val)]='\0';
            responseIndex++;
          }
          break;
        }
      }
      lineIndex=0;
      line[0] = '\0';
    }
  }
  free(line);

}

boolean MpdClient::isConnected(){
  connected = connected && client.connected();
  return connected;
}

void MpdClient::disconnect(){
  client.stop();
  connected = false;
}

boolean MpdClient::connect(byte* serverIp, unsigned int port){
  if(!isConnected() && client.connect(serverIp, port)){
    char connectResponse[20]="";
    while(!client.available());
    unsigned int i=0;
    while(client.available()){
      connectResponse[i] = (char) client.read();
      connectResponse[i+1] = '\0';
      i++;
    }
    debug(connectResponse);
    
    if(strncmp(connectResponse, "OK MPD", 6) == 0){
      connected = true;
    }
  }
  return connected;
}


