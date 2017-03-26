
#include <ESP8266httpUpdate.h>

//---------- IOTappStory FUNCTIONS ----------
byte iotUpdaterSketch(String server, String url, String firmware, bool immediately) {
  byte retValue;
  DEBUG_MSG("\n\nUpdating Sketch from...\n\n");

  DEBUG_MSG("Update_server %s\n", server.c_str());
  DEBUG_MSG("UPDATE_URL %s\n", url.c_str());
  DEBUG_MSG("FIRMWARE_VERSION %s\n", firmware.c_str());
  t_httpUpdate_return ret = ESPhttpUpdate.update(server, 80, url, firmware);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      DEBUG_MSG("SKETCH_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      retValue = 'F';
      break;

    case HTTP_UPDATE_NO_UPDATES:
      DEBUG_MSG("---------- SKETCH_UPDATE_NO_UPDATES ------------------\n");
      retValue = 'A';
      break;

    case HTTP_UPDATE_OK:
      DEBUG_MSG("SKETCH_UPDATE_OK\n\n");
      retValue = 'U';
      break;
  }
  return retValue;
}

byte iotUpdaterSPIFFS(String server, String url, String firmware, bool immediately) {
  byte retValue;
  DEBUG_MSG("Updating SPIFFS from...\n");
  DEBUG_MSG("Update_server %s\n", server.c_str());
  DEBUG_MSG("UPDATE_URL %s \n", url.c_str());
  DEBUG_MSG("FIRMWARE_VERSION %s\n", firmware.c_str());

  t_httpUpdate_return retspiffs = ESPhttpUpdate.updateSpiffs("http://" + String(server + url), firmware);
  switch (retspiffs) {
    case HTTP_UPDATE_FAILED:
      DEBUG_MSG("SPIFFS_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      retValue = 'F';
      break;

    case HTTP_UPDATE_NO_UPDATES:
      DEBUG_MSG("---------- SPIFFS_UPDATE_NO_UPDATES ------------------\n");
      retValue = 'A';
      break;

    case HTTP_UPDATE_OK:
      DEBUG_MSG("SPIFFS_UPDATE_OK\n");
      retValue = 'U';
      break;
  }
  return retValue;
}



void IOTappStory(bool spiffs) {
  // update from IOTappStory.com
  bool updateHappened = false;
  byte res1, res2;

  //  sendSysLogMessage(7, 1, config.boardName, FIRMWARE, 10, counter++, "------------- IOTappStory -------------------");
  int wCounter = 0;
  while (!wifiConnected()&& wCounter++<30) delay(500);
  if (wifiConnected()) {
    // DEBUG_MSG("IP = %s \n\n\n", WiFi.localIP().toString().c_str());

    ESPhttpUpdate.rebootOnUpdate(false);

    res1 = iotUpdaterSketch(IOTappStory1, IOTappStoryPHP1, APP_NAME " " APP_VERSION, true);
    if (res1 == 'F') {
      //    String message = IOTappStory1 + ": Update not succesful";
      //    sendSysLogMessage(2, 1, config.boardName, FIRMWARE, 10, counter++, message);
      res2 = iotUpdaterSketch(IOTappStory2, IOTappStoryPHP2, APP_NAME, true) ;
      if (res2 == 'F') {
        //     message = IOTappStory2 + ": Update not succesful";
        //      sendSysLogMessage(2, 1, config.boardName, FIRMWARE, 10, counter++, message);
      }
    }
    if (res1 == 'U' || res2 == 'U')  updateHappened = true;

    DEBUG_MSG("\n\n");

    if (spiffs) {
      res1 = iotUpdaterSPIFFS(IOTappStory1, IOTappStoryPHP1, APP_NAME " " APP_VERSION, true);
      if (res1 == 'F') {
        //     String message = IOTappStory1 + ": Update not succesful";
        //      sendSysLogMessage(2, 1, config.boardName, FIRMWARE, 10, counter++, message);
        res2 = iotUpdaterSPIFFS(IOTappStory2, IOTappStoryPHP2, APP_NAME, true);
        if (res2 == 'F') {
          //      message = IOTappStory2 + ": Update not succesful";
          //        sendSysLogMessage(2, 1, config.boardName, FIRMWARE, 10, counter++, message);
        }
      }
    }
    if (res1 == 'U' || res2 == 'U')  updateHappened = true;

    DEBUG_MSG("\nReturning from IOTAppstory\n\n\n\n");

    if (updateHappened) {
      ESP.restart();
    }
  } else DEBUG_MSG("Wi-Fi not connected...\n");
}


void IOTappStory() {
  IOTappStory(false);
}
