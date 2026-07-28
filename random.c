#pragma config(Sensor, S2, touchSensor1, sensorTouch)
#pragma config(Sensor, S3, touchSensor2, sensorTouch)
#pragma config(Sensor, S4, lightSensor, sensorLightActive)
#pragma config(Motor, motorA, motorA, tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB, motorB, tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC, motorC, tmotorNXT, PIDControl, encoder)

#include "speedsensor.c"
// Human Interface sorter

int count = 0;
int status = 0;
bool held = false;
bool reverse = false;
int encod;
int light_val = 28;
int light;

void moveMotorPos(int position) {
	encod = nMotorEncoder[motorC];
	bFloatDuringInactiveMotorPWM = false;
	nMaxRegulatedSpeedNxt = 1000;

	nMotorEncoderTarget[motorC] = position;

	if (status == 1) {
		nMotorEncoderTarget[motorC] = position;

		if (reverse == true) {
			motor[motorC] = -100;
			reverse = false;
		}
		else {
			motor[motorC] = 100;
			reverse = true;
		}
	}
	else if (status == 2) {
		encod = nMotorEncoder[motorC];
		int diff = 105 - encod;
		nMotorEncoderTarget[motorC] = abs(diff);

		if (abs(diff) < 5) {
			motor[motorC] = 0;
			bFloatDuringInactiveMotorPWM = true;
			return;
		}

		if (diff > 0) {
			motor[motorC] = 100;
			reverse = !reverse;
		}
		else {
			motor[motorC] = -100;
			reverse = !reverse;
		}
	}


	while (nMotorRunState[motorC] != runStateIdle) {
	}

	motor[motorC] = 0;

	wait1Msec(100);
	bFloatDuringInactiveMotorPWM = true;
}

void moveMotorLight() {
	light = SensorValue(lightSensor);
	bFloatDuringInactiveMotorPWM = false;
	nMaxRegulatedSpeedNxt = 1000;

	if (light >= 10 && light <= 22) { // detects track
		motor[motorC] = 0;
		return;
	}

	else if (light > light_val) {
		encod = nMotorEncoder[motorC];
		int diff = 105 - encod; // go to 105
		nMotorEncoderTarget[motorC] = abs(diff);

		if (abs(diff) < 5) {
			motor[motorC] = 0;
			bFloatDuringInactiveMotorPWM = true;
			return;
		}

		if (diff > 0) {
			motor[motorC] = 100;
			encod = nMotorEncoder[motorC];
			reverse = !reverse;

			wait1Msec(1000);

			nMotorEncoderTarget[motorC] = 105;
			motor[motorC] = -100;
			reverse = !reverse;
			encod = nMotorEncoder[motorC];
		}
	}

	else if (light < light_val) {
		motor[motorC] = 0;
		wait1Msec(5);
	}

	while (nMotorRunState[motorC] != runStateIdle) {
	}

	motor[motorC] = 0;
	wait1Msec(100);
	bFloatDuringInactiveMotorPWM = true;
}

/*void moveMotorLight() {
light = SensorValue(lightSensor);

if (light >= 10 && light <= 23) {
motor[motorC] = 0;
return;
}

bFloatDuringInactiveMotorPWM = false;
nMaxRegulatedSpeedNxt = 1000;
nMotorEncoderTarget[motorC] = 105;

if (light > light_val) {
motor[motorC] = 100;
}
else if (light < light_val) {
motor[motorC] = -100;
}

while (nMotorRunState[motorC] != runStateIdle) {
}

motor[motorC] = 0;
wait1Msec(100);
bFloatDuringInactiveMotorPWM = true;
}*/

task sorter {
	while(true) {
		if (SensorValue(touchSensor2) > 0) {
			clearTimer(T1);

			while (time1[T1] < 600) {
				wait1Msec(5);
				if (SensorValue(touchSensor2) > 0) {
					if(!held) {
						count++;
						held = true;
					}
					} else {
					held = false;
				}
			}

			status = count;
			count = 0;

			if (status > 3) {
				status = 3;
			}

			moveMotorPos(105);

			if (status == 3) {
				while (SensorValue(touchSensor2) > 0) {
					wait1Msec(5);
				}

				while (SensorValue(touchSensor2) == 0) {
					moveMotorLight();
					wait1Msec(50);
				}

				status = 1;

				while (SensorValue(touchSensor2) > 0) {
					wait1Msec(5);
				}
			}
		}
		else {
			held = false;
		}
	}
}


// Basket PID control


float speed;
float distance;
int position;
int touch;

void moveBasket() {
	motor[motorB] = 50;
	while (true) {
		touch = SensorValue(touchSensor1);
		if (touch > 0) { // Is touching
			motor[motorB] = 0;
			break;
		}
	}
}

void basketPID(float speed) {
	// 11 & 14.5 inches
	distance = (speed * sqrt((2 * 0.2667) / 9.81)); //meters
	position = (int) (distance * 1700); // encoder ticks


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

task basket {
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

int flipEncode;

// Flips directions every second
task flipper {
	while (true) {
		bFloatDuringInactiveMotorPWM = false;
		nMaxRegulatedSpeedNxt = 1000;
		flipEncode = nMotorEncoder[motorA];
		int diff = -30 - flipEncode;

		nMotorEncoderTarget[motorA] = abs(diff);
		motor[motorA] = -100;

		while(nMotorRunState[motorA] != runStateIdle) {
		}
		motor[motorA] = 0;

		flipEncode = nMotorEncoder[motorA];

		wait1Msec(1000);
		// Wait

		diff = 0 - flipEncode;
		nMotorEncoderTarget[motorA] = abs(diff);
		motor[motorA] = 100;

		while(nMotorRunState[motorA] != runStateIdle) {
		}
		motor[motorA] = 0;

		flipEncode = nMotorEncoder[motorA];
		wait1Msec(1000);
		bFloatDuringInactiveMotorPWM = true;
	}
}



task main() {
	startTask(sorter);
	startTask(basket);
	startTask(flipper);

	while (true) {
		wait1Msec(100);
	}
}

// Signed: Rhoy Xu
// Copyright: Rhoy Xu 2026
