// Definitions
/// For the servo stuff
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>

// To do
// 1. Deal with looping sound clip

////////////////////////////////////////////////////////////////////////////////////////////
// NOTE - this sketch asssumes the element number of the gAddresses array is the same as the DCC
// address. i.e DCC address 1 -16 for an adafruit servo shield. This avoids the performance overhead of iterating through the array seeking the
// correct element. If you wish to use a range of consecutive DCC addresses starting
// at a number higher than 1, the see the onreceive event below where you can handle this by
// using an offset.
////////////////////////////////////////////////////////////////////////////////////////////////  

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Definitions
//



//#define DEBUG 1  //uncomment to turn on debug messages

typedef struct                                //Added RL
{
 int                  ard_pin;                    // Arduino pin used for trigger
 unsigned long        clipLengthMilli;        // the length of the audio clip in milliseconds
 unsigned long        triggerLengthMilli;     // How long LOW voltage to be presented on the pin to fire the sound
 unsigned long        triggerEndMilli;        // The system time at which to reset pin voltage HIGH
 boolean              isPlaying;              // Set true while the clip is being played
} mySound;

 mySound mySounds[2];                        //initialise an array of sound effects

typedef struct
{
    int               soundClip;               // Added RL audio clip to use 0 = none;
    boolean           soundTriggered;         // Added RL true means this decoder triggered the sound that is playing
    int               durationMilli;          // Milliseconds to leave output on for.  0 means don't auto off
    int               servonum;               // Added RL - the address of the servo on the servo controller board
    int               sweepMin;               // Added RL - inactive servo position
    int               sweepMax;               // Added RL - active servo position - may be smaller than.sweepMin 
                                              // when .incToActivate is -1 
    int               sweepPos;               // Added RL - current position of servo
    int               incToActivate ;          // Added RL - servo may activate the accessory by moving clockwise
                                              // or anti-clockwise. Set Inc(rement)ToActivate to 1 or -1 to configure this.
    boolean           isMoving;               // Added RL - Is the servo in the process of opening or closing
    int               targetState;            // Added RL - the state we want the servo in 1 = sweepMax, 0 = sweepMin
    int               lastCommand;            // Added RL - used to store newly received command before handling it
    boolean           lastCommandHandled;     // Added RL - true if new command has not been handled, otherwise false
    
    unsigned long     onMilli;                // Used internally for timing
    unsigned long     offMilli;               // 
} DCCAccessoryAddress;

DCCAccessoryAddress gAddresses[9];            //initialise an array of accessory decoders


// for servo management

byte value;

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


// our servo # variable
uint8_t servonum = 0;

