#include <Servo.h>

Servo Saservo;
Servo Sbservo;
Servo Scservo;
Servo Sdservo;
Servo Seservo;

int Sa = 90;  // default Position
int Sb = 50;  //
int Sc = 40;  //
int Sd = 50;  //
int Se = 50;  //

int cst = 15;
int fSa = 160-cst;  // default Position
int fSb = 150-cst;  //
int fSc = 130-cst;  //
int fSd = 150-cst;  //
int fSe = 140-cst;  //

float EMG_1[2];
float EMG_2[2];
float EMG_3[2];

const int EMG1PIN = A0;
const int EMG2PIN = A1;
const int EMG3PIN = A2;

float alpha_low = 0.05;    //initialization of alpha (cutoff-frequency)
float alpha_high = 0.4;    //initialization of alpha
float a_low = 0.3;

float EMG_prime_low = 0;        //initialization of EMA S
float EMG_prime_high = 0;
 
int highpass = 0;
int bandpass = 0;
int bandstop = 0;

long previousTime = 0;
long timeIndex = 0;
int flag = 0;



void setup() {
Serial.begin(9600);
//EMG_prime_low = (float)analogRead(A1);
//EMG_prime_high = analogRead(A1);
EMG_1[1] = (float)analogRead(EMG1PIN);
EMG_2[1] = (float)analogRead(EMG2PIN);
EMG_3[1] = (float)analogRead(EMG3PIN);

  Saservo.attach(8);   //thumb servo
  Sbservo.attach(9);   //little finger servo
  Scservo.attach(10);  //ring finger servo
  Sdservo.attach(11);  //middle finger servo
  Seservo.attach(12);  //index finger servo

  Saservo.write(fSa);   //thumb
  Sbservo.write(fSb);   //little finger
  Scservo.write(fSc);   //ring finger
  Sdservo.write(fSd);   //middle finger
  Seservo.write(fSe);   //index finger
}

void loop() {
EMG_1[0] = (float)analogRead(EMG1PIN);
EMG_2[0] = (float)analogRead(EMG2PIN);
EMG_3[0] = (float)analogRead(EMG3PIN);

filtrePassBas(EMG_1);
filtrePassBas(EMG_2);
filtrePassBas(EMG_3);
//EMG_prime_low = (alpha_low*EMG_1) + ((1-alpha_low)*EMG_prime_low);  //run the EMA
//EMG_prime_high = (alpha_high*EMG_1) + ((1-alpha_high)*EMG_prime_high); 
//highpass = EMG_1 - EMG_prime;
//bandpass = EMG_prime_high - EMG_prime_low;
//bandstop = EMG_1 - bandpass;

Serial.print(EMG_1[1]);
Serial.print(",");
Serial.print (EMG_2[1]);
Serial.print(",");
Serial.println(EMG_3[1]);
delay(1);

bougerMain(EMG_1[1], 5);//, Sbservo, Sb, fSb);
/*bougerDoigt(EMG_1[1], 50, Scservo, Sc, fSc);
bougerDoigt(EMG_1[1], 50, Sdservo, Sd, fSd);
bougerDoigt(EMG_1[1], 50, Seservo, Se, fSe);
bougerDoigt(EMG_1[1], 50, Saservo, Sa, fSa);*/
if (flag == 0 && millis() - previousTime > 500) // middle
{
bougerDoigt(EMG_3[1], 19, 100, Sdservo, Sd, fSd);
if (EMG_3[1] > 19){
timeIndex = millis();}
}

if (flag == 0 && millis() - previousTime > 500 && EMG_3[1] < 19 && millis()-timeIndex > 500) // index
{
bougerDoigt(EMG_2[1], 380, 1000, Seservo, Se, fSe);
}

if (flag == 0 && millis() - previousTime > 500) //little finger (Sb) and ring finger (Sc) "are the same"
{
//bougerDoigt(EMG_1[1], 500, Scservo, Sc, fSc);
//bougerDoigt(EMG_2[1], 60, Seservo, Se, fSe);
bougerDoigt(EMG_3[1], 200,300, Sbservo, Sb, fSb);
bougerDoigt(EMG_3[1], 200,300, Scservo, Sc, fSc);
}

}

void filtrePassBas(float EMG[])
{
  EMG[1] = a_low*EMG[0] + (1 - a_low)*EMG[1];
  
  // EMG[1] = (alpha_low * (EMG[0])) + (EMG[1])*(1-alpha_low);  //run the EMA
  // EMG[2] = (alpha_high*EMG[0]) + ((1-alpha_high) * EMG[2]);
}


/*
This function will control the extension of a given finger depending on the value received by
the EMG. If the value is higher than a certain threshold, the chosen finger will move proprtionally
to the amplitude of the EMG. Maximum EMG will give full extension, while minimum EMG will give
minimal movement.
*/
void bougerDoigt(float valueEMG, int thresholdmin, int thresholdmax, Servo finger, int valMin, int valMax)
{
  if(valueEMG > thresholdmin && valueEMG < thresholdmax)
  {
  
   // if probeme, replace with: finger.write(valMax);
   finger.write(valMin);
   flag = 1;
   // finger.write(map((int)(valueEMG), threshold, 1023, valMin, valMax)); // pt mettre le 1023 plus bas
  }
  else
  {
    finger.write(valMax);
    flag = 0;
  }
}

void bougerMain(float valueEMG, int threshold)
{
  if((valueEMG > threshold))
  {
   // if probeme, replace with: finger.write(valMax);
   previousTime = millis();
   flag = 1;
   Sbservo.write(Sb);
   Saservo.write(Sa);
   Scservo.write(Sc);
   Sdservo.write(Sd);
   Seservo.write(Se);

   // finger.write(map((int)(valueEMG), threshold, 1023, valMin, valMax)); // pt mettre le 1023 plus bas
  }
  else
  {
   flag = 0;
   Sbservo.write(fSb);
   Saservo.write(fSa);
   Scservo.write(fSc);
   Sdservo.write(fSd);
   Seservo.write(fSe);

  }
}
  
/*
This function is similar to the previous one. However, it will need to signals to determine if a
finger needs to be moved. This function helps with fingers with lower EMGs and compensate for
the lack of EMG devices (3 devices for 5 fingers). 
*/

  void bougerDoigt2(float valueEMG, float valueEMG2, int threshold, Servo finger, int valMax, int valMin)
{
  if(valueEMG > threshold && valueEMG2 >threshold)
  {
   finger.write(map((int)(valueEMG), threshold, 1023, valMin, valMax)); // pt mettre le 1023 plus bas
  }
  else
  {
    finger.write(valMin);
  }
}
