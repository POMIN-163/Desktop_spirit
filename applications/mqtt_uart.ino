/*
 * @Author: Pomin
 * @Date: 2021-11-16 11:47:52
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-11-16 15:24:57
 * @Description:
 */
#include <SPI.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FastLED.h>

#define LED_PIN     2        
#define NUM_LEDS    3     
CRGB leds[NUM_LEDS];

// const char* ssid = "Landian04_2.4G"; // Enter your WiFi name
// const char* password = "Landian04";  // Enter WiFi password
const char* mqtt_broker    = "broker-cn.emqx.io";
const char* mqtt_subscribe = "/pomin/mqtt/toMcu";
const char* mqtt_publish   = "/pomin/mqtt";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wm;

/* EEPROM参数存储地址位 */
int wifi_addr = 10; /* 被写入数据的EEPROM地址编号 */
const char* wifi_config_ap = "ESP-01 SAT";

struct config_type {
    char stassid[32]; /* 定义配网得到的WIFI名长度(最大32字节) */
    char stapsw[64];  /* 定义配网得到的WIFI密码长度(最大64字节) */
};
config_type wificonf = { {""},{""} };

void saveParamCallback() {

}
/* wifi ssid，psw保存到eeprom */
void savewificonfig() {
    //开始写入
    char* p = (char*)(&wificonf);
    for (int i = 0; i < sizeof(wificonf); i++) {
        EEPROM.write(i + wifi_addr, *(p + i));
    }
    delay(10);
    if (EEPROM.commit()) {
        Serial.println("保存配置成功");
    } else {
        Serial.println("保存配置失败");
    }
    delay(10);
}
/* 删除原有eeprom中的信息 */
void deletewificonfig() {
    config_type deletewifi = { {""},{""} };
    char* p = (char*)(&deletewifi);
    for (int i = 0; i < sizeof(deletewifi); i++) {
        EEPROM.write(i + wifi_addr, *(p + i));
    }
    delay(10);
    if (EEPROM.commit()) {
        Serial.println("清空配置成功");
    } else {
        Serial.println("清空配置失败");
    }
    delay(10);
}
/* 从eeprom读取 WiFi信息ssid，psw */
void readwificonfig() {
    uint8_t* p = (uint8_t*)(&wificonf);
    for (int i = 0; i < sizeof(wificonf); i++) {
        *(p + i) = EEPROM.read(i + wifi_addr);
    }
    // EEPROM.commit();
    // ssid = wificonf.stassid;
    // pass = wificonf.stapsw;
    Serial.printf("读取WiFi配置.....\r\n");
    Serial.printf("SSID:%s\r\n",wificonf.stassid);
    Serial.printf("PSW:%s\r\n", wificonf.stapsw);
}
/* WEB配网 */
void Webconfig() {
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    delay(3000);
    wm.resetSettings(); // wipe settings
    int customFieldLength = 40;
    wm.setSaveParamsCallback(saveParamCallback);
    std::vector<const char*> menu = { "wifi","restart" };
    wm.setMenu(menu);
    wm.setClass("invert");
    wm.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in scans, null = 8%
    bool res;
    res = wm.autoConnect(wifi_config_ap); // anonymous ap
    while (!res);
}
void callback(char* topic, byte* payload, unsigned int length) {
    static bool sw = true;
    // Serial.print("收到消息, 来自: ");
    // Serial.println(topic);
    // Serial.print("消息:");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    sw = !sw;
    if (sw) {
        digitalWrite(2, HIGH);
    } else {
        digitalWrite(2, LOW);
    }
    // Serial.println();
    // Serial.println("-----------------------");
}
void setup() {
    int load_timeout = 0;
    pinMode(2, OUTPUT);

    Serial.begin(9600);
    EEPROM.begin(1024);
    Serial.println("\n------------------------");

    readwificonfig();

    WiFi.begin(wificonf.stassid, wificonf.stapsw);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        load_timeout++;
        Serial.println("连接WiFi....");
        if (load_timeout > 20) {
            Serial.println("连接WiFi失败, 开启手动配置");
            Webconfig();
            break;
        }
    }
    if (WiFi.status() == WL_CONNECTED) {
        strcpy(wificonf.stassid, WiFi.SSID().c_str()); // 名称复制
        strcpy(wificonf.stapsw, WiFi.psk().c_str());   // 密码复制

        savewificonfig();
        readwificonfig();

        Serial.println("连接WiFi成功");

        client.setServer(mqtt_broker, mqtt_port);
        client.setCallback(callback);
        while (!client.connected()) {
            Serial.println("连接mqtt服务器.....");
            if (client.connect("esp8266-client")) {
                Serial.println("连接mqtt服务器成功");
            } else {
                Serial.print("连接mqtt服务器失败");
                Serial.print(client.state());
                delay(2000);
            }
        }
        /* 订阅主题 */
        // client.publish(mqtt_publish, "hello emqx");
        client.subscribe(mqtt_subscribe);
    }
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(100);  //可以设置全局亮度，调低亮度不刺眼
}
int h = 0;
int times = 0;
void loop() {
    client.loop();
    /* 判读是否串口有数据 */
    if (Serial.available() > 0) {
        String comdata = "";
        while (Serial.available() > 0) {
            /* 叠加数据到comdata */
            comdata += char(Serial.read());
            delay(2);
        }
        /* 如果串口有数据 */
        if (comdata.length() > 0) {
            Serial.println(comdata);
            client.publish(mqtt_publish, comdata.c_str());
        }
    }
    times++;
    if(times > 5000) {
      for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CHSV((h + (255 / NUM_LEDS) * i), 255, 255); //用HSV色彩空间，不断改变H即可
          FastLED.show();
      }      
       h = (h + 3) % 255; 
       times = 0;
    }
}
