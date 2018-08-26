//Sketch provided by: T. Vandenryt
#include "esp_wpa2.h"
#include <WiFi.h>
String line; //variable for response
const char* ssid = "eduroam"; // Eduroam SSID
const char* host = "arduino.php5.sk"; //external server domain
#define EAP_IDENTITY "login@university.domain" //identity@youruniversity.domain
#define EAP_PASSWORD "password" //your Eduroam password
void setup() {
	byte error = 0;
	Serial.begin(115200);
	delay(10);
	Serial.println("Connecting to: ");
	Serial.println(ssid);
	WiFi.disconnect(true);  //disconnect from wifi to set new wifi connection
	WiFi.mode(WIFI_STA);
	error += esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
	error += esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
	//error += esp_wifi_sta_wpa2_ent_set_new_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));  //First time running, i think i needed to set this, although i received an error from the server, claiming the password was not set?
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
void loop() {
    delay(5000);
 if (WiFi.status() != WL_CONNECTED) { //if we lost connection, retry
        WiFi.begin(ssid);
        delay(500);        
    }
    Serial.print("Connecting to website: ");
    Serial.println(host);
    WiFiClient client;
    if (!client.connect(host, 80)) { // HTTP connection on port 80
        Serial.println("Connection lost! - Failed response");
    }
    String url = "/rele/rele1.txt"; //read .txt file    
    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println("Client timed out! - retry");
            client.stop();
        }
    }
    while(client.available()) {
         line = client.readStringUntil('\n');
        Serial.println(line);  
    }
    Serial.println();
    Serial.println("End connection");
}
