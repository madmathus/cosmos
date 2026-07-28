#pragma config(Sensor, S1, touchSensor, sensorTouch)

int touch;
int count;

task main() {
	while (true) {
		count = 0;
		touch = sensorValue(touchSensor);


}
