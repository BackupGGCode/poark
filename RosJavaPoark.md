# Introduction #

This wiki page will give you brief introduction into using the Poark with [RosJava](http://www.ros.org/wiki/rosjava).

The rosjava\_poark library gives you mich higer-level interface to the Poark as the other examples that are part of the poark\_client package. The idea when I wrote this library was to keep the object-oriented spirit of RosJava into the Poark port.

When working with this library the user will not need to create or fill ROS messages himself but rather use methods with clear and simple interfaces and good documentation. However performance is not sacrificed and the layer that is built on top of the native ROS interface is as thin as possible. Using the library does not exclude programming the ROS topics directly for whoever needs to squeeze the last bit of performance out of ROS.

This tutorial expects you to know the basics of ROS and how to use it though the RosJava clone. the main concepts of the ros library which you can read from the [HowTo Page](http://code.google.com/p/poark/wiki/HowToUse). If you need help with installing and building RosJava you should read the manuals on the projects page (or even better the code ;) ). Installing the poark is rather straightforward but you can have a look at the  checklist [here](http://code.google.com/p/poark/wiki/InstallFAQ).

# Setting up a project with RosJava and the Poark #

I assume when you are reading this you have checked out the project and flashed the Arduino component already on your board. If not go back to the [Install FAQ](http://code.google.com/p/poark/wiki/InstallFAQ) and do this now.

First I would suggest you try to build the examples that are included with the package and try to run them. This will be good start into making sure that all the components work well and you have made your installation right. You will achieve that by running

```
rosmake
```

in the rosjava\_poark directory of your checkout. You can also use the

```
rospack list
```

command to check if the rosjava and rosjava\_poark packages have been found by the ROS toolkit. If you don't see them there check your **ROS\_PACKAGES\_PATH** environment variable if it includes the place where the rosjava and rosjava\_poark packages are.

Next you should create a new RosJava package either using the roscreate-pkg tool and fixing it for using ant as make tool (or for the lazy ones by copying an existing one and fixing all the names and paths in it). you can find more information on that [here](http://www.ros.org/wiki/rosjava/Build).

Don't forget to add the rosjava\_poark as dependency on your package. The poark itself is not a dependency as it uses only standard messages and thus does not include any ROS library as part of the server interface.

Once you have the empty package and can build it we are ready to put some poark code in it. Still in case you missed that on the RosJava project set-up page a reminder to use at least once the rosmake on the new project and also every time you want to make sure the projects you depend on are in freshly built and in sync or you need new Eclipse project files. For the rest of your builds using only ant will lead to way shorter compile times and no side effects.

# Using Poark's basic functionallity #

The Poark in Java is really easy to use. Basically there are two main interfaces in the rosjava\_poark package - **org.ros.poark.Poark** and **org.ros.poark.PoarkClient**. The first one provides only basic helper functions for setting up the ROS messages needed to talk to the Poark server and the second one provides convenient wrapper around those messages and gives really high level programing API to the user.

Let's first start with building a simple main loop for ROS as any program using it should do.

```
public class Basic implements NodeMain {

  private Node node;
  // All class fields we'll define in this tutorial should come here.

  @Override
  public void main(NodeConfiguration configuration) {
    Preconditions.checkState(node == null);
    Preconditions.checkNotNull(configuration);
    try {
      node = new DefaultNodeFactory().newNode("basic_poark_client", configuration);
      final Log log = node.getLog();
      // We'll initialize the PoarkClient here.
      while (true) {
        // We can do some stuff with the Poark here later.
        Thread.sleep(1000);
      }
    } catch (Exception e) {
      if (node != null) {
        node.getLog().fatal(e);
      } else {
        e.printStackTrace();
      }
    }
  }

  @Override
  public void shutdown() {
    node.shutdown();
    node = null;
  }
}
```

This empty node should compile and run as it is but it won't really do much. To be precise it won't do anything but burn CPU cycles. :)

Let's start with PoarkClient as this will maybe be your preferred way of using the Poark in Java.

Using this API is really simple. We shall start by creating an instance of the class:

```
  // Creates the client just after we created the RosJava Node.
  poarkClient = new PoarkClient(Poark.BoardLayout.MegaLayout, node);
```

As you can see the constructor for the client needs some information. The first is the configuration of the Arduino board we are using on the server side and the other is the reference to the node object we just created. This example assumes you have Arduino Mega 1280 or 2560 board. If you are on a smaller board like the Nano you will have to change the first parameter to **Poark.BoardLayout.MiniLayout**. _Soon the client will be able to query this on its own._

Now if we build and run the program it will still do nothing on first sight but in fact we already have a Poark client up and running that can send data to the server on the board and respond to messages from the board. We can observe that by using the
```
rostopic list
```
or
```
roswtf
```
commands.

Before we can send or receive data from the board we need to set up some pins as being either input or output. This is achieved with the **setPinMode** method of the client class. A pin can be in one of the following modes (as of now) **IN, OUT, PWM\_MODE, [ANALOG](#Using_Analog_Input.md), ANALOG\_FILTERED, SERVO, NONE**. When the board boots it sets all its pins in the **NONE** mode so that they won't be polled for state or receive unexpected state change requests. Depending on the pin mode we can set initial state for the pin. For digital pins it can be **HIGH** or **LOW**, for PWM pins it can be a value between 0 and 255 and for servo pins a value between 0 and 180.

For our example we will try to create a combo of two of the basic examples that are part of the Arduino tutorial but using the poark instead of directly programming the board. We will control the LED on the board found on pin 13 and read out the state of a button attached to pin 2.

You can see the schematics for connecting the button [here](http://arduino.cc/en/Tutorial/Button).

We can set the pins like this:

```
  // After we have the client instance we can set up some pins.
  poarkClient.setPinMode(13, Poark.OUT, Poark.HIGH);
  poarkClient.setPinMode(2, Poark.IN, Poark.HIGH);
```

Or alternatively we can set both (or even more) pins with a single call using the vectorized version of this command:

```
  poarkClient.setPinsMode(new byte[]{13, 2},
                          new byte[]{Poark.OUT, Poark.IN},
                          new byte[]{Poark.HIGH, Poark.HIGH});
```

The latter is actually preferred because it sends a single ROS message and stresses the serial communication less.

So far we have set up the pins as being input and output respectively now we need some way to know what the state of the input pin is. The client class itself will be receiving information for all state changes of input pins from the server however it will only notify us further for the ones we have declared our interest in observing. This is made so that a single client instance can be shared by multiple subsystems in an application to reduce the amount of nodes in a ROS setup and thus the load on the infrastructure.

To declare that we want to observe the state of the button pin we need to create a listener object that implements the **org.ros.poark.PinStateListener** interface which has only one method **onPinStateChanged(byte pin, int new\_value)**. The **new\_value** range depends on the type of the pin again. For digital input it will be either **HIGH** or **LOW** and for analog input a value between 0 and 1023.

Let's create our listener somewhere in the Node class:

```
  // Handles events from the pin connected to the button.
  private class ButtonHandler implements PinChangeListener {
    @Override
    public void onPinStateChange(byte pin, int newValue) {
      log.info("Pushed the button : \"" + pin + " - " + new_value + "\"");
    }
  }
```

This handler will print _Pushed the button : 2 - 0_ and _Pushed the button : 2 - 1_ repsectively when the button is pressed and released.

Next we need to tell the client that we want to have our listener called every time this pin changes state. This is done with the addPinChangeListener method. We can this before or after we set the pin to being input.

```
  poarkClient.addPinChangeListener(2, new ButtonListener());
```

For those who strive for brevity we can use anonymous implementation of the interface and put it like that:

```
  poarkClient.addPinChangeListener(2, new PinChangeListener() {
      @Override
      public void onPinStateChange(byte pin, int new_value) {
        log.info("Pushed the button : \"" + pin + " - " + new_value + "\"");
      }});
```

This is handy for one time throw away listeners but in most real world cases we will need some more state than the method input parameters.

Now we should be able to build our example and run it. It will do...nothing!? Why?
Because there is one issue with ROS serial that swallows our very first message we send down to the board so our setPinsMode command just got lost on the wire. We need to start by sending one dummy message and give it some time to settle the transfer before we send our real messages. So let's put this just after the creation of our client instance:

```
  // This will get wallowed by ROS :(
  poarkClient.setPinMode(0, Poark.NONE, Poark.LOW);
  Thread.sleep(1000);
```

Now if we build and run this sample we should be seeing our button messages appearing on the console.

Now let's make the LED blink all the time to notify us that we are up and kicking. For that purpose we will modify the empty loop we created in the beginning to something more useful using the **setPinState** command.

```
  // Start blinking the light.
  byte light = Poark.LOW;
  while (true) {
    light = (byte)(Poark.HIGH - light);
    poarkClient.setPinState(13, light);
    Thread.sleep(1000);
  }
```

The set pin state has almost the same syntax as **setPinMode** with the exception of the missing **mode** parameter. Again you can set multiple pins in one message using the **setPinsMode** vectorized version.

Let's see the complete code in the end (you can find this program in the tutorial folder of the rosjava\_poark source tree) :

```
import com.google.common.base.Preconditions;

import org.apache.commons.logging.Log;

import org.ros.node.DefaultNodeFactory;
import org.ros.node.Node;
import org.ros.node.NodeConfiguration;
import org.ros.node.NodeMain;

import org.ros.poark.PinChangeListener;
import org.ros.poark.Poark;
import org.ros.poark.PoarkClient;

/**
 * This is a simple rosjava {@link Node} to demonstrate using the Poark.
 * It assumes an external roscore is already running.
 */
public class Basic implements NodeMain {

  private Node node;
  private PoarkClient poarkClient;
  // the led on the main Arduino board is connected to pin 13.
  private final byte kLedPin = 13;
  private final byte kButton1 = 2;

  @Override
  public void main(NodeConfiguration configuration) {
    Preconditions.checkState(node == null);
    Preconditions.checkNotNull(configuration);
    try {
      node = new DefaultNodeFactory().newNode("basic_poark_client", configuration);
      final Log log = node.getLog();
      // Atach the client to the node and add some pin listeners.
      poarkClient = new PoarkClient(Poark.BoardLayout.MegaLayout, node);
      poarkClient.addPinChangeListener(kButton1,
          new PinChangeListener() {
            @Override
            public void onPinStateChange(byte pin, int new_value) {
              log.info("Pushed the button : \"" + pin + " - " + new_value + "\"");
            }
          });
      poarkClient.setPinMode(0, Poark.NONE, Poark.LOW);
      Thread.sleep(1000);
      // Setup the Poark I/O.
      poarkClient.setPinsMode(new byte[]{kLedPin, kButton1},
                               new byte[]{Poark.OUT, Poark.IN},
                               new byte[]{Poark.LOW, Poark.HIGH});
      // Start blinking the light.
      byte light = Poark.LOW;
      while (true) {
        light = (byte)(Poark.HIGH - light);
        poarkClient.setPinState(kLedPin, light);
        Thread.sleep(1000);
      }
    } catch (Exception e) {
      if (node != null) {
        node.getLog().fatal(e);
      } else {
        e.printStackTrace();
      }
    }
  }

  @Override
  public void shutdown() {
    node.shutdown();
    node = null;
  }
}
```

Now we have completed our aim and have our first Poark program using basic I/O functionality. The next chapter will deal with some advanced capabilities of the Poark so go grab a beer or a coke and stay tuned.

# More advanced API #

## Using Analog Input ##

Using the analog input on the Arduino board is as simple as using the digital input mode. You should use the **ANALOG** or **ANALOG\_FILTERED** modes when you set up the pin with **setPinMode** and then add a listener for the pin you just set up.

The Atmel chip has a built in 10 bit ADC so you will get values between 0 and 1023 that will be mapped to the voltage range [0V..5V]. We are working on extending the capabilities of the Poark to be able to utilize external VREF input or the built in 1.1V and 2.56V references.

The **ANALOG\_FILTERED** output gives you a 100 frames filter that smooths out the analog readings which is very handy given the low precision of the ADC on the board.

You can see a good example of using the analog input mode in the **org.ros.poark.tutorial.DemoKit** application.

## Using the I2C Communication Bus ##

[The I2C bus](http://en.wikipedia.org/wiki/I%C2%B2C) is a simple multi-device communication bus where devices can be either master or slaves. Slaves have assigned 7-bit addresses and master devices have no address and only they can initiate transfer on the bus. However more than one master can exist simultaneously but they can not use the bus at the same time.

The Poark can be used to communicate with a devices on the I2C but but only as a master. This means the Poark library can initiate transfers to and from other nodes on the bus but cannot itself be addressed for communication by other master devices.

The I2C capabilities on the server can be switched on or off through a compiler flag which means your particular server might be lacking that feature. You can use the **[RequestStatus](#Utilizing_the_Configuration_Channel.md)** message to query for the enabled capabilities on your server.

There are two topics that are used for I2C communication. **[i2c\_io](http://code.google.com/p/poark/wiki/HowToUse#i2c_io)** and **[i2c\_response](http://code.google.com/p/poark/wiki/HowToUse#i2c_response)** represented through the call **PoarkClient.performI2CTransfer** and implementing the listener interface **I2CResponseListener**.

The function **PoarkClient.performI2CTransfer** has four parameters - the address of the slave device on the I2C bus as a byte, the token identifying this transmission again being a byte, a byte array representing the data to be sent to the slave (can be empty) and last the number of bytes expected as response from the server.

A sample I2C call would look like this:

```
// Send the byte 15 (0x0f) to the device with address 64 (0x40) and expect 
// one byte response (typical byte register read). Use 123 as a token for that
// communication.
poarkClient.i2CTransfer((byte)0x40, (byte)123, new byte[] {(byte)0x0f}, 1);
```

The token is used to identify the corresponding response because there is no other way to identify which call to the listener corresponds to which transfer.

Here is a small **I2CResponseListener** implementation embedded in a call to **PoarkClient.addI2CResponseListener** which registers it on the client.

```
// Register the listener for all transfers to the device with address 64 (0x40).
poarkClient.addI2CResponseListener((byte)0x40, new I2CResponseListener() {
  @Override
  public void onI2CResponse(byte address, byte token, byte[] data) {
    // We expect one byte so print it with the token for that communication.
    log.info("I2C Got : " + token + " - " + data[0]);
  }
});
```

As you can see the listener class has only one method - **addI2CResponseListener** which is being called every time some I2C transfer has finished. Even in transfers where no data is expected back to the master a listener is called if one is registered for this address.

_There are still some quirks and limitations to the implementation of this protocol mainly due to the lack of hardware to test it by the authors. Any help with the implementation or donating devices using this bus will be greatly appreciated._
## Using Serial Communications ##

**Coming soon.**

## Utilizing the Configuration Channel ##

**Coming soon.**

# RosJava + Poark + Arduino #

**Coming soon.**

## The End ##