void setupWeb()
{
  server.on("/all", HTTP_GET, []() {
    digitalWrite(LED_BUILTIN, 0);
    String json = getFieldsJson(fields, fieldCount);
    server.send(200, "application/json", json);
    digitalWrite(LED_BUILTIN, 1);
  });

  server.on("/fastLedInfo", HTTP_GET, []() {
    digitalWrite(LED_BUILTIN, 0);
    String result = getFastLedInfo();
    server.send(200, "application/json", result);
    digitalWrite(LED_BUILTIN, 1);
  });

  server.on("/fieldValue", HTTP_GET, []() {
    digitalWrite(LED_BUILTIN, 0);
    String name = server.arg("name");
    String value = getFieldValue(name, fields, fieldCount);
    StaticJsonDocument<256> root;
    root["name"] = name;
    root["value"] = value;
    String result;
    serializeJsonPretty(root, result);
    server.send(200, "application/json", result);
    digitalWrite(LED_BUILTIN, 1);
  });

  server.on("/fieldValue", HTTP_POST, []() {
    digitalWrite(LED_BUILTIN, 0);
    String name = server.arg("name");
    String value = server.arg("value");
    try
    {
      String newValue = setFieldValue(name, value, fields, fieldCount);
      StaticJsonDocument<256> root;
      root["name"] = name;
      root["userValue"] = value;
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
    }
    digitalWrite(LED_BUILTIN, 1);
  });

  // server.on("/", HTTP_GET, handleRoot);
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
    {
      server.send(404, "text/plain", "Not Found");
    }
  });
}