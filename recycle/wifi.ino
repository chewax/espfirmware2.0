void connectToWifi()
{
    WiFi.softAPdisconnect(true);
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.ssid, config.password);

    Utils::logger(String("Connecting to: ") + String(config.ssid), "WLAN");
    while (WiFi.status() != WL_CONNECTED)
    { // Wait for the Wi-Fi to connect
        delay(1000);
        USE_SERIAL.print('.');
    }

    USE_SERIAL.println(".");

    Utils::logger("Connected :)", "WLAN");
    Utils::logger( String("IP address: ") + Utils::toStringIp(WiFi.localIP()) , "WLAN");
}

//InitializesWiFi
void initializeWiFi()
{

    // connect = strlen(ssid) > 0; // Request WLAN connect if there is a SSID
    if (strlen(config.ssid) > 0)
    {   
        Utils::logger("YES CONFIG --> Initializing as STA (Station)", "WLAN");
        connectToWifi();
    }
    else
    {   
        Utils::logger("NO CONFIG --> Initializing as AP (Access Point)", "WLAN");
        startAP();
    }
}

void startAP()
{
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_AP);
    WiFi.hostname("espconfig");

    if (config.name)
    {
        strcat(softAP_ssid, config.name);
    }
    else
    {
        strcat(softAP_ssid, WiFi.macAddress().c_str());
    }

    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(softAP_ssid, softAP_password);

    delay(500); // Without delay I've seen the IP address blank


    Utils::logger("SoftAP Configured", "WLAN");
    Utils::logger(String("SSID: ") + String(softAP_ssid), "WLAN");
    Utils::logger(String("IP address: ") + Utils::toStringIp(WiFi.softAPIP()) , "WLAN");

    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    dnsServer.start(DNS_PORT, "*", apIP);
}
