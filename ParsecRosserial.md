# ROSSerial for Arduino #

The current version of ROSSerial that comes with ROS Electric version is API incompatible with previous versions.  As Ubuntu 11.10 only provides packages for this ROS version, it has been suggested to upgrade the Poark server to be compatible with this version (see [issue 27](https://code.google.com/p/poark/issues/detail?id=27)).  A patch has even been provided.

Unfortunately, there seems to be issues with Arduino support in the current ROSSerial which is the default ROS version provided by Ubuntu 11.10.  Messages sent to the Arduino seem to get seriously mangled.  The problem can be resolved using the ROSSerial implementation in the Parsec project.

The following document attempt to outline the steps needed to install and use the Parsec ROSSerial version.


## Installing parsec.rosserial ##

  1. Download parsec.rosserial using Mercurial:
```
hg clone https://code.google.com/p/parsec.rosserial/
```
  1. Build the new ROS stack, a make in the checked out directory probably works... (Please verify).   If not, consult http://www.ros.org/wiki/ROS/Tutorials/StackInstallation.
  1. Make ROS aware of your new stack by prepending the parsec.rosserial directory to your `ROS_PACKAGE_PATH`.  That is, add the following to your start up script after sourcing the ROS setup.sh:
```
export ROS_PACKAGE_PATH="<path_to_parsec.rosserial>:$ROS_PACKAGE_PATH"
```


## Using Poark with the parsec.rosserial stack ##

You should now be setup to use the new ROS stack.

  1. Checkout a version of Poark that support the new ROS syntax.  When writing this, the your best bet is to checkout `gh/config-3`.
  1. Run CMake in the poark\_server directory.  When CMaking the Poark server, be sure to set `ARDUINO_BOARD` (`mega` (Arduino Mega 1280), `mega2560` (Arduino Mega 2560),...) and `ARDUINO_PORT` correctly.
```
cmake -DARDUINO_BOARD=mega2560 -DARDUINO_PORT=/dev/ttyACM0
```

It should now be possible to compile and upload the Poark server both from the command line and from the Arduino IDE.  From the command line:
```
make && make upload
```