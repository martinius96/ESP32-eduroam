/*|----------------------------------------------------------|*/
/*|THINGSPEAK ENTERPRISE CHANNEL WRITER FOR MULTIPLE FIELDS  |*/
/*|EMAIL: martinius96@gmail.com                              |*/
/*|NOT TESTED OFFICIALY YET                                  |*/
/*|CORE: June 2018                                           |*/
/*|----------------------------------------------------------|*/
#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include "ThingSpeak.h"
#define SECRET_CH_ID 000000      // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "XYZ"   // replace XYZ with your channel write API Key
#define EAP_ANONYMOUS_IDENTITY "anonymous@example.com"
#define EAP_IDENTITY "nickname@example.com"
#define EAP_PASSWORD "password"
const char* ssid = "eduroam"; // Eduroam SSID
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
int counter = 0;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
int number1 = 0;
int number2 = random(0, 100);
int number3 = random(0, 100);
int number4 = random(0, 100);
String myStatus = "";

void setup() {
  Serial.begin(115200);  //Initialize serial
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
  WiFi.begin(ssid); //connect to wifi network under Enterprise 802.1x
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 60) { //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    if (counter != 0) {
      counter = 0;
    }
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to AP");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  // set the fields with the values
  ThingSpeak.setField(1, number1);
  ThingSpeak.setField(2, number2);
  ThingSpeak.setField(3, number3);
  ThingSpeak.setField(4, number4);

  // figure out the status message
  if (number1 > number2) {
    myStatus = String("field1 is greater than field2");
  }
  else if (number1 < number2) {
    myStatus = String("field1 is less than field2");
  }
  else {
    myStatus = String("field1 equals field2");
  }

  // set the status
  ThingSpeak.setStatus(myStatus);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  number1++;
  if (number1 > 99) {
    number1 = 0;
  }
  number2 = random(0, 100);
  number3 = random(0, 100);
  number4 = random(0, 100);
  delay(20000); // Wait 20 seconds to update the channel again
}
