# THiNX Lib (ESP32)

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/8dded023f3d14a69b3c38c9f5fd66a40)](https://www.codacy.com/app/suculent/thinx-lib-esp32-arduinoc?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=suculent/thinx-lib-esp32-arduinoc&amp;utm_campaign=Badge_Grade)

An Arduino/ESP32 library to wrap client for OTA updates and RTM (Remote Things Management) based on THiNX platform.

# What's New

* In the process of merging ESP8266 and ESP32 implementations to single library. One of these will soon deprecate.
* HTTPS CA validation
* Improving method naming. `setStatus` is now deprecated in favour of `setDashboardStatus`.

See changelog at the end for rest.

# Usage

> WARNING! Arduino Library Manager is supported through the thinx.yml file, however this library already contains all required dependencies, because your local Arduino Libraries are not located on the CI server.

> Copy dependencies from the `lib` folder to your Arduino libraries to compile locally.

> Use the thinx.yml to add more dependencies for THiNX CI, but be aware that those will be merged with libraries from lib folder next to .ino file.

## Include

```c
#include <THiNX32Lib.h>
```

## Definition

### THiNX Library

The singleton class started by library should not require any additional parameters except for optional API Key.
Connects to WiFI and reports to main THiNX server; otherwise starts WiFI in AP mode (AP-THiNX with password PASSWORD by default)
and awaits optionally new API Key (security hole? FIXME: In case the API Key is saved (and validated) do not allow change from AP mode!!!).

* if not defined, defaults to thinx.cloud platform
* TODO: either your local `thinx-device-api` instance or [currently non-existent at the time of this writing] `thinx-secure-gateway` which does not exist now, but is planned to provide HTTP to HTTPS bridging from local network to

```c
#include "Arduino.h"
#include <THiNX32Lib.h>

THiNX thx;

void setup() {

  Serial.begin(115200);

#ifdef __DEBUG__
  while (!Serial); // wait for debug console connection
  WiFi.begin("THiNX-IoT+", "<enter-your-ssid-password>");
#endif

   // Enter API Key and Owner ID
  thx = THiNX("71679ca646c63d234e957e37e4f4069bf4eed14afca4569a0c74abf503076732", "cedc16bb6bb06daaa3ff6d30666d91aacd6e3efbf9abbc151b4dcade59af7c12");
  thx.setFinalizeCallback(finalizeCallback);
  thx.setPushConfigCallback(pushConfigCallback);
}

/* Loop must call the thx.loop() in order to pickup MQTT messages and advance the state machine. */
void loop()
{
  thx.loop();
}
```

### Configurations Priority

0. When using WiFiManager, the WiFi SSID/PASS is stored in device's Flash (only on successful connection).

1. THiNXLib is built with null default values (mostly).

2. THiNXLib is configured from thinx.h file, which is overwritten by the THiNX CI for each build.

3. As a user, you are allowed to initialize THiNX() with API Key and Owner ID entered into the code sketch or thinx.h file, so you can run it against backend while building locally (without THiNX CI).

4. Additional data are loaded from EEPROM/SPIFFS, where saved Owner ID takes precedence before user value to support OTA device migration.

5. On successful checkin, incoming data incl. UDID (unique device identifier) and Owner ID is stored to EEPROM or SPIFFS for further use after reboot.

6. Configuration Push can be used to inject custom Environment Variables over the network, without need to have them stored anywhere in the code on the device (e.g. WiFi credentials)


### Finalize callback

When THiNX connects safely to network and connection is working, you'll get this callback.

```
/* Called after library gets wifi_connected and registered */
void finalizeCallback () {
  Serial.println("*INO: Finalize callback called.");
  ESP.deepSleep(3e9);
}
```

### Environment Variables

```
/* Example of using Environment variables */
void pushConfigCallback (String config) {

  // Convert incoming JSON string to Object
  DynamicJsonBuffer jsonBuffer(512);
  JsonObject& root = jsonBuffer.parseObject(config.c_str());
  JsonObject& configuration = root["configuration"];

  if ( !configuration.success() ) {
    Serial.println(F("Failed parsing configuration."));
  } else {

    // Parse and apply your Environment vars
    const char *ssid = configuration["THINX_ENV_SSID"];
    const char *pass = configuration["THINX_ENV_PASS"];

    // password may be empty string
    if ((strlen(ssid) > 2) && (strlen(pass) > 0)) {
      WiFi.disconnect();
      WiFi.begin(ssid, pass);
      long timeout = millis() + 20000;
      Serial.println("Attempting WiFi migration...");
      while (WiFi.status() != WL_CONNECTED) {
        yield();
        if (millis() > timeout) break;
      }
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi migration failed."); // TODO: Notify using publish() to device status channel
      } else {
        Serial.println("WiFi migration successful."); // TODO: Notify using publish() to device status channel
      }
    }
  }
}
```

### Location Support

You can update your device's location aquired by WiFi library or GPS module using `thx.setLocation(double lat, double lon`) from version 2.0.103 (rev88).
Device will be forced to checked in when you change those values.
