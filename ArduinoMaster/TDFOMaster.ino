// Hacked up version of the demo gateway from Mr. Rusu at http://www.lowpoerlab.com, the moteino's come from there
// and i highly recomend them.
#include <RFM69.h>
#include <TDFBO.h>

#define NODEID        1    //unique for each node on same network

#define TRANSMITPERIOD 100 //How often do we spam our network?  10 times a second seems good.  Should be quick enough, unless those
                           //slaves are being chatty.

#define LED_COUNT 4
#define WEAK_RANGE -50 //What do we consider weak?

unsigned long lastPeriod = -1;
int barPins[] = {3, 5, 6, 9};
RFM69 radio;

void Blink(byte PIN, int DELAY_MS)
{
   pinMode(PIN, OUTPUT);
   for(int x = 0; x < LED_COUNT ;x++){
      pinMode(barPins[x], OUTPUT);
   } 
   digitalWrite(PIN,HIGH);
   delay(DELAY_MS);
   digitalWrite(PIN,LOW);
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower();
  radio.encrypt(0);//Don't bother encrypting it
  radio.promiscuous(false); //We dont' want the master listening to everything.  That's kind of pointless
}

void loop() {
   //Anybody have anything for us?
   //There is a bug in here:  If a radio stops sending us stuff we have no way of catching that
   // we'll keep it's last report as current forever
   if (radio.receiveDone()){
      if (radio.DATALEN != sizeof(SignalStrengthPing)) {
         Serial.print("Invalid payload received, not matching Payload struct!");
      } else {
         SignalStrengthPing theData = *(SignalStrengthPing*)radio.DATA;

         Serial.print("Node [");
         Serial.print(radio.SENDERID);
         Serial.print("] Strength: [");
         Serial.print(theData.avgStrength);

         //To make this photogenic we're going to have to pick some variables
         // RIGHT NEXT STORE == -26
         // TOO FAR == 100
         // let's scale it between those too
         
         int topVal = constrain(theData.avgStrength, -25, -100);
         //int percent = map(theData.avgStrength, -100, -25 , 0, 100);
         int percent = map(theData.avgStrength, WEAK_RANGE, -25 , 0, 100);
         if(percent < 0){ //The map above will cause us to wrap back to 0
            percent =0;
         }
         Serial.print("] Mapped to [");
         Serial.print(percent);
         Serial.println("]");

         // Going to have 4 LED's
         //BRUTE FORCE
         int finalPin = 0;
         int finalTally = 0;
         
         for(int q=0;q < 4;q++){
            if(percent >25){
               //Drop the percentage down a notch, up the final pin
               analogWrite(barPins[q], 255);
               finalPin++;
               percent -= 25; 
/*               Serial.print("Turning on Pin [");
               Serial.print(barPins[q]);
               Serial.println("]");            
*/
            } else {
               digitalWrite(barPins[q], LOW);
 /*              Serial.print("Turning off Pin [");
               Serial.print(barPins[q]);
               Serial.println("]");            
*/
            }
         }

         //light up final pin with final tally mapped from 0, 1023
         //0 - 255
         int brightness = map(percent, 0, 20, 0, 255);
         analogWrite(barPins[finalPin], brightness); 
/*         Serial.print("Pin [");
         Serial.print(barPins[finalPin]);
         Serial.print("] Brightness [");
         Serial.print(brightness);
         Serial.println("]"); 
*/
      }

      if (radio.ACK_REQUESTED){
         radio.sendACK();
         delay(10);
     }
  }

   //Every period we're going to blast this thing out.
   // we're not expecting anybody to answer, they're just going to
   // use it to figure out how far away they are.
   unsigned int currPeriod = millis()/TRANSMITPERIOD;
   if (currPeriod != lastPeriod){
      lastPeriod=currPeriod;
      byte payload = 1;
      radio.send(SPAM_NODE, (const void*)(&payload), sizeof(payload));
  }
}


