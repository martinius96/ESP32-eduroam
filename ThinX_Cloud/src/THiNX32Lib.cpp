extern "C" {
  #include "thinx.h"
  #include <time.h>
  #include <stdlib.h>
}

#include "THiNX32Lib.h"

#ifndef UNIT_TEST // IMPORTANT LINE FOR UNIT-TESTING!

#ifndef THINX_FIRMWARE_VERSION_SHORT
#define THINX_FIRMWARE_VERSION_SHORT VERSION
#endif

#ifndef THINX_COMMIT_ID
#define THINX_COMMIT_ID "0"
#endif

char* THiNX::thinx_api_key;
char* THiNX::thinx_owner_key;

const char THiNX::time_format[] = "%T";
const char THiNX::date_format[] = "%Y-%M-%d";

#ifdef __USE_WIFI_MANAGER__
char THiNX::thx_api_key[65] = {0};
char THiNX::thx_owner_key[65] = {0};
int THiNX::should_save_config = 0;

#include "thinx_root_ca.h"

WiFiManagerParameter * THiNX::api_key_param;
WiFiManagerParameter * THiNX::owner_param;

void THiNX::saveConfigCallback() {
  Serial.println(F("* TH: WiFiManager's saveConfigCallback called. Counfiguration should be saved now!"));
  should_save_config = true;
  strcpy(thx_api_key, api_key_param->getValue());
  strcpy(thx_owner_key, owner_param->getValue());
}
#endif

double THiNX::latitude = 0.0;
double THiNX::longitude = 0.0;
String THiNX::statusString = "Registered";
String THiNX::accessPointName = "THiNX-AP";
String THiNX::accessPointPassword = "PASSWORD";

/* Constructor */

THiNX::THiNX() {

}

/* Designated Initializers */

THiNX::THiNX(const char * __apikey) {

  THiNX(__apikey, "");
}

THiNX::THiNX(const char * __apikey, const char * __owner_id) {

  thinx_phase = INIT;

  #ifdef __USE_WIFI_MANAGER__
  should_save_config = false;
  WiFiManager wifiManager;
  api_key_param = new WiFiManagerParameter("apikey", "API Key", thinx_api_key, 64);
  wifiManager.addParameter(api_key_param);
  owner_param = new WiFiManagerParameter("owner", "Owner ID", thinx_owner_key, 64);
  wifiManager.addParameter(owner_param);
  wifiManager.setTimeout(5000);
  wifiManager.setDebugOutput(true); // does some logging on mode set
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.autoConnect(accessPointName.c_str());
  #endif

  Serial.print(F("\n*TH: THiNXLib rev. "));
  Serial.print(thx_revision);
  Serial.print(" (");

  if (strlen(thx_commit_id) > 8) { // unknown
    thinx_commit_id = strdup(thx_commit_id);
  } else {
    thinx_commit_id = strdup(THINX_COMMIT_ID);
  }

  Serial.print(thinx_commit_id); // returned string is "not declared in expansion of THX_CID, why?
  Serial.println(")");

  if (once != true) {
    once = true;
  }

  status = WL_IDLE_STATUS;
  wifi_connected = false;
  mqtt_client = NULL;
  mqtt_payload = "";
  mqtt_connected = false;
  performed_mqtt_checkin = false;
  wifi_connection_in_progress = false;
  wifi_retry = 0;

  app_version = strdup("");
  available_update_url = strdup("");
  thinx_cloud_url = strdup("thinx.cloud");

  thinx_firmware_version_short = strdup("");
  thinx_firmware_version = strdup("");
  thinx_mqtt_url = strdup("thinx.cloud");
  thinx_version_id = strdup("");
  thinx_api_key = strdup("");
  thinx_forced_update = false;
  last_checkin_timestamp = 0; // 1/1/1970

  checkin_interval = millis() + checkin_timeout / 4; // retry faster before first checkin
  reboot_interval = millis() + reboot_timeout;

  // will be loaded from SPIFFS/EEPROM or retrieved on Registration later
  if (strlen(__owner_id) == 0) {
    thinx_owner = strdup("");
  }

  EEPROM.begin(512); // should be SPI_FLASH_SEC_SIZE

  import_build_time_constants();
  restore_device_info();
  info_loaded = true;

  #ifdef __USE_WIFI_MANAGER__
  wifi_connected = true;
  #else
  if ((WiFi.status() == WL_CONNECTED) && (WiFi.getMode() == WIFI_STA)) {
    wifi_connected = true;
    wifi_connection_in_progress = false;
  } else {
    WiFi.mode(WIFI_STA);
  }
  #endif

  if (strlen(__apikey) > 4) {
    Serial.println(F("*TH: With custom API Key..."));
    thinx_api_key = strdup(__apikey);
  } else {
      if (strlen(thinx_api_key) > 4) {
          Serial.print(F("*TH: With thinx.h API Key..."));
      } else {
          Serial.print(F("*TH: No API Key!"));
          return;
      }
  }

  if (strlen(__owner_id) > 4) {
    Serial.println(F("*TH: With custom Owner ID..."));
    thinx_owner = strdup(__owner_id);
  } else {
      if (strlen(thinx_owner) > 4) {
          Serial.print(F("*TH: With thinx.h owner..."));
      } else {
          Serial.print(F("*TH: No API Key!"));
          return;
      }
  }

  initWithAPIKey(thinx_api_key);
  wifi_connection_in_progress = false; // last
}

// Designated initializer
void THiNX::initWithAPIKey(const char * __apikey) {

  #ifdef __USE_SPIFFS__
  Serial.println(F("*TH: Checking filesystem, please don't turn off or reset the device now..."));
  if (!fsck()) {
    Serial.println(F("*TH: Filesystem check failed, disabling THiNX."));
    return;
  }
  #endif

  if (info_loaded == false) {
   restore_device_info(); // loads saved apikey/ownerid
   info_loaded = true;
  }

  if (strlen(__apikey) > 4) {
   thinx_api_key = strdup(__apikey);
  } else {
   if (strlen(thinx_api_key) < 4) {
     Serial.print(F("*TH: No API Key!"));
     return;
   }
  }

  wifi_connection_in_progress = false;
  thinx_phase = CONNECT_WIFI;
}

/*
* Connection management
*/

char* THiNX::get_udid() {
  return strdup(thinx_udid);
}

