#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Adafruit_AHTX0.h"

// Thiết lập tên và mật khẩu của mạng Wi-Fi
const char* ssid = "Space65";
const char* password = "28112001";

// Thiết lập thông tin broker MQTT
const char* mqtt_server = "641712bdeca84cd99a23e6b291cbf298.s2.eu.hivemq.cloud";
const uint16_t mqtt_port = 8883;
const char* mqtt_user = "20520597";
const char* mqtt_password = "Brs13125";

// Thiết lập chủ đề MQTT để gửi dữ liệu nhiệt độ
const char* pub_topic = "20520597/pub";
const char* sub_topic = "esp32/sub";

// Khai báo đối tượng MQTT client và đối tượng WiFiClientSecure
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

// Khai báo đối tượng cảm biến nhiệt độ AHT20
Adafruit_AHTX0 aht;

// Hàm kết nối đến mạng WiFi
void connectToWiFi() {
Serial.print("Đang kết nối WiFi...");
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

Serial.println(" Đã kết nối với WiFi");
}

// Hàm kết nối đến MQTT broker
void connectToMQTT() {
Serial.print("Đang kết nối MQTT broker...");
String clientId = "ESP32-"; // Định danh cho đối tượng kết nối
clientId += String(random(0xffff), HEX);

wifiClient.setInsecure();
client.setServer(mqtt_server, mqtt_port);

if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
Serial.println(" Đã kết nối với MQTT broker");
}
else {
Serial.print("Kết nối thất bại, mã lỗi: ");
Serial.print(client.state());
Serial.println(", thử lại sau 5 giây");
delay(5000);
}

void setup() {
Serial.begin(9600);

connectToWiFi();

if (!aht.begin()) {
Serial.println("Không tìm thấy cảm biến AHT20!");
while (1);
}

connectToMQTT();
}

void loop() {
sensors_event_t humidity, temp;
aht.getEvent(NULL, &temp); // Đọc nhiệt độ từ cảm biến AHT20

StaticJsonDocument<200> doc; // Tạo đối tượng StaticJsonDocument để lưu trữ dữ liệu JSON
doc["Nhiệt độ"] = temp.temperature; // Lưu nhiệt độ vào đối tượng StaticJsonDocument
char jsonBuffer[512]; // Tạo bộ đệm để lưu trữ chuỗi JSON
serializeJson(doc, jsonBuffer); // Chuyển đối tượng StaticJsonDocument sang chuỗi JSON và lưu vào bộ đệm

client.publish(pub_topic, jsonBuffer); // Gửi chuỗi JSON đến MQTT broker

Serial.println(jsonBuffer); // Hiển thị chuỗi JSON trên Serial Monitor

delay(5000);
}