# Introduction #

This page should give you a short overview of how the Poark is intended to be used. And should help you understand the sample applications found in the poark\_client dir.

# The name #

You might have noticed that I don't just write Poark but _the_ Poark because it is an abreviation for Pin On Arduino-Ros K_^H\_Communication. Ok...ok...I admit it should sound something like pork and this was my goal so there you are with the Poark in your hands use it wisely!_

# Details #

The Poark is intended to give you low level access to the I/O pins on an Arduino driven board. It is not really mean to be a very high-performance tool (_If you need that you should write custom software for the CPU on the board anyway._) but it should allow for rapid development in cases where the Arduino is meant to only be used as an interface between a PC or other computing unit and some periphery.
The Poark is similar to [Firmata](http://firmata.org) but works with the [ROS](http://ros.org) serial communication protocol.

The Poark supports the following pin modes.

  * Digital I/O.
  * Interrupt activations.
  * Analog input.
  * PWM (pulse-width-modulation) output.
  * Timed single-pulse output (for servo control).
  * I2C bus I/O.
  * Serial I/O.

# Usage #

To start using the Poark you need to burn the poark\_server on your Arduino board. It should work on any board because it uses the Arduino API for accessing the pins as they are numbered by the API.

However you might need to tune the |kPinCount| in the source for boards that has less pins and memory. Every pin uses around 12bytes now so if you have a Nano board with very limited memory which anyhow has fewer pins you might want to set this constant to 13 which will still give you access to all I/O pins but will use less than 1/4 of the memory. You can also comment out support for some of the more expensive protocols like I2C or Servo by commenting out the defines for them (e.g. the lines **#define WITH\_SERVO** or **#define WITH\_WIRE**).

From that point on you can use any ROS compatible client to talk to the board and use the Poark messages for sending or receiving data from the board.

# Messages #

There are a few types of messages implemented or being implemented in the Poark. Pin I/O including digital, analog and servo control. I2C I/O messages and serial I/O messages. All messages use std\_msgs::UIntXXArray as input and output message types. Whenever byte data is sent 8 bit elemets are used and whenever a word is enough UInt16 is being used.

## set\_pins\_mode ##

The Poark subscribes to that message and uses it to set the modes for one or more pins. The message format is a UInt8Array with the following structure:

| pin1\_id | pin1\_mode | pin1\_state |
|:---------|:-----------|:------------|
| pin2\_id | pin2\_mode | pin2\_state |
| ... | ... | ... |

Where pin\_id is the pin number on the arduino board, pin\_mode is one of the modes : NONE, IN, OUT, ANALOG, ANALOG\_FILTERED, PWM\_MODE, SERVO.

  * NONE - that the pin is not controlled by the Poark.
  * IN - the pin is in digital input mode and will be sampled regularly and in case of changes a _pins_ message will be published.
  * INTERRUPT - the pin should be able to trigger interrupts (ie, pins 2 and 3, and on the Arduino Megas also pins 18-21.  pin\_state should be one of INT\_LOW, INT\_RISING, INT\_FALLING, INT\_CHANGE, or INT\_NONE (turn off the feature).  When activated, the pin will send back a message with how many times the interrupt has been triggered since the last report.  No message is sent for no activations.  See http://www.arduino.cc/en/Reference/AttachInterrupt for more information about interrupts on the Arduino boards.
  * OUT - the pin is in digital output mode and can be set to HIGH (1) or LOW (0) through a _set\_pins\_state_ message.
  * ANALOG - the pin is in analog read mode and will be sampled regularly and in case of changes a _pins_ message will be published with a value between 0 and 1024.
  * ANALOG\_FILTERED - the pin is in analog read mode and will be sampled regularly and the raw reading will be filtered through a sliding window filter to reduce noise from the ADC and in case of changes a _pins_ message will be published with a value between 0 and 1024.
  * PWM\_MODE - the pin is in PWM ([pulse width modulation](http://arduino.cc/en/Tutorial/PWM)) output mode and can be set to a value between 0 and 255 through a _set\_pins\_state_ message. **NOTE: When using the MsTimer2 library (which is used by default), the PWM cycle time of pin 9 and 10 is increased from 32kHz to 1kHz.**
  * SERVO - the pin is set to be a control pin for a servo motor and can be set to a value between 0 and 180 (degrees) with a _set\_pins\_state_ message. The port has to be set to the wished value every 20ms or most servos will actually forget their position and not perform correctly.

pin\_state is the initial state of the pin - a value as described in the listed cases above.

## request\_poark\_config ##

The Poark server listens for empty messages, and when one is received constructs a poark\_status message containing information about the current server status.

## set\_poark\_config ##

This message is subscribed to by the Poark server to configure its behavior.  The messages is a list of one or more of the following sub messages (consisting of 16 bit words):
  * REQUEST\_CONFIG - equivalent to a request\_poark\_config message.  See [HowToUse#request\_poark\_config](HowToUse#request_poark_config.md).
  * SET\_FREQUENCY, frequency - set the Poark sampling frequency to frequency.
  * SET\_CONTINUOUS\_MODE, 1/0 - determine if the Poark should deliver data at the set frequency, regardless if it has changed or not.  Note, if set the communication between the Poark server and the clients increase.
  * SET\_FILER\_LAMBDA, lambda\_times\_1000 - set the lambda used to low-pass filter measurements when running a pin in ANALOG\_FILTERED mode. See [HowToUse#set\_pins\_mode](HowToUse#set_pins_mode.md).
  * SET\_TIMESTAMP, 1/0 - determine if the Poark should deliver time stamps with its measurements or not.  If set, time stamps are delivered as a special pin value.
  * SET\_ANALOG\_REF, reference\_mode - is used to set the reference value for the on board A/D converter.  Valid values for reference mode are: DEFAULT, INTERNAL, INTERNAL1V1, INTERNAL2V56, and EXTERNAL.  For more details consult the Arduino reference: http://arduino.cc/it/Reference/AnalogReference.
  * SETUP\_SERIAL, port, baud\_rate - setup the Poark serve to use the serial I/O interfaces on the Arduino board.  port is a number 0 (0-3 with the Arduino Megas), and a baud\_rate (BAUD0 (hang up), BAUD50, BAUD75, BAUD110,  BAUD134, BAUD150, BAUD200, BAUD300, BAUD600, BAUD1200, BAUD1800, BAUD2400, BAUD4800, BAUD9600, BAUD19200, BAUD38400, BAUD57600, BAUD115200,  BAUD230400, BAUD460800, BAUD500000, BAUD576000, BAUD921600, BAUD1000000, BAUD1152000, BAUD1500000, BAUD2000000, BAUD2500000, BAUD3000000, BAUD3500000, and BAUD4000000).

## set\_pins\_state ##

The Poark server subscribes to _set\_pins\_state_ messages.  These messages contain instructions for setting the state of given pins.  The message has the following format:

| pin1\_id | pin1\_state |
|:---------|:------------|
| pin2\_id | pin2\_state |
| ...     | ...        |

pin\_id is the pin number of a pin on the arduino board, pin\_state is state to set the pin to.  The pin is assumed set to as an output, ie, OUT, PWM\_MODE, or SERVO.  See the documentation for [set\_pins\_mode](HowToUse#set_pins_mode.md) for more details.

## pins ##

The Poark server emits _pins_ messages to communicate changed values on pins declared for input.  The message is constructed as follows:

| pin1\_id (2 bytes) | pin1\_state (2 bytes) |
|:-------------------|:----------------------|
| pin2\_id (2 bytes) | pin2\_state (2 bytes) |
| ...               | ...                  |

Where once again pin\_id is the pin number of the pin that has changed values, and pin\_state is the new state.  Note, this message is constructed from 2 byte ints.  This is needed in order to fully utilize the boards 10 bit the A/D converters.

## i2c\_io ##

The Poark subscribes to this message and initiates transfers on the I2C bus for writing and reading as requested through the message.

Message format:

| address | send\_len | receive\_len | token | data1 | data2 | ... |
|:--------|:----------|:-------------|:------|:------|:------|:----|

The address is the bus address of the other side for the communication. (_Currently the Poark supports only master mode and switches the bus in master mode upon initialization._)
Send\_len is the length of the data to be transmitted. The bytes are then appended after the message header.
Receive\_len is the number of bytes to be read from the bus after the transmition of the data to be sent is finished.
Both send\_len and receive\_len can be 0 in which case only one-directional transimission occurs.
Token is a user chosen value that is being sent back with the [i2c\_response](HowToUse#i2c_response.md) corresponding to that message and can be used to identify the transmission in case of multiple ones running in parallel.

## i2c\_response ##

After each [i2c\_io](HowToUse#i2c_io.md) message the Poark responds by sending back one i2c\_response message. It contains both the received data from and the token sent with the i2c\_io message.

The message structure is as follows:

| address | token | data1 | data2 | ... |
|:--------|:------|:------|:------|:----|

As desribed above address is the bus address of the communicating device. Token is the same value as sent in the i2c\_io message and the data is the transmitted data if any.

**It is important to wait for a i2c\_response message before sending second i2c\_io call because the bus does not support multiplexing and will mix responses or even corrupt transmitted data!**

## serial\_send ##

The Poark server listens to the messages serial\_send (and serial1\_send, serial2\_send, and serial3\_send in the case of a Arduino Mega board).  The message contains a number of bytes which are transmitted on the appropriate serial interface of the board.

## serial\_recieve ##

The Poark server sends a serial\_receive message (or serial1\_receive, serial2\_receive, or serial3\_receive with the Arduino Mega board) when an active serial interface receives data.  The received data is mapped one-to-one into the message.