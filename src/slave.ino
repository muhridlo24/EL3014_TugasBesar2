// Include the required Wire library for I2C
#include <Wire.h>
float xn = 0;
float xn1= 0;
float yn=0;
float yn1=0;
float T_plant=0.003;
int device=9;
int count;
int state_feedback;
int master=1;
int recv,reqst;

//Struktur data
typedef union{
  float value;
  uint8_t bytes[4];
} floated;

floated values;

//Fungsi penerimaan data
float getDouble(){
  int count=0;
  floated f;
  while(count<4){
    f.bytes[count] = Wire.read();
    count = count+1;
  }
  return f.value;
}

//Fungsi untuk menerima dan memproses data dari Master
void receiveEvent(int x){
  xn=getDouble();
  yn=((940-T_plant)/(940+T_plant))*yn1+(10*T_plant/(940+T_plant))*xn+(10*T_plant/(940+T_plant))*xn1;
  yn1=yn;
  xn1=xn;
  values.value=yn;
  
  Serial.print(yn); Serial.print(" ");
  Serial.println(xn);
  int count=0;
}

//Fungsi untuk mengirim data dari Slave ke Master
void requestEvent(){
  int count=0;
  while(count<4){
    Wire.write(values.bytes[count]);
    count++;
  }
}

//setup
void setup()
{
  Serial.begin(115200);
  Serial.println("SETUP....");
  // Start the I2C Bus as Slave on address 9
  Wire.begin(device);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.println("SETUP COMPLETE");
}

void loop(){
}
