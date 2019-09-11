#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Stepper.h>

#define BUFSIZE 10000
#define NUMREADINGS 10 // 평균내는 개수 수정

int readings[NUMREADINGS] = {0,};
int index = 0;
float total = 0;
float val = 10;

int measurePin = A2; //Connect dust sensor to Arduino A0 pin
int ledPower = 3;   //Connect 3 led driver pins of dust sensor to Arduino D2

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

const int stepsPerRev = 2048; //회전각도 설정 = 2048:한바퀴(360도)
Stepper myStepper(stepsPerRev,7,5,6,4); //모터설정

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA };
EthernetClient client;
char server[] = "simddong.ga";

unsigned long time;

#define DHTTYPE DHT11

DHT dht = DHT(2, DHTTYPE);

float temp, humi;

int sensorPin = A1;   //강우

int Vo = A2; //미세먼지
int V_LED = 3;  //미세먼지

bool statue = false;

int dhtt_time = 0;

void setup() {
  //랜선
  Serial.begin(9600);
  Ethernet.begin(mac);
  dht.begin();
  Serial.println(Ethernet.localIP());
  delay(1000);
  myStepper.setSpeed(12);
  while(true)
  {
    client.connect(server, 9670);
    if(client.connected()){
      Serial.println();
      Serial.println("연결 성공");
      client.print("13");
      break;
    }
    else
    {
      Serial.println("연결 실패 재 접속 시도");
      delay(1000);
    }
  }
  
  pinMode(ledPower, OUTPUT);
  pinMode(measurePin, INPUT);
  
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  delay(5000);
  for(int i = 0; i < 10; i++)
  {
    dust();
    delay(1000);
  }
}

void loop() {
  String reciveMsg = client.readString();
  if(reciveMsg.length())
  {
    Serial.println("받은 메시지: " + reciveMsg);
    if(reciveMsg.indexOf("server\trequestinfo") >= 0){
      Serial.println("요청 받음");
      dust();
      dhtt();
      String sendWindow = "window\tinputsql\t" + String(statue ? "true" : "false") + "\t" + temp + "\t" + humi + "\t" + rain() + "\t" + val;
      Serial.println(sendWindow);
      char ch[256] = {0};
      sendWindow.toCharArray(ch, sendWindow.length());
      client.write(ch);
    }
    if(reciveMsg.indexOf("server\topenwindow") >= 0){
      //창문
      dust();
      dhtt();
      String sendWindow = "window\tinputsql\t" + String(statue ? "true" : "false") + "\t" + temp + "\t" + humi + "\t" + rain() + "\t" + val;
      char ch[256] = {0};
      sendWindow.toCharArray(ch, sendWindow.length());
      client.write(ch);
      openWindow();
    }
    if(reciveMsg.indexOf("server\tclosewindow") >= 0){
      //창문
      dust();
      dhtt();
      String sendWindow = "window\tinputsql\t" + String(statue ? "true" : "false") + "\t" + temp + "\t" + humi + "\t" + rain() + "\t" + val;
      char ch[256] = {0};
      sendWindow.toCharArray(ch, sendWindow.length());
      client.write(ch);
      closeWindow();
    }
  }
}

void openWindow()
{
  if(statue)
    return ;
  Serial.println("창문 열림");
  myStepper.step(stepsPerRev*1.3);
  statue = true;
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
}

void closeWindow()
{
  if(!statue)
    return ;
  Serial.println("창문 닫힘");
  myStepper.step(-stepsPerRev*1.3);
  statue = false;
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
}
void dhtt()
{
  temp = dht.readTemperature();
  humi = dht.readHumidity();
  Serial.print("온도:");
  Serial.println(temp);
  Serial.print("습도:");
  Serial.println(humi);
}

float rain()
{
  int rains = analogRead(sensorPin);
  Serial.print("온도:");
  Serial.println(rains);
  return rains;
}

void dust()
{
  digitalWrite(ledPower, LOW); // power on the LED
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin); // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);
  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 0.17 * calcVoltage - 0.1;
  Serial.print(" - Dust Density: ");
  Serial.println(dustDensity * 1000); // unit: mg/m3
  Serial.print(" - index: ");
  Serial.println(index); // unit: mg/m3
  Serial.print(" - val: ");
  Serial.println(val); // unit: mg/m3
  if(dustDensity*1000 <= 5)
    return ;

  total -= readings[index];
  readings[index] = dustDensity * 1000;
  total += readings[index];
  index++;
  if (index >= NUMREADINGS) {
    index = 0;
  }
  if(readings[NUMREADINGS-1] == 0)
    val = total / index;
  else
    val = total / NUMREADINGS;
}
