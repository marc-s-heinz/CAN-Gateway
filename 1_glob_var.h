#include "BluetoothSerial.h"
#include <WiFi.h>
#include <WiFiAP.h>
#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>


//Für debugmodus auf true setzen, ansonsten false
#define debugmode true

//modes for status led
typedef enum {
  NO_ERROR = 0,
  WARNING = 1,
  CRIT_ERROR = 2
} error_code;

//connected leds
typedef enum {
  LED_GREEN = 21,
  LED_RED = 22
} leds;

//possible connection modes
typedef enum {
  MODE_BT = 0,
  MODE_WIFI = 1
} prog_mode;

//bytesize can ids
typedef enum {
  SIZE_CAN_STD_ID = 2,
  SIZE_CAN_EXT_ID = 4
} can_msg_size;

//apps/modes for can filter
typedef enum {
  UK_FC = 0,
  UK_P0P4 = 1,
  DAIMLER = 2//no own filter yet
} app_type;

//Program sequence
volatile boolean anythingConnected = false;
volatile uint8_t prog_state = NO_ERROR;
const long led_blink_interval = 350;
volatile boolean err_triggered = false;
volatile boolean green_led_on = false;
volatile boolean red_led_on = false;
int serviceMode;
long previousMillis = 0;
const long timeout_interval = 10000;
const long timeout_reset_interval = 25000;
long previousTimeoutCheck = 0;

//BT
BluetoothSerial SerialBT;
const char *btDevName = "CAN_Test";


//WIFI
const char *ssid = "ESP32-AP";
//const char *password = "test";
IPAddress static_ip(192, 168, 0, 177);
IPAddress static_gateway(192, 168, 0, 200);
IPAddress static_subnet(255, 255, 255, 0);


//Server
AsyncWebServer server(80);
const char* PARAM_INPUT_1 = "site";
const char* PARAM_INPUT_2 = "value";


//CAN
//const int app = DAIMLER;
const int app = UK_P0P4;

CAN_device_t CAN_cfg;// CAN Config
const int rx_queue_size = 10;// Receive Queue size
const byte sof = 0x43;//start of bt frame
const byte eof = 0x0D;//end of bt frame
const byte com = 0x00;//CAN message with 11-bit ID received
const int bt_frame_overhead = 5;//für Berechnung arraysize; 5 byte overhead: sof, eof, com, len, sum

//struct for storing can data like in a dbc file
typedef struct {
  uint32_t parentID;
  String valName;
  uint8_t bitposition;
  uint8_t bitsize;
  boolean valSigned;
  double factor;
  double offset;
  double minVal;
  double maxVal;
  uint32_t rawValue;
  String endian;
  String unit;
  uint64_t bitmask;
} can_data;

/*
 * mimic dbc file data
 * each object is a signal from a dbc file
 */
//CAN Data
//Rapid Status Monitor Daimler
//C:\Users\u26h76\Documents\Daimler App\Trunk\app\src\main\java\com\example\schrick_hmi_daimler\StatusMonitor\RapidStatusMonitor.java
can_data ActPowP0{0x501,"ActPowP0",8,8,false,0.1,-13.0,-13.0,12.5,0, "motorola", "-", 0xFFFFFF};
can_data ActPowP4{0x501,"ActPowP4",16,8,false,0.1,-13.0,-13.0,12.5,0, "motorola", "-", 0xFFFFFF};
        //TODO P0 Current and ICE Torque missing
can_data P0_Current{0x501,"P0_Current",33,14,false,0.1,-819.2,-819.2,819.1,0, "motorola", "-", 0xFFFFFF};
can_data ICE_Tq{0x504,"ICE_Tq",32,13,false,0.25,-500.0,-500.0,1547.75,0, "motorola", "-", 0xFFFFFF};
can_data BSG_Tq{0x1E,"BSG_Tq",48,13,false,0.25,-500,-500,1547.75,0, "motorola", "-", 0xFFFFFF};
can_data TrqEstimd{0x650,"TrqEstimd",41,11,false,0.1,-55.0,-55.0,55.0,0, "motorola", "-", 0xFFFFFF};
can_data PN48_Bat_Curr{0x35A,"PN48_Bat_Curr",10,10,false,1,-500,-500,523,0, "motorola", "-", 0xFFFFFF};
can_data IDCEstimd{0x650,"IDCEstimd",63,10,false,1.0,-400.0,-400.0,400.0,0, "motorola", "-", 0xFFFFFF};
can_data EMM_PNHV_Bat_SOC{0x2FA,"EMM_PNHV_Bat_SOC",38,10,false,0.1,0.0,0.0,102.3,0, "motorola", "-", 0xFFFFFF};
can_data PN48_Bat_Volt{0x35A,"PN48_Bat_Volt",0,10,false,0.1,0,0,15.0,0, "motorola", "-", 0xFFFFFF};
can_data HCU_MIL{0x507,"HCU_MIL",45,3,false,1.0,0.0,0.0,7.0,0, "motorola", "-", 0xFFFFFF};
can_data WhlSpd_FR{0x505,"WhlSpd_FR",8,14,false,0.5,0.0,0.0,8191.5,0, "motorola", "-", 0xFFFFFF};
can_data WhlSpd_FL{0x505,"WhlSpd_FL",24,14,false,0.5,0.0,0.0,8191.5,0, "motorola", "-", 0xFFFFFF};
can_data WhlSpd_RR{0x505,"WhlSpd_RR",40,14,false,0.5,0.0,0.0,8191.5,0, "motorola", "-", 0xFFFFFF};
can_data WhlSpd_RL{0x505,"WhlSpd_RL",56,14,false,0.5,0.0,0.0,8191.5,0, "motorola", "-", 0xFFFFFF};

