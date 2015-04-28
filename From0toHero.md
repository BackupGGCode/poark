# Introduction #

This document is a log of what I did to get a fresh VirtualBox installation of Kubuntu to fully functional Poark coding machine.
IT does not describe anything but only lists the plain steps I took. If you need more inromation on those or want to configure them differently please read the more thorough descriptions avaiable in the Wiki.

# Log #

  * install kubuntu with freshest updates
  * ...WAIT...
  * install ros as described on ros.org for ubuntu:
```
   sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu oneiric main" > /etc/apt/sources.list.d/ros-latest.list'
   wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
   sudo apt-get update
   sudo apt-get install ros-electric-desktop-full
   ...WAIT...
```

  * Do :
```
   apt-get install git git-gui gitk git-doc
```

  * download Arduino 64 bit v1.0 from arduino.cc

> unpack and install in /usr/share/arduino

  * download parsec as described on poarks wiki

> patch with Gustaf's patches.

  * do rosmake in the parsec folder
...WAIT...

> builds up to the rosserial\_arduino\_test cuz of the hardcoded wrong path.

> as suggested run:
```
   rosdep install rosserial_client rosserial_msgs rosserial_arduino \
   rosserial_python rosserial_xbee rosserial_arduino_test rosserial_java 
```

  * run roscore
  * run rosrun

> it failed with missing module error for 'serial' - fixed by doing:
```
   apt-get install python-serial
```
> upon rerun all is good

  * get poark from git
  * add poark\_server to the ros packages
  * do:
```
   cmake . -DARDUINO_PORT=/dev/ttyUSB0
   make
   make upload
```

  * Enjoy!