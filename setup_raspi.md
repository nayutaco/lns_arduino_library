# [Index](index.html)> Raspberry Pi

* [SD card](#sd-card)
* [WiFi setting and reboot CLIENT mode](#wifi-setting-and-reboot-client-mode)


## SD card

RaspbianOS + Lightning Shield for Arduino modules

### 1. Download a SD image file

* (Download URL: under development!)

### 2. Write the image to the SD card

Please refere to [how to write a SD card image](https://www.raspberrypi.org/documentation/installation/installing-images/README.md)

### 3. Turn on the power

Before turning on the power, insert the SD card into the Raspberry Pi combined with the Lightning Shield.  
![insert](images/insert_sd.jpg)

After that, supply power through USB/AC adapter/micro-B USB and turn on the power! Do not worry about the first boot, it takes about a few minutes to show the first display. 

The [first boot](https://youtu.be/kF_WI_qtKfI) video can help you!  

## WiFi setting and CLIENT mode

### 1. First, [AP mode](setup_faq.md#ap-mode) starts

![apstart](images/wifi_00.jpg)

### 2. Connect your phone/PC to Raspberry Pi via Wi-Fi

* SSID: `PtarmServer`
* Password: `Ptarmigan`

![SSID](images/android_ssid.jpg)


* If you see the following dialog, just tap "Yes".

![no Internet](images/android_nointernet.jpg)

### 3. Open Browser

* [http://192.168.4.1/](http://192.168.4.1/)
* Authentication
  * Username: `ptarm` / Password: `ptarm`

![login](images/web_login.jpg)

### 4. **Menu: WiFi Settings > WPA_Supplicant [SSID] [PASS]**  

![ssid](images/wifi_01.jpg)

### 5. Set WiFi SSID/PASS that you want to connect to (your home, office, ...)

* In the same way to connect your phone to WiFi

### 6. **Menu: WiFi Settings > Reboot with WiFi Client Mode**  

![cli](images/wifi_02.jpg)

### 7. Raspberry Pi reboots with [CLIENT mode](setup_faq.md#client-mode)

![reboot](images/wifi_03.jpg)

### 8. CLIENT mode starts  

![cli](images/wifi_04.jpg)

### 9. Done!

You can open URL displayed IP address.

* http://IP_ADDRESS/

![ipaddr](images/ipaddr.jpg)

## NOTE

* `Syncing...` shows block downloads.
  * It takes a lot of time and sometimes restarts.
* If ePaper displays "no IP addr", Raspberry Pi does not get IP address from DHCP server.  
  ![noipaddr](images/noipaddr.jpg)
  * It might be caused by the fail Wi-Fi setting.  
* If you want to change the mode from CLIENT to AP, press Shield's switch for more than 5 sec..  
  ![cli](images/emer_01.jpg)

----

[Next> Lightning Network](setup_ln.md)
