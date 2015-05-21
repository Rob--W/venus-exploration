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
unsigned int Waarde = 0;
// the setup function runs once when you press reset or power the board
void setup() {


		servoLeft.attach(12);                      // Attach left signal to pin 13
		servoRight.attach(13);                     // Attach right signal to pin 12

		

}

// the loop function runs over and over again until power down or reset
void loop() {
	if ((15 - DraaiCounter) < 8){				//left angle
		Waarde = DraaiCounter - 8;			//oke marijn wat welke kennis mis ik?
		Delay = Waarde * (600 / 8);
		PLeft();
	}
	else if (DraaiCounter = 8){				//straight forward
		FForward();
	}
	else{										//right angle
		Waarde = 8 - DraaiCounter;
		Delay = Waarde * (600 / 8);
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
	delay(Delay);                                // ...for 0.6 seconds (er stond 600)
}

// Turn right in place
void PRight(){
	servoLeft.writeMicroseconds(1700);         // Left wheel counterclockwise
	servoRight.writeMicroseconds(1700);        // Right wheel counterclockwise
	delay(Delay);                                // ...for 0.6 seconds ( er stond 600)
}
// Full speed backward
void FBack(){
	servoLeft.writeMicroseconds(1300);         // Left wheel clockwise
	servoRight.writeMicroseconds(1700);        // Right wheel counterclockwise
	delay(2000);                               // ...for 2 seconds
}