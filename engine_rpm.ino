float sp,flag1=0,flag2=0,t2=0,t1=0,ac=0;
volatile byte rpmcount;
unsigned int rpm;
unsigned long timeold;
void setup()
{
  
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), rpm_fun, CHANGE);
  rpmcount = 0;
  rpm = 0;
  timeold = 0;
}

void loop()
{

    if (rpmcount >= 3) 
    { 
     rpm = ((60000/(millis() - timeold))*rpmcount);
     timeold = millis();
     rpmcount = 0;
     /*sp=0.105728*rpm;
     Serial.print(sp);
     Serial.print("    ");*/
     Serial.print(rpm);
     Serial.println();
    }
   /* if(sp>20&&flag1==0)
    {
     flag1++;
     t1=millis();
    }
    if(sp>50&&flag2==0)
     {
      flag2++;
      t2=millis();
      Serial.println("\nTime :");
      Serial.println(t2-t1);
      ac=8.333/((t2-t1)/1000);
      Serial.println("Acceleration:");
      Serial.println(ac);
     }*/


}
void rpm_fun()
{
  rpmcount++;
}
