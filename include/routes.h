void setupWeb()
{
  server.on("/all", HTTP_GET, []()
            {
    String json = getAllFields(fields, fieldCount);
    server.send(200, "application/json", json); });

  server.on("/fastLedInfo", HTTP_GET, []()
            {
    String result = getFastLedInfo();
    server.send(200, "application/json", result); });

  server.on("/fieldOptions", HTTP_GET, []()
            {
    String name = server.arg("name");
    String value = getFieldOptions(name, fields, fieldCount);
    StaticJsonDocument<512> root;
    root["name"] = name;
    root["options"] = value;
    String result;
    serializeJsonPretty(root, result);
    server.send(200, "application/json", result); });

  server.on("/fieldValue", HTTP_GET, []()
            {
    String name = server.arg("name");
    String value = getFieldValue(name, fields, fieldCount);
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
      String newValue = setFieldValue(name, value, fields, fieldCount);
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

  // server.on("/", HTTP_GET, handleRoot);
  server.onNotFound([]()
                    {
    if (!handleFileRead(server.uri()))
    {
      server.send(404, "text/plain", "Not Found");
    } });
}