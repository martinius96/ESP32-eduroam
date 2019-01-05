/*|----------------------------------------------------------|*/
/*|IBM WATSON ENTERPRISE connection                          |*/
/*|EMAIL: martinius96@gmail.com                              |*/
/*|NOT TESTED OFFICIALY YET                                  |*/
/*|CORE: June 2018                                           |*/
/*|----------------------------------------------------------|*/
#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <PubSubClient.h>
#include <SPI.h>
//-------- Customise these values -----------
const char* ssid = "eduroam";
#define EAP_ANONYMOUS_IDENTITY "anonymous@example.com"
#define EAP_IDENTITY "nickname@example.com"
#define EAP_PASSWORD "password"
//-------- END Customise these values -----------
#define ORG "quickstart" // your organization or "quickstart"
#define DEVICE_TYPE "esp32" // use this default for quickstart or customize to your registered device type
#define DEVICE_ID "test1" // use this default for quickstart or customize to your registered device id
#define TOKEN "<yourDeviceToken>" // your device token or not used with "quickstart"
//-------- Customise the above values --------

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/status/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, wifiClient);
int counter = 0;
void setup() {
 Serial.begin(115200); delay(1); Serial.println();
Serial.print("Connecting to network: ");
  Serial.println(ssid);
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY)); 
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
 initWiFi();
}

void loop() {

 if (!!!client.connected()) {
   Serial.print("Reconnecting client to "); Serial.println(server);
   while (!!!client.connect(clientId, authMethod, token)) {
     Serial.print(".");
     delay(500);
   }
   Serial.println();
 }

 String payload = "{ \"d\" : {\"counter\":";
 payload += millis()/1000;
 payload += "}}";
 
 Serial.print("Sending payload: "); Serial.println(payload);
 
 if (client.publish(topic, (char*) payload.c_str())) {
   Serial.println("Publish ok");
 } else {
   Serial.println("Publish failed");
 }

 delay(3000);
}

void initWiFi() {
 if(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid);
  }else{
    counter=0;
    }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if(counter>=60){ //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
  Serial.println("WiFi connected");
  Serial.println("IP address set: "); 
  Serial.println(WiFi.localIP()); //print LAN IP
}
