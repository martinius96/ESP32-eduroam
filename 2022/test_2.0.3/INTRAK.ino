/*|----------------------------------------------------------|*/
/*|Experimental example for 802.1X connection                |*/
/*|Sketch wasn't tested, I am not more student, can't try it |*/
/*|Changes from @debsahu (Github) and  esp_wpa2 library ref. |*/
/*|Edited by: Martin Chlebovec (martinius96)                 |*/
/*|Compilation under 2.0.3 Arduino Core worked               |*/
/*|Previous stable cores are NOT usable, does not have       |*/
/*|WiFi.begin() definition with these parameters for PEAP... |*/
/*|----------------------------------------------------------|*/
/*|Let me know if you were successful in connecting to WiFi: |*/
/*|with screenshots from Serial at martinius96@gmail.com     |*/
/*|----------------------------------------------------------|*/

//Code based on commit by @jpswensen from 15th March 2022: https://github.com/espressif/arduino-esp32/commit/d977359e343bd1dfd83b82d14b6afc2a84fdd998
//Commit is to 2.0.3-RC1 Arduino Core, used in 2.0.3 Release version of Arduino Core for ESP32

#include <WiFi.h> //Wifi library
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks

#define EAP_IDENTITY "meno.priezvisko" //INTRAK WiFi username from https://userpanel.pcklub.sk/
#define EAP_PASSWORD "heslo"           //INTRAK WiFi password from https://userpanel.pcklub.sk/

//SSID NAME
const char* ssid = "INTRAK"; // eduroam SSID

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print(F("Connecting to network: "));
  Serial.println(ssid);
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_IDENTITY, EAP_PASSWORD); //without CERTIFICATE, NO ANONYMOUS IDENTITY
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println("");
  Serial.println(F("WiFi is connected!"));
  Serial.println(F("IP address set: "));
  Serial.println(WiFi.localIP()); //print LAN IP
}
void loop() {
  yield();
}
