/*****************
 * Example: ATEM Monitor
 * Connects to the Atem Switcher and outputs changes to Preview and Program on the Serial monitor (at 9600 baud)
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an Atem Switcher connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */



// Including libraries: 
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>

//#include <MemoryFree.h>



// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB2 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 200, 50);              // <= SETUP!  IP address of the Arduino


// Include ATEM library and make an instance:
// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
#include <ATEM.h>
ATEM AtemSwitcher(IPAddress(192, 168, 200, 240), 56417);  // <= SETUP (the IP address of the ATEM switcher)


//define gpio input pins panasonic aw-rp120 console
#define PINDSUB10 A0 // select the input pin
#define PINDSUB11 A2 // select the input pin
#define PINDSUB23 A1 // select the input pin
#define PINDSUB24 A3 // select the input pin

#define Tally1 2 // select the output pin
#define Tally2 3 // select the output pin
#define Tally3 4 // select the output pin
#define Tally4 5 // select the output pin
#define Tally5 6 // select the output pin
#define Tally6 7 // select the output pin

int bi, bin, binA; // bits from panasonic console
//int auxCurrent;



// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}

int PGtally = 0;
int NewPGtally = 0;

void setup() { 

pinMode(A0, INPUT);
pinMode(A1, INPUT);
pinMode(A2, INPUT);
pinMode(A3, INPUT);

pinMode(Tally1, OUTPUT);
pinMode(Tally2, OUTPUT);
pinMode(Tally3, OUTPUT);
pinMode(Tally4, OUTPUT);
pinMode(Tally5, OUTPUT);
pinMode(Tally6, OUTPUT);


    randomSeed(analogRead(5));
  
  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(9600);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  // Initialize a connection to the switcher:
  AtemSwitcher.serialOutput(0x80);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();

  // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
    AtemSwitcher.runLoop();

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    Serial << F("Connection to ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher.connect();

    
  }  
  
  NewPGtally = AtemSwitcher.getProgramInput();
    if (NewPGtally != PGtally){
            PGtally = NewPGtally;
            Serial.print("Program Tallly has changed: ");
            Serial.println(PGtally);

       digitalWrite(Tally1, !AtemSwitcher.getProgramTally(1));
       digitalWrite(Tally2, !AtemSwitcher.getProgramTally(2));
       digitalWrite(Tally3, !AtemSwitcher.getProgramTally(3));
       digitalWrite(Tally4, !AtemSwitcher.getProgramTally(4));
       digitalWrite(Tally5, !AtemSwitcher.getProgramTally(5));
       digitalWrite(Tally6, !AtemSwitcher.getProgramTally(6));
       
            }

            //decoding code from panasonic aw-rp120 console
            bi = (digitalRead(PINDSUB10)<<3)+(digitalRead(PINDSUB11)<<2)+(digitalRead(PINDSUB23)<<1)+digitalRead(PINDSUB24); // Read data from pins and convert it to 4 bit int
            if (bin != bi){
            bin = bi;
            btnPressed(bin); // pass Panasonic code to function
            }
 
  
}

void btnPressed (int bin){
  Serial.print("pressed button : ");
  switch (bin) {
   case 7:
    binA=1; 
    break;
   case 11: 
    binA=2; 
    break;
   case 3: 
    binA=3; 
    break;
   case 13: 
    binA=4; 
    break;
   case 5: 
    binA=5; 
    break;
   case 9: 
    binA=6; 
    break;
   case 1: 
    binA=7; 
    break;
   case 14: 
    binA=8; 
    break;
   case 6: 
    binA=10011; 
    break;
   case 10: 
    binA=10010; 
    break;
   default:
   Serial.print("not defined: ");
   Serial.println(bin);
  }
  
  Serial.println(binA);
  AtemSwitcher.changeAuxState(2, binA);
}

/*
 * change ATEM.CPP changeAuxState
  void ATEM::changeAuxState(uint8_t auxOutput, uint16_t inputNumber)  {

  _wipeCleanPacketBuffer(); // Pan Change
      uint8_t commandBytes[4] = {1, auxOutput-1, highByte(inputNumber), lowByte(inputNumber)};
      _sendCommandPacket("CAuS", commandBytes, 4);
 }
 
 */
