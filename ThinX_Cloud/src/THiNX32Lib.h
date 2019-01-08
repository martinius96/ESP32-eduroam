#include <Arduino.h>

// enables stack/heap debugging
#define __DEBUG__

// enable automatic WiFi disconnect/reconnect on Configuration Push (THINX_ENV_SSID and THINX_ENV_PASS)
#define __ENABLE_WIFI_MIGRATION__

// if disabled, you need to `WiFi.begin(ssid, pass)` on your own
#define __USE_WIFI_MANAGER__

// if disabled, uses EEPROM instead
#define __USE_SPIFFS__

// if enabled, requires TinyGSM library for network connection instead of WiFi
#define __USE_TGSM__

// Provides placeholder for THINX_FIRMWARE_VERSION_SHORT
#ifndef VERSION
#define VERSION "2.3.181"
#endif

#ifndef THX_REVISION
#ifdef THINX_FIRMWARE_VERSION_SHORT
#define THX_REVISION THINX_FIRMWARE_VERSION_SHORT
#else
#define THX_REVISION "181"
#endif
#endif

#ifdef __USE_WIFI_MANAGER__
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#endif

// extern C
#include <stdio.h>
#include <time.h>

// Supported by Arduino ESP32 Framwork
#include <SPIFFS.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h> // already part of firmware, deprecated
#include <WiFiClientSecure.h>

// 3rd party libraries
#include <ArduinoJson.h>
#include "sha256.h" // actually <AESLib.h>

// Originally using better than Arduino-bundled version of MQTT https://github.com/Imroy/pubsubclient
// will need to refactor back to support __USE_TGSM__
#include <PubSubClient.h>

#ifdef __USE_TGSM__
#include <TinyGSM.h> // just for testing sketch size, may not make it.

// Before:
// Sketch uses 1151758 bytes (58%) of program storage space. Maximum is 1966080 bytes.
// Global variables use 50464 bytes (15%) of dynamic memory, leaving 277216 bytes for local variables. Maximum is 327680 bytes.
//
// After (no linkage):
// Sketch uses 1151758 bytes (58%) of program storage space. Maximum is 1966080 bytes.
// Global variables use 50464 bytes (15%) of dynamic memory, leaving 277216 bytes for local variables. Maximum is 327680 bytes.

#endif


class THiNX {

    public:

    // Root certificate used by thinx.cloud
    static const char * thx_ca_cert;

    static double latitude;
    static double longitude;
    static String statusString;
    static String accessPointName;
    static String accessPointPassword;

#ifdef __USE_WIFI_MANAGER__
    static WiFiManagerParameter *api_key_param;
    static WiFiManagerParameter *owner_param;
    static int should_save_config; // after autoconnect, may provide new API Key
    static void saveConfigCallback();
#endif

    THiNX();
    THiNX(const char *, const char *); // (const char * __apikey, const char * __owner_id)
    THiNX(const char *);  // (const char * __apikey)

    enum payload_type {
        Unknown = 0,
        UPDATE = 1,         // Firmware Update Response Payload
        REGISTRATION = 2,   // Registration Response Payload
        NOTIFICATION = 3,   // Notification/Interaction Response Payload
        CONFIGURATION = 4,  // Environment variables update
        Reserved = 255,     // Reserved
    };

    enum phase {
        INIT = 0,
        CONNECT_WIFI = 1,
        CONNECT_API = 2,
        CONNECT_MQTT = 3,
        CHECKIN_MQTT = 4,
        FINALIZE = 5,
        COMPLETED = 6
    };

    phase thinx_phase;

    // Public API
    void initWithAPIKey(const char *);
    void loop();

    String checkin_body();  // TODO: Refactor to C-string

    // MQTT
    PubSubClient *mqtt_client = NULL;

    char mqtt_device_channel[128];
    char mqtt_device_channels[128];
    char mqtt_device_status_channel[128];
    String thinx_mqtt_channel();
    String thinx_mqtt_channels();
    String thinx_mqtt_status_channel();

    // Values imported on build from thinx.h
    const char* app_version;                  // max 80 bytes
    const char* available_update_url;         // up to 1k
    const char* thinx_cloud_url;              // up to 1k but generally something where FQDN fits
    const char* thinx_commit_id;              // 40 bytes + 1
    const char* thinx_firmware_version_short; // 14 bytes
    const char* thinx_firmware_version;       // max 80 bytes
    const char* thinx_mqtt_url;               // up to 1k but generally something where FQDN fits
    const char* thinx_version_id;             // max 80 bytes (DEPRECATED?)

    bool thinx_auto_update;
    bool thinx_forced_update;

    long thinx_mqtt_port;
    long thinx_api_port;

