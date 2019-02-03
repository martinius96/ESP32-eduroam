/*
The MIT License(MIT)

Cayenne Arduino Client Library
Copyright (c) 2016 myDevices

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files(the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _CAYENNEMQTTWIFICLIENT_h
#define _CAYENNEMQTTWIFICLIENT_h

#include "CayenneArduinoMQTTClient.h"

#ifndef WRITE_CHUNK_SIZE
#define WRITE_CHUNK_SIZE 0 // The chunk size to use when sending data, 0 means data will not be sent in chunks.
#endif // !WRITE_CHUNK_SIZE


class CayenneMQTTWiFiClient : public CayenneArduinoMQTTClient
{
public:
	/**
	* Begins Cayenne session and in the process establishes a WIFI connection with the supplied ssid and WIFI password
	* @param username Cayenne username
	* @param password Cayenne password
	* @param clientID Cayennne client ID
	* @param ssid WiFi network id
	* @param wifiPassword WiFi network password
	*/
	void begin(const char* username, const char* password, const char* clientID, const char* ssid, const char* EAP_ANONYMOUS_IDENTITY, const char* EAP_IDENTITY, const char* EAP_PASSWORD)
	{
		WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
	esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY)); 
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
}
		CAYENNE_LOG("Connected to WiFi");

		IPAddress local_ip = WiFi.localIP();
		CAYENNE_LOG("IP: %d.%d.%d.%d", local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
		CayenneArduinoMQTTClient::begin(_wifiClient, username, password, clientID, WRITE_CHUNK_SIZE);
	}
	/**
	* Begins Cayenne session, assumes that the WIFI is already up and running. 
	* @param username Cayenne username
	* @param password Cayenne password
	* @param clientID Cayennne client ID
	*/
	void begin(const char* username, const char* password, const char* clientID)
	{
		if (WiFi.status() != WL_CONNECTED) {
			CAYENNE_LOG("CayenneMQTTWiFiClient.begin called without WIFI being connected. Either use begin (..., ssid, wifipassword) to establish the connection here. Or setup the WIFI connection manually before calling this variant of the begin function");
		}
		IPAddress local_ip = WiFi.localIP();
		CAYENNE_LOG("IP: %d.%d.%d.%d", local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
		CayenneArduinoMQTTClient::begin(_wifiClient, username, password, clientID, WRITE_CHUNK_SIZE);
	}


private:
	WiFiClient _wifiClient;
};

CayenneMQTTWiFiClient Cayenne;

#endif
