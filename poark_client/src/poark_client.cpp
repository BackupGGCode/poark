#include "ros/ros.h"
#include "std_msgs/UInt8MultiArray.h"
#include "std_msgs/UInt16MultiArray.h"

#include <sstream>

const bool LOW = false;
const bool HIGH = true;

enum PinMode { OUT, IN, ANALOG, ANALOG_FILT, PWM_MODE, SERVO, NONE=0xff };

const int kPinCount = 70;
const int kServoControlPin = 54;
const int kServoPin = 7;

// Variables to control the update of the servo position.  Keep
// volatile as they are changed asynchronously in a call back
// function.
volatile int g_servo_angle = 90;
volatile bool g_update_servo_angle = true;

// A callback for the /pins message from a Poark server.
// |msg| has the following layout:
// [ pin_id_1, pin_reading_1, pin_id_2, pin_reading_2, ...]
// For digital pins pin_reading_n will be either LOW=0 or HIGH = 1.
// For analog pins it will be a value between 0 and 1023.
void PinsCallback(const std_msgs::UInt16MultiArray::ConstPtr& msg)
{
  for (size_t i = 0;i < msg->data.size()/2;i++) {
    int pin = static_cast<int>(msg->data[i*2]);
    int value = static_cast<int>(msg->data[i*2 + 1]);
    ROS_INFO("Pin %d : %d", pin, value);
    if (pin == kServoControlPin) {
      // The servo control should be an angle between 0 and 180 degrees.
      int angle = static_cast<int>(value*180/1024);
      g_servo_angle = angle;
      g_update_servo_angle = true;
    }
  }
}

// Adds a pin definition for /set_pins_mode message.
void AddPinDefinition(std_msgs::UInt8MultiArray* msg,
                     int pin,
                     PinMode mode,
                     int state) {
  msg->data.push_back(pin);
  msg->data.push_back(mode);
  msg->data.push_back(state);
}

// Adds a pin state for /set_pins_state message.
void AddPinState(std_msgs::UInt8MultiArray* msg, int pin, int state) {
  msg->data.push_back(pin);
  msg->data.push_back(state);
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "poark_client");
  ros::NodeHandle n;
  ros::Publisher pins_mode_pub =
      n.advertise<std_msgs::UInt8MultiArray>("set_pins_mode", 1000);
  ros::Publisher pins_state_pub =
      n.advertise<std_msgs::UInt8MultiArray>("set_pins_state", 1000);
  ros::Subscriber sub = n.subscribe("pins", 1000, PinsCallback);
  ros::Rate loop_rate(100);

  // Set up some pins in different modes.
  std_msgs::UInt8MultiArray msg;
  msg.data.clear();
  AddPinDefinition(&msg, 13, OUT, LOW);
  AddPinDefinition(&msg, 8, PWM_MODE, 0);
  AddPinDefinition(&msg, 9, PWM_MODE, 0);
  AddPinDefinition(&msg, 10, PWM_MODE, 0);
  AddPinDefinition(&msg, kServoControlPin, ANALOG, LOW);
  AddPinDefinition(&msg, kServoPin, SERVO, 90);
  ROS_INFO("Sending /set_pins_mode msg.");
  pins_mode_pub.publish(msg);
  // Repeat the sending because ros-serial seems to eat our first message.
  ros::spinOnce();
  for (int i = 0;i < 20;i++)
    loop_rate.sleep();
  ROS_INFO("Sending /set_pins_mode msg.");
  pins_mode_pub.publish(msg);
  ros::spinOnce();
  loop_rate.sleep();
  // Start the main loop.
  int count = 0;
  while (ros::ok())
  {
    if (count % 10 == 0) {
      std_msgs::UInt8MultiArray msg2;
      msg2.data.clear();
      AddPinState(&msg2, 8, (count/10) % 50 + 200);
      AddPinState(&msg2, 9, (count/10) % 50 + 200);
      AddPinState(&msg2, 10, (count/10) % 50 + 200);
      AddPinState(&msg2, 13, count % 200 == 0 ? HIGH : LOW);
      ROS_INFO("Sending set_pins_state msg : %d.", count);
      pins_state_pub.publish(msg2);
    }
    if (g_update_servo_angle) {
      std_msgs::UInt8MultiArray msg;
      msg.data.clear();
      AddPinState(&msg, kServoPin, g_servo_angle);
      pins_state_pub.publish(msg);
      g_update_servo_angle = false;
    }
    ros::spinOnce();
    loop_rate.sleep();
    ++count;
    if (count > 30000)
      break;
  }

  msg.data.clear();
  AddPinDefinition(&msg, 13, NONE, LOW);
  AddPinDefinition(&msg, 8, PWM_MODE, 255);
  AddPinDefinition(&msg, 9, PWM_MODE, 255);
  AddPinDefinition(&msg, 10, PWM_MODE, 255);
  AddPinDefinition(&msg, kServoControlPin, NONE, LOW);
  AddPinDefinition(&msg, kServoPin, NONE, LOW);
  ROS_INFO("Sending /set_pins_mode msg.");
  pins_mode_pub.publish(msg);
  return 0;
}