//sound control globals
unsigned long playbackEndMillis;
boolean       soundPlaying;              // set true when a sound effect is being played
int           playbackClip;              // the clipNumber from mySounds[] being played



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Decoder Init 
//
void ConfigureDecoder()
{

    int eepromState;
    
    gAddresses[0].soundClip = 1;
    gAddresses[0].soundTriggered = false;
    gAddresses[0].durationMilli = 10;
    gAddresses[0].servonum = 0;
    gAddresses[0].sweepMin = 300;
    gAddresses[0].sweepMax = 420;
    gAddresses[0].incToActivate = 1;
    gAddresses[0].sweepPos = gAddresses[0].sweepMin; 
    gAddresses[0].isMoving = false;   
    gAddresses[0].targetState = 0;
    gAddresses[0].lastCommand = 0;
    gAddresses[0].lastCommandHandled = true; 
    gAddresses[0].onMilli = 0;
    gAddresses[0].offMilli = 0; 
     
    
    gAddresses[1].soundClip = 1;
    gAddresses[1].soundTriggered = false;
    gAddresses[1].durationMilli = 10;
    gAddresses[1].servonum = 1;
    gAddresses[1].sweepMin = 300;
    gAddresses[1].sweepMax = 420;
    gAddresses[1].incToActivate = 1;
    gAddresses[1].sweepPos = gAddresses[1].sweepMin; 
    gAddresses[1].isMoving = false;   
    gAddresses[1].targetState = 0;
    gAddresses[1].lastCommand = 0;
    gAddresses[1].lastCommandHandled = true; 
    gAddresses[1].onMilli = 0;
    gAddresses[1].offMilli = 0; 

    
    gAddresses[2].soundClip = 1;
    gAddresses[2].soundTriggered = false;
    gAddresses[2].durationMilli = 10;
    gAddresses[2].servonum = 2;
    gAddresses[2].sweepMin = 300;
    gAddresses[2].sweepMax = 420;
    gAddresses[2].incToActivate = 1;
    gAddresses[2].sweepPos = gAddresses[2].sweepMin; 
    gAddresses[2].isMoving = false;   
    gAddresses[2].targetState = 0;
    gAddresses[2].lastCommand = 0;
    gAddresses[2].lastCommandHandled = true; 
    gAddresses[2].onMilli = 0;
    gAddresses[2].offMilli = 0;

    
    gAddresses[3].soundClip = 1;
    gAddresses[3].soundTriggered = false;
    gAddresses[3].durationMilli = 10;
    gAddresses[3].servonum = 3;
    gAddresses[3].sweepMin = 300;
    gAddresses[3].sweepMax = 420;
    gAddresses[3].incToActivate = 1;
    gAddresses[3].sweepPos = gAddresses[3].sweepMin; 
    gAddresses[3].isMoving = false;   
    gAddresses[3].targetState = 0;
    gAddresses[3].lastCommand = 0;
    gAddresses[3].lastCommandHandled = true; 
    gAddresses[3].onMilli = 0;
    gAddresses[3].offMilli = 0; 

    
    gAddresses[4].soundClip = 1;
    gAddresses[4].soundTriggered = false;
    gAddresses[4].durationMilli = 10;
    gAddresses[4].servonum = 4;
    gAddresses[4].sweepMin = 300;
    gAddresses[4].sweepMax = 420;
    gAddresses[4].incToActivate = 1;
    gAddresses[4].sweepPos = gAddresses[4].sweepMin; 
    gAddresses[4].isMoving = false;   
    gAddresses[4].targetState = 0;
    gAddresses[4].lastCommand = 0;
    gAddresses[4].lastCommandHandled = true; 
    gAddresses[4].onMilli = 0;
    gAddresses[4].offMilli = 0; 
     
    
    gAddresses[5].soundClip = 1;
    gAddresses[5].soundTriggered = false;
    gAddresses[5].durationMilli = 10;
    gAddresses[5].servonum = 5;
    gAddresses[5].sweepMin = 300;
    gAddresses[5].sweepMax = 420;
    gAddresses[5].incToActivate = 1;
    gAddresses[5].sweepPos = gAddresses[5].sweepMin; 
    gAddresses[5].isMoving = false;   
    gAddresses[5].targetState = 0;
    gAddresses[5].lastCommand = 0;
    gAddresses[5].lastCommandHandled = true; 
    gAddresses[5].onMilli = 0;
    gAddresses[5].offMilli = 0; 

    
    gAddresses[6].soundClip = 1;
    gAddresses[6].soundTriggered = false;
    gAddresses[6].durationMilli = 10;
    gAddresses[6].servonum = 6;
    gAddresses[6].sweepMin = 300;
    gAddresses[6].sweepMax = 420;
    gAddresses[6].incToActivate = 1;
    gAddresses[6].sweepPos = gAddresses[6].sweepMin; 
    gAddresses[6].isMoving = false;   
    gAddresses[6].targetState = 0;
    gAddresses[6].lastCommand = 0;
    gAddresses[6].lastCommandHandled = true; 
    gAddresses[6].onMilli = 0;
    gAddresses[6].offMilli = 0; 
    
    
    gAddresses[7].soundClip = 1;
    gAddresses[7].soundTriggered = false;
    gAddresses[7].durationMilli = 10;
    gAddresses[7].servonum = 7;
    gAddresses[7].sweepMin = 300;
    gAddresses[7].sweepMax = 420;
    gAddresses[7].incToActivate = 1;
    gAddresses[7].sweepPos = gAddresses[7].sweepMin; 
    gAddresses[7].isMoving = false;   
    gAddresses[7].targetState = 0;
    gAddresses[7].lastCommand = 0;
    gAddresses[7].lastCommandHandled = true; 
    gAddresses[7].onMilli = 0;
    gAddresses[7].offMilli = 0; 

    gAddresses[8].soundClip = 1;
    gAddresses[8].soundTriggered = false;
    gAddresses[8].durationMilli = 10;
    gAddresses[8].servonum = 0;
    gAddresses[8].sweepMin = 420;
    gAddresses[8].sweepMax = 300;
    gAddresses[8].incToActivate = -1;
    gAddresses[8].sweepPos = gAddresses[8].sweepMin; 
    gAddresses[8].isMoving = false;   
    gAddresses[8].targetState = 0;
    gAddresses[8].lastCommand = 0;
    gAddresses[8].lastCommandHandled = true; 
    gAddresses[8].onMilli = 0;
    gAddresses[8].offMilli = 0; 

    // set them to the state stored in Non volatile memory
    // Iniatialised above to inactive so only change the ones that are active
    for (int i=0; i <= (int)(sizeof(gAddresses)/sizeof(gAddresses[0])); i++)
       {
         eepromState = EEPROM.read(i);
         #ifdef DEBUG
           Serial.print(i);
           Serial.print(":");
           Serial.println(eepromState);
         #endif
         if (eepromState == 1)
              {
                gAddresses[i].targetState = 1;
                gAddresses[i].sweepPos = gAddresses[i].sweepMax; 
                
              }  //end if
         
       }   //end for loop
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Configure sound clips
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConfigureSoundClips()
{
    // mySounds array element 0 is never used. The program treats sound clip 0 as no sound clip
    
    mySounds[0].ard_pin = 21;                      // Arduino pin used for trigger
    //pinMode(mySounds[0].ard_pin, OUTPUT);          // take the opportunity to configure the pin
    //digitalWrite(mySounds[0].ard_pin, HIGH);       // set it high so it doesn't start playing
    mySounds[0].clipLengthMilli = 1498;        // the length of the audio clip in milliseconds
    mySounds[0].triggerLengthMilli = 1000;     // How long LOW voltage to be presented on the pin to fire the sound 1 second 
                                               // seems to work to play it once
    mySounds[0].triggerEndMilli= 0;            // The system time at which to reset pin voltage HIGH 
    mySounds[0].isPlaying = false;             // If the clip is available to be played or is already playing
  
    mySounds[1].ard_pin = 22;                      // Arduino pin used for trigger
    pinMode(mySounds[1].ard_pin, OUTPUT);          // take the opportunity to configure the pin
    digitalWrite(mySounds[1].ard_pin, HIGH);       // set it high so it doesn't start playing
    mySounds[1].clipLengthMilli = 1498;        // the length of the audio clip in milliseconds
    mySounds[1].triggerLengthMilli = 200;     // How long LOW voltage to be presented on the pin to fire the sound 1 second 
                                               // seems to work to play it once
    mySounds[1].triggerEndMilli= 0;            // The system time at which to reset pin voltage HIGH 
    mySounds[1].isPlaying = false;             // If the clip is available to be played or is already playing
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Play a sound
//
void myPlaySound(int callingDecoder)
{
    
    #ifdef DEBUG
    Serial.print("Calling decoder = ");
    Serial.println(callingDecoder);
    Serial.print(gAddresses[callingDecoder].soundClip);
    Serial.println(" = Sound clip to play");
    Serial.print("Pin to trigger = ");
    Serial.println(mySounds[gAddresses[callingDecoder].soundClip].ard_pin);
    #endif

   
     //Check if the clip is already playing -  global soundPlaying true....
     if (soundPlaying)      
     {
        #ifdef DEBUG
          Serial.println(" A Soundclip is already Playing!");
        #endif
        return;
     }



     #ifdef DEBUG
     Serial.print("Write LOW to pin ");
     Serial.println(mySounds[gAddresses[callingDecoder].soundClip].ard_pin);
     #endif
     digitalWrite(mySounds[gAddresses[callingDecoder].soundClip].ard_pin, LOW);  //set pin LOW to start triggering the clip
          
     mySounds[gAddresses[callingDecoder].soundClip].isPlaying = true;
     mySounds[gAddresses[callingDecoder].soundClip].triggerEndMilli = (millis() + mySounds[gAddresses[callingDecoder].soundClip].triggerLengthMilli);    //set timer to set trigger pin high. Sound will have triggered.         
     playbackEndMillis = (millis() + mySounds[gAddresses[callingDecoder].soundClip].clipLengthMilli);
     soundPlaying = true;
     playbackClip = gAddresses[callingDecoder].soundClip;
     gAddresses[callingDecoder].soundTriggered = true;

     #ifdef DEBUG
     Serial.print("Sound triggered :");
     Serial.println(millis());
     Serial.print("Trigger end :");
     Serial.print(mySounds[gAddresses[callingDecoder].soundClip].triggerEndMilli);
     Serial.print(" Clip end :");
     Serial.println(playbackEndMillis);
     #endif
      
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Handle Command
//
// Setup the "decoder" (gAddresses element) to execute the command

void handleCommand(int md, int myEnable)

{
            
            //#ifdef DEBUG
            Serial.print("Command received: ");
            Serial.print(md,DEC);
            Serial.print("   activate: ");
            Serial.println(myEnable,DEC);
            //#endif

            // Is this command changing the state of the device?
            if (gAddresses[md].isMoving == false)
            {
              ///////////////////////////// Enable or Activate ///////////////////////////////////////////
              if( myEnable == 1 )
              {
                    //is this command changing the state of the accessory?
                    if (gAddresses[md].targetState == 0)           //currently disabled so enable
                            {
                            gAddresses[md].targetState = 1;
                            gAddresses[md].isMoving = true;
                            gAddresses[md].sweepPos = gAddresses[md].sweepMin;
                            // set the onmilli time
                            gAddresses[md].onMilli = (millis() + gAddresses[md].durationMilli);

                            // Play a sound clip ?
                            if (gAddresses[md].soundClip > 0)
                                     {
                                      myPlaySound(md);
                                     }

                            #ifdef DEBUG
                            Serial.print(md,DEC);
                            Serial.println(" Handler changed target state to 1"); 
                            #endif 

                            //Store the state to nonvolatile memory
                            EEPROM.update(md, myEnable);

                            }else {                                //already has target state enabled so don't do anything
                              #ifdef DEBUG
                              Serial.print(md,DEC);
                              Serial.println(" Accessory aleady enabled");
                              #endif
                            }                                      //end else target state
                        
              }                                                    // end if enable
              //////////////////////////// Disable or deactivate /////////////////////////////////////
                else{                                              //process request disable
                       #ifdef DEBUG
                       Serial.print(md,DEC);
                       Serial.println(" Handler get current accessory state");
                       Serial.print(md,DEC);
                       Serial.print(" targetState= ");
                       Serial.println(gAddresses[md].targetState);
                       #endif
                      //is this command changing the state of the accessory? ////////////////////////
                    if (gAddresses[md].targetState == 1)           //currently enabled so disable
                            {
                            gAddresses[md].targetState = 0;
                            gAddresses[md].isMoving = true;
                            // initialise the position counter with the fully active position
                            gAddresses[md].sweepPos = gAddresses[md].sweepMax;
                            // set the onmilli timer
                            gAddresses[md].onMilli = (millis() + gAddresses[md].durationMilli);

                            // Play a sound clip ?
                            if (gAddresses[md].soundClip > 0)
                                     {
                                      myPlaySound(md);
                                     }
                            
                            #ifdef DEBUG
                            Serial.print(md,DEC);
                            Serial.println("Handler Set target state to 0");
                            #endif

                            //Store the state to nonvolatile memory
                            EEPROM.update(md, myEnable);

                            }else {                                //already has target state disabled so don't do anything
                              #ifdef DEBUG
                              Serial.print(md,DEC);
                              Serial.println(" Accessory already disabled");
                              #endif
                            }                                      //end else target state
                   
                               
                     }                                             //end else enable
            }                                      // if ismoving false 
            
            ////////// Device already responding to a command ///////////////////////////
                 
            else{                                  // else ismoving is true
                 #ifdef DEBUG
                 Serial.print(md,DEC);
                 Serial.println(" Command ignored - accessory already changing state");
                 #endif
                 }                                 //end else ismoving is true
                 
            ///////////////////// end of address handling ////////////////////////////////
} //end function

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// receiveEvent handler - handles incoming I2C comms ////////////////////////////  
// function that executes whenever data is received from the DCC Listener Arduino which is attached to the layout, and 
// connected to the device running this servo controller sketch over I2C 
// this function is registered as an event, see setup(). 
// We expect to receive 2 bytes, address and command 
void receiveEvent(int howMany)
{
  int c = 0;
  int x = 0;
    while (1 < Wire.available()) 
         { // loop through all but the last 
          c = Wire.read(); // receive byte as integer
          }
    x = Wire.read();    // receive byte as an integer
     

   /////////////////////////////////////////////////////////////////////////////////
   // If this is a secondary accessory controller with a consecutive address range
   // starting higher than 16, say, subtract the offset from c before using it below
   // eg if you are using this controller to handle addresses 17 to 33, subtract 16 from c
   // to get the appropriate accessory array element
   /////////////////////////////////////////////////////////////////////////////////?

    gAddresses[c].lastCommand = x;
    gAddresses[c].lastCommandHandled = false;    //set flag so the loop process picks it up
   
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Setup
//
void setup() 
{ 
   Serial.begin(9600);
//   DCC.SetBasicAccessoryDecoderPacketHandler(BasicAccDecoderPacket_Handler, true);
   ConfigureSoundClips();
      
   ConfigureDecoder();

   pwm.begin();
  
   pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

   Wire.begin(8);                // join i2c bus with address #8
   Wire.onReceive(receiveEvent); // register event
  
   playbackClip = 0;
   playbackEndMillis = 0;
   soundPlaying = false;
   delay(10);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Main loop
//
void loop()
{
    
    static int addr = 0;
    
    
        ////////////////////////////////////////////////////////////////
        // Bump to next address to test
    if( ++addr >= (int)(sizeof(gAddresses)/sizeof(gAddresses[0])) )
    {
        addr = 0;
    }

    
        /////////////////////////////////////////////////////////////
        // See if there is a new command to process
        if (gAddresses[addr].lastCommandHandled == false)
            {
              handleCommand(addr,gAddresses[addr].lastCommand);
              gAddresses[addr].lastCommandHandled = true;
              #ifdef DEBUG
                Serial.print("Handled command : ");
                Serial.println(addr,DEC);
              #endif  
            }


        ///// see if this decoder has triggered a sound to play /////
        if (gAddresses[addr].soundTriggered)
        {
            if ((mySounds[gAddresses[addr].soundClip].triggerEndMilli) > 0 )     // timer is set so trigger still active
              {
                if ((mySounds[gAddresses[addr].soundClip].triggerEndMilli) <= millis())  // trigger off timer up
                        {
                          //turn of the trigger
                          #ifdef DEBUG
                          Serial.print(addr);
                          Serial.println(" Sound trigger off");
                          #endif
                          mySounds[gAddresses[addr].soundClip].triggerEndMilli = 0;
                          digitalWrite(mySounds[gAddresses[addr].soundClip].ard_pin, HIGH);  //set pin HIGH to stop triggering the clip
                        }
              }
               
                // See if the clip has finished playing
                // Need to cater for looping clips here.....
             if (millis() > playbackEndMillis) 
                { 
                mySounds[gAddresses[addr].soundClip].isPlaying = false;
                mySounds[gAddresses[addr].soundClip].triggerEndMilli = 0;
                playbackEndMillis = 0;
                soundPlaying = false;
                playbackClip = 0;
                gAddresses[addr].soundTriggered = false;
                #ifdef DEBUG
                Serial.print(addr);
                Serial.println(" Sound playback complete");
                #endif
                }

            
        }

        ///////////////////////////////////////////////////////////////////////////   
        /////// See if this is a servo and if we need to move servo a click ///////

        ////////////////////////////////////
        // Is the isMoving attribute set ?
        if (gAddresses[addr].isMoving)
        {
          //Serial.print("Is moving address: ");
          //Serial.print(addr);
        //Yes
            ////////////////////////////////////
            // Is the elapsed time(.onMilli  system time) >= the .durationMilli ?
            if(gAddresses[addr].onMilli <= millis())
               {
                    //Yes
                        //Reset the .onMilli
                        gAddresses[addr].onMilli = (millis() + gAddresses[addr].durationMilli);
                              // Move the servo one tick
                              //////////////////////////////
                              // Is the target state active
                              if (gAddresses[addr].targetState == 1)
                                   //Yes
                                   {
                                      gAddresses[addr].sweepPos = (gAddresses[addr].sweepPos + gAddresses[addr].incToActivate);    // increment .sweepPos
                                      // if sweepMax reached then Clear .IsMoving flag
                                      if (gAddresses[addr].sweepPos == gAddresses[addr].sweepMax)
                                              {
                                                gAddresses[addr].isMoving = false;
                                                #ifdef DEBUG
                                                Serial.print(addr);
                                                Serial.println(" Moving flag cleared");
                                                Serial.print(addr);
                                                Serial.print(" Target state = ");
                                                Serial.println(gAddresses[addr].targetState);
                                                #endif
                                              }
                                   }                                                                          
                                   //No
                                   if (gAddresses[addr].targetState == 0)
                                   {
                                      gAddresses[addr].sweepPos = (gAddresses[addr].sweepPos + (gAddresses[addr].incToActivate) * -1);  // decrement .sweepPos      
                                      //if sweepmin reached then clear .isMoving flag
                                      if (gAddresses[addr].sweepPos == gAddresses[addr].sweepMin)
                                              {
                                                gAddresses[addr].isMoving = false;
                                                #ifdef DEBUG
                                                Serial.print(addr);
                                                Serial.println(" Moving flag cleared");
                                                Serial.print(addr);
                                                Serial.print(" Target state = ");
                                                Serial.println(gAddresses[addr].targetState);
                                                #endif
                                              }
                                   }    
                        // send the new postition to the servo
                        pwm.setPWM(gAddresses[addr].servonum, 0, gAddresses[addr].sweepPos);  

                           
               }                           //onMilli reached
        }
        //No isMoving not set
           // do nothing with this address 
        
       
    
}       //end loop

// end sketch
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
