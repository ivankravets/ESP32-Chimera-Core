# ESP32-Chimera-Core Library

![Illustration By Jacopo Ligozzi](https://user-images.githubusercontent.com/1893754/71980273-ed9bb100-321f-11ea-8982-49702af29c9f.png)


This library is a *substitute* of the original [M5Stack](https://github.com/m5stack/M5Stack/) library, with added support for the following devices:

  - [M5Stack Classic](https://m5stack.com/products/basic-core-iot-development-kit)
  - [M5Stack Fire](https://m5stack.com/collections/m5-core/products/fire-iot-development-kit)
  - [Odroid-Go](https://www.hardkernel.com/shop/odroid-go/)
  - [D-Duino-32-XS](https://www.tindie.com/products/lspoplove/dstike-d-duino-32-xs/)
  - [ESP32-Wrover-Kit (v4.1, v4.2, v4.3)](https://www.sparkfun.com/products/14917)
  - [Lilygo TTGO-TS](https://www.banggood.com/LILYGO-TTGO-TS-ESP32-1_44-Inch-TFT-MicroSD-Card-Slot-Speakers-bluetooth-Wifi-Module-p-1273383.html)
  
Support coming soon:

  - [LoLin D32-Pro](https://www.aliexpress.com/item/32883116057.html)
  - [M5StickC](https://m5stack.com/collections/m5-core/products/stick-c)
  - [M5Atom](https://m5stack.com/collections/m5-core/products/atom-matrix-esp32-development-kit)


It also implements a set of extra features:

  - Zero-config automatic device selection based on the Arduino Boards menu selection
  - Screenshots!
  - I2C Scanner


# Usage

  - Download the latest release and unzip it in the `~/Arduino/libraries` folder.
  - Delete the `~/Arduino/M5Stack` folder to prevent any confusion at compilation (you can still restore it later using the Library Manager)
  - You're done!

# Developers

Use `#if defined(_CHIMERA_CORE_)` macros to isolate ESP32-Chimera-Core specific statements.

  ```C
    #if defined(_CHIMERA_CORE_)
      M5.ScreenShot.init( &M5.Lcd, M5STACK_SD );
      M5.ScreenShot.begin();
      M5.ScreenShot.snap();
    #endif

  ```

Automatic board selection is based on the boards.txt definition, so changing the board from the Arduino Tools menu is enough to trigger the detection.
Sketch compilation can eventually be tuned-up to a specific device by using macros.

  ```C
    #if defined(_CHIMERA_CORE_)
      #if defined( ARDUINO_M5Stack_Core_ESP32 )
        #warning M5STACK CLASSIC DETECTED !!
      #elif defined( ARDUINO_M5STACK_FIRE )
        #warning M5STACK FIRE DETECTED !!
      #elif defined( ARDUINO_ODROID_ESP32 )
        #warning ODROID DETECTED !!
      #elif defined( ARDUINO_TTGO_T1 )
        #warning Lilygo TTGO-TS DETECTED !!
      #elif defined ( ARDUINO_ESP32_DEV )
        #warning WROVER DETECTED !!
      #else
        #warning NOTHING DETECTED !!
      #endif
    #else
      #warning M5Stack legacy core detected
    #endif
  ```


# Credits & Thanks

  - Special thanks to [らびやん (Lovyan03)](https://github.com/lovyan03) for providing strong inspiring support, unconditional kindness and unlimited patience
  - Kudos to [M5Stack](https://github.com/m5stack) for being the creators of the original M5Stack and its legacy library without which this project would not exist.
  - [紅樹　タカオ (Takao Akaki)](https://github.com/mongonta0716)
  - [Nochi](https://github.com/shikarunochi)
  - [こばさん](https://github.com/wakwak-koba)
  - [Illustration By Jacopo Ligozzi](https://commons.wikimedia.org/w/index.php?curid=53514521)
