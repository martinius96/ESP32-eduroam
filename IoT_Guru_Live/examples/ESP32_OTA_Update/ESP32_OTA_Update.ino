/**
 * 04. OTA firmware update
 *
 * Periodically checks in to the IoT Guru cloud and update the firmware to target one if
 * the current version is not equals.
 *
 * You need:
 * - the user's short identifier (you can find it on the Account page)
 * - the device's short identifier (you can find it on the Device page)
 * - the device's key (you can find it on the Device page)
 *
 * Tutorial: https://iotguru.live/tutorials/devices
 *
 * If your device is online and operating, the 'check()' function periodically sends
 * check in messages to our cloud service where you can see the last check in timestamp
 * on the list of your devices page 'Last firmware check' column.
 *
 * You can upload your firmwares: https://iotguru.live/firmwares
 *
 * Set the "Device firmware version" field of your device and our library will
 * update your device if the reported firmware version and the target firmware
 * version is not equals.
 */
#include <IoTGuru.h>
#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
const char* ssid      = "eduroam";
#define EAP_ANONYMOUS_IDENTITY "anonymous@example.com"
#define EAP_IDENTITY "nickname@example.com"
#define EAP_PASSWORD "password"

const char* ota_version = "example-1.0.1";

String userShortId      = "xxxxxxxxxxxxxxxxxxxxxx";
String deviceShortId    = "yyyyyyyyyyyyyyyyyyyyyy";
String deviceKey        = "zzzzzzzzzzzzzzzzzzzzzz";
IoTGuru iotGuru         = IoTGuru(userShortId, deviceShortId, deviceKey);

void setup() {
    Serial.begin(115200);
    delay(10);
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
  WiFi.begin(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(50);
        Serial.print(".");
    }
    Serial.println("");

    /**
     * Set check in duration, the default is 60000 milliseconds.
     */
    iotGuru.setCheckDuration(60000);
    /**
     * Set the debug printer.
     */
    iotGuru.setDebugPrinter(&Serial);

    /**
     * Check new firmware and try to update during the clean boot.
     */
    iotGuru.firmwareUpdate(ota_version);
}

void loop() {
    iotGuru.check(ota_version);
}
