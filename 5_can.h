

/*
 * Sets can hardware filter and initializes can interface
 */
void initCAN() {
  debugPrintln("Initialisiere CAN-Bus...");
  CAN_cfg.speed = CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_23;
  CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));

  //set hardware filter for CAN IDs
  //comment out if not needed
  //refer Datasheet SJA1000 (6.3.9.1) or esp32can-Library
  CAN_filter_t p_filter;
  p_filter.FM = Single_Mode;//Dualmode also possible but then only half size
  //Filter for FuelCell App
  if (app == UK_FC) {
    //Filter
    p_filter.ACR0 = 0x6E;
    p_filter.ACR1 = 0;
    p_filter.ACR2 = 0;
    p_filter.ACR3 = 0;
    //Mask
    p_filter.AMR0 = 0x60;
    p_filter.AMR1 = 0xFF;
    p_filter.AMR2 = 0xFF;
    p_filter.AMR3 = 0xFF;
  }
  //Hardware Filter for P0P4 dbc
  else if (app == UK_P0P4) {
    //Filter
    p_filter.ACR0 = 0xC0;
    p_filter.ACR1 = 0;
    p_filter.ACR2 = 0;
    p_filter.ACR3 = 0;
    //Mask
    p_filter.AMR0 = 0xFE;
    p_filter.AMR1 = 0xFF;
    p_filter.AMR2 = 0xFF;
    p_filter.AMR3 = 0xFF;
  }
  //No filter
  else {
    //Filter
    p_filter.ACR0 = 0;
    p_filter.ACR1 = 0;
    p_filter.ACR2 = 0;
    p_filter.ACR3 = 0;
    //Mask
    p_filter.AMR0 = 0xFF;
    p_filter.AMR1 = 0xFF;
    p_filter.AMR2 = 0xFF;
    p_filter.AMR3 = 0xFF;
  }
  ESP32Can.CANConfigFilter(&p_filter);
  
  // Init CAN Module
  if (ESP32Can.CANInit() == 0) {
    debugPrintln("CAN-Bus gestartet.");
  } else {
    debugPrintln("ERROR Initializing CAN...");
    prog_state = CRIT_ERROR;
    setStatusLED();
  }
}

/*
 * checks can bus if messages are available when in BT mode
 * delivers message frame to BT frame handler
 */
void checkCanForBT() {
  CAN_frame_t rx_frame;
  // Receive next CAN frame from queue
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {
    //falls neuer Standard frame und kein rtr
    if ((rx_frame.FIR.B.FF == CAN_frame_std) && (rx_frame.FIR.B.RTR != CAN_RTR)) {
      buildAndSendBTFrame(rx_frame);
    }
  }
}

/*
 * handler for message id 0x601 (P0P4 App or website)
 */
void handle0x601(CAN_frame_t &rx_frame) {
  setRawValue(rx_frame, HMI_SocHvbatAct);
  setRawValue(rx_frame, HMI_StHCUErr);
  setRawValue(rx_frame, HMI_ChrgTFull_hh);
  setRawValue(rx_frame, HMI_ChrgTFull_mm);
  setRawValue(rx_frame, HMI_Teng);
  setRawValue(rx_frame, HMI_PwrEngMin);
  setRawValue(rx_frame, HMI_PwrEngMax);
  setRawValue(rx_frame, HMI_THvbatAct);
  setRawValue(rx_frame, HMI_PhvbatMin);
}
/*
 * handler for message id 0x602 (P0P4 App or website)
 */
void handle0x602(CAN_frame_t &rx_frame) {
  setRawValue(rx_frame, HMI_PhvbatMax);
  setRawValue(rx_frame, HMI_TBsgAct);
  setRawValue(rx_frame, HMI_PwrBsgMin);
  setRawValue(rx_frame, HMI_PwrBsgMax);
  setRawValue(rx_frame, HMI_TEmAct);
  setRawValue(rx_frame, HMI_TACDCAct);
  setRawValue(rx_frame, HMI_PwrEmMin);
  setRawValue(rx_frame, HMI_PwrEmMax);
  setRawValue(rx_frame, HMI_TripElecEnergyConsum);
}
/*
 * handler for message id 0x603 (P0P4 App or website)
 */
void handle0x603(CAN_frame_t &rx_frame) {
  setRawValue(rx_frame, HMI_TripDistTot);
  setRawValue(rx_frame, HMI_TripAvSpd);
  setRawValue(rx_frame, HMI_TripEDriveDis);
  setRawValue(rx_frame, HMI_TripElecEnergyRecup);
  setRawValue(rx_frame, HMI_TripDistEV);
  setRawValue(rx_frame, HMI_TripDistHy);
}
/*
 * handler for message id 0x604 (P0P4 App or website)
 */
