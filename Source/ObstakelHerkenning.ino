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
void drive();
void stop();
void bdrive();
void IRO();
void grab();
int IRM();
int IRG();
int light();
int USU();
int USD();
int counter = 0;
unsigned int intensiteitB;
bool intensiteitM;
bool intensiteitG;
// the setup function runs once when you press reset or power the board
void setup() {
	
	
}

// the loop function runs over and over again until power down or reset
void loop() {
	stop();
	USU();
	USD();
	if (USU() < ){

	}
	else if(USU( > && < ){

	}
	else if(USU() > ){

	}
	else{
	}

}

int USD(){
	
}

/*IRO*/
void IRO(){
	if (intensiteitB < 15){
		stop();			//auto stopt, want cliff of einde map
		if (USU() > 0){
			drive(/*gegeven input coördinaten USU*/);
		}
		else{		//geen nieuw punt gevonden
			drive(5, 90); // rijd 10cm naar rechts
			drive(0, -90); //draai weer naar oorspronkelijke punt
			drive(/*richting oorspronkelijke punt*/);
		}
		if (intensiteitB > 100 && intensiteitB< 240){
			if (counter > 2){
				;//grijper los
				counter = 0;
			}
			else{
				++counter;
			}
		}
		else{
			counter = 0;
		}
	}

	/*IRM*/
	bool IRM(){
		if (intensiteitM == 1){
			drive(5, 0);
			stop();
			grab(); //moet nog weten wat de juiste benaming is voor deze functie
		}
		else{
			stop();
			USU();
			USD();
		
		}

		}

	/*IRG*/
	bool IRG(){
		if (intensiteitG == 0){
			stop();
			drive(5, 90);
			stop();
			drive(0, -90);
			IRG();
		}
		else{
			drive(20, 0);
			stop();
		}
	}