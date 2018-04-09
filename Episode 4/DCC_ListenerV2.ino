/////////////////////////////////// DCC Listener ////////////////////////////////////////////////
// This sketch runs on a dedicated Arduino connected to the layout using the MynaBay DCC Monitor
// Library to listen for accessory commands.
// When it hears on, it sends the command over the I2C bus to the servo controller whose I2C address
// is configured in the myAccessories array element corresponding to the accessory address.
// e.g a command is received for accessory 4. Element [4] in myAccessories[] contains the value 8, so the 
// command is sent to the Arduino using I2C to execute the command.
// This means we can scale for larger layouts by configuring multiple servo controllers each controlling 
// 16 servos/accessory addresses. (assuming use of the Adafruit servo controller shield).
// Richard Latham 22 March 2018
// Add check that address in range 3 April 18 RL



#include <DCC_Decoder.h>
#include <Wire.h>


int lastCommand;
int lastAddress;

int myAccessories[16];    //used to determine which (servo controller board) I2C address to send incoming command to. See Setup below 
                          //Change the size of this array and populate the controller I2C address in the setup if you want more 
                          //controller boards. Obviously the servo controller board sketch needs to be configured with the same
                          // I2C address!

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Definitions
//
#define kDCC_INTERRUPT            0


#define DEBUG 1  //uncomment to turn on debug messages



/////////////////////////////// Accessory Packed Handler pseudo interrupt service routine ////////////////////////////
// Don't add too much code here or it will start missing packets
//

void BasicAccDecoderPacket_Handler(int address, boolean activate, byte data)
{
        // Convert NMRA packet address format to human address
    address -= 1;
    address *= 4;
    address += 1;
    address += (data & 0x06) >> 1;
    
    boolean enable = (data & 0x01) ? 1 : 0;

    if ((lastCommand == enable) && (lastAddress == address))
    {
     #ifdef DEBUG 
     Serial.println("Duplicate command");
     #endif 
    }
    else
    {
          if ((address > 0) && (address < (sizeof(myAccessories)/sizeof(myAccessories[0]))))
           {
            Wire.beginTransmission(myAccessories[address]); // transmit to device #8
            Wire.write(address);       
            Wire.write(enable);        
            Wire.endTransmission();    // stop transmitting            
            
            #ifdef DEBUG   
             Serial.println("Valid address");     
             Serial.print(address,DEC);
             Serial.print(":");
             Serial.println(enable,DEC);
             Serial.print("I2C target address: ");
             Serial.println(myAccessories[address]);
            #endif
            }
      lastCommand = enable;
      lastAddress = address;
    }
  
} //end function

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Setup
//
void setup() 
{ 
   #ifdef DEBUG 
    Serial.begin(9600);
   #endif 
   DCC.SetBasicAccessoryDecoderPacketHandler(BasicAccDecoderPacket_Handler, true);
   DCC.SetupDecoder( 0x00, 0x00, kDCC_INTERRUPT );

   
   Wire.begin(10);                // join i2c bus with address #10

  
   myAccessories[0] = 8;   //will never be used with JMRI as 0 not a valid address
   myAccessories[1] = 8;
   myAccessories[2] = 8;
   myAccessories[3] = 8;
   myAccessories[4] = 8;
   myAccessories[5] = 8;
   myAccessories[6] = 8;
   myAccessories[7] = 8;
   myAccessories[8] = 8;
   myAccessories[9] = 8;
   myAccessories[10] = 8;
   myAccessories[11] = 8;
   myAccessories[12] = 8;
   myAccessories[13] = 8;
   myAccessories[14] = 8;
   myAccessories[15] = 8;
   myAccessories[16] = 8;
       
   lastCommand = 3;   //Not a valid command so first command received will be treated as different to last
   lastAddress = 0;   //Not a valid address in JMRI so first command will be treated as different to last
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
        // Loop DCC library
    DCC.loop();

    
}  //end loop

// end sketch
