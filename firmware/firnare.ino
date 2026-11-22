#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

void setup_wifi() {
  delay(10);
  Serial.print("Menghubungkan ke WiFi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung! IP ESP32: " + WiFi.localIP().toString());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT Broker...");
    String clientId = "ESP32_Kel2_" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("Terhubung!");
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 5000) { // Kirim data setiap 5 detik
    lastMsg = now;

    float kelembapan = dht.readHumidity();
    float suhu = dht.readTemperature();

    if (isnan(kelembapan) || isnan(suhu)) {
      Serial.println("Gagal membaca sensor DHT!");
      return;
    }

    // Format JSON sesuai parser Telegraf
    String payload = "{\"suhu\":" + String(suhu, 1) + ",\"kelembapan\":" + String(kelembapan, 1) + "}";
    
    Serial.print("Mengirim data: ");
    Serial.println(payload);
    client.publish(mqtt_topic, payload.c_str());
  }
}
