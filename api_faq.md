# [Index](index.html)> FAQ

* [Which pins are used for Lightning Shield?](#which-pins-are-used-for-Lightning-Shield)
* [Can I use Serial?](#can-i-use-serial)

## Which pins are used for Lightning Shield?

* Pin 0, 1, 4. Connecting anything to these pins can interfere with the communication.

![using pin](images/lnshield_pin.jpg)

## Can I use Serial?

Sorry, you cannot. Lightning Shield has already used Serial between Arduino and Raspberry Pi.  
If you really want to do so, we suggest using [`SoftwareSerial Library`](https://www.arduino.cc/en/Reference/SoftwareSerial).
