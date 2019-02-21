/*|----------------------------------------------------------|*/
/*|ThinX Cloud ENTERPRISE connection                         |*/
/*|EMAIL: martinius96@gmail.com                              |*/
/*|WEBSITE: https://arduino.php5.sk                          |*/
/*|----------------------------------------------------------|*/
#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <Arduino.h>
#include <THiNX32Lib.h>

#define __DEBUG__
#define ARDUINO_IDE

//-------- Customise these values -----------
const char *apikey = "4721f08a6df1a36b8517f678768effa8b3f2e53a7a1934423c1f42758dd83db5";
const char *owner_id = "cedc16bb6bb06daaa3ff6d30666d91aacd6e3efbf9abbc151b4dcade59af7c12";
const char* ssid = "eduroam";
#define EAP_ANONYMOUS_IDENTITY "anonymous@example.com"
#define EAP_IDENTITY "nickname@example.com"
#define EAP_PASSWORD "password"
//-------- END Customise these values -----------

THiNX thx;

/* Called after library gets connected and registered */
void finalizeCallback () {
  Serial.println("*INO: Finalize callback called. Will fall asleep.");
  ESP.deepSleep(3e9);
}

void setup() {
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
  Serial.begin(230400);

#ifdef __DEBUG__
  while (!Serial); // wait for debug console connection
  Serial.println("Connecting to predefined WiFi...");
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
  WiFi.begin(ssid); //connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 60) { //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
#endif

  // Enter API Key and Owner ID (should be faster)
  thx = THiNX(apikey, owner_id);
  thx.setFinalizeCallback(finalizeCallback);
}

/* Loop must call the thx.loop() in order to pickup MQTT messages and advance the state machine. */
void loop()
{
  thx.loop(