//Status Monitor P0P4
//  C:\Users\u26h76\Documents\P0P4\App\UK_P0P4\app\src\main\java\com\example\schrick_hmi_uk_p0p4\StatusMonitor\StatusMonitor.java
    //0x601 1000ms
    can_data HMI_SocHvbatAct{0x601,"HMI_SocHvbatAct",0,7,false,1.0,0.0,0.0,100.0,0,"Intel","%", 0x7F};
  can_data HMI_StHCUErr{0x601,"HMI_StHCUErr",7,2,false,1.0,0.0,0.0,3.0,0,"Intel","-", 0x0180};//warnlampe unten rechts in app
can_data HMI_ChrgTFull_hh{0x601,"HMI_ChrgTFull_hh",9,4,false,1.0,0.0,0.0,15.0,0,"Intel","Hr", 0x1E};
can_data HMI_ChrgTFull_mm{0x601,"HMI_ChrgTFull_mm",13,6,false,1.0,0.0,0.0,59.0,0,"Intel","Min", 0x07E0};
    can_data HMI_Teng{0x601,"HMI_Teng",19,8,false,1.0,-60.0,-60.0,195.0,0,"Intel","degC", 0x07F8};
    can_data HMI_PwrEngMin{0x601,"HMI_PwrEngMin",27,8,false,1.0,0.0,0.0,255.0,0,"Intel","kW", 0x07F8};//APP ICE gauge unterer wert (links)
    can_data HMI_PwrEngMax{0x601,"HMI_PwrEngMax",35,8,false,1.0,0.0,0.0,255.0,0,"Intel","kW", 0x07F8};//APP ICE gauge oberer wert (rechts)
    can_data HMI_THvbatAct{0x601,"HMI_THvbatAct",43,8,false,1.0,-78.0,-78.0,177.0,0,"Intel","degC", 0x07F8};
    can_data HMI_PhvbatMin{0x601,"HMI_PhvbatMin",51,7,false,1.0,0.0,0.0,102.4,0,"Intel","kW", 0x03F8};//APP Battery gauge unterer wert (links)
    //0x602 1000ms
    can_data HMI_PhvbatMax{0x602,"HMI_PhvbatMax",0,7,false,1.0,0.0,0.0,102.375,0,"Intel","kW", 0x7F};//App battery gauge oberer wert (rechts)
    can_data HMI_TBsgAct{0x602,"HMI_TBsgAct",7,8,false,1.0,-60.0,-60.0,195.0,0,"Intel","degC", 0x7F80};
    can_data HMI_PwrBsgMin{0x602,"HMI_PwrBsgMin",15,5,false,1.0,0.0,0.0,31.0,0,"Intel","kW", 0x0F80};//app P0 gauge unterer wert (links)
    can_data HMI_PwrBsgMax{0x602,"HMI_PwrBsgMax",20,5,false,1.0,0.0,0.0,31.0,0,"Intel","kW", 0x01F0};//App P0 gauge oberer wert (rechts)
    can_data HMI_TEmAct{0x602,"HMI_TEmAct",25,9,false,1.0,-50.0,-50.0,210.0,0,"Intel","degC", 0x03FE};
    can_data HMI_TACDCAct{0x602,"HMI_TACDCAct",34,8,false,1.0,-50.0,-50.0,130.0,0,"Intel","degC", 0x03FC};
    can_data HMI_PwrEmMin{0x602,"HMI_PwrEmMin",42,6,false,1.0,0.0,0.0,63.0,0,"Intel","kW", 0xFC};//App p4 gauge unterer wert (links)
    can_data HMI_PwrEmMax{0x602,"HMI_PwrEmMax",48,6,false,1.0,0.0,0.0,63.0,0,"Intel","kW", 0x3F};//App P4 gauge oberer Wert (rechts)
    can_data HMI_TripElecEnergyConsum{0x602,"HMI_TripElecEnergyConsum",54,10,false,1.0,0.0,0.0,1000.0,0,"kWh","kW", 0xFFC0};
    //0x603 1000ms
    can_data HMI_TripDistTot{0x603,"HMI_TripDistTot",0,14,false,0.1,0.0,0.0,1000.0,0,"Intel","km", 0x3FFF};
    can_data HMI_TripAvSpd{0x603,"HMI_TripAvSpd",14,12,false,0.1,0.0,0.0,255.0,0,"Intel","kmph", 0x03FFC0};
    can_data HMI_TripEDriveDis{0x603,"HMI_TripEDriveDis",26,14,false,0.1,0.0,0.0,1000.0,0,"Intel","km", 0xFFFC};
    can_data HMI_TripElecEnergyRecup{0x603,"HMI_TripElecEnergyRecup",40,10,false,1.0,0.0,0.0,1000.0,0,"Intel","kWh", 0x03FF};
  can_data HMI_TripDistEV{0x603,"HMI_TripDistEV",50,7,false,1.0,0.0,0.0,100.0,0,"Intel","%", 0x01FC};//App TripInfo Gauge rechts (Tortendiagram) kommt nur wenn werte vorhanden
  can_data HMI_TripDistHy{0x603,"HMI_TripDistHy",57,7,false,1.0,0.0,0.0,100.0,0,"Intel","%", 0xFE};//App TripInfo Gauge rechts (Tortendiagram) kommt nur wenn werte vorhanden
    //0x604 1000ms
  can_data HMI_TripDistICE{0x604,"HMI_TripDistICE",0,7,false,1.0,0.0,0.0,100.0,0,"Intel","%", 0x07};//App TripInfo Gauge rechts (Tortendiagram) kommt nur wenn werte vorhanden
    //0x605 100ms
    can_data HMI_ULvbatAct{0x605,"HMI_ULvbatAct",0,8,false,0.1,0.0,0.0,16.0,0,"Intel","V", 0xFF};//sysinfo ice
    can_data HMI_StHvbatAct{0x605,"HMI_StHvbatAct",8,8,false,1.0,0.0,0.0,255.0,0,"Intel","-", 0xFF};
    can_data HMI_UHvbatAct{0x605,"HMI_UHvbatAct",16,10,false,0.1,0.0,0.0,102.3,0,"Intel","V", 0x03FF};//sysinfo batt
    can_data HMI_StModeAct{0x605,"HMI_StModeAct",26,8,false,1.0,0.0,0.0,255.0,0,"Intel","-", 0x03FC};//p4 mode
