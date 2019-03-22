/*
  SD card write
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 */
/*
 * INSTRUCTIONS FOR RPM and SPEED (+ACCELERATION)
 * connect the Black pin to pin 2
 * connect the brown pin to vcc
 * connect the blue 'pin to gnd
 */
#include <SPI.h>
#include <SD.h>
#include <NewPing.h>
 
#define SONAR_NUM     4 // Number or sensors.
#define MAX_DISTANCE 200 // Max distance in cm.
#define PING_INTERVAL 33 // Milliseconds between pings.

File myFile;
float sp,flag1=0,flag2=0,flag_start=0,t2=0,t1=0,ac=0;
volatile byte rpmcount=0,nw;
unsigned int rpm;
unsigned long timeold;
/*
 * SPEED CODE END
 */

 /*
  * DAMPER TRAVEL START
  */
unsigned long pingTimer[SONAR_NUM]; // When each pings.
unsigned int cm[SONAR_NUM]; // Store ping distances.
uint8_t currentSensor = 0; // Which sensor is active.

 
NewPing sonar[SONAR_NUM] = { // Sensor object array.
  NewPing(A5, 5, MAX_DISTANCE),
  NewPing(A2, 9, MAX_DISTANCE),
  NewPing(A3, 6, MAX_DISTANCE),
  NewPing(A4, 3, MAX_DISTANCE)
 // NewPing(A1,10, MAX_DISTANCE)
};

 /*
 * DAMPER TRAVEL END
 */
void setup() {
  pinMode(A0,OUTPUT);
  pinMode(A6,OUTPUT);
  pinMode(A7,OUTPUT);
  digitalWrite(A0,1);
  //Serial.begin(115200);
  /*
   * Damper travel code starts here:
   */
  pingTimer[0] = millis() + 75; // First ping start in ms.
  for (uint8_t i = 1; i < SONAR_NUM; i++)
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
    /*
     *
     *SPEED CODE STARTS HERE 
     
     */
  attachInterrupt(digitalPinToInterrupt(2), rpm_fun, RISING);
  rpmcount = 0;
  rpm = 0;
  timeold = 0;

  /*while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }*/
  

  //Serial.print("Initializing SD card...");
  nw++;
  if (!SD.begin(4)) {
    //Serial.println("initialization failed!");
    digitalWrite(A7,HIGH);
    digitalWrite(A6,LOW);
    return;
  }
  //Serial.println("initialization done.");
  pinMode(A0,OUTPUT);
  digitalWrite(A0,1);
  /*
     *
     *SPEED CODE ENDS HERE 
     
     */
    flag_start=0; 
}
 
void loop() {/*
     *
     *SPEED CODE STARTS HERE 
     
     */
     myFile = SD.open("RPM.txt", FILE_WRITE);
     if(flag_start==0)
     {
      myFile.println("New DATA:");
     }
     
    if (rpmcount >= 3) 
    { 
     rpm = ((60000/(millis() - timeold))*rpmcount);
     timeold = millis();
     rpmcount = 0;
     sp=0.105728*rpm;
    // Serial.print(sp);
     //Serial.print("    ");
     //Serial.print(rpm);
     //Serial.println();
     //Serial.println("Writing of SD card!");
     myFile.print(sp);
     myFile.print("    ");
     myFile.print(rpm);
     myFile.println();
    }
    if(sp>20&&flag1==0)
    {
     flag1++;
     t1=millis();
    }
    if(sp>50&&flag2==0)
     {
      flag2++;
      t2=millis();
      //Serial.println("\nTime :");
      //Serial.println(t2-t1);
      ac=8.333/((t2-t1)/1000);
      //Serial.println("Acceleration:");
      //Serial.println(ac);
      myFile.println("\nTime :");
      myFile.println(t2-t1);
      myFile.println("Acceleration:");
      myFile.println(ac);
     }

    myFile.close();
     /*
     *
     *SPEED CODE ENDS HERE 
     
     */

     
  /*
   * Damper travel code starts here:
   */
   myFile = SD.open("Damper.txt", FILE_WRITE);
   if(flag_start==0)
   {
      myFile.println("New DATA:");
      flag_start++;
   }
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    if (millis() >= pingTimer[i]) {
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;
      if (i == 0 && currentSensor == SONAR_NUM - 1)
        oneSensorCycle(); // Do something with results.
      sonar[currentSensor].timer_stop();
      currentSensor = i;
      cm[currentSensor] = 0;
      sonar[currentSensor].ping_timer(echoCheck);
    }
  }
  myFile.close();
  // The rest of your code would go here.
  /*
   * Damper travel code ENDS here:
   */
}
 
void echoCheck() { // If ping echo, set distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}
 
void oneSensorCycle() { // Do something with the results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    //Serial.print(i+1);
    //Serial.print("=");
    //Serial.print(cm[i]);
    //Serial.print("cm ");
    myFile.print(cm[i]);
    myFile.print("\t");
    
  }
  //Serial.println();
  myFile.println();
}
void rpm_fun()
{
  rpmcount++;
}
