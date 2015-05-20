#include <Arduino.h>
#include <Servo.h>
#include "softwaredrivers.h"


Servo venusLeft;
Servo venusRight;
Servo servoUltra;
Servo servoGrabber;

const int leftservo = 12;
const int rightservo = 13;
const int ultraservo = 11;
const int grabberservo = 10;

const int leftencoder = 7;
const int rightencoder = 8;

void startSetup(){
	venusLeft.attach(leftservo, 540, 2400);
	venusRight.attach(rightservo, 540, 2400);
	servoUltra.attach(ultraservo, 540, 2400);
	servoGrabber.attach(grabberservo, 540, 2400);

	pinMode(leftencoder, INPUT);
	pinMode(rightencoder, INPUT);
}

int readIRLB(){		//Reads IR Left Bottom. Return unit to be determined

}

int readIRRB(){		//Reads IR Right Bottom. Return unit to be determined

}

void stop(){	//Stop function, makes the robot stop driving (if the servo's are properly set)
	venusLeft.write(90);
	venusRight.write(90);
}

void servoDrive(int left, int right, int n){
	int pulse;
	int prevpulse;
	int countpulse = 0;

	prevpulse = digitalRead(rightencoder); //initial value encoder

	venusLeft.write(left);
	venusRight.write(right);

	while (n<countpulse){
		pulse = digitalRead(rightencoder);
		if (pulse != prevpulse){
			prevpulse = pulse;
			countpulse += 1;
		}
		if ((readIRLB() || readIRRB) < 400){//hole sensing code has to be added
			stop();
			break; //Some kind of notification has to be added that an emergency brake has been executed.
		}
	}
	stop();
}

void drive(int distance, int angle){ //drive function with integer parameters distance and angle in degrees
	//turn part
	angle = angle % 360; //2*pirad*k=2*pirad*(k+1)
	if (angle < 180){	//turn right
		int turnn = (angle *(10 / 1437)); //Number of pulses needed to turn
		servoDrive(180, 180, turnn);
	}
	else{	//turn left
		int turnn = ((angle - 180) *(10 / 1437)); //Number of pulses needed to turn
		servoDrive(0, 0, turnn);
	}
	//forward movement part
	int n = ((distance)*(10 / 1437)); //Number of pulses needed to drive distance. Distance in mm
	servoDrive(180, 0, n);
}

void reverse(int distance){	//Makes the robot drive backwards a certain distance
	int n = ((distance)*(10 / 1437)); //Number of pulses needed to drive distance
	servoDrive(0, 180, n);
}

int readIRMid(){	//reads Middle IR. Return unit to be determined

}

int readIRGrab(){		//Reads IR Grabber. Return unit to be determined

}

int readLightSense(){	//Reads Light Sensor. Return unit to be determined

}

int readUltraTop(int angle){		//Reads Top Ultrasonic sensor. Return unit is a distance. Input parameter is a angle; full left is angle 0 degrees full right is 180 degrees
	servoUltra.write(angle);
	/*
	ultra read code
	*/
}

int readUltraBot(){
	/*
	ultra read code
	*/
}
void openGrabber(){
	//servoGrabber.attach(grabberservo);
	servoGrabber.write(0);
	//servoGrabber.detach();
}
void closeGrabber(){
	// servoGrabber.attach(grabberservo);
	servoGrabber.write(180);
	//servoGrabber.detach();
}