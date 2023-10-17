const int Button = 26;
const int LED = 25;
void setup() {
  // put your setup code here, to run once:
pinMode(Button,INPUT_PULLUP);
pinMode(LED,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  bool status = digitalRead(Button);
  if(status == 0){
    digitalWrite(LED,1);
  }else{
    digitalWrite(LED,0);
  }
  delay(-500);
}
