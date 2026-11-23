#ifndef SECRETS_H
#define SECRETS_H

// Pengaturan Wi-Fi & MQTT Server (SESUAIKAN DENGAN HOTSPOT HP & IP LAPTOP BARU)
const char* ssid        = "paramadina_cikarang";
const char* password    = "cikarang";
const char* mqtt_server = "192.168.xx.xx"; 

// --- TAMBAHAN KONFIGURASI STATIC IP ESP32 ---
// Pastikan 3 angka depan (192, 168, 43) sama dengan jaringan Hotspot HP kamu
IPAddress local_IP(192, 168, xx, xxx);  // IP tetap yang diinginkan untuk ESP32 (angka terakhir bebas 2-254 asal tidak bentrok)
IPAddress gateway(192, 168, xx, x);   // IP Gateway (dari remote)
IPAddress subnet(255, 255, 255, 0);     // Subnet mask standar
IPAddress primaryDNS(8, 8, 8, 8);       // DNS Google 
IPAddress secondaryDNS(8, 8, 4, 4);     // DNS Google sekunder 

// 2. Kredensial MQTT
const int   mqtt_port   = 1883;
const char* mqtt_user   = "user_mgtt";
const char* mqtt_pass   = "password_mqtt";
const char* mqtt_topic  = "iot/kel2/sensor";

#endif
