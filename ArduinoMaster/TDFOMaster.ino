// Hacked up version of the demo gateway from Mr. Rusu at http://www.lowpoerlab.com, the moteino's come from there
// and i highly recomend them.
#include <RFM69.h>
#include <TDFBO.h>

#define NODEID        1    //unique for each node on same network
#define NETWORKID     16  //the same on all nodes that talk to each other
#define FREQUENCY     RF69_915MHZ

#define LED           9  // Moteinos have LEDs on D9
#define SERIAL_BAUD   115200
#define SPAM_NODE     99   //This is the node we send all of our power checks too. 
                           // All the slaves will be in promiscous mode and get these
                           // but that node will never be listening.
#define TRANSMITPERIOD 100 //How often do we spam our network?  10 times a second seems good.  Should be quick enough, unless those
                           //slaves are being chatty.

unsigned long lastPeriod = -1;

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
         Serial.print("] Strength: ");
         Serial.println(theData.avgStrength);

         //TODO: Adjust information for node radio.SENDERID
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


