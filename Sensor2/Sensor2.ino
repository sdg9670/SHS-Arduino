#include <DHT.h>
#include <Ethernet.h>
#define BUFSIZE 10000
#define DHTTYPE DHT11

DHT dht = DHT(2, DHTTYPE);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xBC };
EthernetClient client;
char server[] = "simddong.ga";

float temp, humi;
int sensorPin = A0;   //가스
void setup() {
  //랜선
  Ethernet.begin(mac);
  Serial.begin(9600);
  dht.begin();
  Serial.println(Ethernet.localIP());
  delay(1000);
  while(true)
  {
    client.connect(server, 9670);
    if(client.connected()){
      Serial.println();
      Serial.println("연결 성공");
      client.print("9");
      break;
    }
    else
    {
      Serial.println("연결 실패 재 접속 시도");
      delay(1000);
    }
  }
  //랜선

}


void loop() { 
  String reciveMsg = client.readString();
  if(reciveMsg.length())
  {
    Serial.println("받은 메시지: " + reciveMsg);
    if(reciveMsg.compareTo("server\trequestinfo") == 0){
      Serial.print("서버 성공");
      dhtt();
      String sendgas = "sensor\tinputsql\t" + String(temp) +"\t"+ String(humi) +"\t"+ gas();
      char ch[256] = {0};
      sendgas.toCharArray(ch, sendgas.length());
      client.write(ch);
    }
  }
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

float gas()
{
  
  return analogRead(sensorPin);
}
