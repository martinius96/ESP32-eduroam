#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <HTTPClient.h>
HTTPClient ask;
// TODO: user config
//-------- Customise these values -----------
const char* ssid = "eduroam";
#define EAP_ANONYMOUS_IDENTITY "anonymous@example.com"
#define EAP_IDENTITY "nickname@example.com"
#define EAP_PASSWORD "password"
String apiKeyIn = "............."; // API Key
//-------- END Customise these values -----------
const char* host = "asksensors.com";  // host
const int httpPort = 80;      // port
int counter = 0;  
void setup(){
  
  // open serial
  Serial.begin(9600);
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Connect ESP32 to AskSensors.");
  Serial.println("Wait for WiFi... ");
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY)); 
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
  WiFi.begin(ssid); //connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if(counter>=60){ //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
  // connected
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop(){
  if (WiFi.status() == WL_CONNECTED) { //if we are connected to Eduroam network
    counter = 0; //reset counter
    Serial.println("Wifi is still connected with IP: "); 
    Serial.println(WiFi.localIP());   //inform user about his IP address
  }else if (WiFi.status() != WL_CONNECTED) { //if we lost connection, retry
    WiFi.begin(ssid);      
  }
  while (WiFi.status() != WL_CONNECTED) { //during lost connection, print dots
    delay(500);
    Serial.print(".");
    counter++;
    if(counter>=60){ //30 seconds timeout - reset board
    ESP.restart();
    }
  }
  // Use WiFiClient class to create TCP connections
  WiFiClient client;


  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }else {

    // Create a URL for the request
  String url = "https://asksensors.com/api.asksensors/write/";
  url += apiKeyIn;
  url += "?module1=";
  url += random(10, 100);
  
  Serial.print("********** requesting URL: ");
  Serial.println(url);
 
   // ask.begin("https://asksensors.com/api.asksensors/write/ONKMHCC38YT74K5NGT8JC1GF8B1O9QKT?module1=100"); //Specify the URL
   ask.begin(url); //Specify the URL
  
    //Check for the returning code
    int httpCode = ask.GET();          
 
    if (httpCode > 0) { 
 
        String payload = ask.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      } else {
      Serial.println("Error on HTTP request");
    }
 
    ask.end(); //End 
    Serial.println("********** End ");
    Serial.println("*****************************************************");

  }

  client.stop();  // stop client
  
  delay(20000);    // delay
}
