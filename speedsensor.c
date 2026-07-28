/* Speedsensor include file

   DO NOT MODIFY THIS FILE!
   
   Should only be used via an 
   #include "speedsensor.c"
   statement in any RobotC programs.

   Defines common functions needed to communicate with the UCSD COSMOS
   speedsensor using the digital sensor framework and I2C communication
   available on the LEGO NXT. Functions currently supported:

   InitializeSpeedSensor(S#);    (to initialize the speed sensor)
   speed = PollSpeedSensor(S#);  (to poll the latest speed measurement)

   History
   =======
   Version 1.0, 2009
   - Developed by Michelle Chen, Torrey Pines High School, 2009
   Version 1.0a
   - Developed by Scott Briscoe and Justin Lorenzo, UCSD
   Version 1.1 2010
   - Updated by R.A. de Callafon
   - First Version used for COSMOS course
   Version 1.2, 2013
   - Updated by Connor Worley to support ROBOTC Version 3.0
   - I2CMessage replaced by &I2CMessage in function call sendI2CMsg
   - hbyte, lbyte and direction replaced by &hbyte, &lbyte and &direction in function call readI2CReply
   Version 1.3, 2022
   - Tested with ROBOTC Version 4.x
   - Comments updated

   Maintained by R.A. de Callafon, Dept. of MAE
   Developed for the UCSD COSMOS Cluster 2 on Engineering Design and Control of Kinetic Sculptures.
   More info: https://sites.google.com/eng.ucsd.edu/cosmoscluster2

   Latest version: 1.3, July 2022 for RobotC Version 4.x
*/

// Initialization of address and common I2S write commands and I2C read commands
#define SSENSOR_ADDRESS      22   // hardware programmed I2C address of speedsensor
#define SSENSOR_INITIALIZE   10   // to intialize sensor for measurement
#define SSENSOR_LEDSON       13   // to turn sensor LEDS on
#define SSENSOR_LEDSOFF      14   // to turn sensor LEDS off
#define SSPEED_LBYTE         3    // to get the low byte of the speed measurement
#define SSPEED_HBYTE         4    // to get the high byte of the speed measurement
#define SSPEED_DIRECTION     5    // to get the direction of the speed measurement


// Function to initialize (turn on measurement function) of sensor
// Example code when sensor is connected to port S1:
//   SensorType[S1] = sensorI2CCustom;
//   InitializeSpeedSensor(S1);
void InitializeSpeedSensor(const tSensors Port)
{
  // set up port again as a I2C sensor
  SensorType[Port] = sensorI2CCustom;

  // array of three bytes for I2C messaging
  byte I2CMessage[3];

  // initialize sensor to make a measurement
  I2CMessage[0]=2;                    // message size of 2 bytes
  I2CMessage[1]=SSENSOR_ADDRESS;      // default I2C address of sensor
  I2CMessage[2]=SSENSOR_INITIALIZE;   // actual byte/message to write (SSENSOR_INITIALIZE = make a measurement)
  // wait in case I2C bus is busy
  while (nI2CStatus[Port] == STAT_COMM_PENDING)
  {
    wait1Msec(20);
  }
  // send actual I2C message, we expect 0 bytes back
  sendI2CMsg(Port, &I2CMessage[0] ,0);
  //wait AT LEAST 1 second for sensor to be initialized (blinking of LEDs)
  wait10Msec(100);
}


