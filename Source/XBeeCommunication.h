/* HOW THE XBEE COMMUNICATION WILL WORK:
 
 -Serial/"Internal" communication
 -Cross Arduino communication
 -Complete message steps


 
 Serial communication:

 The arduino communicates with the XBee module through it's serial monitor.
 After a small jumper is switched it is possible to use the serial.x commands for cross arduino communication.
 A serial.write("xxx") command will broadcast the message "xxx".


 Cross Arduino communication:

 The idea for communication is that feedback should be reported to the broadcasting arduino. 
 Psuedocode:
 if(messagereceived)
     serial.write("next message");

Furthermore the info which will be communicated will only be a coordinate and it's status, as indicated by the following header/prototypes.


Complete message steps:

The steps of completing a succesful broadcast will be as follows:

1, Arduino 1: Sends message
1, Arduino 2: Looks for available broadcasts

2, Arduino 2: Message will be read

3, Arduino 2: Confirmation message sent

4, Arduino 1: Looks for confirmation message
If confirmation received:                                   If confirmation message not received after 2 seconds:
5, Arduino 1: Finished                                      5, Arduino 1: Start at step 1
*/

void xBeeBroadcast(byte X, byte Y, byte status);   // Handles entire broadcast
void xBeeWrite(byte X, byte Y, byte status);      // Broadcasts message of the form coord 1 coord 2 status
void xBeeReadMessage();                           // Will read out the last message sent
bool xBeeAvailable();                             // Returns true if a broadcast has been found
void xBeeConfirm(byte X, byte Y, byte status);    // Confirms a received broadcast
byte xBeeRead(byte n);                            // Reads the message byte for byte