can_data HMI_EVModeSwAck{0x605,"HMI_EVModeSwAck",34,1,false,1.0,0.0,0.0,1.0,0,"Intel","-", 0x04};
can_data HMI_DriveModeSwAck{0x605,"HMI_DriveModeSwAck",35,1,false,1.0,0.0,0.0,1.0,0,"Intel","-", 0x08};
can_data HMI_TripResetSwAck{0x605,"HMI_TripResetSwAck",36,1,false,1.0,0.0,0.0,1.0,0,"Intel","-", 0x10};
can_data HMI_ChargeActive{0x605,"HMI_ChargeActive",37,1,false,1.0,0.0,0.0,1.0,0,"Intel","-", 0x20};
    //0x607 10ms
  can_data HMI_EFICE{0x607,"HMI_EFICE",0,1,false,1.0,0.0,0.0,1.0,0,"Intel","V", 0x01};//APP vehicle motor hervorheben
  can_data HMI_EFP0{0x607,"HMI_EFP0",1,2,false,1.0,0.0,0.0,3.0,0,"Intel","-", 0x06};//APP vehicle pfeile bat-p0
  can_data HMI_EFP4{0x607,"HMI_EFP4",3,2,false,1.0,0.0,0.0,3.0,0,"Intel","V", 0x18};//App vehicle pfeile bat-p4 + hinterachse
  can_data HMI_EFHvBat{0x607,"HMI_EFHvBat",5,2,false,1.0,0.0,0.0,3.0,0,"Intel","-", 0x60};//App vehicle hv batt leuchtet
