# Kelompok 2 - Pemantau Suhu dan Kelembapan

## Anggota dan peran
| Nama | NIM | Peran |
| :--- | :--- | :--- |
| Septyana Dwi Rahmawaty | 125103024 | B. Sensor dan Firmware |
| Imelda Cikal Pratiwi | 125103023 | C. Platform dan Akses Internet |
| Fatihaturrohmah | 125103007 | D. Data dan Dashboard |
| Farrell Alvaro Wielianto | 125103011 | A. Network dan RPI |

## Arsitektur

```
+-------------------+      WiFi 2,4 GHz      +-----------------------------------------------------------------+
|   ESP32 + DHT22   | ---------------------> |                       Raspberry Pi (rpi-kel2)                       |
| (Interval 5 dt)   |       MQTT :1883       |                                                                 |
+-------------------+                        |  +------------+     +----------+     +------------+     +---------+  |
                                             |  | Mosquitto  | --> | Telegraf | --> | InfluxDB 2 | --> | Grafana |  |
                                             |  |  (:1883)   |     |  (MQTT)  |     |  (:8086)   |     | (:3000) |  |
                                             |  +------------+     +----------+     +------------+     +---------+  |
                                             +--------------------------------------------------------------+--+
                                                                                                            |
+-------------------+     HTTPS (Publik)     +--------------------+            Localhost :3000              |
|  Browser Publik   | <--------------------- |  Tailscale Funnel  | <---------------------------------------+
|   (Wajib Login)   |                        | (Layanan systemd)  |
+-------------------+                        +--------------------+

*Jalur Manajemen Terpisah (Bukan Jalur Data):
[Mahasiswa] <---------> [Raspberry Pi Connect] <---------> [Terminal & Desktop rpi-kel2]
```

## Hasil uji per tahap
| Tahap | Bukti | Tanggal |
| :--- | :--- | :--- |
| 1. RPI ke Internet | | Nov 2026 |
| 2. Sensor ke RPI || Nov 2026 |
| 3a. Broker MQTT | | Nov 2026 |
| 3b. Akses Internet | Nov 2026 |
| 4. Basisdata dan Dashboard | | Nov 2026 |

## Akses publik
- **Alamat dashboard:** `https://rpi-kel2.tailbcfae1.ts.net` *(isi dengan URL Tailscale Funnel Kelompok 2)*
- **Jalur tunnel yang dipakai:** Tailscale Funnel (meneruskan Grafana port `3000` via HTTPS)

## Kendala dan solusi
- **Gejala:** **
  - **Penyebab:** ...
  - **Perbaikan:** ...

## Cara menjalankan ulang
1. **Persiapan Raspberry Pi & Keamanan Dasar:**
   sudo ufw allow in on lo to any port 1883
   sudo ufw allow from 192.168.0.0/16 to any port 1883
   sudo ufw enable
