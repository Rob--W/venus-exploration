/* HOW THE XBEE COMMUNICATION WILL WORK:
 
 -Serial/"Internal" communication
 -Cross Arduino communication


 
 Serial communication:

 The arduino communicates with the XBee module through it's serial monitor.
 After a small jumper is switched it is possible to use the serial.x commands for cross arduino communication.
 A serial.write("xxx") command will broadcast the message "xxx".


 Cross Arduino communication:

 The idea for communication is that feedback should be reported to the broadcasting arduino. 
 Psuedocode:
 if(messagereceived)
     serial.write("next message pl0x");

Furthermore the info which will be communicated will only be a coordinate and it's status, as indicated by the following header/prototypes.
*/

void xBeeWrite(byte X, byte Y, byte status);
void xBeeConfirm(byte X, byte Y, byte status);