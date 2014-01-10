#include <RFM69.h> 
#include <TDFBO.h>
#include <FrequencyTimer2.h>

#define NODEID        2    //unique for each node on same network

int TRANSMITPERIOD = 500; //transmit a packet to gateway so often (in ms)
                           //Half a second right now, we should be seeing a running average around
                           //10 seconds.  Don't need to update all that often and if we're too chatty then 
                           //the master is going to struggle to keep up (is it? no idea.  Should run that experiment)

#define SAMPLE_SIZE  100    //First try was at 100, but it was slow to respond.  Don't want it jumping all around but want
                           // it clear what the cause is
RFM69 radio;

unsigned long lastPeriod = -1;
int signalSamples[SAMPLE_SIZE];  //We take 100 samples
int currentSample = 0;

//TODO: Move this into an object so I can reuse it.
void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

volatile bool TransmitNow = false;
void ToggleTransmit(){
   TransmitNow = true;
}

void setup() {
   Serial.begin(SERIAL_BAUD);

   pinMode(FREQUENCYTIMER2_PIN, OUTPUT);
   FrequencyTimer2::setPeriod(TRANSMITPERIOD * 1000); //convert to milliseconds
   FrequencyTimer2::setOnOverflow(ToggleTransmit);
   FrequencyTimer2::enable();

   radio.initialize(FREQUENCY,NODEID,NETWORKID);
   radio.setHighPower();
   radio.promiscuous(true); //listen to everything!  Is this wise?  Rx is low power but the master has a much
                           //more reliable power rating... yes it is.  Because you want the one with the reliable
                           //power broadcasting, not listening
   radio.encrypt(0);
   delay(2000);  //Pause for a second...
   Serial.println("Beginning Transmission");
 }

void loop() {
   if (radio.receiveDone()){
      Serial.print("WE GOT SOMETHING FOR [");
      Serial.print(radio.TARGETID);
      Serial.println("]");
      //We're listening for anything, check to see if it's for something we want to hear about
      if(radio.TARGETID == SPAM_NODE){
         //This is a broadcast message to everybody.  We use this to measure signal strength,
         //TODO:  Check RSSI and make sure it's valid.  We get some funky readings from this.
         Serial.print("Received RSSI [");
         Serial.print(radio.RSSI);
         Serial.println("]");
         signalSamples[currentSample] = radio.RSSI;
         currentSample++;
         if(currentSample >= SAMPLE_SIZE){
            currentSample = 0;
         }
      } else if (radio.TARGETID == NODEID){
         //OMG THEY CALLED US!!!!
         //Not entirely sure what we would be doing this for..
         Serial.print("Recieved [");Serial.print(radio.SENDERID, DEC);Serial.print("] ");
         for (byte i = 0; i < radio.DATALEN; i++)
            Serial.print((char)radio.DATA[i]);
         Serial.println();
         Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
        
         //Only ACK if it was too us 
         if (radio.ACK_REQUESTED){
            radio.sendACK();
            delay(10); 
         }
      } else if(radio.TARGETID == GATEWAYID) {
         //Somebody is trying to tell the master how far away they are
         //We can use this to find out if we're the closest.  
         
      } else {
         Serial.print("Trying to reach [");
         Serial.print(radio.TARGETID);
         Serial.println("]");
      }
      Serial.println();
   }
  
   if(TransmitNow){

      //Step 1: figure out the average
      //Step 2: Let somebody know

      //Are we going to run into overflow problems here?  I doubt it at 100 samples most less than 1000 we should
      // have plenty of space      
      int totalStrength = 0;
      int sampleCount = 0;
   
      for(int x=0;x < SAMPLE_SIZE; x++){
         //TODO: Floor check?  This needs some experimentation to set so leaving it out.
         if(signalSamples[x] != 0){ //Zero means we haven't seen it yet.  Going to be noisy to start.
            totalStrength += signalSamples[x];
            sampleCount++;         
         } 
      }
      int avgSignal = 0;
      
      if(sampleCount > 0){
         avgSignal = totalStrength / sampleCount;
      } 

      SignalStrengthPing reading;
      reading.avgStrength = avgSignal;
     // Serial.print("AVG: [");
     // Serial.print(avgSignal);

      if(radio.sendWithRetry(GATEWAYID, (const void*)(&reading), sizeof(reading), 2,30 )){
      //   Serial.println("] [SUCCESS]"); 
      } else {
       //  Serial.println("] [FAILURE]");       
      }
      TransmitNow = false;   
  }
}

