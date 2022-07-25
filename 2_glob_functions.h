

/*
 * helpers for debug
 */
void debugPrint(String msg) {
  if (debugmode) {
    Serial.print(msg);
  }
}
void debugPrintln(String msg) {
  if (debugmode) {
    Serial.println(msg);
  }
}

/*
 * Sends data on can bus when requested by webserver
 */
//TODO implement value parsing if necessary
void sendCanMessage(String siteID, String value) {
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;
  if (siteID == "trip_reset") {
    tx_frame.MsgID = 0x606;
    tx_frame.FIR.B.DLC = 1;
    tx_frame.data.u8[0] = 0x04;
  }
  else if (siteID == "drivemode_sw") {
    tx_frame.MsgID = 0x606;
    tx_frame.FIR.B.DLC = 1;
    tx_frame.data.u8[0] = 0x02;
  }
  else if (siteID == "evmode_sw") {
    tx_frame.MsgID = 0x606;
    tx_frame.FIR.B.DLC = 1;
    tx_frame.data.u8[0] = 0x01;
  }
  ESP32Can.CANWriteFrame(&tx_frame);
}

/*
 * handles battery-state raw data and return state as string
 */
String getBattState(can_data &val_data) {
  int state = (int) val_data.rawValue;
  switch (state) {
    case 0x00:
      return "Undefined";
    case 0x01:
      return "Offline";
    case 0x02:
      return "Pre-charge";
    case 0x03:
      return "Connect";
    case 0x04:
      return "Online";
    case 0x05:
      return "Shutoff";
    case 0x06:
      return "Disconnect";
    case 0x07:
      return "Offline Pre-charged";
    case 0x10:
      return "Emergency Disconnect";
    case 0x20:
      return "Service Mode";
    case 0x30:
      return "Pre-Drive check";
    default:
      return "invalid value";
  }
}

/*
 * handles engine-state raw data and return state as string
 */
String getEngState(can_data &val_data) {
  int eng_state = (int) val_data.rawValue;
  switch (eng_state) {
    case 0x0:
      return "Off";
    case 0x1:
      return "On";
    case 0x2:
      return "Auto Stop";
    case 0x3:
      return "Not Used";
    default:
      return "invalid value";
  }
}

/*
 * handles gear lever position raw data and return position as string
 */
String getGearLever(can_data &val_data) {
  int lever_pos = (int) val_data.rawValue;
  switch (lever_pos) {
    case 0:
      return "Park";
    case 1:
      return "Reverse";
    case 2:
      return "Neutral";
    case 3:
      return "Drive";
    case 4:
      return "Sport";
    case 5:
      return "Manual_1_Low";
    case 6:
      return "Manual_2";
    case 7:
      return "Manual_3";
    default:
      return "invalid value";
  }
}

/*
 * handles p0-state raw data and return state as string
 */
String getP0Status(can_data &val_data) {
  int p0_status = (int) val_data.rawValue;
  switch (p0_status) {
    case 0x01:
      return "Initilisation";
    case 0x02:
      return "Initilisation Fault";
    case 0x10:
      return "Neutral";
    case 0x40:
      return "Generator Active";
    case 0x41:
      return "Generator Fault";
    case 0x42:
      return "Generator Disabled";
    case 0x43:
      return "Generator Derating";
    case 0x44:
      return "Generator Active";
    case 0x50:
      return "Torque Control Active";
    case 0x51:
      return "Torque Control Active";
    case 0x52:
      return "Torque Control Fault";
    case 0x53:
      return "Torque Control Disabled";
    case 0x54:
      return "Torque Control Disabled";
    case 0x55:
      return "Torque Control Derating";
    case 0x56:
      return "Torque Control Active";
    case 0x70:
      return "Stop Assist Active";
    case 0x71:
      return "Stop Assist Complete";
    case 0x72:
      return "Stop Assist Fault";
    case 0x73:
      return "Stop Assist Disabled";
    case 0x74:
      return "Stop Assist Disabled";
    case 0x80:
      return "Discharge Active";
    case 0x81:
      return "Discharge Complete";
    case 0x82:
      return "Discharge Fault";
    case 0x83:
      return "Discharge Disabled";
    case 0x90:
      return "Sleep Mode";
    case 0x91:
      return "Sleep Mode";
    case 0x92:
      return "Sleep Mode Fault";
    case 0x93:
      return "Sleep Mode";
    default:
      return "invalid value";
  }
}

