#include <MQ135.h>

#include <TM74HC595Display.h>
#include <TimerOne.h>


float varVolt = 85.18;  
float varProcess = 0.008; // скорость реакции на изменение (подбирается вручную)
float Pc = 0.0;
float G = 0.0;
float P = 1.0;
float Xp = 0.0;
float Zp = 0.0;
float Xe = 0.0;

unsigned long lastMillis;

bool sensorPreHeated = false;
#define sensorPin A5
int SCLK = 7;
int RCLK = 6;
int DIO = 5;
TM74HC595Display disp(SCLK, RCLK, DIO);
MQ135 gasSensor(sensorPin);
unsigned char LED_0F[31];

void setup() {
  lastMillis = millis();
  LED_0F[0] = 0xC0; //0
  LED_0F[1] = 0xF9; //1
  LED_0F[2] = 0xA4; //2
  LED_0F[3] = 0xB0; //3
  LED_0F[4] = 0x99; //4
  LED_0F[5] = 0x92; //5
  LED_0F[6] = 0x82; //6
  LED_0F[7] = 0xF8; //7
  LED_0F[8] = 0x80; //8
  LED_0F[9] = 0x90; //9
  LED_0F[10] = 0x88; //A
  LED_0F[11] = 0x83; //b
  LED_0F[12] = 0xC6; //C
  LED_0F[13] = 0xA1; //d
  LED_0F[14] = 0x86; //E
  LED_0F[15] = 0x8E; //F
  LED_0F[16] = 0xC2; //G
  LED_0F[17] = 0x89; //H
  LED_0F[18] = 0xF9; //I
  LED_0F[19] = 0xF1; //J
  LED_0F[20] = 0xC3; //L
  LED_0F[21] = 0xAB; //n
  LED_0F[22] = 0xC0; //O
  LED_0F[23] = 0x8C; //P
  LED_0F[24] = 0x98; //q
  LED_0F[25] = 0x92; //S
  LED_0F[26] = 0xC1; //U
  LED_0F[27] = 0x91; //Y
  LED_0F[28] = 0xFE; //hight -
  LED_0F[29] = 0x8B; //h
  LED_0F[30] = 0x7F; //.
  
  Timer1.initialize(1000); // set a timer of length 1000
  Timer1.attachInterrupt(timerIsr); // attach the service routine here
  Serial.begin(9600);
  delay(1000);
  sayLoading();
}

void loop() { 
  if((gasSensor.getRZero() >= gasSensor.getNominalRZero()*0.6) && !sensorPreHeated)
  {
    sensorPreHeated = true;
  }
  if(sensorPreHeated)
  {
     float ppm = filter(gasSensor.getPPM());
     if(millis()-lastMillis > 1500)
     {
        if(ppm<3000.0)
        {
          lastMillis = millis();
          disp.clear();
          disp.digit4(round(ppm));
        }
        else
        {
          sensorPreHeated = false;
        }
     }
  }
  else
  {
    disp.clear();
    int perc = (gasSensor.getRZero()/(gasSensor.getNominalRZero()*0.6))*100;
    disp.set(LED_0F[23], 3);//P
    disp.digit4(perc);
    delay(5000);
  }
}


void timerIsr()
{
  disp.timerIsr();
}
void sayPPm()
{
  disp.set(LED_0F[23], 3);//P
  disp.set(LED_0F[23], 2);//P
  disp.set(LED_0F[29], 1);//h
  disp.set(LED_0F[21], 0);//n
}

float filter(float val) {  //функция фильтрации
  Pc = P + varProcess;
  G = Pc/(Pc + varVolt);
  P = (1-G)*Pc;
  Xp = Xe;
  Zp = Xp;
  Xe = G*(val-Zp)+Xp; // "фильтрованное" значение
  return(Xe);
}
void sayLoading()
{
  disp.set(LED_0F[30], 2);//.
  disp.set(LED_0F[30], 1);//.
  disp.set(LED_0F[30], 0);//.
}