void handle0x604(CAN_frame_t &rx_frame) {
  //byte mask = 0b01111111;
  //HMI_TripDistICE.rawValue = (mask & rx_frame.data.u8[0]);
  //TODO Wert einer Variable zuweisen
  setRawValue(rx_frame, HMI_TripDistICE);
}
/*
 * handler for message id 0x605 (P0P4 App or website)
 */
void handle0x605(CAN_frame_t &rx_frame) {
  setRawValue(rx_frame, HMI_ULvbatAct);
  setRawValue(rx_frame, HMI_StHvbatAct);
  setRawValue(rx_frame, HMI_UHvbatAct);
  setRawValue(rx_frame, HMI_StModeAct);
  setRawValue(rx_frame, HMI_EVModeSwAck);
  setRawValue(rx_frame, HMI_DriveModeSwAck);
  setRawValue(rx_frame, HMI_TripResetSwAck);
  setRawValue(rx_frame, HMI_ChargeActive);
}
/*
 * handler for message id 0x607 (P0P4 App or website)
 */
void handle0x607(CAN_frame_t &rx_frame) {
  setRawValue(rx_frame, HMI_EFICE);
  setRawValue(rx_frame, HMI_EFP0);
  setRawValue(rx_frame, HMI_EFP4);
  setRawValue(rx_frame, HMI_EFHvBat);
  setRawValue(rx_frame, HMI_EFLvBat);
  setRawValue(rx_frame, HMI_VVehAct);
  setRawValue(rx_frame, HMI_VehPwr);
  setRawValue(rx_frame, HMI_EVMode);
  setRawValue(rx_frame, HMI_DialPos);
  setRawValue(rx_frame, HMI_DialMaxEV);
  setRawValue(rx_frame, HMI_StEngAct);
  setRawValue(rx_frame, HMI_TqEngAct);
  setRawValue(rx_frame, HMI_Phvbat);
}
/*
 * handler for message id 0x608 (P0P4 App or website)
 */
void handle0x608(CAN_frame_t &rx_frame) {
  setRawValue(rx_frame, HMI_SpdEngAct);
  setRawValue(rx_frame, HMI_StGearLvr);
  setRawValue(rx_frame, HMI_NrGear);
  setRawValue(rx_frame, HMI_PwrEng);
  setRawValue(rx_frame, HMI_IHvbatAct);
  setRawValue(rx_frame, HMI_StBsgAct);
  setRawValue(rx_frame, HMI_StBsgReq);
  setRawValue(rx_frame, HMI_TqBsgAct);
}
/*
 * handler for message id 0x609 (P0P4 App or website)
 */
void handle0x609(CAN_frame_t &rx_frame) {
  setRawValue(rx_frame, HMI_IBsgAct);
  setRawValue(rx_frame, HMI_SpdBsgAct);
  setRawValue(rx_frame, HMI_PwrBsg);
  setRawValue(rx_frame, HMI_StEmAct);
  setRawValue(rx_frame, HMI_TqEmAct);
}
/*
 * handler for message id 0x610 (P0P4 App or website)
 */
void handle0x610(CAN_frame_t &rx_frame) {
  setRawValue(rx_frame, HMI_SpdEmAct);
  setRawValue(rx_frame, HMI_IEmAct);
  setRawValue(rx_frame, HMI_PwrEm);
}

/*
 * checks can bus if messages are available when in WiFi mode
 * delivers message frame to can msg id specific handler
 */
void checkCanForWifi() {
  CAN_frame_t rx_frame;
  // Receive next CAN frame from queue
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {
    //falls neuer Standard frame und kein rtr
    if ((rx_frame.FIR.B.FF == CAN_frame_std) && (rx_frame.FIR.B.RTR != CAN_RTR)) {
      switch (rx_frame.MsgID) {
        case 0x607:
                handle0x607(rx_frame);
                break;
        case 0x608:
                handle0x608(rx_frame);
                break;
        case 0x609:
                handle0x609(rx_frame);
                break;
        case 0x610:
                handle0x610(rx_frame);
                break;
        case 0x605:
                handle0x605(rx_frame);
                break;
        case 0x601:
                handle0x601(rx_frame);
                break;
        case 0x602:
                handle0x602(rx_frame);
                break;
        case 0x603:
                handle0x603(rx_frame);
                break;
        case 0x604:
                handle0x604(rx_frame);
                break;
        default:
            debugPrintln("Can Message mit unbekannter ID empfangen.");
      }
    }
  }
}
