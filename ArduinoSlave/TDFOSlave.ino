#include <RFM69.h> 

#define NODEID        2    //unique for each node on same network, need to figure out a way to get this dynamically
#define NETWORKID     16  //the same on all nodes that talk to each other
#define GATEWAYID     1

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    0 //Off otherwise exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      30 // max # of ms to wait for an ack
#define LED           9  // Moteinos have LEDs on D9
#define SERIAL_BAUD   115200
#define LOOPS         25

#define SUCCESS_LED  4
#define ACK_LED      6

int TRANSMITPERIOD = 300; //transmit a packet to gateway so often (in ms)

unsigned int sequence = 0;
unsigned int runNumber = 0;
unsigned int loopCount = 0;

volatile bool advanceRun = false;
volatile long lastInterruptFire = 0;

RFM69 radio;

typedef struct {
  unsigned int runNumber;
  long sendTime;
  unsigned int sequence;
} Payload;

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

void nextRun(){
   if(millis() - lastInterruptFire > 100){
      advanceRun = true;
   }
   lastInterruptFire = millis();
}

void setup() {
   pinMode(SUCCESS_LED, OUTPUT);
   attachInterrupt(1, nextRun, RISING);
   digitalWrite(SUCCESS_LED, LOW); 
   Serial.begin(SERIAL_BAUD);
   radio.initialize(FREQUENCY,NODEID,NETWORKID);
#ifdef IS_RFM69HW
   radio.setHighPower(); //uncomment only for RFM69HW!
#endif
   radio.encrypt(ENCRYPTKEY);
   Serial.println("Beginning Transmission");
   Blink(ACK_LED, 1000);
   Blink(LED, 1000); 
 }

long lastPeriod = -1;
void loop() {
   if(advanceRun){
      runNumber++;
      digitalWrite(SUCCESS_LED, LOW);
      loopCount = 0;
      advanceRun = false;
   }

   //check for any received packets.  Shouldn't actually be anything.  But maybe I'll change that.
   if (radio.receiveDone()){
      Serial.print("Recieved [");Serial.print(radio.SENDERID, DEC);Serial.print("] ");
      for (byte i = 0; i < radio.DATALEN; i++)
        Serial.print((char)radio.DATA[i]);
      Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
    
      if (radio.ACK_REQUESTED){
         radio.sendACK();
         delay(10); 
      } 
      Serial.println();
   }
   
   int currPeriod = millis()/TRANSMITPERIOD;
   if (currPeriod != lastPeriod){
      lastPeriod=currPeriod;

      if(loopCount >= LOOPS){
         digitalWrite(SUCCESS_LED, HIGH);
         loopCount = 0;
      }

      Payload output;   
      output.runNumber = runNumber;
      output.sendTime = millis();
      output.sequence = sequence;
 
      Serial.print("Sending Run [");
      Serial.print(runNumber);
      Serial.print("], Sequence [");
      Serial.print(sequence);
   
      if(radio.sendWithRetry(GATEWAYID, (const void*)(&output), sizeof(output), 2,ACK_TIME )){
         Serial.println("] [SUCCESS]");       
         //Blink the ACK_LED
         Blink(ACK_LED, 3);
      } else {
         Serial.println("] [FAILURE]");       
      }

      //Pop our counters
      sequence++;
      loopCount++;
   
      Blink(LED,3);
  }
}

