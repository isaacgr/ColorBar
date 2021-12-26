int writeString(uint8_t addr, char data[])
{
  int _size = strlen(data);
  for (int i = 0; i < _size; i++)
  {
    EEPROM.write(addr + i, data[i]);
  }
  EEPROM.write(addr + _size, '\0'); // Add termination null character for String Data
  EEPROM.commit();
  return _size + 1;
}

char *readString(uint8_t addr)
{
  static char data[100]; // Max 100 Bytes
  int len = 0;
  unsigned char k = EEPROM.read(addr);
  while (k != '\0' && len < 500) // Read until null character
  {
    k = EEPROM.read(addr + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return data;
}

void writeWifiEEPROM(char ssid[], char pass[])
{
  int index = 128;
  if (strlen(ssid) > 100 || strlen(pass) > 100)
  {
    throw std::length_error("Cannot exceed 100 characters");
  }
  index += writeString(index, ssid);
  EEPROM.write(508, index);
  index += writeString(index, pass);
  EEPROM.write(509, index);
  EEPROM.write(510, 1);
  EEPROM.commit();
}

void writeDeviceNameEEPROM(char name[])
{
  if (strlen(name) > 100)
  {
    throw std::length_error("Cannot exceed 100 characters");
  }
  int index = 384;
  index += writeString(index, name);
  EEPROM.write(511, index);
  EEPROM.write(512, 1);
  EEPROM.commit();
}