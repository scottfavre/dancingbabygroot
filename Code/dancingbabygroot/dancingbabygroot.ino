#include <Adafruit_MotorShield.h>
#include <Wire.h>

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

#define MOTOR_SHIELD_POWER_CTRL_PIN 2

Adafruit_VS1053_FilePlayer musicPlayer = 
      // create breakout-example object!
      //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
      // create shield-example object!
      Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);
      
File currentFile;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
      
// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
      
      
void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Library Test");

  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
    
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(50,50);

  /***** Two interrupt options! *******/ 
  // This option uses timer0, this means timer1 & t2 are not required
  // (so you can use 'em for Servos, etc) BUT millis() can lose time
  // since we're hitchhiking on top of the millis() tracker
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  // *** This method is preferred
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
    
  currentFile = SD.open("/").openNextFile();
  
  pinMode(MOTOR_SHIELD_POWER_CTRL_PIN, OUTPUT);
  digitalWrite(MOTOR_SHIELD_POWER_CTRL_PIN, HIGH);
 
  delay(50);
  
  AFMS.begin();
}

void loop() {  
  if(!musicPlayer.playingMusic) {
    myMotor->run(RELEASE);
    currentFile.close();
    currentFile = currentFile.openNextFile();
    
    if(!currentFile) {
      Serial.println("Done playing music");
      currentFile = SD.open("/").openNextFile();
    }
    
    if(!musicPlayer.startPlayingFile(currentFile.name())) {
      Serial.println("Could not open file:");
      Serial.println(currentFile.name());
      while (1);
    } else {
      Serial.println("Playing:");
      Serial.println(currentFile.name());
 
      delay(2000);
     
      myMotor->setSpeed(75);
      myMotor->run(FORWARD);
    }
  }
  else
  {
    delay(10);
  }
}
