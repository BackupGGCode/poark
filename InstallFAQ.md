# What do you need to start #

## Software ##

### Mandatory components ###

  * The Arduino SDK - http://arduino.cc (_also to be found as a package in Ubuntu_)
  * ROS - http://ros.org (_I installed it on Ubutu from the ros-diamond packages_)
  * ROS Serial - (**please read the comments below**)
  * CMake - build tool should be avaialbe as a package for you
  * **The Poark** - http://code.google.com/p/poark/ (_not a package **yet**_)

### Optional components ###

  * RosJava - http://code.google.com/p/rosjava (_If you want to use the Poark from inside java._)
  * MsTimer library for Arduino - http://www.arduino.cc/playground/Main/MsTimer2 (_highly recommended_)
  * GLCD Arduino library - http://arduino.cc/forum/index.php/topic,56705.0.html (_for the display_, also not found as package).  This replaces the previous KS0108 dependency and does at the same time allow for using a wider variety of LCDs.
  * T6963C Arduino Library - http://en.radzio.dxp.pl/t6963/ . An alternative to GLCD for displays with the T6963C chipset. (_Have to be downloaded and installed manually! Possibly you'll need to configure different IO pins._)

## Hardware ##

  * Arduiono Board (_preferably Mega 1280 or Mega 2560_)
  * LCD with KS0108 or T69663C controller (_Optional. For debug output only._)
  * Some buttons, LEDs etc or whatever you got for testing it.
  * Breadboard (_very handy for doing fast wiring_)

# Some hints and underwater riffs to what out for #

## Getting the thing to compile ##
**All this info is on the ROS page so if you feel like you need to know why go read it there**

### The Server ###

  * We have diverged from a pure Arduino pde project because of the limitations this imposes on the invocation of the build tools and the fact that the packaged rosserial packages are too bugy and we decided to go with the version of rosserial provided by the parsec project. Therefore you'll want to download the proper version from the parsec site. Instructions for that you can find in [here](http://code.google.com/p/poark/wiki/ParsecRosserial). Do the installation step from this document then come back here to finish the list of prerequisites before you proceed with compiling the project. Alternatively if you still wanto to use the rosserial package that comes with ros make sure the ros serial lib is also either copied or linked in the arduino _libraries_ directory (on a Ubuntu install from deb packages you have to link (/opt/ros/diamondback/stacks/rosserial/rosserial\_arduino/libraries/ros\_lib into /usr/share/arduino/libraries)

  * Make sure you have the GLCD/T6963C and MsTimer2 libs in the arduino _libraries_ directory.

  * If you don't have a display comment out or set to 0 the definition of `LCD_DEBUG`.

  * Now you should be set to build the Poark using the following commands:
```
cmake -DARDUINO_BOARD=<board_type> -DARDUINO_PORT=<device>
make
make upload
```

In the first command replace **board\_type** with your particular board model (for example mega2560 is the Arduino Mega board with Atmel 2560 chip and mega is the same board with Atmel 1280 chip on it) and the device with the proper device where the board is attached like /dev/ttyUSB0.
  * If all worked well you should see **Ready** on the LCD, provided you have one ;)

### The Client ###

  * Get to learn ROS and ROS Serial (_see links above_).
  * Before trying to build set your environment properly. _Hint: ROS\_PACKAGE\_PATH=/path/to/poak:$ROS\_PACKAGE\_PATH_
  * Run rosmake in the poark\_client dir.
  * If it gives you any errors go back to tip one.
  * To run it you need to have running
    * roscore
    * rosrun rosserial\_python serial\_node.py /dev/ttyUSB0 (_substitute ttyUSB0 with your proper device name_)
    * poark\_client

### RosJavaPoark ###

Use the instructions on [this page](http://code.google.com/p/poark/wiki/RosJavaPoark) to set up and use RosJavaPoark.

# Have fun and may the shworz be with you! #