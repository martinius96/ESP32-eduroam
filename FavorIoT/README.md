* https://github.com/LintangWisesa/ESP32_Favoriot - Sketch and informations from this repo
#

![favoriot](https://www.favoriot.com/wp-content/uploads/2017/03/favoriot-1.png)

# ESP32 & FavorIoT

### **[Favoriot](https://www.favoriot.com/home/iotplatform)** offers an IOT platform specifically designed for any Internet of Things (IoT) projects. The platform is developed to support the integration of data from sensors and actuators on the internet. Collecting and storing data from IOT devices become much easier. Moreover, the platform also helps developers in building vertical applications without worry about the hosting. 

### This article will present a step by step example how to upload analog value of a potentiometer from ESP32 development board to Favoriot IoT platform. Watch video below (or [click here](https://www.youtube.com/watch?v=-iJud3bU3HY)) for its demonstration, then follow these instructions to start building your project with ESP32 & Favoriot.

#

[![Video ESP32 & Favoriot](https://img.youtube.com/vi/-iJud3bU3HY/0.jpg)](https://www.youtube.com/watch?v=-iJud3bU3HY)

#

### **1. What You Need** :gift:
To build this project, you need the following items:
- 1 ESP32 board (I'm using NodeMCU 32S)
- 1 breadboard
- 1 potentiometer
- some jumper wires
- Arduino IDE ([download here](https://www.arduino.cc/en/Main/Software))

#

### **2. Schematics** :wrench::hammer:

Gather your parts then follow the schematics below.

![ESP32 Favoriot](https://raw.githubusercontent.com/LintangWisesa/ESP32_Favoriot/master/ESP32_Favoriot.png)

#

### **3. Favoriot Setup** :purple_heart:

- __Device Developer ID__

  Signup & login to [Favoriot](https://platform.favoriot.com/login). If you are registered in Favoriot, you have a default device already created for you. Go to right platform on https://platform.favoriot.com and see the device panel to see the devices that are present. Basically you need the ```device_developer_id``` that might be something like ```defaultDevice@myusername```. But if you want, you can create a new device and use it in this example.

- __API Key__

  Now, go to your Favoriot *Account Setting* which is available on the top right corner in the dropdown and check your API key. You need this API key to complete this example. It's a big alphanumeric token like:
  ```c++
  '986673a6377ef1fde2357ebdcb0da582lintang150b00cabcd5a0d83045425407ab4'
  ```

#

### **4. Arduino Sketch** :clipboard:

First you need to install ESP32 platform on Arduino IDE, follow these instructions: [click here](https://github.com/espressif/arduino-esp32). Copy sketch below to your Arduino IDE. Make sure you have chosen the right option for **_Board_** and **_Port_** under **_Tools_** menu, then upload to your Arduino board.

```c++
#include <SPI.h>
#include <WiFi.h>
#include "esp_wpa2.h"

const char* ssid = "eduroam";
#define EAP_ANONYMOUS_IDENTITY "anonymous@example.com"
#define EAP_IDENTITY "nickname@example.com"
#define EAP_PASSWORD "password"

const String yourDevice = "deviceDefault@your_Username"; // change it!
int status = WL_IDLE_STATUS;
char server[] = "api.favoriot.com";
WiFiClient client;

void setup() {
  Serial.begin(115200);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
  WiFi.begin(ssid); //connect to wifi
  while((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print("...");
  }
  Serial.println(WiFi.status());
  Serial.println("Connected!");
  Serial.println("");
}

void loop() {
  String myData = String(analogRead(A0));
  String json = "{\"device_developer_id\":\""+yourDevice+"\",\"data\":{\"Data\":\""+myData+"\"}}";
  Serial.println(json);
  if (client.connect(server, 80)) {
    client.println("POST /v1/streams HTTP/1.1");
    client.println("Host: api.favoriot.com");
    client.println(F("apikey: your_API_key"));  // change it!
    client.println("Content-Type: application/json");
    client.println("cache-control: no-cache");
    client.print("Content-Length: ");
    int thisLength = json.length();
    client.println(thisLength);
    client.println("Connection: close");
    client.println();
    client.println(json);
  }
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  if (!client.connected()) {
    client.stop();
  }
  delay(5000);
}
```

To upload multiple values, use this:

```c++
String json = "{\"device_developer_id\":\"deviceDefault@User\",
\"data\":{ \"Sensor_1\":\""+sensor1+"\",
           \"Sensor_2\":\""+sensor2+"\",
           \"Sensor_3\":\""+sensor3+"\" }
}";
```

#

### **5. Have Fun!** :sunglasses:
Now you can monitor your potentiometer value every 5 seconds on Favoriot dashboard. For more information about Favoriot, read its full documentation: [click here](https://platform.favoriot.com/tutorial/).

__*My other tutorials on Favoriot:*__
- [ESP8266 & Favoriot](https://github.com/LintangWisesa/ESP8266_Favoriot)
- [ESP32 & Favoriot](https://github.com/LintangWisesa/ESP32_Favoriot)
- [RobotDyn Uno+WiFi & Favoriot](https://github.com/LintangWisesa/RobotDyn_UnoWiFi_Favoriot)
- [Arduino MKR1000 & Favoriot](https://github.com/LintangWisesa/Arduino_MKR1000_Favoriot)
