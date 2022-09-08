# ShineWorld 2.13in ePaper Driver for AVR MCU

## Motivation

While doing a ePaper (or E-Ink) related project, I found that there was no public driver library for [ShineWorld's epapers](https://shineworld.en.alibaba.com/productgrouplist-806500286/EPD.html).
As these Epaper displays were a way cheaper than other competitors, we wanted to use one of thir proudct, [SOPM021EB 2.13in ePaper](doc/datasheet-epd-sopm021eb.pdf).

I tried several common Epaper libraries, but none of them were able to display data on ShineWorld's 2.13in ePaper.
* [GxEPD](https://github.com/ZinggJM/GxEPD)
* [Adafruit EPD Library](https://github.com/adafruit/Adafruit_EPD)
* [SparkFun ePaper Library](https://github.com/sparkfun/SparkFun_ePaper_Arduino_Library)

The ePaper was using [UC8251](doc/datasheet-uc8251.pdf) as controller, and none of the above libraries have support for it.
I contacted the ShineWorld company and they provided an example code written for MSP430, which  had become the starting point of this project.

## Hardware

+ 