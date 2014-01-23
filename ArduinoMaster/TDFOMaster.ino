// Hacked up version of the demo gateway from Mr. Rusu at http://www.lowpoerlab.com, the moteino's come from there
// and i highly recomend them.

/*

   Let's talk message formats.  Master writes to serial to be read in python.  In a perfect world
I'd write some code to turn my arduino header into python readable code.  But I'm not going to
worry about that now.  ##TODO

   For now:

      <MessageType>|<NodeID>|<Payload>

   MessageType := 
      1 : Debug = Somebody just wants to hollar at you.
      2 : Signal Strength = This is the average signal strength for this node.  Normalized 0-100;
*/

#include <RFM69.h>
#include <TDFBO.h>

#define NODEID        1    //unique for each node on same network

#define TRANSMITPERIOD 100 //How often do we spam our network?  10 times a second seems good.  Should be quick enough, unless those
                           //slaves are being chatty.

#define WEAK_RANGE -50 //What do we consider weak?
#define BLINK_CYCLES  10
int cycles =0;

unsigned long lastPeriod = -1;
RFM69 radio;

void Blink(byte PIN, int DELAY_MS)
{
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

         // Got ourselves a signal strength
         // Need a better way to send these debug messages.  Serial handles the toString
         //    bits for me so I'm using it.
         Serial.print(Debug);
         Serial.print("|");
         Serial.print(radio.SENDERID);
         Serial.print("|");
         Serial.print("Strength: [");
         Serial.print(theData.avgStrength);
         Serial.println("]");

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
         Serial.print(Debug);
         Serial.print("|");
         Serial.print(radio.SENDERID);
         Serial.print("|");
         Serial.print("Mapped strength to [");
         Serial.print(percent);
         Serial.println("]");

         Serial.print(SignalStrength);
         Serial.print("|");
         Serial.print(radio.SENDERID);
         Serial.print("|");
         Serial.println(percent);

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
      cycles++;
      if(cycles > BLINK_CYCLES){
         Blink(9, 30);
         cycles = 0;
      }
  }
}


