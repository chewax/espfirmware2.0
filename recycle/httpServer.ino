String style = "<style> body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; } </style>";
String meta = "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
String title = "<title>SmartH</title>";
String navigation = "";
String footer = "<footer>" + navigation + "</footer>";
String scripts = "";

String startHTML = "<html> <head>" + meta + title + style + "</head> <body>";
String endHTML = scripts + "</body>" + footer + "</html>";

void handleDeviceSettings()
{
    String html = startHTML;

    html += "<h1>SmartH - Home Automation</h1>";

    if (httpServer.client().localIP() == apIP)
    {
        html += "<p>You are connected through the soft AP: " + String(softAP_ssid) + "</p>";
    }
    else
    {
        html += "<p>You are connected through the wifi network: " + String(config.ssid) + "</p>";
    }

    html += "<h2>Device Settings</h2>";

    uint32_t n = WiFi.scanNetworks();

    html += "<table>";
    html += "<tbody>";
    html += "<tr>";
    html += "<td>Connected to:</td>";
    html += "<td>" + WiFi.SSID() + "</td>";
    html += "</tr>";
    html += "<tr>";
    html += "<td>IP:</td>";
    html += "<td>" + toStringIp(WiFi.localIP()) + "</td>";
    html += "</tr>";
    html += "</tbody>";
    html += "</table>";

    html += "<form method='POST' action='settingssave'>";
    html += "<table>";

    //ROW ====================
    html += "<tr>";
    html += "<td>";
    html += "Network:";
    html += "</td>";
    html += "<td>";

    html += "<select name=\"ssid\">";

    if (n > 0)
    {
        for (uint32_t i = 0; i < n; i++)
        {
            html += "<option value = \"" + WiFi.SSID(i) + "\">";
            html += WiFi.SSID(i) + " | " + String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "open" : "protected") + " | (" + WiFi.RSSI(i) + ")";
            html += "</option>";
        }
    }
    else
    {
        html += "<option value=\"none\">No WiFi Avaiable</option>";
    }

    html += "</select>";

    html += "</td>";

    //ROW ====================
    html += "</tr>";
    html += "<td>";
    html += "Password:";
    html += "</td>";
    html += "<td>";
    html += "<input type='text' placeholder='Wifi Password' name='pass'/>";
    html += "</td>";
    html += "<tr>";

    //ROW ====================
    html += "</tr>";
    html += "<td>";
    html += "Server IP";
    html += "</td>";
    html += "<td>";
    html += "<input type='text' placeholder='0.0.0.0' name='server'/>";
    html += "</td>";
    html += "<tr>";

    //ROW ====================
    html += "</tr>";
    html += "<td>";
    html += "Device Name";
    html += "</td>";
    html += "<td>";
    html += "<input type='text' placeholder='Device name' name='name'/>";
    html += "</td>";
    html += "<tr>";

    //ROW ====================
    html += "</tr>";
    html += "<td>";
    html += "Mode";
    html += "</td>";
    html += "<td>";
    html += "<select name=\"mode\">";
    html += "<option value=\"none\">none</option>";
    html += "<option value=\"motor\">Motor Controller</option>";
    html += "<option value=\"dht\">DHT Controller</option>";
    html += "<option value=\"valve\">Water Valve Controller</option>";
    // html += "<option value=\"switch\">Switch controller</option>";
    // html += "<option value=\"blinds\">Blinds Controller</option>";
    // html += "<option value=\"valve\">Valve Controller</option>";
    // html += "<option value=\"luminosity\">Luminosity Controller</option>";
    // html += "<option value=\"motion\">Motion Controller</option>";
    // html += "<option value=\"entry\">Entry Controller</option>";
    html += "</select>";
    html += "</td>";
    html += "<tr>";

    //ROW ====================
    html += "</tr>";
    html += "<td>";
    html += "</td>";
    html += "<td>";
    html += "<input type='submit' value='Connect'/>";
    html += "</td>";
    html += "<tr>";

    html += "</table>";
    html += "</form>";

    html += endHTML;

    httpServer.send(200, "text/html", html);
}

void handleSettingsSave()
{
    String html = startHTML;

    html += "<h1>Settings Saved</h1>";

    // httpServer.arg("ssid").toCharArray(ssid, sizeof(ssid));
    // httpServer.arg("pass").toCharArray(password, sizeof(password));
    // httpServer.arg("name").toCharArray(device_name, sizeof(device_name));
    // httpServer.arg("sensor").toCharArray(device_sensor, sizeof(device_sensor));
    // httpServer.arg("actuator").toCharArray(device_actuator, sizeof(device_actuator));
    // httpServer.arg("server").toCharArray(device_server, sizeof(device_server));

    // html += endHTML;

    ConfigStruct newconf;

    httpServer.arg("ssid").toCharArray(newconf.ssid, sizeof(newconf.ssid));
    httpServer.arg("pass").toCharArray(newconf.password, sizeof(newconf.password));
    httpServer.arg("name").toCharArray(newconf.name, sizeof(newconf.name));
    httpServer.arg("mode").toCharArray(newconf.mode, sizeof(newconf.mode));
    httpServer.arg("server").toCharArray(newconf.server, sizeof(newconf.server));

    //TODO send socket message to remove board.
    httpServer.send(200, "text/html", html);

    // saveBoardConfig(newconf);
    Utils::saveConfig(newconf);

    ESP.restart();
}

void handleNotFound()
{
    String html = startHTML;
    html += "<h1>SmartH: PAGE NOT FOUND</h1>";
    html += endHTML;
    httpServer.send(404, "text/html", html);
}

void initializeWebServer()
{

    Utils::logger("Initializing HTTP Server", "SETUP");

    httpServer.on("/", handleDeviceSettings);
    httpServer.on("/settingssave", handleSettingsSave);
    httpServer.onNotFound(handleNotFound);
    httpServer.begin(); // Web server start

    Utils::logger("HTTP Server Running on port 80", "SETUP");
}