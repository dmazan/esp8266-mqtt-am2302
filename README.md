# ESP8266 MQTT AM2302 Temperature and Humidity Sensor

An Arduino sketch that runs on an ESP8266 microcontroller to read temperature and humidity data from an AM2302/DHT22 sensor and publishes the readings via MQTT for integration with home automation systems like OpenHAB.

## Features

- Reads temperature and humidity from AM2302/DHT22 sensor
- Publishes sensor data via MQTT with configurable topics
- Automatic WiFi connection with Station mode (no AP broadcasting)
- MQTT reconnection with DNS resolution fallback
- Unique device identification using MAC address
- Compatible with OpenHAB and other home automation platforms
- Configurable polling intervals (30 seconds default)

## Hardware Requirements

- ESP8266 development board (NodeMCU, Wemos D1 Mini, etc.)
- AM2302/DHT22 temperature and humidity sensor
- Jumper wires for connections

## Wiring

Connect the AM2302 sensor to your ESP8266:

- **VCC** → 3.3V or 5V (depending on your board)
- **GND** → GND
- **DATA** → D7 (GPIO13)

## Software Dependencies

Install the following libraries through the Arduino IDE Library Manager:

1. **ESP8266 Arduino Core** - https://github.com/esp8266/Arduino
2. **DHT New Library** - https://github.com/RobTillaart/DHTNew
3. **PubSubClient** - https://github.com/knolleary/pubsubclient

## Configuration

Create a `secret.h` file in the same directory as the sketch with your network credentials:

```cpp
// secret.h
#ifndef SECRET_H
#define SECRET_H

#define SECRET_SSID "your_wifi_network_name"
#define SECRET_PASSWORD "your_wifi_password"
#define SECRET_MQTT_SERVER "your_mqtt_broker_hostname_or_ip"

#endif
```

## MQTT Topic Structure

The device publishes to the following topics:

- Temperature: `ha/_[MAC_SUFFIX]/_temperature`
- Humidity: `ha/_[MAC_SUFFIX]/_humidity`

Where `[MAC_SUFFIX]` is the last segment of the device's MAC address (e.g., `ab:cd` becomes `abcd`).

Example topics:
```
ha/_a1b2/_temperature
ha/_a1b2/_humidity
```

## OpenHAB Configuration

### 1. Install MQTT Binding

Install the MQTT binding in OpenHAB through the Add-ons section.

### 2. Configure MQTT Broker Thing

Create a MQTT Broker thing in `things/mqtt.things`:

```
Bridge mqtt:broker:myBroker [ host="YOUR_MQTT_BROKER_IP", port=1883 ]
```

### 3. Create Thing Configuration

Add your sensor thing configuration to `things/sensors.things`:

```
Thing mqtt:topic:myBroker:am2302_sensor "AM2302 Sensor" (mqtt:broker:myBroker) {
    Channels:
        Type number : temperature "Temperature" [ stateTopic="ha/_XXXX/_temperature" ]
        Type number : humidity "Humidity" [ stateTopic="ha/_XXXX/_humidity" ]
}
```

Replace `XXXX` with your device's MAC suffix (check serial monitor output when device boots).

### 4. Create Items

Add items in `items/sensors.items`:

```
Number:Temperature AM2302_Temperature "Temperature [%.1f °C]" { channel="mqtt:topic:myBroker:am2302_sensor:temperature" }
Number:Dimensionless AM2302_Humidity "Humidity [%.1f %%]" { channel="mqtt:topic:myBroker:am2302_sensor:humidity" }
```

### 5. Create Sitemap (Optional)

Add to your sitemap in `sitemaps/default.sitemap`:

```
Frame label="Climate" {
    Text item=AM2302_Temperature icon="temperature"
    Text item=AM2302_Humidity icon="humidity"
}
```

## Installation and Setup

1. **Hardware Setup**: Connect the AM2302 sensor to your ESP8266 as described in the wiring section.

2. **Software Setup**:
   - Install the Arduino IDE
   - Add ESP8266 board support
   - Install required libraries
   - Create your `secret.h` file with WiFi and MQTT credentials

3. **Upload Code**:
   - Open `esp8266-mqtt-am2302.ino` in Arduino IDE
   - Select your ESP8266 board and port
   - Upload the sketch

4. **Monitor Serial Output**:
   - Open Serial Monitor (115200 baud)
   - Note the MAC suffix shown during boot for OpenHAB configuration
   - Verify successful WiFi and MQTT connections

## Troubleshooting

### WiFi Connection Issues
- Verify SSID and password in `secret.h`
- Check WiFi signal strength
- Ensure 2.4GHz network (ESP8266 doesn't support 5GHz)

### MQTT Connection Issues
- Verify MQTT broker hostname/IP in `secret.h`
- Check if MQTT broker is running and accessible
- Review firewall settings on MQTT broker
- Monitor serial output for connection error codes

### Sensor Reading Issues
- Verify sensor wiring connections
- Check if sensor is receiving proper power (3.3V or 5V)
- Ensure data pin is connected to D7 (GPIO13)

## Customization

### Change Polling Interval
Modify the delay value in the main loop (line 146):
```cpp
delay(30000); // 30 seconds (30000ms)
```

### Modify MQTT Topics
Update topic variables (lines 32-34):
```cpp
String mainTopic = "ha";
String temperatureTopic = "_temperature";
String humidityTopic = "_humidity";
```

### Change Sensor Pin
Modify the sensor initialization (line 41):
```cpp
DHTNEW am2302(D7); // Change D7 to your desired pin
```

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Author

Written by: dmazan  
Source code: https://github.com/dmazan/esp8266-mqtt-am2302

## Dependencies Credits

- ESP8266 Arduino Core: https://github.com/esp8266/Arduino
- DHT New Library: https://github.com/RobTillaart/DHTNew
- PubSubClient: https://github.com/knolleary/pubsubclient