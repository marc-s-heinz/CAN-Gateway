/*
 *  TODO
 *      - http post funktionen
 */

String processor(const String& var){
  //bisher leer weil nicht notwendig
}//End: processor()

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "404: Not found");
}//End: notFound()

void loadServerCallbacks() {
  debugPrintln("Lade Server Callbacks...");
  //Route to Root
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {request->send(SPIFFS, "/index.html", "text/html");});
  //Route to m_start
  server.on("/mobile/m_start.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/mobile/m_start.html", "text/html");});
  //Route to Vehicle Site
  server.on("/mobile/m_vehicle.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/mobile/m_vehicle.html", "text/html");});
  //Route to Settings Site
  server.on("/mobile/m_settings.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/mobile/m_settings.html", "text/html");});
  //Route to Analytics Site
  server.on("/mobile/m_analytics.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/mobile/m_analytics.html", "text/html");});
  //Route to m_sys_ice
  server.on("/mobile/m_sys_ice.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/mobile/m_sys_ice.html", "text/html");});
  //Route to m_sys_battery
  server.on("/mobile/m_sys_battery.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/mobile/m_sys_battery.html", "text/html");});
  //Route to m_sys_p0
  server.on("/mobile/m_sys_p0.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/mobile/m_sys_p0.html", "text/html");});
  //Route to m_sys_p4
  server.on("/mobile/m_sys_p4.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/mobile/m_sys_p4.html", "text/html");});
  //Route to start
  server.on("/default/start.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/default/start.html", "text/html");});
  //Route to vehicle
  server.on("/default/vehicle.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/default/vehicle.html", "text/html");});
  //Route to sysinfo_ice
  server.on("/default/sysinfo_ice.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/default/sysinfo_ice.html", "text/html");});
  //Route to sysinfo_batt
  server.on("/default/sysinfo_batt.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/default/sysinfo_batt.html", "text/html");});
  //Route to sysinfo_p0
  server.on("/default/sysinfo_p0.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/default/sysinfo_p0.html", "text/html");});
  //Route to sysinfo_p4
  server.on("/default/sysinfo_p4.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/default/sysinfo_p4.html", "text/html");});
  //Route to settings
  server.on("/default/settings.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/default/settings.html", "text/html");});
  //Route to analytics
  server.on("/default/analytics.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/default/analytics.html", "text/html");});
  //Route to test
  server.on("/default/test.html", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/default/test.html", "text/html");});
 
  //Route to Style files
  server.on("/mobile/m_stylesheet.css", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/mobile/m_stylesheet.css", "text/css");});
  server.on("/default/d_stylesheet.css", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/default/d_stylesheet.css", "text/css");});
  server.on("/mobile/m_sysinfo.css", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/mobile/m_sysinfo.css", "text/css");});

  //Route to Script files
  server.on("/script_vehicle.js", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/script_vehicle.js", "text/js");});
  server.on("/script_sysinfo_ice.js", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/script_sysinfo_ice.js", "text/js");});
  server.on("/script_sysinfo_battery.js", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/script_sysinfo_battery.js", "text/js");});
  server.on("/script_sysinfo_p0.js", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/script_sysinfo_p0.js", "text/js");});
  server.on("/script_sysinfo_p4.js", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/script_sysinfo_p4.js", "text/js");});
  server.on("/script_analytics.js", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/script_analytics.js", "text/js");});
  server.on("/plot.js", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/plot.js", "text/js");});
  server.on("/chart.js", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/chart.js", "text/js");});
  server.on("/gauge.js", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/gauge.js", "text/js");});  
  
  //Route to Images
  server.on("/res/vehicle_klein.png", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/res/vehicle_klein.png", "image/png");});
  server.on("/res/system_klein.png", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/res/system_klein.png", "image/png");});
  server.on("/res/settings_klein.png", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/res/settings_klein.png", "image/png");});
  server.on("/res/analytics_klein.png", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/res/analytics_klein.png", "image/png");});
  server.on("/res/vehicle_klein_grau.png", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/res/vehicle_klein_grau.png", "image/png");});
  server.on("/res/system_klein_grau.png", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/res/system_klein_grau.png", "image/png");});
  server.on("/res/settings_klein_grau.png", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/res/settings_klein_grau.png", "image/png");});
  server.on("/res/analytics_klein_grau.png", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/res/analytics_klein_grau.png", "image/png");});
  server.on("/res/logo_white_avl.png", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/res/logo_white_avl.png", "image/png");});
  server.on("/res/button_reset1.png", HTTP_GET, [](AsyncWebServerRequest *request)  {request->send(SPIFFS, "/res/button_reset1.png", "image/png");});
  
  //404 Callback
  server.onNotFound(notFound);

  //Requests
  //Send a GET request to <IP>/get?site=<requestingSite>&value=<requestedValueID>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String req_site;
    String req_val_id;
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      req_site = request->getParam(PARAM_INPUT_1)->value();
      req_val_id = request->getParam(PARAM_INPUT_2)->value();
    }
    String response = handleSite(req_site, req_val_id);
    request->send(200, "text/plain", response);
  });

/*
  //TODO testen und auf Seite implementieren
  // Send a POST request to <IP>/post?id=<message_id>&value=<value>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
    String site_id;
    String id_value;
    if (request->hasParam(PARAM_INPUT_1, true) && request->hasParam(PARAM_INPUT_2, true)) {
      site_id = request->getParam(PARAM_INPUT_1, true)->value();
      id_value = request->getParam(PARAM_INPUT_2, true)->value();
      sendCanMessage(site_id, id_value);
    }
    request->send(200);
  });
*/
  debugPrintln("Server Callbacks geladen.");
}//End: loadServerCallbacks()

void init_server() {
  debugPrintln("Initialisiere Webserver...");
  //SPIFFS Initialisieren
  if (!SPIFFS.begin(true)) {
    prog_state = CRIT_ERROR;
    setStatusLED();
  }
  //Lade Callbacks für AsyncServer
  loadServerCallbacks();
  //Webserver erstellen
  server.begin();
  debugPrintln("Webserver initialisiert.");
}//End: init_server()
