#pragma config(Motor,  motorB, basketMotor,    tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC, interfaceMotor, tmotorNXT, PIDControl, encoder)
#pragma config(Sensor, S2, basketTouch,    sensorTouch)
#pragma config(Sensor, S4, interfaceTouch, sensorTouch)
#include "speedsensor.c"

int count = 0;
int status = 0;
bool held = false;
bool reverse = false;

void moveMotorPos(int position)
{
    nMotorEncoder[interfaceMotor] = 0;
    bFloatDuringInactiveMotorPWM = false;
    nMaxRegulatedSpeedNxt = 1000;

    nMotorEncoderTarget[interfaceMotor] = position;

    if (reverse)
    {
        motor[interfaceMotor] = 100;
    }
    else
    {
        motor[interfaceMotor] = -100;
    }

    reverse = !reverse;

    while (nMotorRunState[interfaceMotor] != runStateIdle)
    {
    }

    motor[interfaceMotor] = 0;

    wait1Msec(100);
    bFloatDuringInactiveMotorPWM = true;
}

task humanInterface()
{
    while (true)
    {
        if (SensorValue(interfaceTouch) > 0)
        {
            clearTimer(T1);

            while (time1[T1] < 600)
            {
                wait1Msec(5);

                if (SensorValue(interfaceTouch) > 0)
                {
                    if (!held)
                    {
                        count++;
                        held = true;
                    }
                }
                else
                {
                    held = false;
                }
            }

            status = count;
            count = 0;

            if (status > 3)
                status = 3;

            if (status == 1)
                moveMotorPos(60);

            while (SensorValue(interfaceTouch) > 0)
                wait1Msec(5);
        }
        else
        {
            held = false;
        }
    }
}

int touch;
float speed;
float distance;
int position;

void moveBasket()
{
    motor[basketMotor] = -80;

    while (true)
    {
        touch = SensorValue(basketTouch);

        if (touch > 0)
        {
            motor[basketMotor] = 0;
            break;
        }
    }
}

void basketPID(float speed)
{
    distance = speed * sqrt((2 * 0.2667) / 9.81);
    position = (int)(distance * 1250);

    nMotorEncoder[basketMotor] = 0;
    bFloatDuringInactiveMotorPWM = false;
    nMaxRegulatedSpeedNxt = 1000;

    nMotorEncoderTarget[basketMotor] = position;
    motor[basketMotor] = -100;

    while (nMotorRunState[basketMotor] != runStateIdle)
    {
    }

    motor[basketMotor] = 0;

    wait1Msec(500);
    bFloatDuringInactiveMotorPWM = true;
}

task basketControl()
{
    moveBasket();

    while (true)
    {
        InitializeSpeedSensor(S1);

        position = 0;
        speed = 0;
        distance = 0;

        while (speed == 0)
        {
            speed = PollSpeedSensor(S1);
        }

        basketPID(speed);

        nxtDisplayTextLine(2, "Speed:");
        nxtDisplayTextLine(3, "%0.2f m/s", speed);

        moveBasket();
    }
}

//====================
// Main
//====================

task main()
{
    startTask(humanInterface);
    startTask(basketControl);

    while (true)
    {
        wait1Msec(100);
    }
}
