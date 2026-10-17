/* Wi-Fiルーターに接続する */
void connectToWifi()
{
    Serial.println("\nScanning APs...");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false);

    int n = WiFi.scanNetworks();

    int targetIndex = -1;
    int bestRSSI = -1000;

    for (int i = 0; i < n; i++)
    {
        if (WiFi.SSID(i) == WIFI_SSID)
        {
            int rssi = WiFi.RSSI(i);

            Serial.printf("Found: BSSID=%s CH=%2d RSSI=%3d\n", WiFi.BSSIDstr(i).c_str(), WiFi.channel(i), rssi);

            if (rssi > bestRSSI)
            {
                bestRSSI = rssi;
                targetIndex = i;
            }
        }
    }

    if (targetIndex != -1)
    {
        uint8_t bssid[6];
        memcpy(bssid, WiFi.BSSID(targetIndex), 6);

        int channel = WiFi.channel(targetIndex);

        WiFi.begin(
            WIFI_SSID,
            WIFI_PASSWORD,
            channel,
            bssid,
            true);

        Serial.print("Connecting");

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }

        Serial.println();
        Serial.println("WiFi connected");
        Serial.printf("IP Address : %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("Connected BSSID : %s\n", WiFi.BSSIDstr().c_str());
        Serial.printf("Connected CH    : %d\n", WiFi.channel());
        Serial.printf("Connected RSSI  : %d\n", WiFi.RSSI());
    }
    else
    {
        Serial.println("No AP found");
    }

    WiFi.scanDelete();
}

/* Multicast DNSを開始する */
void startMDNS()
{
  Serial.print("mDNS server instancing ");
  while (!MDNS.begin(DEVICE_NAME))
  {
    Serial.print(".");
    delay(100);
  }
  Serial.print("  mDNS NAME:");
  Serial.print(DEVICE_NAME);
  Serial.println(".local");
}

void startOTA()
{
  ArduinoOTA.setHostname(DEVICE_NAME)
      .onStart([]()
               {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else  // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS
        // using SPIFFS.end()
        Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed"); });
  ArduinoOTA.begin();
  Serial.println("OTA Started.");
}
