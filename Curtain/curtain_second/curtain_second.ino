#include <Stepper.h>
const int stepsPerRev = 2048; //회전각도 설정 = 2048:한바퀴(360도)
Stepper myStepper(stepsPerRev,11,9,10,8); //모터설정

void setup() {
  myStepper.setSpeed(14);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  Serial.begin(9600);
}

void loop() {
  if(digitalRead(2) == HIGH)
    opencurtain();
  else if(digitalRead(3) == HIGH)
    closecurtain();
}

void opencurtain()
{
    Serial.println("커튼 열림");
    myStepper.step(-stepsPerRev*4.2);
}

void closecurtain()
{
    Serial.println("커튼 닫힘");
    myStepper.step(stepsPerRev*4.2);
}
