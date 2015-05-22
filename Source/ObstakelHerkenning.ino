/*
 Name:		ObstakelHerkenning.ino
 Created:	5/7/2015 9:41:56 AM
 Author:	Fernando
*/

/*
To do:
BEGIN: scannen om zich heen, alle kanten op en zoek het dichtsbezijnde object. 
Als er een object wordt gescand door alleen de onderste prioriteit, anders de dichtsbijzijnde.
Als er niks wordt gevonden rijd 5 tellen naar voren. BEGIN
NIKS DRIVE en CLIFF


Alleen onderste scanner ziet iets, rijd erheen tot 20 cm afstand, check met IR. ALs IR reflecteert->steen TERUG. Niet vals object. BEGIN
dichtsbijzijnde: Rijd erheen ->BEGIN.

ALS ergens tijdens DRIVE CLIFF wordt gedetecteerd STOP-> BEGIN.
Als niks anders gevonden rijdt rechts om cliff heen, 5 sec rechts en probeer opnieuw naar object.
Als ^ 3x achter elkaar gebeurd, BORDER-> 180D -> BEGIN

TERUG: CLIFF check
LAB_RAMP:turn on LIGHT, DRIVE tot grijper_IR reflecteert.
beweeg rechts(+90D) 2 sec, beweeg links(-90D).
Als grijper_IR relfecteert^
Als grijper_IR niks ziet en LIGHT links(-90D) licht ziet DRIVE links(-90D).
ALS IR 30-80% detect op RAMP, DRIVE_SLOW 3X MID_IR. -> GRIJPER_LOS ->GRIJPER DICHT -> 5 sec DRIVE_ACHTER -> 180D
rijd terug naar laatste punt -> BEGIN
Als grijper_IR relfecteert-> LAB_RAMP

ALS IR 30-80% detect op RAMP.


*/
void IRU();
void IRM();
void IRG();
void light();
void USU();
void USD();
int counter = 0;
// the setup function runs once when you press reset or power the board
void setup() {
	
	
}

// the loop function runs over and over again until power down or reset
void loop() {
	stop(); //stop

}
/*UltrasoonUP*/
void USU(){
	if (readUltraTop() > 280){
		if (readUltraBot() < 280){							//we might have found a stone
			drive(readUltraBot());							// go for it wall-e
			stop();
		}
		else{												//nothing found yet
			drive(25, 0);									//we drive a little further and try it again
		}
	}
	else if (readUltraTop() > 40 && readUltraTop() < 280){
		if (abs(readUltraBot() - readUltraTop()) < 10){
			drive(readUltraTop());						//we are going on an adventure
		}
		else if ((readUltraBot() - readUltraTop()) > 10){ //There is an object closer to Top than Bot so let's explore it
			drive(readUltraTop());
		}
		else{											//maybe a possible stone 0.o
			drive(readUltraBot());
		}
	}
	else{
		if (abs(readUltraBot() - readUltraTop()) > 10){ //we are standing in front of a rock....
			stop();
			drive(10, 90);									//rotate 10 cm to the right
		}
		else{
			stop();
			IRM();										//start stone scan procedure
		}
	}
}

	/*UltrasoonDown*/
void USD(){
	readUltraBot() < 15{
		drive(5, 0);			//rijd er nog iets dichter naartoe
	}
}

/*IRUnder*/
void IRU(){
	if (readIRLB() || readIRRB() < 15){
		stop();												//car stops, because end of the world or a cliff
		if (readUltraTop > 0){
			drive(readUltraTop());
		}
		else{												//no new point found
			drive(5, 90);									//drive 5cm to the right
			drive(0, -90);									//drive back to the left
			drive(/*shortestpath functie*/);
		}
		if (readIRLB() || readIRRB > 100 && readIRLB() || readIRRB < 240){
			if (counter > 0){								//count grey stripes
				openGrabber();								//drop the stone
				counter = 0;
				reverse(25);
			}
			else{											//i can count grey stripes
				++counter;
			}
		}
		else{
			counter = 0;
		}
	}
}

	/*IRMmid*/
	bool IRM(){
		if (readIRMid() == 1){
			drive(5, 0);
			stop();
			openGrabber();					//get the stone
			closeGrabber();					//we got the stone
			reversePath();					//get the stone home wall-e
		}
		else{
			stop();							//stop
		}
	}

	/*IRGrabber*/
	bool IRG(){
		if (readIRGrab() == 0){				//We found a wall of the lab, but not the ramp
			stop();
			drive(5, 90);					// 5 cm to the right
			stop();
			drive(0, -90);					//back to the begin position
		}
		else{								//we found the ramp, go for it wall-e
			drive(5, 0);					//straight to the the goal	
			IRU();							//scan the ground for the grey stripes
			stop();							//stop
		}
	}