void THiNX::connect() {

  if (wifi_connected) {
    Serial.println(F("*TH: connected"));
    return;
  }

  Serial.print(F("*TH: connecting: ")); Serial.println(wifi_retry);

  #ifndef __USE_WIFI_MANAGER__
  if (WiFi.SSID()) {

    if (wifi_connection_in_progress != true) {
      Serial.print(F("*TH: SSID ")); Serial.println(WiFi.SSID());
      if (WiFi.getMode() == WIFI_AP) {
        Serial.print(F("THiNX > LOOP > START() > AP SSID"));
        Serial.println(WiFi.SSID());
      } else {
        if (strlen(THINX_ENV_SSID) > 2) {
          Serial.println(F("*TH: LOOP > CONNECT > STA RECONNECT"));
          WiFi.begin(THINX_ENV_SSID, THINX_ENV_PASS);
          Serial.println(F("*TH: Enabling connection state (197)"));
        } else {
          Serial.println(F("*TH: LOOP > CONNECT > NO CREDS"));
          wifi_connection_in_progress = true;
          Serial.println(F("*TH: WARNING: Dead branch (201)"));
        }
        wifi_connection_in_progress = true; // prevents re-entering connect_wifi(); should timeout
      }
      //
    }
  } else {
    Serial.print(F("*TH: No SSID."));
  }
  #endif

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("THiNX > LOOP > ALREADY CONNECTED"));
    wifi_connected = true; // prevents re-entering start() [this method]
    wifi_connection_in_progress = false;
  } else {
    Serial.println(F("THiNX > LOOP > CONNECTING WiFi:"));
    connect_wifi();
    Serial.println(F("*TH: Enabling connection state (237)"));
    wifi_connection_in_progress = true;
  }
}

/*
* Connection to WiFi, called from connect() [if SSID & connected]
*/

void THiNX::connect_wifi() {

  #ifdef __USE_WIFI_MANAGER__
  return;
  #else

  if (wifi_connected) {
    return;
  }

  if (wifi_connection_in_progress) {
    if (wifi_retry > 1000) {
      if (WiFi.getMode() == WIFI_STA) {
        Serial.println(F("*TH: Starting AP with PASSWORD..."));
        WiFi.mode(WIFI_AP);
        WiFi.softAP(accessPointName.c_str(), accessPointPassword.c_str()); // setup the AP on channel 1, not hidden, and allow 8 clients
        wifi_retry = 0;
        wifi_connection_in_progress = false;
        wifi_connected = true;
        return;
      } else {
        if (strlen(THINX_ENV_SSID) > 2) {
          Serial.println(F("*TH: Connecting to AP with pre-defined credentials...")); Serial.flush();
          WiFi.mode(WIFI_STA);
          WiFi.begin(THINX_ENV_SSID, THINX_ENV_PASS);
          Serial.println(F("*TH: Enabling connection state (283)"));
          wifi_connection_in_progress = true; // prevents re-entering connect_wifi()
          wifi_retry = 0; // waiting for sta...
        }
      }

    } else {
      Serial.print("*TH: WiFi retry"); Serial.println(wifi_retry); Serial.flush();
      wifi_retry++;
    }

  } else {

    if (strlen(THINX_ENV_SSID) > 2) {
      if (wifi_retry == 0) {
        Serial.println(F("*TH: Connecting to AP with pre-defined credentials..."));
        // 1st run
        if (WiFi.getMode() != WIFI_STA) {
          WiFi.mode(WIFI_STA);
        } else {
          WiFi.begin(THINX_ENV_SSID, THINX_ENV_PASS);
          Serial.println(F("*TH: Enabling connection state (272)"));
          wifi_connection_in_progress = true; // prevents re-entering connect_wifi()
        }
      }
    }
  }
  #endif
}

/*
* Registration
*/

void THiNX::checkin() {
  Serial.println(F("\n*TH: Contacting API..."));
  if(!wifi_connected) {
    Serial.println(F("*TH: Cannot checkin while not connected, exiting."));
  } else {
    if (strlen(thx_ca_cert) < 2) {
      senddata(checkin_body()); // HTTP fallback
    } else {
      send_data(checkin_body()); // HTTPS
    }
    checkin_interval = millis() + checkin_timeout;
  }
}

/*
* Registration - JSON body constructor
*/

String THiNX::checkin_body() {

  DynamicJsonBuffer jsonBuffer(768);
  JsonObject& root = jsonBuffer.createObject();

  root["mac"] = thinx_mac();

  if (strlen(thinx_firmware_version) > 1) {
    root["firmware"] = thinx_firmware_version;
  } else {
    root["firmware"] = THINX_FIRMWARE_VERSION;
  }

  if (strlen(thinx_firmware_version_short) > 1) {
    root["version"] = thinx_firmware_version_short;
  }

  if (strlen(thx_commit_id) > 1) {
    root["commit"] = thx_commit_id;
  }

  if (strlen(thinx_owner) > 1) {
    root["owner"] = thinx_owner;
  }

  if (strlen(thinx_alias) > 1) {
    root["alias"] = thinx_alias;
  }

  if (strlen(thinx_udid) > 4) {
    root["udid"] = thinx_udid;
  }

  if (statusString.length() > 0) {
    root["status"] = statusString.c_str();
  }

  // Optional location data
  root["lat"] = String(latitude);
  root["lon"] = String(longitude);

  root["rssi"] = String(WiFi.RSSI());
  //root["snr"] = String(100 + WiFi.RSSI() / WiFi.RSSI()); // approximate only

  // Flag for THiNX CI
  #ifndef PLATFORMIO_IDE
  // THINX_PLATFORM is not overwritten by builder in Arduino IDE
  root["platform"] = "arduino";
  #else
  root["platform"] = strdup(THINX_PLATFORM);
  #endif

  DynamicJsonBuffer wrapperBuffer(1024);
  JsonObject& wrapper = wrapperBuffer.createObject();
  wrapper["registration"] = root;

#ifdef __DEBUG_JSON__
  wrapper.printTo(Serial);
  Serial.println();
#endif

  json_output = "";
  wrapper.printTo(json_output);
  return json_output;
}


/*
* Registration - HTTPS POST
*/

