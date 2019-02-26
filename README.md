# ESP32-eduroam
# Tested with latest Arduino core and connection is Working!

# This repo offers
* Wifi connection to eduroam powered networks with university login and password
* Supported methods: PEAP + MsCHAPv2 & EAP-TTLS + MsCHAPv2
* In this repository, there are examples for MQTT, HTTP, HTTPS connections to user applications or IoT applications - Thingspeak, IBM, Cayenne, IBM Watson, AzureIoT, Thinx Cloud, etc.

# Format
* ANONYMOUS IDENTITY = anonymous@example.com (for instance: anonymous@tuke.sk)
* IDENTITY = id@example.com (for instance: nickname@tuke.sk)
* PASSWORD = your password

# Usage
* Change your identities (anonymous + visible) 
* Change password
* Upload sketch and enjoy!

# Tested locations (WifiClientSecure example)
|University|Board|Method|Result|
|-------------|-------------| -----|------|
|Technical University in Košice (Slovakia)|ESP32 Devkit v1|PEAP + MsCHAPv2|Working|
|Technical University in Košice (Slovakia)|ESP32 Devmodule v4|PEAP + MsCHAPv2|Working|
|Slovak Technical University in Bratislava (Slovakia)|ESP32 Devkit v1|PEAP + MsCHAPv2|Working|
|University of Antwerp (Belgium)|Lolin32|PEAP + MsCHAPv2|Working|
|UPV Universitat Politècnica de València (Spain)|ESP32 Devmodule v4|PEAP + MsCHAPv2|Working|
|Local Zeroshell powered network|ESP32 Devkit v1|PEAP + MsCHAPv2|*Not working*|
|Hasselt University (Belgium)|xxx|PEAP + MsCHAPv2|Working with fix sketch|
|Universidad de Granada (Spain)|Lolin D32 Pro|PEAP + MsCHAPv2|Working|
|Universidad de Granada (Spain)|Lolin D32|PEAP + MsCHAPv2|Working|
|Universidade Federal de Santa Catarina (Brazil)|xxx|EAP-TTLS + MsCHAPv2|Working|
|University of Central Florida (Orlando, Florida)|ESP32 Built-in OLED – Heltec WiFi Kit 32|PEAP + MsCHAPv2|Working|
|Université de Montpellier (France)|NodeMCU-32S|PEAP + MsCHAPv2|Working|
|University of Edinburgh (Scotland)|ESP-WROOM-32 Devkit|PEAP + MsCHAPv2|Working|

# Sucessful connection (WifiClientSecure example result)
 ![alt text](https://i.nahraj.to/f/24Kc.png)
# Unsucessful connection
 ![alt text](https://camo.githubusercontent.com/87e47d1b27f4e8ace87423e40e8edbce7983bafa/68747470733a2f2f692e6e616872616a2e746f2f662f323435572e504e47)

# Donate for more examples
* https://www.paypal.me/chlebovec

# Contact
* martinius96@gmail.com

# Tested boards
* ESP32 Devkit v1
* ESP32 Devmodule v4
* Lolin D32
* Lolin D32 Pro
* NodeMCU-32S
