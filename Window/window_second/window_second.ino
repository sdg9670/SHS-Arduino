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
    openWindow();
  else if(digitalRead(3) == HIGH)
    closeWindow();
}

void openWindow()
{
    Serial.println("창문 열림");
    myStepper.step(-stepsPerRev*2.4);
}

void closeWindow()
{
    Serial.println("창문 닫힘");
    myStepper.step(stepsPerRev*2.4);
}