void THiNX::senddata(String body) {

  char buf[1024];
  int pos = 0;

  // Serial.print("Sending data over HTTP to: "); Serial.println(thinx_cloud_url);

  if (thx_wifi_client.connect(thinx_cloud_url, 7442)) {

    thx_wifi_client.println(F("POST /device/register HTTP/1.1"));
    thx_wifi_client.print(F("Host: ")); thx_wifi_client.println(thinx_cloud_url);
    thx_wifi_client.print(F("Authentication: ")); thx_wifi_client.println(thinx_api_key);
    thx_wifi_client.println(F("Accept: application/json")); // application/json
    thx_wifi_client.println(F("Origin: device"));
    thx_wifi_client.println(F("Content-Type: application/json"));
    thx_wifi_client.println(F("User-Agent: THiNX-Client"));
    thx_wifi_client.print(F("Content-Length: "));
    thx_wifi_client.println(body.length());
    thx_wifi_client.println();
    thx_wifi_client.println(body);

    long interval = 30000;
    unsigned long currentMillis = millis(), previousMillis = millis();

    // Wait until client available or timeout...
    while(!thx_wifi_client.available()){
      delay(1);
      if( (currentMillis - previousMillis) > interval ){
        thx_wifi_client.stop();
        return;
      }
      currentMillis = millis();
    }

    // Read while connected
    while ( thx_wifi_client.connected() ) {
      if ( thx_wifi_client.available() ) {
          buf[pos] = thx_wifi_client.read();
          pos++;
      }
    }

    Serial.printf("Received %u bytes\n", pos);
    String payload = String(buf);
    parse(payload);

  } else {
    Serial.println(F("*TH: API connection failed."));
    return;
  }
}

/* Secure version */
void THiNX::send_data(String body) {
  Serial.println("» Secure API checkin...");
  HTTPClient http;
  http.begin("https://thinx.cloud:7443/device/register", thx_ca_cert);
  http.addHeader("User-Agent", "THiNX-Client");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("Origin", "device");
  http.addHeader("Authentication", thinx_api_key);

  int httpCode = http.POST(body);
  if (httpCode > 0) {
      String payload = http.getString();
#ifdef __DEBUG__
      Serial.println(httpCode);
#endif
      //Serial.println(payload);
      parse(payload);
  } else {
    Serial.println("Error on HTTP request");
  }
  http.end();
}

/*
* Response Parser
*/

