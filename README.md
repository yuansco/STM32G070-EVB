# STM32G070-EVB

<img src="https://github.com/yuansco/STM32G070-EVB/blob/main/Image/image1.PNG" style="width:750px;"/>

## Overview

The STM32G070-EVB board features an ARM Cortex-M0+ based STM32G070CBT6 MCU with a integrated USB to UART serial console. Here are some highlights of the STM32G070-EVB board:

 * ARM Cortex-M0+ processor at 64 MHz
 
 * 128 Kbytes Flash and 36 Kbytes SRAM

 * 16 MHz main crystal oscillator

 * 32.768 kHz crystal oscillator for internal RTC
 
 * On-board USB to UART serial console

 * SSD1306 based 128x64 OLED display module

 * AT24C02 2 Kbytes EEPROM
 
 * 2 user LEDs
 
 * 2 user buttons
 
 * Boot mode switch
 
 * Reset button

## Zephyr OS support

You can copy Demo/zephyr/stm32g070_evb directory to the zephyr project's zephyr/boards/arm to support hardware config of this evaluation board.

Most improtan file is `stm32g070_evb.dts` dts file, Which defines the hardware connection and components of this evaluation board.

### building hello world example

```
BOARD=stm32g070_evb
west build -p always -b $BOARD samples/hello_world
```
<img src="https://github.com/yuansco/STM32G070-EVB/blob/main/Image/image2.PNG" style="width:550px;"/>




