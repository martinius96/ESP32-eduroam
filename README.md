# ESP32-Eduroam
* Eduroam wifi connection with university login identity
* Working under RADIUS, not under TACACS.

# Format
* IDENTITY = nickname@organization.com (for instance: nickname@tuke.sk)
* PASSWORD = your password :-)

# Usage
* Change IDENTITY or use anonymous identity --> anonymous@university.domain
* Change password
* Upload sketch and enjoy!

# Tested locations
* Technical University in Košice (Slovakia) - Working
* Slovak Technical University in Bratislava (Slovakia) - Working
* University of Antwerp (Belgium) - Working
* Zeroshell at home - Not working, error in handshake - timeout

# Testing sketch was user on Network with methodds
* PEAP on 1st step
* MsCHAPv2 on 2nd step

# Contact
* martinius96@gmail.com

# Boards
* Tested Under ESP32 Devkit v1