void THiNX::parse(String payload) {

  // TODO: Should parse response only for this device_id (which must be internal and not a mac)

  payload_type ptype = Unknown;

  int start_index = 0;
  int endIndex = payload.length();

  int reg_index = payload.indexOf("{\"registration\"");
  int upd_index = payload.indexOf("{\"FIRMWARE_UPDATE\"");
  int not_index = payload.indexOf("{\"notification\"");
  int cfg_index = payload.indexOf("{\"configuration\"");
  int undefined_owner = payload.indexOf("old_protocol_owner:-undefined-");

  if (upd_index > start_index) {
    start_index = upd_index;
    ptype = UPDATE;
  }

  if (reg_index > start_index) {
    start_index = reg_index;
    endIndex = payload.indexOf("}}") + 2;
    ptype = REGISTRATION;
  }

  if (not_index > start_index) {
    start_index = not_index;
    endIndex = payload.indexOf("}}") + 2; // is this still needed?
    ptype = NOTIFICATION;
  }

  if (cfg_index > start_index) {
    start_index = cfg_index;
    endIndex = payload.indexOf("}}") + 2; // is this still needed?
    ptype = CONFIGURATION;
  }

  if (undefined_owner > start_index) {
    Serial.println(F("ERROR: Not authorized. Please copy your owner_id into thinx.h from RTM Console > User Profile."));
    return;
  }

  String body = payload.substring(start_index, endIndex);

#ifdef __DEBUG__
  Serial.print(F("*TH: Parsing response:\n'"));
  Serial.print(body);
  Serial.println("'");
#endif

  DynamicJsonBuffer jsonBuffer(1024);
  JsonObject& root = jsonBuffer.parseObject(body.c_str());

  if ( !root.success() ) {
    Serial.println(F("Failed parsing root node."));
    return;
  }

  switch (ptype) {

    case UPDATE: {

      JsonObject& update = root["registration"];
      Serial.println(F("TODO: Parse update payload..."));

      String mac = update["mac"];
      String this_mac = String(thinx_mac());
      Serial.println(String("mac: ") + mac);

      if (!mac.equals(this_mac)) {
        Serial.println(F("*TH: Warning: firmware is dedicated to device with different MAC."));
      }

      String udid = root["udid"];
      if ( udid.length() > 4 ) {
        thinx_udid = strdup(udid.c_str());
      }

      // Check current firmware based on commit id and store Updated state...
      String commit = update["commit"];
      Serial.println(String("commit: ") + commit);

      // Check current firmware based on version and store Updated state...
      String version = update["version"];
      Serial.println(String("version: ") + version);

      //if ((commit == thinx_commit_id) && (version == thinx_version_id)) { WHY?
      if (strlen(available_update_url) > 5) {
        Serial.println(F("*TH: firmware has same thx_commit_id as current and update availability is stored. Firmware has been installed."));
        available_update_url = strdup("");
        notify_on_successful_update();
        return;
      } else {
        Serial.println(F("*TH: Info: firmware has same thx_commit_id as current and no update is available."));
      }

      save_device_info();

      // In case automatic updates are disabled,
      // we must ask user to commence firmware update.
      if (thinx_auto_update == false) {
        if (mqtt_client != NULL) {
          Serial.println(F("* TH: Update availability notification..."));
          mqtt_client->publish(
            thinx_mqtt_channel().c_str(),
            F("{ title: \"Update Available\", body: \"There is an update available for this device. Do you want to install it now?\", type: \"actionable\", response_type: \"bool\" }")
          );
          mqtt_client->loop();
        }

      } else if (thinx_auto_update || thinx_forced_update){

        Serial.println(F("*TH: Starting update A..."));


        // FROM LUA: update variants
        // local files = payload['files']
        // local ott   = payload['ott']
        // local url   = payload['url']
        // local type  = payload['type']

        String type = update["type"];
        Serial.print(F("*TH: Payload type: ")); Serial.println(type);

        String files = update["files"];

        String url = update["url"]; // may be OTT URL
        available_update_url = url.c_str();

        String ott = update["ott"];
        available_update_url = ott.c_str();

        String hash = update["hash"];
        if (hash.length() > 2) {
          Serial.print(F("*TH: #")); Serial.println(hash);
          expected_hash = strdup(hash.c_str());
        }

        String md5 = update["md5"];
        if (md5.length() > 2) {
          Serial.print(F("*TH: #")); Serial.println(md5);
          expected_md5 = strdup(md5.c_str());
        }

        Serial.println("Saving device info before firmware update."); Serial.flush();
        save_device_info();

        if (url) {
          mqtt_client->publish(
            mqtt_device_status_channel,
            F("{ \"status\" : \"update_started\" }")
          );

          mqtt_client->loop();
          Serial.print(F("*TH: Force update URL must not contain HTTP!!!: "));
          Serial.println(url);
          url.replace("http://", "");
          // TODO: must not contain HTTP, extend with http://thinx.cloud/"
          update_and_reboot(url);
        }
        return;
      }

    } break;

    case NOTIFICATION: {

      // Currently, this is used for update only, can be extended with request_category or similar.
      JsonObject& notification = root["notification"];

      if ( !notification.success() ) {
        Serial.println(F("*TH: Failed parsing notification node."));
        return;
      }

      String type = notification["response_type"];
      if ((type == "bool") || (type == "boolean")) {
        bool response = notification["response"];
        if (response == true) {
          Serial.println(F("*TH: User allowed update using boolean."));
          if (strlen(available_update_url) > 4) {
            update_and_reboot(available_update_url);
          }
        } else {
          Serial.println(F("*TH: User denied update using boolean."));
        }
      }

      if ((type == "string") || (type == "String")) {
        String response = notification["response"];
        if (response == "yes") {
          Serial.println(F("*TH: User allowed update using string."));
          if (strlen(available_update_url) > 4) {
            update_and_reboot(available_update_url);
          }
        } else if (response == "no") {
          Serial.println(F("*TH: User denied update using string."));
        }
      }

    } break;

    case REGISTRATION: {

      JsonObject& registration = root["registration"];

      if ( !registration.success() ) {
        Serial.println(F("*TH: Failed parsing registration node."));
        return;
      }

      bool success = registration["success"];
      String status = registration["status"];

      if (status == "OK") {

        String alias = registration["alias"];
        if ( alias.length() > 1 ) {
          thinx_alias = strdup(alias.c_str());
        }

        String owner = registration["owner"];
        if ( owner.length() > 1 ) {
          thinx_owner = strdup(owner.c_str());
        }

        String udid = registration["udid"];
        if ( udid.length() > 4 ) {
          thinx_udid = strdup(udid.c_str());
        }

        if (registration.containsKey(F("auto_update"))) {
          thinx_auto_update = (bool)registration[F("auto_update")];
        }

        if (registration.containsKey(F("forced_update"))) {
          thinx_forced_update = (bool)registration[F("forced_update")];
        }

        if (registration.containsKey(F("timestamp"))) {
          Serial.print("Updating time (incl. timezone_offset): ");
          last_checkin_timestamp = (long)registration[F("timestamp")] + timezone_offset * 3600;
          last_checkin_millis = millis();
          Serial.println(thinx_time(NULL));
        }

        save_device_info();

      } else if (status == "FIRMWARE_UPDATE") {

        String udid = registration["udid"];
        if ( udid.length() > 4 ) {
          thinx_udid = strdup(udid.c_str());
        }

        save_device_info();

        String mac = registration["mac"];
        Serial.println(String("*TH: Update for MAC: ") + mac);
        // TODO: must be current or 'ANY'

        String commit = registration["commit"];
        // Serial.println(String("commit: ") + commit);

        // should not be same except for forced update
        if (commit == thinx_commit_id) {
          Serial.println(F("*TH: Warning: new firmware has same thx_commit_id as current."));
        }

        String version = registration["version"];
        Serial.println(String(F("*TH: version: ")) + version);

        if (thinx_auto_update == false) {
          Serial.println(String(F("*TH: Skipping auto-update (disabled).")));
          return;
        }

        String update_url;

        String url = registration["url"];
        if (url.length() > 2) {
          Serial.println(F("*TH: Starting direct update..."));
          update_url = url;
        }

        String ott = registration["ott"];
        if (ott.length() > 2) {
          Serial.println(F("*TH: Starting OTT update..."));
          update_url = "http://thinx.cloud:7442/device/firmware?ott="+ott;
        }

        String hash = registration["hash"];
        if (hash.length() > 2) {
          Serial.print(F("*TH: #")); Serial.println(hash);
          expected_hash = strdup(hash.c_str());
        }

        String md5 = registration["md5"];
        if (md5.length() > 2) {
          Serial.print(F("*TH: #")); Serial.println(md5);
          expected_md5 = strdup(md5.c_str());
        }


        Serial.println(update_url);
        update_and_reboot(update_url);
        return;

      }

    } break;

    case CONFIGURATION: {

      JsonObject& configuration = root["configuration"];

      if ( !configuration.success() ) {
        Serial.println(F("*TH: Failed parsing configuration node."));
        return;
      }


      #ifdef __ENABLE_WIFI_MIGRATION__
      //
      // Built-in support for WiFi migration
      //

      const char *ssid = configuration["THINX_ENV_SSID"];
      const char *pass = configuration["THINX_ENV_PASS"];

      // password may be empty string
      if ((strlen(ssid) > 2) && (strlen(pass) > 0)) {
        WiFi.disconnect();
        WiFi.begin(ssid, pass);
        long timeout = millis() + 20000;
        Serial.println(F("*TH: Attempting WiFi migration..."));
        while (WiFi.status() != WL_CONNECTED) {
          yield();
          if (millis() > timeout) break;
        }
        if (WiFi.status() != WL_CONNECTED) {
          Serial.println(F("*TH: WiFi migration failed."));
        } else {
          Serial.println(F("*TH: WiFi migration successful.")); // TODO: Notify using publish() to device status channel
        }
      }
      #endif
      // Forward update body to the library user
      if (_config_callback != NULL) {
        _config_callback(body);
      }

    } break;

    default:
    break;
  }

}

/*
* MQTT channel names
*/

String THiNX::thinx_mqtt_channel() {
  sprintf(mqtt_device_channel, "/%s/%s", thinx_owner, thinx_udid);
  return String(mqtt_device_channel);
}

String THiNX::thinx_mqtt_channels() {
  sprintf(mqtt_device_channels, "/%s/%s/#", thinx_owner, thinx_udid);
  return String(mqtt_device_channels);
}

String THiNX::thinx_mqtt_status_channel() {
  sprintf(mqtt_device_status_channel, "/%s/%s/status", thinx_owner, thinx_udid);
  return String(mqtt_device_status_channel);
}

