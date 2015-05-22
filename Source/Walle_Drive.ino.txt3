/*
 Name:		Walle_Drive.ino
 Created:	5/21/2015 12:00:44 PM
 Author:	s146991
*/
#include <Servo.h>                   // Include servo library
Servo servoLeft;                     // Declare left & right servos
Servo servoRight;
void FForward();
void FBack();
void PRight();
void PLeft();
unsigned int DraaiCounter;
unsigned int Delay;
// the setup function runs once when you press reset or power the board
void setup() {


		servoLeft.attach(12);                      // Attach left signal to pin 13
		servoRight.attach(13);                     // Attach right signal to pin 12

		

}

// the loop function runs over and over again until power down or reset
void loop() {
	if (DraaiCounter < 0){						//left angle
		Delay = abs(DraaiCounter) * (600 / 9);
		PLeft();
	}
	else if (DraaiCounter == 0){					//straight forward
		FForward();
	}
	else{										//right angle
		Delay = DraaiCounter * (600 / 9);
		PRight();
	}
}

// Full speed forward
void FForward(){
	servoLeft.writeMicroseconds(1700);         // Left wheel counterclockwise
	servoRight.writeMicroseconds(1300);        // Right wheel clockwise
	delay(2000);                               // ...for 2 seconds
}

// Turn left in place
void PLeft(){
	servoLeft.writeMicroseconds(1300);         // Left wheel clockwise
	servoRight.writeMicroseconds(1300);        // Right wheel clockwise
	delay(600);                                // ...for 0.6 seconds (er stond 600)
}

// Turn right in place
void PRight(){
	servoLeft.writeMicroseconds(1700);         // Left wheel counterclockwise
	servoRight.writeMicroseconds(1700);        // Right wheel counterclockwise
	delay(600);                                // ...for 0.6 seconds ( er stond 600)
}
// Full speed backward
void FBack(){
	servoLeft.writeMicroseconds(1300);         // Left wheel clockwise
	servoRight.writeMicroseconds(1700);        // Right wheel counterclockwise
	delay(2000);                               // ...for 2 seconds
}

void stop() //Stop function, makes the robot stop driving (if the servo's are properly set)
{
	servoLeft.write(90);
	servoRight.write(90);
}

//delay 2000 is 30cm, dus delay 66.67(2000/30) is 1 cm. Dus Gescande afstand * (2000/30) -> delay voor drive duur