# ESP32-Eduroam
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
* Technical University in Košice (Slovakia) - Working
* Slovak Technical University in Bratislava (Slovakia) - Working
* University of Antwerp (Belgium) - Working
* UPV Universitat Politècnica de València (Spain) - Working
* Zeroshell at home - Not working, error in handshake - timeout
* Hasselt University (Belgium) - Working with 2018-fix sketch - Thanks to T. Vandenryt for trying different ways to connect to Eduroam network

# Sucessful connection
 ![alt text](https://i.nahraj.to/f/24Kc.png)
# Unsucessful connection
 ![alt text](https://camo.githubusercontent.com/87e47d1b27f4e8ace87423e40e8edbce7983bafa/68747470733a2f2f692e6e616872616a2e746f2f662f323435572e504e47)

# Contact
* martinius96@gmail.com

# Boards
* Tested Under ESP32 Devkit v1
