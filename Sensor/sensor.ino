#include <DHT11.h>
#include <Ethernet.h>
#define BUFSIZE 10000
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client;
char server[] = "simddong.ga";

int pin=2;            //온습도
DHT11 dht11(pin);     
float temp, humi;
int sensorPin = A0;   //가스
void setup() {
  //랜선
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
  delay(1000);

  client.connect(server, 9670);
  if(client.connected()){
   Serial.println();
   Serial.print("연결 성공");
   client.print("9");
  }
  else{
   Serial.print("연결 실패");
  }
  //랜선
  Serial.begin(9600);

}


void loop() { 
  String reciveMsg = client.readString();
  if(reciveMsg.length())
  {
    Serial.println("받은 메시지: " + reciveMsg);
    if(reciveMsg.compareTo("server\trequestinfo") == 0){
      Serial.print("서버 성공");
      dhtt();
      String sendgas = String(temp) +"\t"+ String(humi) +"\t"+ gas();
      char ch[256] = {0};
      sendgas.toCharArray(ch, sendgas.length());
      client.write(ch);
    }
  }
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

float gas()
{
  
  return analogRead(sensorPin);
}
