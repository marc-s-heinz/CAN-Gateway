

/*
 * opens WiFi soft Access point
 */
void initWifiAP() {
  debugPrintln("Initialisiere WiFi AP...");
  WiFi.softAPConfig(static_ip, static_gateway, static_subnet);//Legt Statische IP fest
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid/*, password*/);
  debugPrintln("WiFi AP gestartet.");
}

/*
 * stops WiFi and switches to BT
 */
void switchToBTMode() {
  debugPrintln("BT Verbindung erkannt.");
  debugPrintln("Beende WiFi AP...");
  while (!WiFi.softAPdisconnect()) {
    ;
  }
  anythingConnected = true;
  serviceMode = MODE_BT;
  debugPrintln("WiFi AP beendet. In BT Mode gewechselt.");
}
