//#define HOSTNAME "ESP32-" ///< Hostname. The setup function adds the Chip ID at the end.

//const bool apMode = false;

// AP mode password
//const char WiFiAPPSK[] = "";

void setupMDNS()
{
  if (MDNS.begin("esp32"))
  { // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  }
  else
  {
    Serial.println("Error setting up MDNS responder!");
  }
}

void setupWifi()
{
  //  // Set Hostname.
  //  String hostname(HOSTNAME);
  //
  //  uint64_t chipid = ESP.getEfuseMac();
  //  uint16_t long1 = (unsigned long)((chipid & 0xFFFF0000) >> 16 );
  //  uint16_t long2 = (unsigned long)((chipid & 0x0000FFFF));
  //  String hex = String(long1, HEX) + String(long2, HEX); // six octets
  //  hostname += hex;
  //
  //  char hostnameChar[hostname.length() + 1];
  //  memset(hostnameChar, 0, hostname.length() + 1);
  //
  //  for (uint8_t i = 0; i < hostname.length(); i++)
  //    hostnameChar[i] = hostname.charAt(i);
  //
  //  WiFi.setHostname(hostnameChar);
  //
  //  // Print hostname.
  //  Serial.println("Hostname: " + hostname);

  //  if (apMode)
  //  {
  //    WiFi.mode(WIFI_AP);
  //    WiFi.softAP(hostnameChar, WiFiAPPSK);
  //    Serial.printf("Connect to Wi-Fi access point: %s\n", hostnameChar);
  //    Serial.println("and open http://192.168.4.1 in your browser");
  //  }
  //  else
  //  {
  WiFi.mode(WIFI_STA);
  Serial.printf("Connecting to %s\n", ssid);
  if (String(WiFi.SSID()) != String(ssid))
  {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
  }
  //  }
}