can_data HMI_EFLvBat{0x607,"HMI_EFLvBat",7,2,false,1.0,0.0,0.0,3.0,0,"Intel","-", 0x180};
    can_data HMI_VVehAct{0x607,"HMI_VVehAct",9,8,false,1.0,0.0,0.0,255.0,0,"Intel","kph", 0x01FE};//App vehicle andere anzeige links neben gauge geschwindigkeit
    can_data HMI_VehPwr{0x607,"HMI_VehPwr",17,8,false,1.0,0.0,0.0,255.0,0,"Intel","kW", 0x01FE};//App vehicle andere anzeige rechts neben gauge leistung
  can_data HMI_EVMode{0x607,"HMI_EVMode",25,1,false,1.0,0.0,0.0,1.0,0,"Intel","-", 0x0002};//App vehicle andere anzeige "EV" in grün mittem im gauge
    can_data HMI_DialPos{0x607,"HMI_DialPos",26,8,false,0.5,0.0,0.0,100.0,0,"Intel","%", 0x03FC};//App vehicle andere anzeige zeiger gauge
  can_data HMI_DialMaxEV{0x607,"HMI_DialMaxEV",34,8,false,0.5,0.0,0.0,100.0,0,"Intel","%", 0x03FC};//App vehicle andere anzeige gauge verändert sich grün bei ev bis zu dem wert
    can_data HMI_StEngAct{0x607,"HMI_StEngAct",42,2,false,1.0,0.0,0.0,3.0,0,"Intel","-", 0x000C};
    can_data HMI_TqEngAct{0x607,"HMI_TqEngAct",44,9,false,1.0,-255.0,-255.0,255.0,0,"Intel","Nm", 0x1FF0};
    can_data HMI_Phvbat{0x607,"HMI_Phvbat",53,8,false,1.0,-103.0,-103.0,103.0,0,"Intel","kW", 0x1FE0};//App sysinfo batt gauge zeiger
    //0x608 10ms
    can_data HMI_SpdEngAct{0x608,"HMI_SpdEngAct",0,13,false,2.0,0.0,0.0,16382.0,0,"Intel","rpm", 0x1FFF};
    can_data HMI_StGearLvr{0x608,"HMI_StGearLvr",13,3,false,1.0,0.0,0.0,7.0,0,"Intel","-", 0xE0};
    can_data HMI_NrGear{0x608,"HMI_NrGear",16,4,false,1.0,0.0,0.0,15.0,0,"Intel","-", 0x0F};
    can_data HMI_PwrEng{0x608,"HMI_PwrEng",20,9,false,1.0,-255.0,-255.0,255.0,0,"Intel","kW", 0x1FF0};
    can_data HMI_IHvbatAct{0x608,"HMI_IHvbatAct",29,12,false,1.0,-2048.0,-2048,2047.0,0,"Intel","A", 0x01FFE0}; // DBC check needed
    can_data HMI_StBsgAct{0x608,"HMI_StBsgAct",41,8,false,1.0,0.0,0.0,147.0,0,"Intel","-", 0x01FE};
    can_data HMI_StBsgReq{0x608,"HMI_StBsgReq",49,4,false,1.0,0.0,0.0,10.0,0,"Intel","-", 0x1E};//P0 Mode
    can_data HMI_TqBsgAct{0x608,"HMI_TqBsgAct",53,7,false,1.0,-55.0,-55.0,55.0,0,"Intel","Nm", 0x0FE0};
    //0x609 10ms
    can_data HMI_SpdBsgAct{0x609,"HMI_SpdBsgAct",0,15,false,1.0,-2000.0,-2000.0,22000.0,0,"Intel","rpm", 0x7FFF};
    can_data HMI_IBsgAct{0x609,"HMI_IBsgAct",15,10,false,1.0,-400.0,-400.0,400.0,0,"Intel","A", 0x01FF80};
    can_data HMI_PwrBsg{0x609,"HMI_PwrBsg",25,6,false,1.0,-31.0,-31.0,31.0,0,"Intel","kW", 0x7E};
    can_data HMI_StEmAct{0x609,"HMI_StEmAct",31,16,false,1.0,0.0,0.0,65535.0,0,"Intel","-", 0x7FFF80};
    can_data HMI_TqEmAct{0x609,"HMI_TqEmAct",47,13,false,1.0,-3267.8,-3267.8,3276.7,0,"Intel","Nm", 0x0FFF80}; // DBC check needed   //warum so komische werte? mit dem faktor kommt man da nicht drauf
    //0x610 10ms
    can_data HMI_SpdEmAct{0x609,"HMI_SpdEmAct",0,16,false,1.0,-32768.0,-32768.0,32767.0,0,"Intel","rpm", 0xFFFF};
    can_data HMI_IEmAct{0x609,"HMI_IEmAct",16,12,false,1.0,0.0,0.0,2316.0,0,"Intel","A", 0x0FFF};//Check
    can_data HMI_PwrEm{0x609,"HMI_PwrEm",28,7,false,1.0,-63.0,-63.0,63.0,0,"Intel","kW", 0x07F0};//Check



    
