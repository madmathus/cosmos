#pragma config(Motor,  motorB,          motorB,        tmotorNXT, PIDControl, encoder)
#pragma config(Sensor, S2, touchSensor, sensorTouch)
#include "speedsensor.c"

int touch;
float speed;
float distance;
int position;

void moveBasket() {
	motor[motorB] = -80;
	while (true) {
		touch = SensorValue(touchSensor);
		if (touch > 0) { // Is touching
			motor[motorB] = 0;
			break;
		}
	}
}

void basketPID(float speed) {
	// 11 & 14.5 inches
	distance = speed * sqrt((2 * 0.2667) / 9.81); //meters
	position = (int) (distance * 1250); // encoder ticks


	nMotorEncoder[motorB] = 0;
	bFloatDuringInactiveMotorPWM = false;
	nMaxRegulatedSpeedNxt = 1000;

	//sets target with PID
	nMotorEncoderTarget[motorB] = position;
	motor[motorB] = -100;

	while(nMotorRunState[motorB] != runStateIdle) {
		// Do nothing
	}
	motor[motorB] = 0;

	wait1Msec(500);
	bFloatDuringInactiveMotorPWM = true;
}

task main()
{
	moveBasket();

	while (true) {
		InitializeSpeedSensor(S1);
		position = 0;
		speed = 0;
		distance = 0;

		while (speed == 0) {
			speed = PollSpeedSensor(S1);
		}

		basketPID(speed);
		// Formula to calculate distance
		// 11 inches & 14.5 inches
		// 1 m = 1250 ticks

		nxtDisplayTextLine(2, "Speed was:");
		nxtDisplayTextLine(3, "%0.2f m/s", speed);
		nxtDisplayTextLine(4, "Restart again...");

		moveBasket();
	}
}
