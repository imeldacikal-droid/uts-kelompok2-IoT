#include <WiFi.h>
#include <secret.h>
#include <PubSubClient.h>
#include <DHT.h>

// Pengaturan Pin Sensor DHT
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Inisialisasi Web Server di port 80 & MQTT Client
WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
float suhu = 0.0;
float kelembapan = 0.0;

// Halaman web sederhana saat IP ESP32 dibuka di browser
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='5'>"; // Auto-refresh tiap 5 detik
  html += "<title>Monitoring Sensor ESP32</title>";
  html += "<style>body{font-family:sans-serif;text-align:center;margin-top:50px;}";
  html += ".card{display:inline-block;padding:20px;border:1px solid #ccc;border-radius:10px;}</style></head><body>";
  html += "<div class='card'><h2>Data Sensor DHT22 (Kelompok 2)</h2>";
  html += "<p><strong>IP ESP32:</strong> " + WiFi.localIP().toString() + "</p>";
  html += "<p><strong>Suhu:</strong> " + String(suhu, 1) + " &deg;C</p>";
  html += "<p><strong>Kelembapan:</strong> " + String(kelembapan, 1) + " %</p>";
  html += "<p><small>Status MQTT: " + String(client.connected() ? "Terhubung" : "Terputus") + "</small></p>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke Wi-Fi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);

  // Terapkan konfigurasi IP Statis sebelum WiFi.begin()
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Gagal mengkonfigurasi Static IP!");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi berhasil terhubung!");
  Serial.print("Alamat IP Statis ESP32 (Buka di browser): http://");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  if (!client.connected()) {
    Serial.print("Mencoba konek ke MQTT Broker...");
    String clientId = "ESP32_Kel2_" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("BERHASIL terhubung ke MQTT!");
    } else {
      Serial.print("GAGAL, kode error rc=");
      Serial.println(client.state());
    }
  }
}

void setup() {
  Serial.begin(115200); // Inisialisasi Serial Monitor
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  dht.begin();
  setup_wifi();
  
  // Nyalakan LED biru bawaan ESP32 sebagai tanda sudah konek Wi-Fi
  digitalWrite(2, HIGH);

  // Jalankan Web Server lokal di ESP32
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web Server HTTP aktif.");

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(2, LOW);
    Serial.println("Wi-Fi terputus! Menyambungkan ulang...");
    setup_wifi();
    digitalWrite(2, HIGH);
  }

  server.handleClient();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Gagal membaca sensor DHT22! Cek kabel di Pin D4.");
      return;
    }

    kelembapan = h;
    suhu = t;

    Serial.print("Suhu: ");
    Serial.print(suhu, 1);
    Serial.print(" °C | Kelembapan: ");
    Serial.print(kelembapan, 1);
    Serial.println(" %");

    // Kirim ke MQTT Broker
    if (client.connected()) {
      String payload = "{\"suhu\":" + String(suhu, 1) + ",\"kelembapan\":" + String(kelembapan, 1) + "}";
      client.publish(mqtt_topic, payload.c_str());
      Serial.println("Data terkirim ke MQTT: " + payload);
    }
  }
}
