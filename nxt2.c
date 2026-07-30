#pragma config(Motor, motorA, motorA, tmotorNXT, PIDControl, encoder)

int flipEncode;

// Flips directions every second
task flipper {
	while (true) {
		bFloatDuringInactiveMotorPWM = false;
		nMaxRegulatedSpeedNxt = 1000;
		flipEncode = nMotorEncoder[motorA];
		int diff = -25 - flipEncode;

		nMotorEncoderTarget[motorA] = abs(diff);
		motor[motorA] = -40;

		while(nMotorRunState[motorA] != runStateIdle) {
		}
		motor[motorA] = 0;

		flipEncode = nMotorEncoder[motorA];

		wait1Msec(2000);

		diff = 10 - flipEncode;
		nMotorEncoderTarget[motorA] = abs(diff);
		motor[motorA] = 40;

		while(nMotorRunState[motorA] != runStateIdle) {
		}
		motor[motorA] = 0;

		wait1Msec(2000);
		bFloatDuringInactiveMotorPWM = true;
	}
}

task main() {
	startTask(flipper);

	while (true) {
		wait1Msec(100);
	}
}
