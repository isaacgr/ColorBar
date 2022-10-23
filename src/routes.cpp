#include <ArduinoJson.h>
#include <WebServer.h>
#include "routes.h"
#include "eeprom_utils.h"
#include "field_utils.h"
#include "fields.h"

void setupWeb()
{
  server.begin();
  server.enableCORS();
  server.on("/all", HTTP_GET, []()
            {
    String json = getAllFields();
    server.send(200, "application/json", json); });

  server.on("/fastLedInfo", HTTP_GET, []()
            {
    String result = getFastLedInfo();
    server.send(200, "application/json", result); });

  server.on("/fieldOptions", HTTP_GET, []()
            {
    String name = server.arg("name");
    String value = getFieldOptions(name);
    StaticJsonDocument<512> root;
    root["name"] = name;
    root["options"] = value;
    String result;
    serializeJsonPretty(root, result);
    server.send(200, "application/json", result); });

  server.on("/fieldValue", HTTP_GET, []()
            {
    String name = server.arg("name");
    String value = getFieldValue(name);
    StaticJsonDocument<256> root;
    root["name"] = name;
    root["value"] = value;
    String result;
    serializeJsonPretty(root, result);
    server.send(200, "application/json", result); });

  server.on("/fieldValue", HTTP_POST, []()
            {
    String name = server.arg("name");
    String value = server.arg("value");
    try
    {
      String newValue = setFieldValue(name, value);
      StaticJsonDocument<256> root;
      root["name"] = name;
      root["newValue"] = newValue;
      String result;
      serializeJsonPretty(root, result);
      server.send(200, "application/json", result);
    }
    catch (int e)
    {
      StaticJsonDocument<32> root;
      root["error"] = e;
      String error;
      serializeJsonPretty(root, error);
      server.send(400, "application/json", error);
    } });

  server.on(
      "/wifi", HTTP_POST, []()
      {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    int ssidLen = ssid.length() + 1;
    int passLen = password.length() + 1;
    char ssidArr[ssidLen];
    char passArr[passLen];

    ssid.toCharArray(ssidArr, ssidLen);
    password.toCharArray(passArr, passLen);

    try
    {
      writeWifiEEPROM(ssidArr, passArr);
      server.send(200, "text/plain", "OK");
    }
    catch (const std::length_error &e)
    {
      StaticJsonDocument<32> root;
      root["error"] = e.what();
      String error;
      serializeJsonPretty(root, error);
      server.send(400, "application/json", error);
    } });

  server.on("/deviceName", HTTP_POST, []()
            {
    String name = server.arg("deviceName");
    int nameLen = name.length() + 1;
    char nameArr[nameLen];

    name.toCharArray(nameArr, nameLen);

    try
    {
      writeDeviceNameEEPROM(nameArr);
      server.send(200, "text/plain", "OK");
    }
    catch (const std::length_error &e)
    {
      StaticJsonDocument<32> root;
      root["error"] = e.what();
      String error;
      serializeJsonPretty(root, error);
      server.send(400, "application/json", error);
    } });

  server.onNotFound([]()
                    {
    if (!handleFileRead(server.uri()))
    {
      server.send(404, "text/plain", "Not Found");
    } });
}

String getContentType(String filename)
{
  if (filename.endsWith(".htm"))
    return "text/html";
  else if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "text/javascript";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".xml"))
    return "text/xml";
  else if (filename.endsWith(".pdf"))
    return "application/x-pdf";
  else if (filename.endsWith(".zip"))
    return "application/x-zip";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path)
{ // send the right file to the client (if it exists)
  if (path.endsWith("/") && !EEPROM.read(AP_SET))
  {
    path += "index.html"; // If a folder is requested, send the index file
  }
  else if (path.endsWith("/") && EEPROM.read(AP_SET))
  {
    path += "connect.html";
  }
  String contentType = getContentType(path); // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {
    if (SPIFFS.exists(pathWithGz))        // If there's a compressed version available
      path += ".gz";                      // If the file exists
    File file = SPIFFS.open(path, "r");   // Open it
    server.streamFile(file, contentType); // And send it to the client
    file.close();                         // Then close the file again
    return true;
  }
  return false; // If the file doesn't exist, return false
}