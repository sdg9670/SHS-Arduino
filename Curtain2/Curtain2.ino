#include <SPI.h>
#include <Ethernet.h>
#include <Stepper.h>

#define BUFSIZE 10000

byte mac[] = { 0xAE, 0xAD, 0xBE, 0xEF, 0xFE, 0xBB };
EthernetClient client;
char server[] = "simddong.ga";
const int stepsPerRev = 2048; //회전각도 설정 = 2048:한바퀴(360도)
Stepper myStepper(stepsPerRev,7,5,6,4); //모터설정

unsigned long time;
bool statue = false;
int sunlight = A2;       // 변수 sun를 선언하고, A2을 대입

void setup() {
  //랜선
  Serial.begin(9600);
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
  delay(1000);

  myStepper.setSpeed(13);
  while(true)
  {
    client.connect(server, 9670);
    if(client.connected()){
      Serial.println();
      Serial.println("연결 성공");
      client.print("14");
      break;
    }
    else
    {
      Serial.println("연결 실패 재 접속 시도");
      delay(1000);
    }
  }

}

void loop() {
    String reciveMsg = client.readString();
    if(reciveMsg.length())
    {
      Serial.println("받은 메시지: " + reciveMsg);
      if(reciveMsg.indexOf("server\trequestinfo") >= 0){
        //커튼
        String sendcurtain = "curtain\tinputsql\t" + String(statue ? "true" : "false") + "\t" + right();
        char ch[256] = {0};
        sendcurtain.toCharArray(ch, sendcurtain.length());
        client.write(ch);
      }
      if(reciveMsg.indexOf("server\topencurtain") >= 0){
        //커튼
        opencurtain();
        String sendcurtain = "curtain\tinputsql\t" + String(statue ? "true" : "false") + "\t" + right();
        char ch[256] = {0};
        sendcurtain.toCharArray(ch, sendcurtain.length());
        client.write(ch);
      }
      if(reciveMsg.indexOf("server\tclosecurtain") >= 0){
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
  if(statue)
    return ;
  Serial.println("커튼 열림");
  statue = true;
  myStepper.step(-stepsPerRev*4.2);
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
}

void closecurtain()
{
  if(!statue)
    return ;
  Serial.println("커튼 닫힘");
  statue = false;
  myStepper.step(stepsPerRev*4.2);
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
}

int right(){
  int val= analogRead(sunlight);// 변수 val을 선언, cds(A0) 아날로그 값을 저장
  Serial.print("조도량: ");
  Serial.println(val);   // 변수 val에 저장된 값을 시리얼 통신을 통해 출력.
  return val;
}
