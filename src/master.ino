#include <Wire.h>
#include <TimerOne.h>
int state_pid=0;
int state_send=0;
int state_receive=0;
int device=9;
int master=1;
int counter=0;
int nyala=0;
float Kp=50;
float Ki=20,Kd=0;
float measured_value=0;
float setpoint = 1;
float integral;
float proportional;
float derivative;
float T_PID=0.001; //periode kalkulasi PID adalah 1ms
float yn=0; double yn1=0; 
float en=0; double en1=0; double en2=0;
char temp_x[10];

//Struktur data
typedef union{
  float value;
  uint8_t bytes[4];
} floated;

floated values;

//setup timer dan protokol I2C
void setup()
{
  Serial.begin(115200);
  //STOPWATCH 2 (15,625 kHz dan prescaler 1024
  Serial.println("SETUP BEGIN...");
  Timer1.initialize(T_PID*1000000);
  Timer1.attachInterrupt(ISR_timer);
  pinMode(led7,OUTPUT);
  Wire.begin(master);
}

//Fungsi ISR
void ISR_timer(void){
  //kalkulasi PID, dengan periode 0,001s
  state_pid=1;
  
  //ngirim data dari PID ke plant dengan periode 0,003s
  if (counter%3==0){
    state_send=1;
  }
  
  //nerima data dari slave/plant dengan periode 0,002s
  if (counter%2==0){
    state_receive=1;
  }
  
  //reset counter
  if (counter==6){
    counter=0;
  }
  
  //increment counter
  counter++;
}

//Fungsi untuk menerima data dari Slave
double getDouble(){
  int count=0;
  floated f;
  Wire.requestFrom(device,4);
  while(count<4 && Wire.available()>0){
    f.bytes[count] = Wire.read();
    count = count+1;
  }
  return f.value;
}

//Fungsi perhitungan PID
void PID(float error){
  proportional=Kp*(en-en1);
  integral=Ki*(T_PID/2)*(en+en1);
  derivative=Kd*(1/T_PID)*(en-2*en1+en2);
  yn=yn1+proportional+integral+derivative;
  yn1=yn;
  en2=en1;
  en1=en;
}

//Looping
void loop()
{
  if(state_pid==1){ 
    //Perhitungan PID
    en=setpoint - measured_value;
    PID(en);
    values.value = yn;
    state_pid=0;
    Serial.print(yn); Serial.print(",");
    Serial.print(en); Serial.print(",");
    Serial.println(measured_value);
  }
  if (state_send==1){
    //send data ke slave
    Wire.beginTransmission(device);
    int count=0;
    
    while(count<4){
      Wire.write(values.bytes[count]);
      count++;
    }
    Wire.endTransmission();
    state_send=0; 
  }
  if (state_receive==1){
    //receive data dari slave
    measured_value = getDouble();
    state_receive=0;
  }
}
