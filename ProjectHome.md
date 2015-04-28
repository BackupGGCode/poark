The Poark is a library that gives easy to use high lever interface to the Arduino I/O functionality. It uses the ROS framework to communicate and does not require any programming on the Arduino side.
Once the Poark server is burnt on the board it can be used from a PC or another ROS enabled device using the ros serial protocol.
The Poark currently supports the following list of I/O modes:

  * Digital input and output;
  * Interrupt driven input;
  * Filtered and unfiltered analog input through the 10bit ADC on the Arduino chip;
  * Pulse Width Modulation output;
  * Servo control;
  * I2C bus communication;
  * Serial communication.

The client side code can be written in all languages currently supported by ROS - C++, Java and Python.

The Poark library aims at people that want to use Arduino boards for research and development but don't want or don't have the time to learn how to program the Atmel controller used on those boards. It provides easy high-level interface to the main input/output capabilities of the Arduino board. It can as well serve as a learning tool for understanding those I/O capabilities and how do they interact and potentially hinder each other.

The project is currently being actively develpoed by two people in their spare time but we are open to anyone who would like to devote some time and effort either to help in the development or use the library for their project or for learning the Arduino platform. feel free to contact us through the [Issue tracker](http://code.google.com/p/poark/issues/list) or through mail and we'll try to help or get you involved in the project.

<p align='center'>
<a href='http://www.youtube.com/watch?feature=player_embedded&v=FkowXoBL06s' target='_blank'><img src='http://img.youtube.com/vi/FkowXoBL06s/0.jpg' width='640' height=480 /></a><br>
</p>