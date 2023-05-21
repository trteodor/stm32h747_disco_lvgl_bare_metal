# stm32h747_lvgl_master_yt
This is an repository created to become be a master of stm32h747-disco board

# Demo lvgl video / test:
   https://youtu.be/5MxftdVXUOgd

By default DSI HOST is configured in command mode, screen is divided into 10 ares to remove totally tearing effect.
For me obtained result is fine.

# QSPI
All used images by LVGL lib are stored in QSPI memory. QSPI is configured in memory mapped mode. Thanks to that it's possible to simply execute code directly from QSPI memory. Check Linker file - all lvgl image files are linked to QSPI memory adress space.

# LVGL SQUARE LINE
Configured by default example is created using Square line studio.

![LvglStudioScreenShot](https://github.com/trteodor/stm32h747_disco_lvgl_bare_metal/blob/master/Pictures/LvglStudioDemo.png)

![Discoh747board](https://github.com/trteodor/stm32h747_disco_lvgl_bare_metal/blob/master/Pictures/DiscoH747ScreenShot.png)

# PROJECT

To check changes history please call: "gitk&" in this project at him location to see changes in diffrent implementation states

This project was created only for educational purposes.
Created software is prepared for Board- DISCO-H747

* You are ready to start adventure with this examples when you have this tools in your environment (environment variables)!

    * arm-none-eabi-gcc 9.3.1
    * OpenOCD 0.11.0
    * make 4.2.1 / or 3.81
    * STM32_Programmer_CLI
    * Variable to STM32_CubeProgrammer: STM32_CubeProgrammerPATH eg. path: **D:\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin**


To clone this repo please use following command:
* git clone --recurse-submodules

Finally:

 **_Call "make" in this project at him location to compile provided example

You can call "make cubeF" to program MCU and external QSPI memory.

# Free icons
https://all-free-download.com/free-icon/pack-jpeg.html 