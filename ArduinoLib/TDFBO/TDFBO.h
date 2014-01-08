#ifndef TDFBO_h
#define TDFBO_h 

#include "Arduino.h"

#define NETWORKID     16  //the same on all nodes that talk to each other
#define GATEWAYID     1
#define NETWORKID     16  //the same on all nodes that talk to each other
#define GATEWAYID     1
#define SERIAL_BAUD   115200
#define FREQUENCY     RF69_915MHZ

#define SPAM_NODE     99   //This is the node we send all of our power checks too. 
                           // All the slaves will be in promiscous mode and get these
                           // but that node will never be listening.


typedef struct {
   int avgStrength;
} SignalStrengthPing;

#endif