/*
 * handles p0-mode raw data and return mode as string
 */
String getP0Req(can_data & val_data) {
  int p0_status_req = (int) val_data.rawValue;
  switch (p0_status_req) {
    case 0x01:
      return "Neutral";
    case 0x04:
      return "Generator";
    case 0x05:
      return "Torque Control";
    case 0x07:
      return "Stop Assist";
    case 0x08:
      return "Discharge";
    case 0x09:
      return "Sleep";
    case 0x0A:
      return "Generator Disabled";
    default:
      return "invalid value";
  }
}

/*
 * handles p4-state raw data and return state as string
 */
String getP4Status(can_data &val_data) {
  int p4_status = (int) val_data.rawValue;
  switch (p4_status) {
    case 0:
      return "Initilisation";
    case 1:
      return "Neutral";
    case 2:
      return "Enabled";
    case 3:
      return "Fault";
    case 4:
      return "Current Ability Reduced";
    case 5:
      return "Current Boost Activated";
    case 6:
      return "Fault";
    case 7:
      return "Warning";
    case 9:
      return "Regeneration";
    default:
      return "invalid value";
  }
}

/*
 * handles p4-mode raw data and return state as string
 */
String getP4Mode(can_data &val_data) {
  int p4_mode = (int) val_data.rawValue;
  switch (p4_mode) {
    case 0:
      return "Speed Control";
    case 3:
      return "Current Control";
    case 5:
      return "Torque Control";
    case 8:
      return "DC Voltage Control";
    default:
      return "invalid value";
  }
}

/*
 * calculates the 4-byte raw value of an incoming can signal
 */
void setRawValue(CAN_frame_t &rx_frame, can_data &val_data) {
 uint32_t raw_data = 0;
  int startbit = val_data.bitposition;
  int bitsize = val_data.bitsize;
  int endbit = (startbit + bitsize)-1;
  int startbyte = (int) startbit/8;
  int endbyte = (int) endbit/8;
  int bytesize = (endbyte - startbyte) + 1;
  int bitposition_startbyte = startbit%8;

  switch (bytesize) {
    case 1:
      raw_data = rx_frame.data.u8[startbyte];
      break;
    case 2:
      raw_data = (rx_frame.data.u8[endbyte] << 8) | rx_frame.data.u8[startbyte];
      break;
    case 3:
      int middlebyte = startbyte+1;
      raw_data = (rx_frame.data.u8[endbyte] << 16) | (rx_frame.data.u8[middlebyte] << 8) | rx_frame.data.u8[startbyte];
      break;
  }
  
  uint32_t raw_val = 0;
  raw_val = (uint32_t) ((raw_data & val_data.bitmask) >> bitposition_startbyte);
  val_data.rawValue = raw_val;
  
}

/*
 * calculates the pysical value of an numeric can signal
 */
String getPhysicalValue(can_data &val_data) {
  String temp;
  temp = String((val_data.rawValue * val_data.factor) + val_data.offset);
  return temp;
}

/*
 * calculates the XOR checksum and returns it as byte
 */
byte calcXORChecksum(byte b_arr[], int length_arr) {
  byte sum = 0x00;
  for (int i = 0; i < (length_arr-2); i++) {//-2 weil vom summenfeld und eof keine summe gebildet werden soll
    sum ^= b_arr[i];
  }
  return sum;
}

/*
 * handles requests for system info-vehicle site
 */
String getVehicle(String valueID) {
  String temp;
  if (valueID == "veh_pwr_gauge_val") {
    temp = getPhysicalValue(HMI_DialPos);
  }
  else if (valueID == "veh_velocity") {
    temp = getPhysicalValue(HMI_VVehAct);
  }
  else if (valueID == "veh_power") {
    temp = getPhysicalValue(HMI_VehPwr);
  }
  else {
      temp = "Value N/A";  
    }
  return temp;
}

/*
 * handles requests for system info-ice site
 */
