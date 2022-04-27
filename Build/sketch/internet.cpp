#include "internet.h"

int MyInternet::init() {
    if (initWifi() != WL_CONNECTED) {
        Serial.println("Connection failed");
        return -1;
    }
    if (!setupTime()) {
        Serial.println("Failed to set up time");
        return -2;
    }
    return 0;
}

wl_status_t MyInternet::initWifi() {
    Serial.print("\r\nConnecting to: "); Serial.println(String(mySSID));
    IPAddress dns(8, 8, 8, 8); // Google DNS
    WiFi.disconnect();
    WiFi.mode(WIFI_STA); // switch off AP
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(mySSID, myPassword);
    unsigned long start = millis();
    wl_status_t connectionStatus;
    bool AttemptConnection = true;
    while (AttemptConnection) {
        connectionStatus = WiFi.status();
        if (millis() > start + 15000) { // Wait 15-secs maximum
            AttemptConnection = false;
        }
        if (connectionStatus == WL_CONNECTED || connectionStatus == WL_CONNECT_FAILED) {
            AttemptConnection = false;
        }
        delay(50UL);
    }
    if (connectionStatus == WL_CONNECTED) {
        recentRSSI = WiFi.RSSI(); // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
        Serial.println("WiFi connected at: " + WiFi.localIP());
    }
    else Serial.println("WiFi connection *** FAILED ***");
    return connectionStatus;
}

bool MyInternet::setupTime() {
    configTime(-5 * 3600, 3600, "time.nist.gov", "0.north-america.pool.ntp.org", "");
    delay(100UL);
    return updateTime();
}

bool MyInternet::updateTime() {
    struct tm *timeInfo = new struct tm;
    while (!getLocalTime(timeInfo, 10000)) { // Wait for 10-sec for time to synchronise
        Serial.println("Failed to obtain time");
        return false;
    }
    currentTimestamp = millis();
    delete currentTime;
    currentTime = timeInfo;
    return true;
}

Forecast_t* MyInternet::getForecast(WiFiClient & client, const String & type) {
    Forecast_t* WxConditions;
    client.stop(); // close connection before sending a new request
    HTTPClient http;

    // TODO: add more user preferences
    String uri = "/data/2.5/" + type + "?q=Worcester,USA&APPID=" + myApiKey + "&mode=json&units=imperial&lang=EN";
    if (type != "weather") {
        uri += "&cnt=2"; //+ String(MaxReadings);
    }
    //http.begin(uri,test_root_ca); //HTTPS example connection
    //http.begin(client, "jigsaw.w3.org", 80, "/HTTP/connection.html");
    http.begin(client, server, 80, uri);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        WxConditions = decodeWeather(http.getStream(), type);
        client.stop();
        http.end();
        return WxConditions;
    }
    Serial.printf("connection failed, error: %s", http.errorToString(httpCode).c_str());
    client.stop();
    http.end();
    return nullptr;
}

