char incomingByte;  // входящие данные
int  led = 13;      // LED подключен к 13 пину
 
void setup() {
  Serial.begin(9600); // инициализация порта
  pinMode(led, OUTPUT);
  Serial.println("Press 1 to LED ON or 0 to LED OFF...");
}
 
void loop() {
  if (Serial.available() > 0) {  //если пришли данные
    incomingByte = Serial.read(); // считываем байт
    Serial.print("You press: "); Serial.println(incomingByte);
    if(incomingByte=='0') digitalWrite(led, LOW);
    if(incomingByte=='1') digitalWrite(led, HIGH);
    delay(500);
  }
}