String getICE(String valueID) {
  String temp;
    if (valueID == "ice_eng_pwr") {
      temp = getPhysicalValue(HMI_PwrEng);
    }
    else if (valueID == "sysinfo_eng_state") {
      temp = getEngState(HMI_StEngAct);
    }
    else if (valueID == "sysinfo_eng_torq") {
      temp = getPhysicalValue(HMI_TqEngAct);
    }
    else if (valueID == "sysinfo_eng_spd") {
      temp = getPhysicalValue(HMI_SpdEngAct);
    }
    else if (valueID == "sysinfo_gear_lever") {
      temp = getGearLever(HMI_StGearLvr);
    }
    else if (valueID == "sysinfo_gear") {
      temp = getPhysicalValue(HMI_NrGear);
    }
    else if (valueID == "sysinfo_ice_batt") {
      temp = getPhysicalValue(HMI_ULvbatAct);
    }
    else if (valueID == "sysinfo_eng_temp") {
      temp = getPhysicalValue(HMI_Teng);
    }
    else if (valueID == "ice_pwr_min") {
      temp = getPhysicalValue(HMI_PwrEngMin);
    }
    else if (valueID == "ice_pwr_max") {
      temp = getPhysicalValue(HMI_PwrEngMax);
    }
    else {
      temp = "Value N/A";  
    }
  return temp;
}//End: getICE()

/*
 * handles requests for system info-battery site
 */
String getBatt(String valueID) {
  String temp;
  if (valueID == "sysinfo_batt_pwr") {
    temp = getPhysicalValue(HMI_Phvbat);
  }
  else if (valueID == "sysinfo_batt_min") {
    temp = getPhysicalValue(HMI_PhvbatMin);
  }
  else if (valueID == "sysinfo_batt_max") {
    temp = getPhysicalValue(HMI_PhvbatMax);
  }
  else if (valueID == "sysinfo_batt_state") {
    temp = getBattState(HMI_StHvbatAct);
  }
  else if (valueID == "sysinfo_batt_power") {
    temp = getPhysicalValue(HMI_Phvbat);
  }
  else if (valueID == "sysinfo_batt_curr") {
    temp = getPhysicalValue(HMI_IHvbatAct);
  }
  else if (valueID == "sysinfo_batt_soc") {
    temp = getPhysicalValue(HMI_SocHvbatAct);
  }
  else if (valueID == "sysinfo_batt_volt") {
    temp = getPhysicalValue(HMI_UHvbatAct);
  }
  else if (valueID == "sysinfo_batt_temp") {
    temp = getPhysicalValue(HMI_THvbatAct);
  }
  else {
    temp = "Value N/A";
  }
  return temp;
}//End: getBatt()

/*
 * handles requests for system info-p0 site
 */
String getP0(String valueID) {
  String temp;
    if ( valueID == "p0_pwr") {
      temp = getPhysicalValue(HMI_PwrBsg);
    }
    else if (valueID == "p0_pwr_min") {
      temp = getPhysicalValue(HMI_PwrBsgMin);
    }
    else if (valueID == "p0_pwr_max") {
      temp = getPhysicalValue(HMI_PwrBsgMax);
    }
    else if (valueID == "sysinfo_p0_state") {
      temp = getP0Status(HMI_StBsgAct);
    }
    else if (valueID == "sysinfo_p0_mode") {
      temp = getP0Req(HMI_StBsgReq);
    }
    else if (valueID == "sysinfo_p0_torq") {
      temp = getPhysicalValue(HMI_TqBsgAct);
    }
    else if (valueID == "sysinfo_p0_spd") {
      temp = getPhysicalValue(HMI_SpdBsgAct);
    }
    else if (valueID == "sysinfo_p0_curr") {
      temp = getPhysicalValue(HMI_IBsgAct);
    }
    else if (valueID == "sysinfo_p0_temp") {
      temp = getPhysicalValue(HMI_TBsgAct);
    }
    else {
      temp =  "Value N/A";  
    }
  return temp;
}//End: getP0()

/*
 * handles requests for system info-p4 site
 */
