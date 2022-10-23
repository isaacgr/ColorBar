#ifndef FIELD_UTILS_H
#define FIELD_UTILS_H

#include <Arduino.h>
#include "fields.h"

Field getField(String name);
String getFieldValue(String name);
String setFieldValue(String name, String value);
String getFieldOptions(String name);
String getAllFields();

#endif
