# The IoT Guru Arduino library

## Summary

This is an Arduino library for ESP8266/ESP32 to provide integration with [The IoT Guru cloud](https://iotguru.live) services.

## Examples

- Device connection example: [01_device_connection.ino](https://github.com/IoTGuruLive/theiotguru-arduino-library/blob/master/examples/01_device_connection/01_device_connection.ino)
- Send measurement example: [02_send_measurement.ino](https://github.com/IoTGuruLive/theiotguru-arduino-library/blob/master/examples/02_send_measurement/02_send_measurement.ino)

## Constructors

```IoTGuru(String userShortId, String deviceShortId, String deviceKey);```

## Methods

### Set check duration

```IoTGuru* setCheckDuration(unsigned long checkDuration);```

### Set debug printer

```IoTGuru* setDebugPrinter(HardwareSerial* debugPrinter);```

### Check in

```boolean check();```

### Send float value

```boolean sendFloatValue(String nodeShortId, String fieldName, float value);```