long THiNX::epoch() {
  long since_last_checkin = (millis() - last_checkin_millis) / 1000;
  return last_checkin_timestamp + since_last_checkin;
}

String THiNX::thinx_time(const char* optional_format) {

  char *format = strdup(time_format);
  if (optional_format != NULL) {
    format = strdup(optional_format);
  }

  long stamp = THiNX::epoch();
  struct tm lt;
  char res[32];
  (void) localtime_r(&stamp, &lt);
  if (strftime(res, sizeof(res), format, &lt) == 0) {
      Serial.println(F("cannot format supplied time into buffer"));
  }
  return String(res);
}

String THiNX::thinx_date(const char* optional_format) {

  char *format = strdup(date_format);
  if (optional_format != NULL) {
    format = strdup(optional_format);
  }

  long stamp = THiNX::epoch();
  struct tm lt;
  char res[32];
  (void) localtime_r(&stamp, &lt);
  if (strftime(res, sizeof(res), format, &lt) == 0) {
      Serial.println(F("cannot format supplied date into buffer"));
  }
  return String(res);
}

/*
* Device MAC address
*/

const char * THiNX::thinx_mac() {
  sprintf(mac_string, "5CCF7F%6X", ESP.getEfuseMac());
  return mac_string;
}

/*
* Sends a MQTT message on successful update (should be used after boot).
*/

void THiNX::notify_on_successful_update() {
  if (mqtt_client != NULL) {
    Serial.println(F("*TH: notify_on_successful_update()"));
    mqtt_client->publish(
      mqtt_device_status_channel,
      F("{ title: \"Update Successful\", body: \"The device has been successfully updated.\", type: \"success\" }")
    );
    mqtt_client->loop();
  } else {
    Serial.println(F("*TH: Device updated but MQTT not active to notify. TODO: Store."));
  }
}

/*
* Sends a MQTT message to Device's Status channel (/owner/udid/status)
*/

void THiNX::publishStatus(String message) {
  publishStatusRetain(message, true);
}

void THiNX::publishStatusUnretained(String message) {
  publishStatusRetain(message, false);
}

void THiNX::publishStatusRetain(String message, bool retain) {
  if (mqtt_client != NULL) {
    if (retain) {
      mqtt_client->publish(
        MQTT::Publish(mqtt_device_status_channel, message.c_str()).set_retain()
      );
    } else {
      mqtt_client->publish(mqtt_device_status_channel, message.c_str());
    }
    mqtt_client->loop();
  } else {
    Serial.println(F("*TH: MQTT not active."));
  }
}

/*
* Sends a MQTT message to the Device Channel (/owner/udid)
*/

void THiNX::publish(String message, String topic, bool retain)  {
  if (mqtt_client != NULL) {

    if (retain == true) {
      mqtt_client->publish(
        MQTT::Publish(mqtt_device_channel, message.c_str()).set_retain()
      );
    } else {
      mqtt_client->publish(
        mqtt_device_channel, message.c_str()
      );
    }
    mqtt_client->loop();
  } else {
    Serial.println(F("MQTT not active."));
  }
}

/*
* Starts the MQTT client and attach callback function forwarding payload to parser.
*/

bool THiNX::start_mqtt() {

  if (mqtt_client != NULL) {
    mqtt_connected = mqtt_client->connected();
    if (mqtt_connected) {
      return true;
    } else {
      return false;
    }
  }

  if (strlen(thinx_udid) < 4) {
    Serial.println(F("*TH: MQTT NO-UDID!")); Serial.flush();
    return false;
  }

  if (strlen(thx_ca_cert) < 2) {
    Serial.println(F("*TH: Contacting MQTT server..."));
    mqtt_client = new PubSubClient(thx_wifi_client, thinx_mqtt_url);
  } else {
    https_client.setCACert(thx_ca_cert);
    Serial.println(F("*TH: Contacting MQTTS server..."));
    mqtt_client = new PubSubClient(https_client, thinx_mqtt_url);
  }
  last_mqtt_reconnect = 0;

  if (strlen(thinx_api_key) < 5) {
    Serial.println(F("*TH: API Key not set, exiting."));
    return false;
  }

  const char* id = thinx_mac();
  const char* user = thinx_udid;
  const char* pass = thinx_api_key;
  String willTopic = thinx_mqtt_status_channel();
  int willQos = 0;
  bool willRetain = false;

  if (mqtt_client->connect(MQTT::Connect(id)
  .set_will(willTopic.c_str(), F("{ \"status\" : \"disconnected\" }"))
  .set_auth(user, pass)
  .set_keepalive(60))) {

    mqtt_connected = true;
    performed_mqtt_checkin = true;

    mqtt_client->set_callback([this](const MQTT::Publish &pub){

      if (pub.has_stream()) {
        Serial.println(F("*TH: MQTT Type: Stream..."));
        uint32_t startTime = millis();
        uint32_t size = pub.payload_len();
        /* Currently not suppported {/}
        if ( ESP.updateSketch(*pub.payload_stream(), size, true, false) ) {
          // Notify on reboot for update
          mqtt_client->publish(
            mqtt_device_status_channel,
            "{ \"status\" : \"rebooting\" }"
          );
          mqtt_client->disconnect();
          pub.payload_stream()->stop();
          Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
          ESP.restart();
        } else {
          Serial.println(F("*TH: ESP MQTT Stream update failed..."));
          mqtt_client->publish(
            mqtt_device_status_channel,
            "{ \"status\" : \"mqtt_update_failed\" }"
          );
        }
        */

      } else {
        Serial.println(F("*TH: MQTT Type: String or JSON..."));
        Serial.println(pub.payload_string());
        parse(pub.payload_string());
        if (_mqtt_callback) {
          _mqtt_callback(pub.payload_string());
        }
      }
    }); // end-of-callback

    return true;

  } else {
    mqtt_connected = false;
    Serial.println(F("*TH: MQTT Not connected."));
    return false;
  }
}

/*
* Restores Device Info. Calles (private): initWithAPIKey; save_device_info()
* Provides: alias, owner, update, udid, (apikey)
*/

