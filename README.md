# ESP32-Eduroam
# Sketch is tested under core from June 2018. Newer versions have problem with connection to Enterprise networks.
* Eduroam wifi connection with university login identity
* Working under Eduroam networks under FreeRadius service for 802.1x
* Eduroam is using: PEAP + MsCHAPv2
* **Inform me after you use that .ino file in your university if it is working! Email below**
* Eduroam example with HTTP connection is now available in official repository as example named WifiClientEnterprise: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientEnterprise/WiFiClientEnterprise.ino

# Format
* IDENTITY = nickname@university.domain (for instance: nickname@tuke.sk)
* PASSWORD = your password :-)

# Usage
* Change IDENTITY or use anonymous identity --> anonymous@university.domain
* Change password
* Upload sketch and enjoy!

# Tested locations
|University|Board|Method|Result|
|-------------|-------------| -----|------|
|Technical University in Košice (Slovakia)|ESP32 Devkit v1|PEAP + MsCHAPv2|Working|
|Technical University in Košice (Slovakia)|ESP32 Devmodule v4|PEAP + MsCHAPv2|Working on 6th attempt in loop|
|Slovak Technical University in Bratislava (Slovakia)|ESP32 Devkit v1|PEAP + MsCHAPv2|Working|
|University of Antwerp (Belgium)|Lolin32|PEAP + MsCHAPv2|Working|
|UPV Universitat Politècnica de València (Spain)|ESP32 Devmodule v4|PEAP + MsCHAPv2|Working|
|Local Zeroshell powered network|ESP32 Devkit v1|PEAP + MsCHAPv2|*Not working*|
|Hasselt University (Belgium)|xxx|PEAP + MsCHAPv2|Working with fix sketch|
|Universidad de Granada (Spain)|Lolin D32 Pro|PEAP + MsCHAPv2|Working|
|Universidad de Granada (Spain)|Lolin D32|PEAP + MsCHAPv2|Working|
|Universidade Federal de Santa Catarina (Brazil)|xxx|EAP-TTLS + MsCHAPv2|Working|


# Sucessful connection
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