#define MaxReadings 2
Forecast_t* MyInternet::decodeWeather(WiFiClient & json, String type) {
    Forecast_t* WxConditions = new Forecast_t;
    Serial.print(F("Creating object...and "));
    DynamicJsonDocument doc((MaxReadings + 2) * 1024); // allocate memory for the JsonDocument
    DeserializationError error = deserializeJson(doc, json);
    if (error) { // Test if parsing succeeds.// Deserialize the JSON document
        Serial.print(F("DeserializeJson() failed: "));
        Serial.println(error.c_str());
        return nullptr;
    }
    JsonObject root = doc.as<JsonObject>(); // convert it to a JsonObject
    Serial.println(" Decoding " + type + " data");
    Serial.println(doc.memoryUsage());
    if (type == "weather") {
        // All Serial.println statements are for diagnostic purposes and not required, remove if not needed
        WxConditions[0].lon         = root["coord"]["lon"];
        WxConditions[0].lat         = root["coord"]["lat"];
        WxConditions[0].Main0       = root["weather"][0]["main"].as<char*>();         Serial.print("Main0: "); Serial.println(WxConditions[0].Main0);
        WxConditions[0].Forecast0   = root["weather"][0]["description"].as<char*>();  Serial.print("Fore0: "); Serial.println(WxConditions[0].Forecast0);
        WxConditions[0].Forecast1   = root["weather"][1]["main"].as<char*>();         Serial.print("Fore1: "); Serial.println(WxConditions[0].Forecast1);
        WxConditions[0].Forecast2   = root["weather"][2]["main"].as<char*>();         Serial.print("Fore2: "); Serial.println(WxConditions[0].Forecast2);
        WxConditions[0].Icon        = root["weather"][0]["icon"].as<char*>();         Serial.print("Icon : "); Serial.println(WxConditions[0].Icon);
        WxConditions[0].Temperature = root["main"]["temp"];                           Serial.print("Temp : "); Serial.println(WxConditions[0].Temperature);
        WxConditions[0].Pressure    = root["main"]["pressure"];                       Serial.print("Pres : "); Serial.println(WxConditions[0].Pressure);
        WxConditions[0].Humidity    = root["main"]["humidity"];                       Serial.print("Humi : "); Serial.println(WxConditions[0].Humidity);
        WxConditions[0].Low         = root["main"]["temp_min"];                       Serial.print("TLow : "); Serial.println(WxConditions[0].Low);
        WxConditions[0].High        = root["main"]["temp_max"];                       Serial.print("THigh: "); Serial.println(WxConditions[0].High);
        WxConditions[0].Windspeed   = root["wind"]["speed"];                          Serial.print("WSpd : "); Serial.println(WxConditions[0].Windspeed);
        WxConditions[0].Winddir     = root["wind"]["deg"];                            Serial.print("WDir : "); Serial.println(WxConditions[0].Winddir);
        WxConditions[0].Cloudcover  = root["clouds"]["all"];                          Serial.print("CCov : "); Serial.println(WxConditions[0].Cloudcover); // in % of cloud cover
        WxConditions[0].Dt          = root["dt"];                                     Serial.print("Dtime: "); Serial.println(WxConditions[0].Dt);         // Date-time of reading
        WxConditions[0].Timezone    = root["timezone"];                               Serial.print("Tzone: "); Serial.println(WxConditions[0].Timezone);   // Timezone of data
        WxConditions[0].Visibility  = root["visibility"];                             Serial.print("Visi : "); Serial.println(WxConditions[0].Visibility); // in metres
        WxConditions[0].Country     = root["sys"]["country"].as<char*>();             Serial.print("Coun : "); Serial.println(WxConditions[0].Country);
        WxConditions[0].Sunrise     = root["sys"]["sunrise"];                         Serial.print("SunR : "); Serial.println(WxConditions[0].Sunrise);
        WxConditions[0].Sunset      = root["sys"]["sunset"];                          Serial.print("SunS : "); Serial.println(WxConditions[0].Sunset);
        //if (Units == "M") WxConditions[0].Windspeed *= 3.6; // Convert Windspeed in m/s to KPH
    }
    /*
    if (Type == "forecast") {
    //Serial.println(json);
    JsonArray list = root["list"];
    Serial.print("\nReceiving Forecast period-"); //------------------------------------------------
    for (byte r = 0; r < MaxReadings; r++) {
        Serial.println("\nPeriod-" + String(r) + "--------------");
        WxForecast[r].Dt          = list[r]["dt"];                                    Serial.print("DTime: "); Serial.println(WxForecast[r].Dt);
        WxForecast[r].Temperature = list[r]["main"]["temp"];                          Serial.print("Temp : "); Serial.println(WxForecast[r].Temperature);
        WxForecast[r].Low         = list[r]["main"]["temp_min"];                      Serial.print("TLow : "); Serial.println(WxForecast[r].Low);
        WxForecast[r].High        = list[r]["main"]["temp_max"];                      Serial.print("THig : "); Serial.println(WxForecast[r].High);
        WxForecast[r].Pressure    = list[r]["main"]["pressure"];                      Serial.print("Pres : "); Serial.println(WxForecast[r].Pressure);
        WxForecast[r].Humidity    = list[r]["main"]["humidity"];                      Serial.print("Humi : "); Serial.println(WxForecast[r].Humidity);
        WxForecast[r].Forecast0   = list[r]["weather"][0]["main"].as<char*>();        Serial.print("Fore0: "); Serial.println(WxForecast[r].Forecast0);
        WxForecast[r].Forecast0   = list[r]["weather"][1]["main"].as<char*>();        Serial.print("Fore1: "); Serial.println(WxForecast[r].Forecast1);
        WxForecast[r].Forecast0   = list[r]["weather"][2]["main"].as<char*>();        Serial.print("Fore2: "); Serial.println(WxForecast[r].Forecast2);
        WxForecast[r].Description = list[r]["weather"][0]["description"].as<char*>(); Serial.print("Desc : "); Serial.println(WxForecast[r].Description);
        WxForecast[r].Icon        = list[r]["weather"][0]["icon"].as<char*>();        Serial.print("Icon : "); Serial.println(WxForecast[r].Icon);
        WxForecast[r].Cloudcover  = list[r]["clouds"]["all"];                         Serial.print("CCov : "); Serial.println(WxForecast[0].Cloudcover); // in % of cloud cover
        WxForecast[r].Windspeed   = list[r]["wind"]["speed"];                         Serial.print("WSpd : "); Serial.println(WxForecast[r].Windspeed);
        WxForecast[r].Winddir     = list[r]["wind"]["deg"];                           Serial.print("WDir : "); Serial.println(WxForecast[r].Winddir);
        WxForecast[r].Rainfall    = list[r]["rain"]["3h"];                            Serial.print("Rain : "); Serial.println(WxForecast[r].Rainfall);
        WxForecast[r].Snowfall    = list[r]["snow"]["3h"];                            Serial.print("Snow : "); Serial.println(WxForecast[r].Snowfall);
        WxForecast[r].Period      = list[r]["dt_txt"].as<char*>();                    Serial.print("Peri : "); Serial.println(WxForecast[r].Period);
        if (Units == "M") WxForecast[0].Windspeed *= 3.6; // Convert Windspeed in m/s to KPH
    }
    //------------------------------------------
    float pressure_trend = WxForecast[0].Pressure - WxForecast[1].Pressure; // Measure pressure slope between ~now and later
    pressure_trend = ((int)(pressure_trend * 10)) / 10.0; // Remove any small variations less than 0.1
    WxConditions[0].Trend = "0";
    if (pressure_trend > 0)  WxConditions[0].Trend = "+";
    if (pressure_trend < 0)  WxConditions[0].Trend = "-";
    if (pressure_trend == 0) WxConditions[0].Trend = "0";
    }
    */
    return WxConditions;
}