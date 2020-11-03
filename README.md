# ESP32-eduroam
**Solution for unable to authentication via id@domain using domain\\username format by Jim @gojimmypi**
* https://github.com/martinius96/ESP32-eduroam/issues/4#issue-632608789
# Buy me a coffee for more examples :)
* https://www.paypal.me/chlebovec
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
|University|Board|Method|Realm|Result|
|-------------|-------------| -----|------|------|
|Technical University in Košice (Slovakia)|ESP32 Devkit v1|PEAP + MsCHAPv2|tuke.sk|Working|
|Technical University in Košice (Slovakia)|ESP32 DevkitC v4|PEAP + MsCHAPv2|tuke.sk|Working|
|Slovak Technical University in Bratislava (Slovakia)|ESP32 Devkit v1|PEAP + MsCHAPv2|stuba.sk|Working|
|University of Antwerp (Belgium)|Lolin32|PEAP + MsCHAPv2|ad.ua.ac.be|Working|
|UPV Universitat Politècnica de València (Spain)|ESP32 Devmodule v4|PEAP + MsCHAPv2|alumno.upv.es|Working|
|Local Zeroshell powered network|ESP32 Devkit v1|PEAP + MsCHAPv2|-|*Not working*|
|Hasselt University (Belgium)|xxx|PEAP + MsCHAPv2|uhasselt.be|**Working with fix sketch**|
|Universidad de Granada (Spain)|Lolin D32 Pro|PEAP + MsCHAPv2|ugr.es|Working|
|Universidade Federal de Santa Catarina (Brazil)|xxx|EAP-TTLS + MsCHAPv2|idufsc.ufsc.br|Working|
|University of Central Florida (Orlando, Florida)|ESP32 Built-in OLED – Heltec WiFi Kit 32|PEAP + MsCHAPv2|ucf.edu|Working|
|Université de Montpellier (France)|NodeMCU-32S|PEAP + MsCHAPv2|etu.umontpellier.fr|Working|
|University of Edinburgh (Scotland)|ESP-WROOM-32 Devkit|PEAP + MsCHAPv2|ed.ac.uk|Working|
|University of Debrecen (Hungary)|NodeMCU-32S|PEAP + MsCHAPv2|unideb.hu|Working|
|University of Applied Sciences Bielefeld (Germany)|ESP32 Devkit v1|PEAP + MsCHAPv2|fh-bielefeld.de|Working|
|University of Liege (Belgium)|ESP32 DevkitC v4|PEAP + MsCHAPv2|uliege.be|Working|

# Sucessful connection (WifiClientSecure example result)
 ![alt text](https://i.nahraj.to/f/24Kc.png)
# Unsucessful connection
 ![alt text](https://camo.githubusercontent.com/87e47d1b27f4e8ace87423e40e8edbce7983bafa/68747470733a2f2f692e6e616872616a2e746f2f662f323435572e504e47)

# Contact
* martinius96@gmail.com

# Tested boards
* ESP32 Devkit v1
* ESP32 Devmodule v4
* Lolin D32
* Lolin D32 Pro
* NodeMCU-32S
