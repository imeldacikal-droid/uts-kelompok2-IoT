#ifndef SECRETS_H
#define SECRETS_H

// Pengaturan Wi-Fi
const char* WIFI_SSID     = "NAMA_WIFI_KAMU";
const char* WIFI_PASSWORD = "PASSWORD_WIFI_KAMU";

// Pengaturan Broker MQTT (Raspberry Pi)
const char* MQTT_SERVER   = "192.168.1.XXX";
const int   MQTT_PORT     = 1883;
const char* MQTT_USER     = "USERNAME_MQTT";
const char* MQTT_PASSWORD = "PASSWORD_MQTT";
const char* MQTT_TOPIC    = "iot/kel2/sensor";

#endif