    // dynamic variables
    char* thinx_alias;
    char* thinx_owner;

    char * get_udid();

    void setPushConfigCallback( void (*func)(String) );
    void setFinalizeCallback( void (*func)(void) );
    void setMQTTCallback( void (*func)(String) );

    int wifi_connection_in_progress;

    // MQTT Support

    // publish to device status channel only
    void publishStatus(String);               // send String to status channel
    void publishStatusUnretained(String);     // send String to status channel (unretained)
    void publishStatusRetain(String, bool);   // send String to status channel (optionally retained)

    // publish to specified channel
    void publish(String, String, bool);       // send String to any channel, optinally with retain

    static const char time_format[];
    static const char date_format[];

    // Time and Date support requires checkin against THiNX API > 0.9.305x
    long epoch();                    // estimated timestamp since last checkin as
    long timezone_offset = 2;
    String thinx_time(const char*);        // estimated current Time
    String thinx_date(const char*);        // estimated current Date

    void setCheckinInterval(long interval);
    void setRebootInterval(long interval);

    // checkins
    void checkin();                         // happens on registrationw
    void setDashboardStatus(String);        // performs checkin while updating Status on Dashboard
    void setStatus(String);                 // deprecated 2.2 (3)
    void setLocation(double,double);        // performs checkin while updating Location

    private:

    char* thinx_udid;

    bool wifi_connected;  // WiFi connected in station mode
    bool info_loaded = false;

    static char* thinx_api_key;
    static char* thinx_owner_key;

    //
    // Build-specific constants (override for Arduino IDE which does not set any Environments like PlatformIO)
    //

#ifdef THX_REVISION
    const char* thx_revision = strdup(String(THX_REVISION).c_str());
#else
    const char* thx_revision = "0";
#endif

#ifdef THX_COMMIT_ID
    const char* thx_commit_id = strdup(THX_COMMIT_ID);
#else
    const char* thx_commit_id = "unknown";
#endif

    //
    // THiNXLib
    //

    void configCallback();

    // WiFi Manager
    WiFiClient thx_wifi_client;
    WiFiClientSecure https_client;
    int status;                             // global WiFi status
    bool once;                              // once token for initialization

    // THiNX API
    static char thx_api_key[65];            // static due to accesibility to WiFiManager
    static char thx_owner_key[65];          // static due to accesibility to WiFiManager

    char mac_string[17];
    const char * thinx_mac();

    char json_info[512] = {0};               // statically allocated to prevent fragmentation

    String json_output;

    // In order of appearance

    bool fsck();                            // check filesystem if using SPIFFS

    void connect();                         // start the connect loop
    void connect_wifi();                    // start connecting

    void senddata(String);
    void send_data(String);
    void parse(String);
    void update_and_reboot(String);

    long checkin_timeout = 3600 * 1000;                   // next timeout millis()
    long checkin_interval = 3600 * 1000;    // can be set externaly, defaults to 1h

    long last_checkin_millis;
    long last_checkin_timestamp;

    long reboot_timeout = 86400 * 1000;                    // next timeout millis()
    long reboot_interval = 86400 * 1000;    // can be set externaly, defaults to 24h

    // MQTT
    bool start_mqtt();                      // connect to broker and subscribe
    int mqtt_connected;                     // success or failure on connection
    String mqtt_payload;                    // mqtt_payload store for parsing
    int last_mqtt_reconnect;                // interval
    int performed_mqtt_checkin;             // one-time flag
    int all_done;                           // finalize flag

    void (*_config_callback)(String) = NULL;  // Called when server pushes new environment vars using MQTT
    void (*_mqtt_callback)(String) = NULL;
    void (*_finalize_callback)(void) = NULL;

    // Data Storage
    void import_build_time_constants();     // sets variables from thinx.h file
    void save_device_info();                // saves variables to SPIFFS or EEPROM
    void restore_device_info();             // reads variables from SPIFFS or EEPROM
    void deviceInfo();                      // TODO: Refactor to C-string

    // Updates
    void notify_on_successful_update();     // send a MQTT notification back to Web UI

    // Event Queue / States
    int mqtt_started;
    bool complete;
    void evt_save_api_key();

    // Finalize
    void finalize();                        // Complete the checkin, schedule, callback...

    // Local WiFi Impl
    bool wifi_wait_for_connect;
    unsigned long wifi_wait_start;
    unsigned long wifi_wait_timeout;
    int wifi_retry;
    uint8_t wifi_status;

    // SHA256
    bool check_hash(char * filename, char * expected);
    char * expected_hash;
    char * expected_md5;

    // SSL/TLS
    void sync_sntp();
};