// Function to poll the latest speed measurement from the sensor.
// Example code when sensor is connected to port S1:
//  float speed;
//  SensorType[S1] = sensorI2CCustom;
//  speed=PollSpeedSensor(S1);
// NOTES:
// - The speed variable can be positive or negative, depending on the direction
//   of the object (ball) through the sensor. With the LCD display of the sensor
//   on the front, an object (ball) through the sensor from left to right
//   results in a positive speed, otherwise a negative speed.
// - Keep in mind that if indeed an object passed through the sensor, the
//   sensor would have to be reinitialized for the next measurement, so:
//    if (speed!=0) { InitializeSpeedSensor(S1); }
//
float PollSpeedSensor(const tSensors Port)
{
  // array of three bytes for I2C messaging
  byte I2CMessage[3];
  // bytes to store data coming back from sensor
  unsigned byte hbyte,lbyte;
  // speed variables as an integer and as a float
  int speed_int;
  float speed;
  // speed direction as a boolean
  byte direction;

  // First check if the sensor is busy, as you do NOT want to poll speed
  // before a measurement is done. For that, we temporarily turn the port
  // in a Touch sensor
  SensorType[Port] = sensorTouch;
  //device returns 0 when busy, 1 when not
  if ( SensorValue(Port) == 0 )
  {
    speed=0;
  }
  else
  {
    // set up port again as a I2C sensor
    SensorType[Port] = sensorI2CCustom;

    // Get high byte of speed measurement
    I2CMessage[0]=2;                      // message size of 2 bytes
    I2CMessage[1]=SSENSOR_ADDRESS;        // default I2C address of sensor
    I2CMessage[2]=SSPEED_HBYTE;           // actual byte/message to write (SSPEED_HBYTE = speed high byte)
    // wait in case I2C bus is busy
    while (nI2CStatus[Port] == STAT_COMM_PENDING)
    {
      wait1Msec(20);
    }
    // send actual I2C message, we expect 1 byte back
    sendI2CMsg(Port, &I2CMessage[0], 1);
    // again wait in case I2C bus is busy
    while (nI2CStatus[Port] == STAT_COMM_PENDING)
    {
      wait1Msec(20);
    }
    // read actual I2C message (of 1 byte)
    readI2CReply(Port, &hbyte, 1);
    // for debugging:
    // nxtDisplayTextLine(1, "hbyte = %d",hbyte);

    // Get low byte of speed measurement
    I2CMessage[0]=2;    // message size of 2 bytes
    I2CMessage[1]=22;   // default I2C address of sensor
    I2CMessage[2]=3;    // actual byte/message to write (4 = speed low byte)
    // wait in case I2C bus is busy
    while (nI2CStatus[Port] == STAT_COMM_PENDING)
    {
      wait1Msec(20);
    }
    // send actual I2C message, we expect 1 byte back
    sendI2CMsg(Port, &I2CMessage[0], 1);
    // again wait in case I2C bus is busy
    while (nI2CStatus[Port] == STAT_COMM_PENDING)
    {
      wait1Msec(20);
    }
    // read actual I2C message (of 1 byte)
    readI2CReply(Port, &lbyte, 1);
    // for debugging:
    // nxtDisplayTextLine(2, "lbyte = %i",lbyte);

    // Get direction of speed measurement
    I2CMessage[0]=2;    // message size of 2 bytes
    I2CMessage[1]=22;   // default I2C address of sensor
    I2CMessage[2]=5;    // actual byte/message to write (5 = direction)
    // wait in case I2C bus is busy
    while (nI2CStatus[Port] == STAT_COMM_PENDING)
    {
      wait1Msec(20);
    }
    // send actual I2C message, we expect 1 byte back
    sendI2CMsg(Port, &I2CMessage[0], 1);
    // again wait in case I2C bus is busy
    while (nI2CStatus[Port] == STAT_COMM_PENDING)
    {
      wait1Msec(20);
    }
    // read actual I2C message (of 1 byte)
    readI2CReply(Port, &direction, 1);
    // for debugging:
    // nxtDisplayTextLine(2, "lbyte = %i",lbyte);

    // convert to actual speed measurement
    speed_int = ((hbyte*256) + lbyte);
    speed = speed_int;
    speed = speed/100;
    if (direction==0)
    {
      speed=-speed;
    }
    // for debugging:
    // nxtDisplayTextLine(3, "%0.2f m/s", speed);

  }
  return(speed);
}
