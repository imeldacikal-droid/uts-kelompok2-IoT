# Kelompok 2 - Pemantau Suhu dan Kelembapan
 
Sistem pemantau suhu dan kelembapan udara dari ujung ke ujung. Sensor DHT22 pada ESP32 mengirim data lewat WiFi (MQTT) ke Raspberry Pi di kampus, data disimpan di InfluxDB, dan dashboard Grafana dibuka ke Internet lewat Tailscale Funnel.
 
## Anggota dan peran
 
| Nama | NIM | Peran |
| :--- | :--- | :--- |
| Farrell Alvaro Wielianto | 125103011 | A. Network dan RPI |
| Septyana Dwi Rahmawaty | 125103024 | B. Sensor dan Firmware |
| Imelda Cikal Pratiwi | 125103023 | C. Platform dan Akses Internet |
| Fatihaturrohmah | 125103007 | D. Data dan Dashboard |
 
Koordinator: [nama]. Pengaudit: A mengaudit B, B mengaudit C, C mengaudit D, D mengaudit A.
 
## Arsitektur
 
```text
+-------------------+   WiFi 2,4 GHz +--------------------------------------------------------------------+
|   ESP32 + DHT22   | -------------->|                      Raspberry Pi (rpi-kel2)                       |
|  (Interval 5 dt)  |   MQTT :1883   |                                                                    |
+-------------------+                | +------------+     +----------+     +------------+     +---------+ |
                                     | | Mosquitto  | --> | Telegraf | --> | InfluxDB 2 | --> | Grafana | |
                                     | |  (:1883)   |     |  (MQTT)  |     |  (:8086)   |     | (:3000) | |
                                     | +------------+     +----------+     +------------+     +---------+ |
                                     |                                                                    |
                                     +-------------------------------------------------------------+------+
                                                                                                   |
+-------------------+ HTTPS (Publik) +-------------------+        Localhost :3000                  |
|  Browser Publik   |<-------------->| Tailscale Funnel  |<----------------------------------------+
|   (Wajib Login)   |                | (Layanan systemd) |
+-------------------+                +-------------------+
 
*Jalur Manajemen Terpisah (Bukan Jalur Data):
[Mahasiswa] <---------> [Raspberry Pi Connect] <---------> [Terminal & Desktop rpi-kel2]
```
 
ESP32 dan DHT22 mengirim JSON `{"suhu": 28.4, "kelembapan": 63.2}` setiap 5 detik ke topik `iot/kel2/dht22` di Mosquitto pada RPI (port 1883, dengan user dan password). Telegraf berlangganan topik itu dan menulisnya ke InfluxDB 2.x (bucket `iot`). Grafana membaca InfluxDB dan menampilkan dashboard. Hanya Grafana (port 3000) yang dibuka ke Internet, lewat Tailscale Funnel. Raspberry Pi Connect dipakai untuk mengelola RPI dan bukan jalur data.
 
## Hasil uji per tahap
 
| Tahap | Bukti | Tanggal |
| :--- | :--- | :--- |
| 1. RPI ke Internet | [Pi Connect](docs/tahap1/01-pi-connect.png), [ip a](docs/tahap1/03-ip-a.png), [ufw status](docs/tahap1/04-ufw.png), [ping](docs/tahap1/05-ping.png), [login SSH key](docs/tahap1/06-ssh-key.png) | 18 Nov 2026 |
| 2. Sensor ke RPI | [menunggu: foto rangkaian, serial monitor, pesan di RPI] | [tanggal] |
| 3a. Broker MQTT | [status](docs/tahap3a/01-status.png), [konfigurasi](docs/tahap3a/02-konfigurasi.png), [sub dan pub](docs/tahap3a/03-sub-pub.png), [anonim ditolak](docs/tahap3a/04-anonim-ditolak.png) | 18 Nov 2026 |
| 3b. Akses Internet | [menunggu: alamat publik dari data seluler, halaman login] | [tanggal] |
| 4. Basisdata dan dashboard | [menunggu: dashboard data hidup, query, data setelah reboot] | [tanggal] |
 
## Akses publik
 
- Alamat dashboard: https://rpi-kel2.tailbcfae1.ts.net
- Jalur tunnel: Tailscale Funnel (meneruskan Grafana port 3000 lewat HTTPS)
- Bukti akses dari jaringan seluler: [menunggu]
- Bukti halaman meminta login: [menunggu]
## Video demo
 
