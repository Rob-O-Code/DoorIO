#pragma once

#include "ArduinoJson.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "time.h"
#include "esp32-hal-time.c"

#include "credentials.h"
#include "forecast.h"

class MyInternet {
    public:
        MyInternet();
        void init();
    private:
        Forecast_t* currentWeather; // change to proper type
        int8_t recentRSSI;

        wl_status_t initWifi();
        bool setupTime();
        tm* getTime();
        Forecast_t* getForecast(WiFiClient & client, const String & type);
        Forecast_t* decodeWeather(WiFiClient & json, String type);
};