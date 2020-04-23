#include <WifiEspNow.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

//#include <PirSensor.h>
#include <Ticker.h>
#include <ArduinoJson.h>

void repeatSwitch();
Ticker timer1(repeatSwitch, 500);
int repeatCount;
StaticJsonDocument<64> doc;
char json[64];

//static uint8_t PEER[]{0x52, 0x02, 0x91, 0xDA, 0x7B, 0x0C};
static uint8_t PEER[]{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//PirSensor motion = PirSensor(MOTION_SENSOR_PIN, 2, false, false); 

/* void printReceivedMessage(const uint8_t mac[6], const uint8_t *buf, size_t count, void *cbarg)
{
  if (digitalRead(BUTTON_PIN) == 0)
  {
    Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    for (size_t i = 0; i < 6; ++i)
    {
      PEER[i] = mac[0];
    }
  }
}
 */
void sendData()
{
  //int len = snprintf(json, sizeof(json), "\"Cmd\": \"TOGGLE\",\"Param\": \"%u\"", repeatCount);
  doc["Cmd"] = "TOGGLE";
  doc["Param"] = repeatCount;
  size_t len = serializeJson(doc, json);

  WifiEspNow.send(PEER, reinterpret_cast<const uint8_t *>(json), len);
  Serial.write(json);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  //motion.begin();

  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Switch", nullptr, 3);
  WiFi.softAPdisconnect(false);
  // Serial.print("MAC address of this node is ");
  //Serial.println(WiFi.softAPmacAddress());
  bool ok = WifiEspNow.begin();

  if (!ok)
  {
    //Serial.println("WifiEspNow.begin() failed");
    ESP.restart();
  }

  // WifiEspNow.onReceive(printReceivedMessage, nullptr);
  ok = WifiEspNow.addPeer(PEER);
  if (!ok)
  {
    //Serial.println("WifiEspNow.addPeer() failed");
    ESP.restart();
  }

  sendData();
  timer1.start();
  //ESP.deepSleep(0);
}

void loop()
{
  // put your main code here, to run repeatedly:1
  delay(1);
  timer1.update();
}

void repeatSwitch()
{
  repeatCount++;
  sendData();
}
