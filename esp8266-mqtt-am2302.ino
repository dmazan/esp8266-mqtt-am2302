/***************************************************************************************************/
/* 
   This Sketch runs on an ESP8266 controller with integrated WiFi stack, interfaces with an AH2302
   high resolution temperature and humidity senseor via I2C and publishes the collected temperature
   and humidity values via a MQTT broker in a format that can be consumed by OpenHAB.
   
   written by : dmazan
   sourse code: https://github.com/dmazan/esp8266-mqtt-am2302

   Frameworks & Libraries:
   ESP8266 - https://github.com/esp8266/Arduino
   AM2302  - https://github.com/RobTillaart/DHTNew
   MQTT    - https://github.com/knolleary/pubsubclient

   GNU GPL license, all text above must be included in any redistribution,
   see link for details  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#include <dhtnew.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <secret.h>

/* Local Configuration - ALL_CAPS identifiers are defined in secret.h */

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASSWORD;
const char* mqtt_hostname = SECRET_MQTT_SERVER;
IPAddress mqtt_server_ip;

String mainTopic = "ha";
String temperatureTopic = "_temperature";
String humidityTopic = "_humidity";

uint8_t readStatus = 0;
float temperature = 0;
float humidity = 0;
String clientId = "";

DHTNEW am2302(D7);
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  WiFi.mode(WIFI_STA);
  clientId = String(WiFi.macAddress()).substring(15);
  clientId.toLowerCase();
  temperatureTopic = mainTopic + "/_" + clientId + "/" + temperatureTopic;
  humidityTopic = mainTopic + "/_" + clientId + "/" + humidityTopic;
  Serial.println("Client Id: " + clientId);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Resolving MQTT hostname: ");
    Serial.println(mqtt_hostname);
    
    if (WiFi.hostByName(mqtt_hostname, mqtt_server_ip)) {
      Serial.print("MQTT broker resolved to IP: ");
      Serial.println(mqtt_server_ip);
      client.setServer(mqtt_server_ip, 1883);
      
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect("arduinoClient_temperature_sensor")) {
        Serial.println("connected");
        return; // Successfully connected, exit function
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
    } else {
      Serial.println("DNS resolution failed! Retrying in 5 seconds...");
    }
    
    // Wait 5 seconds before retrying (for both mDNS failure and MQTT connection failure)
    delay(5000);
  }
}

void setup()
{
  WiFi.persistent(false);  //disable saving wifi config into SDK flash area
  //WiFi.forceSleepBegin();  //disable AP & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep
  Serial.begin(115200);
  Serial.println("Initialize AM2302");

  Serial.print("Type: ");
  Serial.println(am2302.getType());
  
  //while (am2315.dataReady != true)
  //{
   // Serial.println(F("AM2315 not connected or not responding")); //(F()) save string to flash & keeps dynamic memory free
   // delay(5000);
  //}
  //Serial.println(F("AM2315 OK"));

  //Wire.setClock(400000); //experimental I2C speed! 400KHz, default 100KHz

  setup_wifi();
}


void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  if (millis() - am2302.lastRead() > 2000) {

    am2302.read();
    
    temperature = am2302.getTemperature();
    Serial.print(F("Temperature: ")); Serial.print(temperature);
    client.publish(temperatureTopic.c_str(), String(temperature).c_str(), true);
    Serial.println("...published");
  
    humidity = am2302.getHumidity();
    Serial.print(F("Humidity...: ")); Serial.print(humidity);
    client.publish(humidityTopic.c_str(), String(humidity).c_str(), true);
    Serial.println("...published");
    Serial.println();
  }
  
  delay(30000); //recomended polling frequency 8sec..30sec
}
