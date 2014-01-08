// Hacked up version of the demo gateway from Mr. Rusu at http://www.lowpoerlab.com, the moteino's come from there
// and i highly recomend them.
#include <RFM69.h>
#include <TDFBO.h>

#define NODEID        1    //unique for each node on same network

#define TRANSMITPERIOD 100 //How often do we spam our network?  10 times a second seems good.  Should be quick enough, unless those
                           //slaves are being chatty.

unsigned long lastPeriod = -1;
int barPins[] = {3, 5, 6, 9, 10};
RFM69 radio;

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
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
   if (radio.receiveDone()){
      if (radio.DATALEN != sizeof(SignalStrengthPing)) {
         Serial.print("Invalid payload received, not matching Payload struct!");
      } else {
         SignalStrengthPing theData = *(SignalStrengthPing*)radio.DATA;

         Serial.print("Node [");
         Serial.print(radio.SENDERID);
         Serial.print("] Strength: [");
         Serial.print(theData.avgStrength);

         //TODO: Adjust information for node radio.SENDERID
         //To make this photogenic we're going to have to pick some variables
         // RIGHT NEXT STORE == -26
         // TOO FAR == 100
         // let's scale it between those too
         
         int topVal = constrain(theData.avgStrength, -25, -100);
         int percent = map(theData.avgStrength, -100, -25 , 0, 100);
         Serial.print("] Mapped to [");
         Serial.print(percent);
         Serial.println("]");

         // Going to have 5 LED's
         //BRUTE FORCE
         int finalPin = 0;
         int finalTally = 0;
         if(percent > 80){
            //Light up 1-4
            finalTally = percent - 80;
            finalPin = 4;
         } else if (percent > 60){
            //light up 1-3
            finalTally = percent - 60;
            finalPin = 3;
         } else if (percent > 40){
            //light up 1-2
            finalTally = percent -40;
            finalPin = 2;
         } else if (percent > 20){
            //light up 1
            finalTally = percent - 20;
            finalPin = 1;
         } else {
            finalTall = percent;
            finalPin = 0;
         }
         //light up final pin with final tally mapped from 0, 1023
         
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