void THiNX::restore_device_info() {

  Serial.println(F("*TH: Restoring device info..."));

  int json_end = 0;

  #ifndef __USE_SPIFFS__

  int value;
  long buf_len = 512;
  long data_len = 0;

  Serial.println(F("*TH: Restoring configuration from EEPROM..."));

  for (long a = 0; a < buf_len; a++) {
    value = EEPROM.read(a);
    json_output += char(value);
    // validate at least data start
    if (a == 0) {
      if (value != '{') {
        return; // Not a JSON, nothing to do...
      }
    }
    if (value == '{') {
      json_end++;
    }
    if (value == '}') {
      json_end--;
    }
    if (value == 0) {
      json_info[a] = char(value);
      data_len++;
      Serial.print("*TH: "); Serial.print(a); Serial.println(F(" bytes read from EEPROM."));
      // Validate JSON
      break;
    } else {
      json_info[a] = char(value);
      data_len++;
    }
    // Serial.flush(); // to debug reading EEPROM bytes
  }

  // Validating bracket count
  if (json_end != 0) {
    Serial.println(F("*TH: JSON invalid... bailing out."));
    return;
  }

  Serial.println(F("*TH: Converting data to String..."));

  #else
  if (!SPIFFS.exists("/thx.cfg")) {
    Serial.println(F("*TH: No saved configuration."));
    return;
  }
  File f = SPIFFS.open("/thx.cfg", "r");
  Serial.println(F("*TH: Found persistent data..."));
  if (!f) {
    Serial.println(F("*TH: No remote configuration found so far..."));
    return;
  }
  if (f.size() == 0) {
    Serial.println(F("*TH: Remote configuration file empty..."));
    return;
  }

  f.readBytesUntil('\n', json_info, 511);
  #endif

  Serial.println(json_info);
  Serial.println(F("*TH: Parsing..."));

  DynamicJsonBuffer jsonBuffer(512);
  JsonObject& config = jsonBuffer.parseObject((char*)json_info); // must not be String!

  if (!config.success()) {
    // Serial.println(F("*TH: No JSON data to be parsed..."));
    return;

  } else {

    Serial.println(F("*TH: Reading JSON..."));

    if (config["alias"]) {
      thinx_alias = strdup(config["alias"]);
      Serial.print("alias: ");
      Serial.println(thinx_alias);
    }

    if (config["udid"]) {
      const char *udid = config["udid"];
      if ( strlen(udid) > 2 ) {
        thinx_udid = strdup(udid);
      } else {
        thinx_udid = strdup(THINX_UDID);
      }
    } else {
      thinx_udid = strdup(THINX_UDID);
    }

    Serial.print("thinx_udid: ");
    Serial.println(thinx_udid);

    if (config["apikey"]) {
      thinx_api_key = strdup(config["apikey"]);
      Serial.print("apikey: ");
      Serial.println(thinx_api_key);
    }

    if (config["owner"]) {
      thinx_owner = strdup(config["owner"]);
      Serial.print("owner: ");
      Serial.println(thinx_owner);
    }

    if (config["ott"]) {
      available_update_url = strdup(config["ott"]);
      Serial.print("available_update_url: ");
      Serial.println(available_update_url);
    }

    #ifdef __USE_SPIFFS__
    Serial.print(F("*TH: Closing SPIFFS file."));
    f.close();
    #else
    #endif
  }
}

/*
* Stores mutable device data (alias, owner) retrieved from API
*/

void THiNX::save_device_info()
{
  deviceInfo(); // update json_output

  // disabled for it crashes when closing the file (LoadStoreAlignmentCause) when using String
  #ifdef __USE_SPIFFS__
  File f = SPIFFS.open("/thx.cfg", "w");
  if (f) {
    Serial.println(F("*TH: Saving configuration to SPIFFS..."));
    f.println(String((char*)json_info)); // String instead of const char* due to LoadStoreAlignmentCause...
    f.close();
    Serial.println(F("*TH: Saved configuration (SPIFFS)."));
  }
  #else
  Serial.println(F("*TH: Saving configuration to EEPROM: "));
  for (long addr = 0; addr < strlen((const char*)json_info); addr++) {
    uint8_t byte = json_info[addr];
    EEPROM.put(addr, json_info[addr]);
    if (byte == 0) break;
  }
  EEPROM.commit();
  Serial.println(F("*TH: Saved configuration (EEPROM)."));
  #endif
}

/*
* Fills output buffer with persistent dconfiguration JSON.
*/

void THiNX::deviceInfo() {

  DynamicJsonBuffer jsonBuffer(512);
  JsonObject& root = jsonBuffer.createObject();

  // Mandatories

  if (strlen(thinx_owner) > 1) {
    root["owner"] = thinx_owner; // allow owner change
  }

  if (strlen(thinx_api_key) > 1) {
    root["apikey"] = thinx_api_key; // allow dynamic API Key
  }

  if (strlen(thinx_udid) > 1) {
    root["udid"] = thinx_udid; // allow setting UDID, skip 0
  }

  // Optionals
  if (strlen(available_update_url) > 1) {
    root["update"] = available_update_url; // allow update
    Serial.print(F("*TH: available_update_url..."));
  }

  json_output = "";
  root.printTo(json_output);
}

/*
* Updates
*/

// update_file(name, data)
// update_from_url(name, url)

void THiNX::update_and_reboot(String url) {

  Serial.print("*TH: Update with URL: ");
  Serial.println(url);

  // #define __USE_STREAM_UPDATER__ ; // Warning, this is MQTT-based streamed update!
  #ifdef __USE_STREAM_UPDATER__
  Serial.println(F("*TH: Starting MQTT & reboot..."));
  uint32_t size = pub.payload_len();
  if (ESP.updateSketch(*pub.payload_stream(), size, true, false)) {
    Serial.println(F("Clearing retained message."));
    mqtt_client->publish(MQTT::Publish(pub.topic(), "").set_retain());
    mqtt_client->disconnect();

    Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);

    // Notify on reboot for update
    if (mqtt_client != NULL) {
      mqtt_client->publish(
        mqtt_device_status_channel,
        thx_reboot_response.c_str()
      );
      mqtt_client->disconnect();
    }
    ESP.restart();
  }
  #else

  Serial.println(F("*TH: Starting ESP8266 HTTP Update & reboot..."));
  t_httpUpdate_return ret = ESPhttpUpdate.update(url.c_str());

  switch(ret) {
    case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    setDashboardStatus(ESPhttpUpdate.getLastErrorString());
    break;

    case HTTP_UPDATE_NO_UPDATES:
    Serial.println(F("HTTP_UPDATE_NO_UPDATES"));
    break;

    case HTTP_UPDATE_OK:
    Serial.println(F("HTTP_UPDATE_OK"));
    ESP.restart();
    break;
  }
  #endif
}

