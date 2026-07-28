/* SpeedDemo example file

Demonstrates how to initialize the UCSD COSMOS digital speedsensor
and subsequently poll the speed measurement from the sensor.

Connections/configuration:
- DO NOT connect a 9V battery to the COSMOS digital speedsensor
- Connect UCSD COSMOS digital speedsensor to port 1
- WAIT till the COSMOS digital speedsensor says "System Ready"

Written by R.A. de Callafon, Dept. of MAE
Developed for the UCSD COSMOS Cluster 2 on Engineering Design and Control of Kinetic Sculptures.
More info: https://sites.google.com/eng.ucsd.edu/cosmoscluster2

Latest version: 1.2, July 2022 for RobotC Version 4.x

*/


// include the driver file for the UCSD COSMOS digital speedsensor
#include "speedsensor.c"

task main()
{
	// speed variable as a float
	float speed;

	// Write some text to the screen
	nxtDisplayTextLine(0, "* SpeedDemo *");
	nxtDisplayTextLine(1, "Speedsensor on S1");

	// initialize sensor to make a measurement
	InitializeSpeedSensor(S1);

	nxtDisplayTextLine(2, "Sensor ready...");

	speed=0;
	while (speed==0)
	{
		speed=PollSpeedSensor(S1);
	}
	nxtDisplayTextLine(2, "Speed was:");
	nxtDisplayTextLine(3, "%0.2f m/s", speed);
	nxtDisplayTextLine(4, "Restart again...");

	while (true) {
		wait1Msec(500);
	}
}
