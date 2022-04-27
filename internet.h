#pragma once

#include "ArduinoJson.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "time.h"
//#include "esp32-hal-time.c"

#include "credentials.h"
#include "forecast.h"

const char server[] = "api.openweathermap.org";

class MyInternet {
    public:
        MyInternet() {};
        int init();

    private:
        Forecast_t* currentWeather; // change to proper type
        tm* currentTime;
        unsigned long currentTimestamp;
        int8_t recentRSSI;

        wl_status_t initWifi();
        bool setupTime();
        bool updateTime();
        Forecast_t* getForecast(WiFiClient & client, const String & type);
        Forecast_t* decodeWeather(WiFiClient & json, String type);

        const char* mySSID     = "RobPhone";     // WiFi SSID to connect to
        const char* myPassword = "hundhund";     // WiFi password needed for the SSID

        String myApiKey = "415b7143952bdee1adb412fa2dc28782";
};