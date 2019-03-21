/**
 * IoTGuru.h - Arduino client of the https://iotguru.live cloud services.
 */
#include "IoTGuru.h"

IoTGuru::IoTGuru(String userShortId, String deviceShortId, String deviceKey) {
    this->userShortId = userShortId;
    this->deviceShortId = deviceShortId;
    this->deviceKey = deviceKey;
}

inline void IoTGuru::debugPrint(String function, int line, String msg) {
    if (this->debugPrinter) {
        debugPrinter->print(function);
        debugPrinter->print(':');
        debugPrinter->print(line);
        debugPrinter->print(" - ");
        debugPrinter->println(msg);
    }
}

IoTGuru* IoTGuru::setCheckDuration(unsigned long checkDuration) {
    this->checkDuration = checkDuration;

    return this;
}

IoTGuru* IoTGuru::setDebugPrinter(HardwareSerial* debugPrinter) {
    this->debugPrinter = debugPrinter;
    return this;
}

boolean IoTGuru::check() {
    if (lastChecked == 0 || lastChecked + checkDuration < millis()) {
        lastChecked = millis();
    } else {
        return false;
    }

    HTTPClient httpClient;
    httpClient.useHTTP10(true);
    httpClient.setTimeout(1000);

    httpClient.begin(String(IOT_GURU_BASE_URL) + "firmware/check/" + this->deviceKey);
    int code = httpClient.GET();
    httpClient.end();

    IOTGURU_DEBUG_PRINT("Check in request sent to the cloud (status code " + String(code) + ")");

    return code == 200;
}

boolean IoTGuru::sendFloatValue(String nodeShortId, String fieldName, float value) {
    HTTPClient http;
    http.useHTTP10(true);
    http.setTimeout(1000);

    HTTPClient httpClient;
    httpClient.useHTTP10(true);
    httpClient.setTimeout(1000);

    httpClient.begin(String(IOT_GURU_BASE_URL) + "measurement/create/" + nodeShortId + "/" + fieldName + "/" + String(value));
    int code = httpClient.GET();
    httpClient.end();

    IOTGURU_DEBUG_PRINT("Measurement create request sent to the cloud (status code " + String(code) + ")");

    return code == 200;
}
