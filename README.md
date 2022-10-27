# ESP32-eduroam

# This repo offers
* Wifi connection to eduroam powered networks with university login, anonymous identity and password
* Anonymous identity (visible as plaintext) is used for redirect of connection from other university to your local RADIUS server if you dont want to provide your username
* Supported methods: PEAP + MsCHAPv2 & EAP-TTLS + MsCHAPv2

# Opened ports at eduroam network
* 50, 51, 1194, 41, 4500, 1723 - **In & Out**
* 0, 500, 1000, 47, 21, 22, 80, 443, 3128, 8080, 465, 579, 143, 993, 110, 995 - **Out**

# Format
* ANONYMOUS IDENTITY = anonymous@example.com (for instance: anonymous@tuke.sk)
* IDENTITY = id@example.com (for instance: nickname@tuke.sk)
* PASSWORD = your password

# Usage
* Change your identities (anonymous + username) 
* Change password
* Link your local Root CA certificate (should find at your local eduroam admin page)
* If using EAP-TTLS with client certificates, you need to link them too and call it in WiFi.begin(), see 2022 example
* Upload sketch 

# Tested locations 2022-2023
* Without certificate connection should work if there is in RADIUS server configuration set **exception for old and special devices**
|University|Reported by|Result|Realm|Note|
|-------------|-------------|-----|------|------|
|Karlsruhe University of applied science (Germany)|@HeinzAlberto|Working|@h-ka.de|Without certificate|
|University of Tuscia (Italy)|@paluigi|Working|@students.unitus.it / @unitus.it|Without certificate|


# Successful connection example
 ![alt text](https://i.nahraj.to/f/24Kc.png)
# Unsucessful connection example
 ![alt text](https://camo.githubusercontent.com/87e47d1b27f4e8ace87423e40e8edbce7983bafa/68747470733a2f2f692e6e616872616a2e746f2f662f323435572e504e47)

# Contact
* martinius96@gmail.com

# Tested boards
* ESP32 Devkit v1
* ESP32 Devmodule v4
* Lolin D32
* Lolin D32 Pro
* NodeMCU-32S
