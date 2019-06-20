#include <DHT11.h>
#include <SPI.h>
#include <Ethernet.h>
#define BUFSIZE 10000
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client;
char server[] = "simddong.ga";

unsigned long time;

int pin=2;            //온습도
DHT11 dht11(pin);     
float temp, humi;

int sensorPin = A1;   //강우

int Vo = A0; //미세먼지
int V_LED = 12;  //미세먼지

bool statue = false;

void setup() {
  //랜선
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
  delay(1000);

  client.connect(server, 9670);
  if(client.connected()){
   Serial.println();
   Serial.print("연결 성공");
   client.print("3");
  }
  else{
   Serial.print("연결 살패");
  }
  //랜선
  
  pinMode(V_LED, OUTPUT);
  pinMode(Vo, INPUT);
  
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  String reciveMsg = client.readString();
  if(reciveMsg.length())
  {
    Serial.println("받은 메시지: " + reciveMsg);
    if(reciveMsg.compareTo("server\trequestinfo") == 0){
      //창문
      dhtt();
      String sendWindow = "window\tinputsql\t" + String(statue ? "true" : "false") + "\t" + temp + "\t" + humi + "\t" + rain() + "\t" + dust();
      char ch[256] = {0};
      sendWindow.toCharArray(ch, sendWindow.length());
      client.write(ch);
    } else if(reciveMsg.compareTo("server\topenwindow") == 0){
      //창문
      openWindow();
      String sendWindow = "window\tinputsql\t" + String(statue ? "true" : "false") + "\t" + temp + "\t" + humi + "\t" + rain() + "\t" + dust();
      char ch[256] = {0};
      sendWindow.toCharArray(ch, sendWindow.length());
      client.write(ch);
    }else if(reciveMsg.compareTo("server\tclosewindow") == 0){
      //창문
      closeWindow();
      String sendWindow = "window\tinputsql\t" + String(statue ? "true" : "false") + "\t" + temp + "\t" + humi + "\t" + rain() + "\t" + dust();
      char ch[256] = {0};
      sendWindow.toCharArray(ch, sendWindow.length());
      client.write(ch);
    }
  }
}

void openWindow()
{
  digitalWrite(8, HIGH);
  delay(10);
  digitalWrite(8, LOW);
  statue = true;
  Serial.println("창문열림");
}

void closeWindow()
{
  digitalWrite(9, HIGH);
  delay(10);
  digitalWrite(9, LOW);
  statue = false;
  Serial.println("창문닫힘");
}
void dhtt()
{
  int err;
  if((err=dht11.read(humi, temp))==0) //에러인지 확인
  {
    Serial.print("온도:");
    Serial.println(temp);
    Serial.print(" 습도:");
    Serial.println(humi);
  }
  else                                //에러일 경우 처리
  {
    Serial.println();
    Serial.print("Error No :");
    Serial.println(err); 
  }
}

float rain()
{
  return analogRead(sensorPin);
}

float dust()
{
  float Vo_value = 0;
  float Voltage = 0;
  float dustCleanVoltage = 0.44;
  int samplingTime = 280;
  int deltaTime = 40;
  int sleepTime = 9680;
   
  digitalWrite(V_LED,LOW); 
  delayMicroseconds(samplingTime);
  Vo_value = analogRead(Vo);
  delayMicroseconds(deltaTime);// pulse width 0.32 - 0.28 = 0.04 msec
  //0.32 msec의 펄스폭을 유지하기 위해 필요한 코드입니다

  digitalWrite(V_LED,HIGH); //ired 'off'
  delayMicroseconds(sleepTime);

  Voltage = Vo_value * (5.0 / 1024.0);
  int dustDensity = (Voltage - dustCleanVoltage)/0.005;

  Serial.print(" 미세먼지:");
  Serial.print(dustDensity);
  return dustDensity;
}
