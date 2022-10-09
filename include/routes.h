#ifndef ROUTES_H
#define ROUTES_H

#include <Arduino.h>
#include <WebServer.h>

void setupWeb();
WebServer server(80);

#endif