#include <Arduino.h>
#include "XBeeCommunication.h"


void xBeeBroadcast(byte X, byte Y, byte status)
{
	byte a, b, c;
	xBeeWrite(X, Y, status);

	while (Serial.available() <= 0)
	{
		delay(1);
	}

	if (xBeeAvailable())
	{
		a = xBeeRead(a);
		b = xBeeRead(b);
		c = xBeeRead(c);
	}

	if (a != X || b != Y || status != c)
	{
		xBeeBroadcast(X, Y, status);
	}
}
void xBeeWrite(byte X, byte Y, byte status)         // Function literally prints XYstatus to the XBee
{                                                   // and thus broadcasts XYstatus to others.
	Serial.print(X);
	Serial.print(Y);
	Serial.print(status);
}

void xBeeReadMessage()
{
	byte X, Y, status;
	X = xBeeRead(X);

	xBeeConfirm(X, Y, status);

}

bool xBeeAvailable()                                // Function looks like bullsh*t, but can later be used
{                                                   // for a starting bit to minimize interference with XBees
	if (Serial.available() > 0)                     // from other groups
		return true;
	else
		return false;

}

void xBeeConfirm(byte X, byte Y, byte status)       // Returns a confirm message.
{
	xBeeWrite(X, Y, status);

}

byte xBeeRead(byte n)
{
	n = Serial.read();
	return n;
}