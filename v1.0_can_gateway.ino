

#include "1_glob_var.h"
#include "2_glob_functions.h"
#include "3_bluetooth.h"
#include "4_wifi.h"
#include "5_can.h"
#include "6_server.h"

void setup() {
  if (debugmode) {
    Serial.begin(115200);
  }

  //initializes modules
  initLEDs();
  initWifiAP();
  initBluetooth();

   //checks periodical for existing connection
   while (!anythingConnected) {
    setStatusLED();
    //if connected to WiFi -> end BT and switch to Wifi mode
    if (WiFi.softAPgetStationNum() > 0) {
      switchToWifiMode();
    }
    //if connected to BT first -> end WiFi and switch to BT mode
    else if (SerialBT.hasClient()) {
      switchToBTMode();
    }
   }

   //initialize can
   initCAN();

  //if WiFi mode -> start webserver
  if (serviceMode == MODE_WIFI) {
    init_server();
  }

}//End: setup()

void loop() {
  //Set status LED
  setStatusLED();
  
  /*
   * reads can frame if available, builds BT message and sends to app
   * checks if BT messages are available for can
   */
  if (serviceMode == MODE_BT) {
    checkCanForBT();
    checkBT();
  }
  /*
   * reads can frame if available, parses and stores data
   */
  else if (serviceMode == MODE_WIFI) {
    checkCanForWifi();
  }

  /*
   * Set timeout counter if nothing connected, after 10 seconds warning indicator set (red led flashing)
   * If nothing connected for 25 seconds -> sets error indicator (red led on)
   * Then after 5 seconds restart
  */
  if ((WiFi.softAPgetStationNum() == 0) && (!SerialBT.hasClient())) {
    if (previousTimeoutCheck == 0) {
      previousTimeoutCheck = millis();
    } else {
      if (millis()-previousTimeoutCheck >= timeout_interval) {
        prog_state = WARNING;
        if (millis()-previousTimeoutCheck >= timeout_reset_interval) {
          prog_state = CRIT_ERROR;
          setStatusLED();
        }
      }
    }
  } else {
    previousTimeoutCheck = 0;
    if (!err_triggered) {
      prog_state = NO_ERROR;
    }
  }

}//End: loop()
