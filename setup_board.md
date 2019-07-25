# [Index](index.html)> Board

First of all, let's prepare to use the board.

* [Lightning Shield for Arduino](#lightning-shield-for-arduino)
* [Arduino pin](#arduino-pin)
* [ePaper](#epaper)
* [Power supply](#power-supply)  

## Lightning Shield for Arduino

### Things need to be ordered at Nayuta Lightning shop  

Board  
![lnshield back](images/board_back.jpg)

ePaper  
![epaper](images/epaper.jpg)

### Things need to be prepared by yourself

Raspberry Pi Zero W  
![pin side](images/raspi_pin.jpg)

Arduino UNO  
![arduino](images/arduino.jpg)


## Arduino pin

you need to solder pins to connect Arduino.  
![solder](images/arduino_solder.jpg)

## ePaper

[Please watch the how-to-use video](https://youtu.be/c-4CIAWcO2I)  
![image](images/youtube_epaper.jpg)  

### 1. Pull the ePaper's connectors

Pull gently...  
  ![pull](images/epaper1.jpg)

Pulled!  
  ![pulled](images/epaper2.jpg)

### 2. Insert the cable and push the connectors carefully...

  ![insert](images/epaper3.jpg)

Pushed!  
  ![insert](images/epaper4.jpg)

## Power supply

There are three ways to supply power to Raspberry Pi.

![power1](images/power_supply1.jpg)

1. USB
2. AC adapter
3. micro-B USB (see [NOTE](#note))

### NOTE

If you supply power through micro-B USB, please change the Lightning Shield's jumper located near the micro-B connector.  
![power2](images/power_supply2.jpg)

* If through Arduino  
  ![ar](images/supply_ar.jpg)

* If through micro-B  
  Separate the jumper from Aduino  
  ![usb](images/supply_usb.jpg)

----

[Next> Raspberry Pi](setup_raspi.md)
