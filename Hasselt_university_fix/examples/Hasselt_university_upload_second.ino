/*|----------------------------------------------------------|*/
/*|WORKING EXAMPLE FOR HASSELT UNIVERSITY                    |*/
/*|Only connection to eduroam network                        |*/
/*|https://www.uhasselt.be/eduroam                           |*/
/*|SKETCH PROVIDED BY: Thijs Vandenryt                       |*/
/*|----------------------------------------------------------|*/
#include <WiFi.h> //Wifi library
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#define EAP_IDENTITY "id@student.uhasselt.be" //for student
//#define EAP_IDENTITY "id@uhasselt.be" //for researchers, scientists, teachers
#define EAP_PASSWORD "password"
const char* ssid = "eduroam"; // Eduroam SSID
void setup() {
	byte error = 0;
	Serial.begin(115200);
	delay(10);
	Serial.println("Connecting to: ");
	Serial.println(ssid);
	WiFi.disconnect(true);  //disconnect from wifi to set new wifi connection
	error += esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
	error += esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
	error += esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); //Following times, it ran fine with just this line (connects very fast).
	if (error != 0) {
		Serial.println("Error setting WPA properties.");
	}
	WiFi.enableSTA(true);
	
	esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
	if (esp_wifi_sta_wpa2_ent_enable(&config) != ESP_OK) {
		Serial.println("WPA2 Settings Not OK");
	}
	
	WiFi.begin(ssid); //connect to Eduroam function
	WiFi.setHostname("RandomHostname"); //set Hostname for your device - not neccesary
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address set: ");
	Serial.println(WiFi.localIP()); //print LAN IP
}
void loop(){
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid);
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}
