#include <SPI.h>
#include <Ethernet.h>

#define BUFSIZE 10000
byte mac[] = { 0xAE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAD };
EthernetClient client;
char server[] = "simddong.ga";

unsigned long time;
bool statue = false;
//int btn=7;            //스텝
int sunlight = A2;       // 변수 sun를 선언하고, A2을 대입
void setup() {
  //랜선
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
  delay(1000);

  client.connect(server, 9670);
  if(client.connected()){
   Serial.println();
   Serial.print("연결 성공");
   client.print("4");
  }
  else{
   Serial.print("연결 살패");
  }
  //랜선
  
  
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
    String reciveMsg = client.readString();
    Serial.println("받은 메시지: " + reciveMsg);
    if(reciveMsg.length())
    {
      if(reciveMsg.compareTo("server\trequestinfo") == 0){
        //커튼
        String sendcurtain = "curtain\tinputsql\t" + String(statue ? "true" : "false") + "\t" + right();
        char ch[256] = {0};
        sendcurtain.toCharArray(ch, sendcurtain.length());
        client.write(ch);
      } else if(reciveMsg.compareTo("server\topencurtain") == 0){
        //커튼
        opencurtain();
        String sendcurtain = "curtain\tinputsql\t" + String(statue ? "true" : "false") + "\t" + right();
        char ch[256] = {0};
        sendcurtain.toCharArray(ch, sendcurtain.length());
        client.write(ch);
      }else if(reciveMsg.compareTo("server\tclosecurtain") == 0){
        //커튼
        closecurtain();
        String sendcurtain = "curtain\tinputsql\t" + String(statue ? "true" : "false") + "\t" + right();
        char ch[256] = {0};
        sendcurtain.toCharArray(ch, sendcurtain.length());
        client.write(ch);
      }
    }
}

void opencurtain()
{
  digitalWrite(8, HIGH);
  delay(10);
  digitalWrite(8, LOW);
  statue = true;
  Serial.println("커튼열림");
}

void closecurtain()
{
  digitalWrite(9, HIGH);
  delay(10);
  digitalWrite(9, LOW);
  statue = false;
  Serial.println("커튼닫힘");
}

int right(){
  int val= analogRead(sunlight);// 변수 val을 선언, cds(A0) 아날로그 값을 저장
  Serial.print("조도량: ");
  Serial.println(val);   // 변수 val에 저장된 값을 시리얼 통신을 통해 출력.
  return val;
}