/*
* Imports all required build-time values from thinx.h
*/

void THiNX::import_build_time_constants() {

  // Only if not overridden by user
  if (strlen(thinx_api_key) < 4) {
    thinx_api_key = strdup(THINX_API_KEY);
  }

  if (strlen(THINX_UDID) > 2) {
    thinx_udid = strdup(THINX_UDID);
  } else {
    thinx_udid = strdup("");
  }

  // Use commit-id from thinx.h if not given by environment
  #ifdef THX_COMMIT_ID
  thinx_commit_id = strdup(thx_commit_id);
  #else
  thinx_commit_id = strdup(THINX_COMMIT_ID);
  #endif

  thinx_mqtt_url = strdup(THINX_MQTT_URL);
  thinx_cloud_url = strdup(THINX_CLOUD_URL);
  thinx_alias = strdup(THINX_ALIAS);
  thinx_owner = strdup(THINX_OWNER);
  thinx_mqtt_port = THINX_MQTT_PORT;
  thinx_api_port = THINX_API_PORT;
  thinx_auto_update = THINX_AUTO_UPDATE;
  thinx_forced_update = THINX_FORCED_UPDATE;
  thinx_firmware_version = strdup(THINX_FIRMWARE_VERSION);
  thinx_firmware_version_short = strdup(THINX_FIRMWARE_VERSION_SHORT);
  app_version = strdup(THINX_APP_VERSION);
}

/*
* Performs the SPIFFS check and format if needed.
*/

bool THiNX::fsck() {
#if defined(ESP8266)
  String realSize = String(ESP.getFlashChipRealSize());
  String ideSize = String(ESP.getFlashChipSize());
  bool flashCorrectlyConfigured = realSize.equals(ideSize);
#else
  bool flashCorrectlyConfigured = true; //?
#endif
  bool fileSystemReady = false;
  if(flashCorrectlyConfigured) {
    Serial.println(F("* TH: Starting SPIFFS..."));
    #if defined(ESP8266)
      fileSystemReady = SPIFFS.begin();
    #else
      fileSystemReady = SPIFFS.begin(true); // formatOnFail=true
    #endif
    if (!fileSystemReady) {
      Serial.println(F("* TH: Formatting SPIFFS..."));
      fileSystemReady = SPIFFS.format();
      Serial.println(F("* TH: Format complete, rebooting...")); Serial.flush();
      ESP.restart();
      return false;
    }
    Serial.println(F("* TH: SPIFFS Initialization completed."));
  }  else {
    Serial.print(F("*TH: Flash incorrectly configured, SPIFFS cannot start."));
#if defined(ESP8266)
    Serial.println(ideSize + ", real size: " + realSize);
#endif
  }
  return fileSystemReady ? true : false;
}

#ifdef __USE_WIFI_MANAGER__
/*
* API key update event
*/

void THiNX::evt_save_api_key() {
  if (should_save_config) {
    if (strlen(thx_api_key) > 4) {
      thinx_api_key = thx_api_key;
      Serial.print(F("Saving thx_api_key from Captive Portal: "));
      Serial.println(thinx_api_key);
    }
    if (strlen(thx_owner_key) > 4) {
      thinx_owner_key = thx_owner_key;
      Serial.print(F("Saving thx_owner_key from Captive Portal: "));
      Serial.println(thinx_owner_key);
    }
    save_device_info();
    should_save_config = false;
  }
}
#endif

/*
* Final callback setter
*/

void THiNX::setPushConfigCallback( void (*func)(String) ) {
  _config_callback = func;
}

void THiNX::setFinalizeCallback( void (*func)(void) ) {
  _finalize_callback = func;
}

void THiNX::setMQTTCallback( void (*func)(String) ) {
    _mqtt_callback = func;
}

void THiNX::finalize() {
  thinx_phase = COMPLETED;
  if (_finalize_callback) {
    Serial.println(F("*TH: Checkin completed, calling finalize_callback()"));
    _finalize_callback();
  } else {
    Serial.println(F("*TH: Checkin completed (no _finalize_callback set)."));
  }
}

/* This is necessary for SSL/TLS and should replace THiNX timestamp */
void THiNX::sync_sntp() {
  Serial.print("*TH: Setting time using SNTP...");
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("*TH: SNTP time: ");
  Serial.print(asctime(&timeinfo));
}

/*
* Core loop
*/

