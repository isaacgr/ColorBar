#define HOSTNAME "ESP32-" ///< Hostname. The setup function adds the Chip ID at the end.
String hostname(HOSTNAME);

String getHostName()
{
  return hostname;
}

// AP mode password
const char WiFiAPPSK[] = "ledwifi32";

void setupMDNS()
{
  const char *name;
  if (EEPROM.read(MDNS_SET) == 1)
  {
    int nameIndex = EEPROM.read(MDNS_INDEX);
    name = readString(nameIndex);
    if ((name != NULL) && (name[0] == '\0'))
    {
      name = mdns_name;
    }
  }
  else
  {
    name = mdns_name;
  }
  if (MDNS.begin(name))
  { // Start the mDNS responder for esp8266.local
    Serial.printf("mDNS responder started: %s\n", name);
  }
  else
  {
    Serial.println("Error setting up MDNS responder!");
  }
}

void setupWifi()
{
  // Set Hostname.
  uint64_t chipid = ESP.getEfuseMac();
  uint16_t long1 = (unsigned long)((chipid & 0xFFFF0000) >> 16);
  uint16_t long2 = (unsigned long)((chipid & 0x0000FFFF));
  String hex = String(long1, HEX) + String(long2, HEX); // six octets
  hostname += hex;

  char hostnameChar[hostname.length() + 1];
  memset(hostnameChar, 0, hostname.length() + 1);

  for (uint8_t i = 0; i < hostname.length(); i++)
    hostnameChar[i] = hostname.charAt(i);

  WiFi.setHostname(hostnameChar);

  // Print hostname.
  // Serial.println("Hostname: " + getHostName());

  if (apmode != 0)
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(hostnameChar, WiFiAPPSK);
    Serial.printf("Connect to Wi-Fi access point: %s\n", hostnameChar);
    Serial.println("and open http://192.168.4.1 in your browser");
  }
  else
  {
    const char *id;
    const char *pass;
    if (EEPROM.read(WIFI_SET) == 1)
    {
      int idIndex = EEPROM.read(SSID_INDEX);
      id = readString(idIndex);
      int passwordIndex = EEPROM.read(PASS_INDEX);
      pass = readString(passwordIndex);
    }
    else
    {
      id = ssid;
      pass = password;
    }
    WiFi.mode(WIFI_STA);
    Serial.printf("Connecting to %s\n", id);
    if (String(WiFi.SSID()) != String(id))
    {
      WiFi.begin(id, pass);
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(1000);
        Serial.println("Connecting to WiFi...");
      }
      Serial.println("Connected to WiFi");
    }
  }
}