[Tautan video demo 3-5 menit menyusul]
 
## Checklist uji akhir
 
Centang hanya bila ada bukti di `docs/`.
 
- [ ] RPI terbuka lewat Raspberry Pi Connect, hostname `rpi-kel2`, SSH key aktif, ufw aktif.
- [ ] DHT22 terbaca stabil di ESP32 selama 10 menit tanpa `nan`.
- [ ] ESP32 mengirim JSON ke topik `iot/kel2/dht22` memakai user dan password.
- [ ] `mosquitto_sub` di RPI menerima pesan tanpa terputus.
- [ ] Data masuk ke bucket InfluxDB dan tetap ada setelah RPI dinyalakan ulang.
- [ ] Dashboard Grafana menampilkan suhu dan kelembapan terbaru, serta satu alert suhu.
- [ ] Alamat publik terbuka dari jaringan seluler dan memerlukan login.
- [ ] Tidak ada port selain Grafana yang terbuka ke Internet.
- [ ] Tidak ada password atau token di repositori.
- [ ] Video demo, log kontribusi, dan README.md sudah terkumpul.
## Kendala dan solusi
 
| Gejala | Penyebab | Perbaikan |
| :--- | :--- | :--- |
| `ufw: command not found` | ufw belum terpasang | `sudo apt install ufw` |
| `ssh-keygen` tidak dikenali di CMD Windows | OpenSSH tidak ada di PATH | Pakai path lengkap `C:\Windows\System32\OpenSSH\ssh-keygen.exe` |
| Login SSH key tetap minta password | Key diketik manual dan ada karakter salah | Kirim key dengan `type ... \| ssh ...`, bukan diketik |
| `unable to resolve host rpi-kel2` | `/etc/hosts` masih berisi nama lama | Tambah `127.0.1.1 rpi-kel2` |
| `mosquitto_pub` gagal | Publish memakai wildcard `#`, opsi user `-U` huruf besar | Publish pakai topik lengkap, user pakai `-u` huruf kecil |
| [kendala Tahap 2, 3b, dan 4 yang benar-benar dialami] | | |
 
## Cara menjalankan ulang
 
### Tahap 1: Raspberry Pi
 
```bash
sudo apt update && sudo apt full-upgrade -y
sudo hostnamectl set-hostname rpi-kel2
echo "127.0.1.1 rpi-kel2" | sudo tee -a /etc/hosts
sudo apt install -y ufw
sudo ufw allow 22/tcp
# sesuaikan subnet dengan jaringan RPI (cek dengan: ip a)
sudo ufw allow from 192.168.0.0/20 to any port 1883 proto tcp
sudo ufw allow from 192.168.0.0/20 to any port 3000 proto tcp
sudo ufw enable
sudo ufw status
```
 
SSH key (dari laptop): buat dengan `ssh-keygen -t ed25519`, tambahkan kunci publik ke `~/.ssh/authorized_keys` di RPI, lalu jalankan `chmod 700 ~/.ssh` dan `chmod 600 ~/.ssh/authorized_keys`.
 
### Tahap 2: ESP32 dan DHT22
 
Salin `firmware/secrets.example.h` menjadi `secrets.h` dan isi nilainya (berkas ini tidak diunggah ke repositori). Buka `firmware/firmware.ino` di Arduino IDE, pilih board ESP32 Dev Module, lalu unggah. Serial Monitor (115200 baud) menampilkan suhu dan kelembapan setiap 5 detik.
 
### Tahap 3a: Mosquitto
 
```bash
sudo apt install -y mosquitto mosquitto-clients
sudo mosquitto_passwd -c /etc/mosquitto/passwd esp32kel2
sudo chown mosquitto:mosquitto /etc/mosquitto/passwd
sudo chmod 600 /etc/mosquitto/passwd
sudo cp rpi/mosquitto/kel2.conf /etc/mosquitto/conf.d/
sudo systemctl restart mosquitto
sudo systemctl enable mosquitto
```
 ### Tahap 3b: Tailscale Funnel

curl -fsSL [https://tailscale.com/install.sh](https://tailscale.com/install.sh) | sh
sudo tailscale up
# Aktifkan HTTPS Certificates dan fitur Funnel di Tailscale Admin Console
sudo tailscale funnel --bg 3000
sudo tailscale funnel status


### Tahap 4: InfluxDB, Telegraf, Grafana