void THiNX::loop() {

  if (thinx_phase < COMPLETED) {
    // Serial.print("Phase: "); Serial.println(thinx_phase);
  }

  // CASE thinx_phase == CONNECT_WIFI

  if (thinx_phase == CONNECT_WIFI) {
    // If not connected manually or using WiFiManager, start connection in progress...
    if (WiFi.status() != WL_CONNECTED) {
      wifi_connected = false;
      if (wifi_connection_in_progress != true) {
        Serial.println(F("*TH: CONNECTING »"));
        Serial.println(F("*TH: LOOP «÷»")); Serial.flush();
        connect(); // blocking
        Serial.println(F("*TH: Enabling connection state (1283)"));
        wifi_connection_in_progress = true;
        Serial.println(F("*TH: LOOP «"));
        wifi_connection_in_progress = true;
        return;
      } else {
        return;
      }
    } else {
      wifi_connected = true;

      sync_sntp();

      // Start MDNS broadcast
      if (!MDNS.begin(thinx_alias)) {
        Serial.println(F("*TH: Error setting up mDNS"));
      } else {
        // Query MDNS proxy
        Serial.println(F("*TH: Searching for thinx-connect on local network..."));
        int n = MDNS.queryService("thinx", "tcp"); // TODO: WARNING! may be _tcp!
        if (n > 0) {
          thinx_cloud_url = strdup(String(MDNS.hostname(0)).c_str());
          thinx_mqtt_url = strdup(String(MDNS.hostname(0)).c_str());

          Serial.println(F("*TH: Routing traffic through thinx-connect..."));
          Serial.print(F("     host            : ")); Serial.println(MDNS.hostname(0));
          Serial.print(F("     IP              : ")); Serial.println(MDNS.IP(0));
          Serial.print(F("     port            : ")); Serial.println(MDNS.port(0));
          Serial.print(F("     thinx_cloud_url : ")); Serial.println(thinx_cloud_url);
          Serial.print(F("     thinx_mqtt_url  : ")); Serial.println(thinx_mqtt_url);
        }
      }

      Serial.println(F("*TH: LOOP » CONNECT_API"));
      thinx_phase = CONNECT_API;
      return;
    }
  }

  // After MQTT gets connected:
  if (thinx_phase == CHECKIN_MQTT) {
    thinx_mqtt_channel(); // initialize channel variable
    if (strlen(mqtt_device_channel) > 5) {
      if (mqtt_client->subscribe(mqtt_device_channel)) {
        Serial.print(F("*TH: MQTT device topic: "));
        Serial.print(mqtt_device_channel);
        Serial.println(F(" successfully subscribed."));
         Serial.println(F("*TH: Publishing `status: connected` to MQTT"));
        // Publish status on status channel
        mqtt_client->publish(
          mqtt_device_status_channel,
          F("{ \"status\" : \"connected\" }")
        );
        mqtt_client->loop();
        Serial.println(F("*TH: LOOP » FINALIZE"));
        thinx_phase = FINALIZE;
        return;
      }
    }
  }

  if ( thinx_phase == CONNECT_MQTT ) {
    if (strlen(thinx_udid) > 4) {
      if (mqtt_connected == false) {
        Serial.println(F("*TH: PHASE » CONNECT_MQTT"));
        mqtt_connected = start_mqtt();
        mqtt_client->loop();
        if (mqtt_connected) {
             Serial.println(F("*TH: PHASE » CHECKIN_MQTT"));
             thinx_phase = CHECKIN_MQTT; // == CONNECT_MQTT+1
         }
        return;
      } else {
        thinx_phase = FINALIZE;
        return;
      }
    } else {
      Serial.println(F("*TH: LOOP » FINALIZE"));
      thinx_phase = FINALIZE;
      return;
    }
  }

  // CASE thinx_phase == CONNECT_API

  // Force re-checkin after specified interval
  if (millis() > checkin_timeout) {
    if (checkin_interval > 0) {
      Serial.println(F("*TH: LOOP » CONNECT_API"));
      thinx_phase = CONNECT_API;
      checkin_interval = millis() + checkin_timeout;
    }
  }

  // If connected, perform the MQTT loop and bail out ASAP
  if (thinx_phase == CONNECT_API) {
    if (WiFi.getMode() == WIFI_AP) {
      Serial.println(F("*TH: LOOP « (AP_MODE)"));
      return;
    }
    if (strlen(thinx_api_key) > 4) {
      Serial.println(F("*TH: LOOP » CONNECT_API in phase"));
      checkin(); // warning, this blocking and takes time, thus return...
      if (mqtt_connected == false) {
        Serial.println(F("*TH: LOOP » CONNECT_MQTT"));
        thinx_phase = CONNECT_MQTT;
      } else {
        Serial.println(F("*TH: LOOP » FINALIZE (mqtt connected)"));
        thinx_phase = FINALIZE;
      }
    }
  }

  if ( thinx_phase == FINALIZE ) {
    finalize();
  }

  if ( thinx_phase >= FINALIZE ) {
    if (mqtt_connected == true) {
      mqtt_client->loop();
    }
  }

  if ( (reboot_interval > 0) && (millis() > reboot_interval) ) {
    Serial.println("Rebooting..."); Serial.flush();
    setDashboardStatus("Rebooting...");
    ESP.restart();
  }

  #ifdef __USE_WIFI_MANAGER__
    // Save API key on change
    if (should_save_config) {
      Serial.println(F("*TH: Saving API key on change..."));
      evt_save_api_key();
      should_save_config = false;
    }
  #endif
}

void THiNX::setLocation(double lat, double lon) {
  latitude = lat;
  longitude = lon;
  // uses mqtt_connected status because this happens only after first checkin
  // and thus prevents premature request to backend.
  if (wifi_connected && thinx_phase > FINALIZE) {
    Serial.println(F("*TH: LOOP » setLocation checkin"));
    checkin();
    if (mqtt_connected == false) {
      Serial.println(F("*TH: LOOP » CONNECT_MQTT"));
      thinx_phase = CONNECT_MQTT;
    } else {
      Serial.println(F("*TH: LOOP » FINALIZE (mqtt connected)"));
      thinx_phase = FINALIZE;
    }
  }
}

void THiNX::setDashboardStatus(String newstatus) {
  statusString = newstatus;
  if (wifi_connected && thinx_phase > FINALIZE) {
    Serial.println(F("*TH: LOOP » setDashboardStatus checkin"));
    checkin();
    if (mqtt_connected == false) {
      Serial.println(F("*TH: LOOP » CONNECT_MQTT"));
      thinx_phase = CONNECT_MQTT;
      return;
    } else {
      Serial.println(F("*TH: LOOP » FINALIZE (mqtt connected)"));
      thinx_phase = FINALIZE;
    }
    if (mqtt_client) {
      String message = String("{ \"status\" : \"") + newstatus + String("\" }");
      mqtt_client->publish(mqtt_device_status_channel, message.c_str());
    }
  }
}

// deprecated since 2.2 (3)
void THiNX::setStatus(String newstatus) {
  setDashboardStatus(newstatus);
}

void THiNX::setCheckinInterval(long interval) {
  checkin_interval = interval;
}

void THiNX::setRebootInterval(long interval) {
  reboot_interval = interval;
}

// SHA256

/* Calculates SHA-256 of a file */
bool THiNX::check_hash(char * filename, char * expected) {
  File file = SPIFFS.open(filename, "r");
  char aes_text[2 * SHA256_BLOCK_SIZE + 1];
  BYTE hash[SHA256_BLOCK_SIZE];
  static uint8_t buf[512] = {0};
  static uint8_t obuf[512] = {0};
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  uint32_t fpos = 0;

  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    Sha256 *sha256Instance = new Sha256();

    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      sha256Instance->update((const unsigned char*)buf, toRead);
      fpos += toRead;
      len -= toRead;
    }

    sha256Instance->final(obuf);
    delete sha256Instance;

    for (int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
      sprintf(aes_text + 2 * i, "%02X", obuf[i]);
    }

    Serial.printf("AES # %s at %u\n", aes_text, fpos);
    Serial.printf("EXPECTED # %s", expected);

    end = millis() - start;
    Serial.printf("%u bytes hashed in %u ms\n\n", flen, end);

    file.close();

    return strcmp(expected, aes_text);

  } else {
    Serial.println("Failed to open file for reading");
    return false;
  }

}

#endif // IMPORTANT LINE FOR UNIT-TESTING!
