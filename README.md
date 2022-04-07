# ESP32-eduroam
**Extended program implementation from @debsahu that is using CA cert:**
* Visit his Github repo: https://github.com/debsahu/Esp32_EduWiFi
* He have also YT video tutorial: https://www.youtube.com/watch?v=bABHeMea-P0
* All questions related to his program implementation address to him please

**Solution for unable to authentication via id@domain using domain\\username AD format by Jim @gojimmypi**
* https://github.com/martinius96/ESP32-eduroam/issues/4#issue-632608789
# Tested implementations were with Arduino Core 1.0.4 for ESP32!
* **Latest examples from 2021 and 2022 I haven't tested, I don't have more access to eduroam network**

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

# Tested locations (WifiClientSecure example)
|University|Board|Method|Realm|Result|Year|
|-------------|-------------| -----|------|------|------|
|Technical University in Košice (Slovakia)|ESP32 Devkit v1|PEAP + MsCHAPv2|tuke.sk|Working|2018|
|Technical University in Košice (Slovakia)|ESP32 DevkitC v4|PEAP + MsCHAPv2|tuke.sk|Working|2018|
|Slovak Technical University in Bratislava (Slovakia)|ESP32 Devkit v1|PEAP + MsCHAPv2|stuba.sk|Working|2018|
|University of Antwerp (Belgium)|Lolin32|PEAP + MsCHAPv2|ad.ua.ac.be|Working|2018|
|UPV Universitat Politècnica de València (Spain)|ESP32 Devmodule v4|PEAP + MsCHAPv2|alumno.upv.es|Working|2018|
|Local Zeroshell powered network|ESP32 Devkit v1|PEAP + MsCHAPv2|-|*Not working*|2018|
|Hasselt University (Belgium)|xxx|PEAP + MsCHAPv2|uhasselt.be|**Working with fix sketch**|2018|
|Universidad de Granada (Spain)|Lolin D32 Pro|PEAP + MsCHAPv2|ugr.es|Working|2018|
|Universidade Federal de Santa Catarina (Brazil)|xxx|EAP-TTLS + MsCHAPv2|idufsc.ufsc.br|Working|2018|
|University of Central Florida (Orlando, Florida)|ESP32 Built-in OLED – Heltec WiFi Kit 32|PEAP + MsCHAPv2|ucf.edu|Working|2018|
|Université de Montpellier (France)|NodeMCU-32S|PEAP + MsCHAPv2|etu.umontpellier.fr|Working|2018|
|University of Edinburgh (Scotland)|ESP-WROOM-32 Devkit|PEAP + MsCHAPv2|ed.ac.uk|Working|2018|
|University of Debrecen (Hungary)|NodeMCU-32S|PEAP + MsCHAPv2|unideb.hu|Working|2018|
|University of Applied Sciences Bielefeld (Germany)|ESP32 Devkit v1|PEAP + MsCHAPv2|fh-bielefeld.de|Working|2018|
|University of Liege (Belgium)|ESP32 DevkitC v4|PEAP + MsCHAPv2|uliege.be|Working|2018|

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