String getP4(String valueID) {
  String temp;
  if ( valueID == "p4_pwr") {
    temp = getPhysicalValue(HMI_PwrEm);
  }
  else if (valueID == "p4_pwr_min") {
    temp = getPhysicalValue(HMI_PwrEmMin);
  }
  else if (valueID == "p4_pwr_max") {
    temp = getPhysicalValue(HMI_PwrEmMax);
  }
  else if (valueID == "sysinfo_p4_state") {
    temp = getP4Status(HMI_StEmAct);
  }
  else if (valueID == "sysinfo_p4_mode") {
    temp = getP4Mode(HMI_StModeAct);
  }
  else if (valueID == "sysinfo_p4_torq") {
    temp = getPhysicalValue(HMI_TqEmAct);
    }
  else if (valueID == "sysinfo_p4_spd") {
    temp = getPhysicalValue(HMI_SpdEmAct);
  }
  else if (valueID == "sysinfo_p4_curr") {
    temp = getPhysicalValue(HMI_IEmAct);
  }
  else if (valueID == "sysinfo_p4_temp") {
    temp = getPhysicalValue(HMI_TEmAct);
  }
  else if (valueID == "sysinfo_p4_acdctemp") {
    temp = getPhysicalValue(HMI_TACDCAct);
  }
  else {
    temp = "Value N/A";  
  }
  return temp;
}//End: getP4()

/*
 * handles requests for analytics site
 */
String getAnalytics(String valueID) {
  String temp;
  if (valueID == "analytics_trip_dist") {
    temp = getPhysicalValue(HMI_TripDistTot);
  }
  else if (valueID == "analytics_avspd") {
    temp = getPhysicalValue(HMI_TripAvSpd);
  }
  else if (valueID == "analytics_edrive_dist") {
    temp = getPhysicalValue(HMI_TripEDriveDis);
  }
  else if (valueID == "analytics_energy_cons") {
    temp = getPhysicalValue(HMI_TripElecEnergyConsum);
  }
  else if (valueID == "analytics_energy_recup") {
    temp = getPhysicalValue(HMI_TripElecEnergyRecup);
  }
  else if (valueID == "trip_reset") {
    String value = "";
    sendCanMessage(valueID, value);
  }
  else {
    temp = "Value N/A";
  }
  return temp;
}//End: getAnalytics()

/*
 * handles general requests and distributes them to specific handler
 */
String handleSite(String siteID, String valID) {
  String temp_site;
  if (siteID == "sysinfo_ice") {
    temp_site = getICE(valID);
  }
  else if (siteID == "sysinfo_batt") {
    temp_site = getBatt(valID);
  }
  else if (siteID == "sysinfo_p0") {
    temp_site = getP0(valID);
  }
  else if (siteID == "sysinfo_p4") {
    temp_site = getP4(valID);
  }
  else if (siteID == "analytics") {
    temp_site = getAnalytics(valID);
  }
  else if (siteID == "vehicle") {
    temp_site = getVehicle(valID);
  }
  else {
    temp_site = "unknown";
  }
  return temp_site;
}//End: handleSite()

/*
 * initializing LEDs
 */
void initLEDs() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
}

/*
 * sets the status LED depending on programm-state
 */
void setStatusLED() {
  switch (prog_state) {
    case NO_ERROR:
      if (!err_triggered) {
        if ((WiFi.softAPgetStationNum() > 0) || (SerialBT.hasClient())) {  //Keine Fehler aber etwas verbunden
          digitalWrite(LED_GREEN, HIGH);
          digitalWrite(LED_RED, LOW);
        } else {                                                          //Keine Fehler aber nichts verbunden
          if (millis()-previousMillis >= led_blink_interval) {
            if(green_led_on) {
              digitalWrite(LED_GREEN, LOW);
            } else {
              digitalWrite(LED_GREEN, HIGH);
            }
            green_led_on = !green_led_on;
            previousMillis = millis();
          }
        }
      }
      break;
    case WARNING:
      digitalWrite(LED_GREEN, LOW);
      if (millis()-previousMillis >= led_blink_interval) {
        if(red_led_on) {
          digitalWrite(LED_RED, LOW);
        } else {
          digitalWrite(LED_RED, HIGH);
        }
        red_led_on = !red_led_on;
        previousMillis = millis();
      }
      err_triggered = true;
      break;
    case CRIT_ERROR:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
      err_triggered = true;
      debugPrint("A critical error occured!");
      debugPrintln("Restarting in 5 seconds...");
      delay(5000);
      debugPrintln("Restarting now...");
      ESP.restart();
      break;
    default:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, LOW);
      break;
  }
}
