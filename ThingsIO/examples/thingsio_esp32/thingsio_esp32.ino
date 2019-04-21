/*|----------------------------------------------------------|*/
/*|ThingsIO enterprise connection Example ESP32              |*/
/*|EMAIL: martinius96@gmail.com                              |*/
/*|WEBSITE: https://arduino.php5.sk                          |*/
/*|----------------------------------------------------------|*/
#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
int count = 0, i, m, j, k;
int t, t1, t2, t3;
int outputpin = A0; //ds18b20
//////////////////////////////////////// ALL DECLARATIONS for WiFi //////////////////////////////
char EAP_ANONYMOUS_IDENTITY[] = "anonymous@example.com";
char EAP_IDENTITY[] = "nickname@example.com";
char EAP_PASSWORD[] = "EDUROAM_PASSWORD";
const char* ssid = "eduroam"; // Eduroam SSID
//////////////////////////////////////// ALL DECLARATIONS for CLOUD //////////////////////////////
const char* host = "api.thingsio.ai";                                 // OR host = devapi2.thethingscloud.com
const char* post_url = "/devices/deviceData";       // OR /api/v2/thingscloud2/_table/data_ac
const char* time_server = "baas.thethingscloud.com";             //this is to convert timestamp
const int httpPort = 80;

char timestamp[10];

// Use WiFiClient class to create TCP connections
WiFiClient client;

/////////////////////////////////////// TIMESTAMP CALCULATION function///////////////////////////////////////
int GiveMeTimestamp()
{
  unsigned long timeout = millis();

  while (client.available() == 0)
  {
    if (millis() - timeout > 50000)
    {
      client.stop();
      return 0;
    }
  }

  while (client.available())
  {
    String line = client.readStringUntil('\r');                    //indexOf() is a funtion to search for smthng , it returns -1 if not found
    int pos = line.indexOf("\"timestamp\"");                       //search for "\"timestamp\"" from beginning of response got and copy all data after that , it'll be your timestamp
    if (pos >= 0)
    {
      int j = 0;
      for (j = 0; j < 10; j++)
      {
        timestamp[j] = line[pos + 12 + j];
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);
  delay(10);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
  WiFi.begin(ssid); //connect to wifi network under Enterprise 802.1x
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}


void loop()
{
  int analogValue = analogRead(outputpin);
  {


    /////////////////////////////////////// SEND THE QUERY AND RECEIVE THE RESPONSE///////////////////////



    t1 = (analogValue / 1024.0) * 5000;
    Serial.print("temp: ");
    Serial.println(t1);

    t2 = (t1 / 10);
    Serial.print("celc: ");
    Serial.println(t2);

    t3 = ((t2 * 9) / 5 + 32);
    Serial.print("fhrn: ");
    Serial.println(t3);        //break;



    Serial.print("connecting to ");
    Serial.println(host);                          //defined upside :- host = devapi2.thethingscloud.com or 139.59.26.117



    ///////////////////////////////////// TIMESTAMP CODE SNIPPET /////////////////////////


    Serial.println("inside get timestamp\n");
    if (!client.connect(time_server, httpPort))
    {
      return;                                                        //*-*-*-*-*-*-*-*-*-*
    }

    client.println("GET /api/timestamp HTTP/1.1");                            //Whats this part doing, i didnt get
    client.println("Host: baas.thethingscloud.com");
    client.println("Cache-Control: no-cache");
    client.println("Postman-Token: ea3c18c6-09ba-d049-ccf3-369a22a284b8");
    client.println();

    GiveMeTimestamp();                        //it'll call the function which will get the timestamp response from the server
    Serial.println("timestamp receieved");
    Serial.println(timestamp);
    ///////////////////////////////////////////////////////////////////////////////

    // Use WiFiClient class to create TCP connections
    if (!client.connect(host, httpPort))            //host = devapi2.thethingscloud.com , port = 80
    {
      Serial.println("connection failed");                  // *-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-
      return;
    }

    client.print("Content-Length: ");
    client.println();

    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 50000)
      {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    Serial.println("***********************************************");
    Serial.println();
    Serial.println("closing connection");

    Serial.println("inside ThingsCloudPost");
    int timestamp1 = 0;
    timestamp1 = 1542800778;

    String PostValue = "{\"device_id\": 209251, \"slave_id\": 2";
    PostValue = PostValue + ",\"dts\":" + timestamp;
    PostValue = PostValue + ",\"data\":{\"celc\":" + t2 + ",\"fahr\":" + t3 + "}" + "}";


    Serial.println(PostValue);

    //////////////////////////////// try to connect to server again and POST the data on the cloud //////////////////////////
    if (!client.connect(host, httpPort))
    {
      return;
    }

    client.print("POST ");
    client.print(post_url);               /// POST api/device-datas         HTTP/1.1     Host: baas.thethingscloud.com      Content-Type: application/json        Cache-Control: no-cache
    client.println(" HTTP/1.1");           //Authorization:          Content-Length:
    client.print("Host: ");
    client.println(host);
    client.println("Content-Type: application/json");
    client.println("Cache-Control: no-cache");
    client.print("Authorization: Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.IjViMTI2OGIzM2FhNTNiMjBlNWFjOTEzOCI.I1moNBMEO3zKCrtY-nW7z-Zmj2fzyOsOT5XgEbxmEW8");
    client.println();
    client.print("Content-Length: ");
    client.println(PostValue.length());
    client.println();
    client.println(PostValue);

    //////////////////////////////////POSTING the data on to the cloud is done and now get the response form cloud server//////////////////
    while (client.available() == 0)
    {
      if (millis() - timeout > 50000)
      {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    Serial.println("Response From Server");
    while (client.available())
    {
      String line2 = client.readStringUntil('\r');
      Serial.print(line2);

    }

    Serial.println("//////////////////////    THE END     /////////////////////");
    delay(3000);
  }
}
