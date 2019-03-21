#include <WiFi.h>       // WiFi library
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks

char EAP_ANONYMOUS_IDENTITY[] = "anonymous@example.com";
char EAP_IDENTITY[] = "nickname@example.com";
char EAP_PASSWORD[] = "EDUROAM_PASSWORD";
const char* ssid = "eduroam"; // Eduroam SSID

const char domain[]   = "---domain---";  // domain.tmep.cz
const char guid[]     = "---guid---"; // mojemereni
WiFiClient client;
void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print(F("Connecting to ")); Serial.println(ssid);
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
  WiFi.begin(ssid); //connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP()); //print LAN IP
}

void loop() {
  char host[50];            // Joining two chars is little bit difficult. Make new array, 50 bytes long
  strcpy(host, domain);     // Copy /domain/ in to the /host/
  strcat(host, ".tmep.cz"); // Add ".tmep.cz" at the end of the /host/. /host/ is now "/domain/.tmep.cz"
  Serial.print(F("Connecting to ")); Serial.println(host);
  if (client.connect(host, 80)) {
    Serial.println(F("Client connected to TMEP"));
    // Make an url. We need: /?guid=t
    String url = "/?";
    url += guid;
    url += "=";
    url += "123456"; //your value
    Serial.print(F("Requesting URL: "));
    Serial.println(url);

    // Make a HTTP GET request.
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: ESP32\r\n" + "Connection: close\r\n\r\n");
  } else {
    Serial.print(F("Connection failed"));
  }
  client.stop();
  Serial.println();
  // Wait for another round
  delay(60000);
}
