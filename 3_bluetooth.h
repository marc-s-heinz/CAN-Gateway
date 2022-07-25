

/*
 * opens BT interface
 */
void initBluetooth() {
  debugPrintln("Initialisiere BT Device...");
  SerialBT.begin(btDevName);
  debugPrintln("BT initialisiert.");
}

/*
 * stops BT and switches to WiFi
 */
void switchToWifiMode() {
  debugPrintln("WiFi Verbindung erkannt.");
  debugPrintln("Beende BT Service...");
  SerialBT.end();
  anythingConnected = true;
  serviceMode = MODE_WIFI;
  debugPrintln("BT Service beendet. In WiFi Mode gewechselt.");
}

/*
 * builds frame as byte-array for BT message and sends it
 */
void buildAndSendBTFrame(CAN_frame_t &rx_frame) {
  debugPrintln("Baue BT Message Frame...");
  //Messung der durchlaufzeit für Performance Test
  unsigned long startMillis;
  if (debugmode) {
    startMillis = millis();
  }
    
  uint8_t can_std_id_size = 2;//Anzahl an Bytes für BT Message, 4 bei Extended ID
  uint8_t canDLC = rx_frame.FIR.B.DLC;
  uint8_t arraysize = canDLC + bt_frame_overhead + can_std_id_size;
  byte bt_msg[arraysize];
  uint8_t offset = 5;//unterschied zwischen bt_msg und can frame
      
  //fülle array
  bt_msg[0] = sof;
  bt_msg[1] = (byte)(arraysize-4);
  bt_msg[2] = com;
  bt_msg[3] = highByte(rx_frame.MsgID);//MSByte
  bt_msg[4] = lowByte(rx_frame.MsgID);//LSByte
  
  for (int i=0; i<canDLC; i++) {
    bt_msg[(i+offset)] = rx_frame.data.u8[i];
  }
  
  // XOR checksum
  bt_msg[(arraysize-2)] = calcXORChecksum(bt_msg, arraysize);
  bt_msg[(arraysize-1)] = eof;
  debugPrintln("BT Message Frame gebaut.");

  //Sende msg via BT
  debugPrintln("Sende BT Message...");
  SerialBT.write(bt_msg, arraysize);
  // Funktion SerialBT.flush() besitzt ein eingebautes 100ms delay und ist nicht zu verwenden
  //SerialBT.flush();

  //Test cycle time ohne BT: <10ms
  /*
  for (int i=0; i<arraysize; i++) {
    Serial.print(bt_msg[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */
  debugPrintln("BT Message gesendet.");
  
  if (debugmode) {
    unsigned long endMillis = millis();
    unsigned long duration = endMillis - startMillis;
    debugPrint("Loop Duration Sending BT: ");
    debugPrintln(String(duration));
  }
}

/*
 * checks if messages are available on bt buffer
 */
void checkBT() {
  if (SerialBT.available()) {
    //Messung der durchlaufzeit für Performance Test
    unsigned long startMillis;
    if (debugmode) {
      startMillis = millis();
    }
    CAN_frame_t tx_frame;
    tx_frame.FIR.B.FF = CAN_frame_std;
    int cnt=0;
    byte buff[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    while (SerialBT.available()) {
      buff[cnt] = SerialBT.read();
      cnt++;
    }
    uint16_t msgID = (buff[3] << 8) | buff[4];
    tx_frame.MsgID = msgID;
    int datasize = ((int) buff[1])-3;//Abzüglich com byte und 2 id bytes
    tx_frame.FIR.B.DLC = datasize;
    for (int i=0; i<datasize; i++) {
      int buff_frame = i+5; //offset bt/can frame
      tx_frame.data.u8[i] = buff[buff_frame];
    }
    ESP32Can.CANWriteFrame(&tx_frame);

    //Performance Test
    if (debugmode) {
      unsigned long endMillis = millis();
      unsigned long duration = endMillis - startMillis;
      debugPrint("Loop Duration Receiving BT: ");
      debugPrintln(String(duration));
    }
  }
}
//BT Message Format
//    sof      len      com      id1      ide2     data     checksum eof
//Trip Reset
//    01000011 00000100 00000000 00000110 00000110 00000100 01000011 00001101
//    43       04       00       06       06       04       43       0D
//EV Mode
//    01000011 00000100 00000000 00000110 00000110 00000001 01000110 00001101
//    43       04       00       06       06       01       46       0D
//Driving Mode
//    01000011 00000100 00000000 00000110 00000110 00000010 01000101 00001101
//    43       04       00       06       06       02       45       0D
