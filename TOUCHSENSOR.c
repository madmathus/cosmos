#pragma config(Sensor, S4, touchSensor, sensorTouch)
#pragma config(Motor,  motorC,     MotorC,            tmotorNXT, PIDControl, encoder)

int count = 0;
int status = 0;
bool held = false;
bool reverse = false;
int encod;

void moveMotorPos(int position) {
	encod = nMotorEncoder[motorB];
	nMotorEncoder[motorB] = 0;
  bFloatDuringInactiveMotorPWM = false;
  nMaxRegulatedSpeedNxt = 1000;

  nMotorEncoderTarget[motorB] = position;

  if (status == 1) {
  	if (reverse == true) {
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

task main() {
    while(true) {
      if (SensorValue(touchSensor) > 0) {
          ClearTimer(T1);

          while (time1[T1] < 600) {
              wait1Msec(5);
              if (SensorValue(touchSensor) > 0) {
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
          if (status == 1) {
          	moveMotorPos(60);
         	}
      }
      else {
          held = false;
      }
    }
}
