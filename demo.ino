const int numOfActuators = 1;
const int relay1Pins[] = {17};
const int relay2Pins[] = {18};

void setup() { 
  //start serial connection
  Serial.begin(9600);

  for (int i = 0; i < numOfActuators; ++i) {
    // initialize each relay pin as an output
    pinMode(relay1Pins[i], OUTPUT);    
    pinMode(relay2Pins[i], OUTPUT);    
    
    // set the relay to low
    digitalWrite(relay1Pins[i], LOW); 
    digitalWrite(relay2Pins[i], LOW); 
  }
}

void extendLinearActuator(int actuatorNum) {
  digitalWrite(relay1Pins[actuatorNum - 1], HIGH);
  digitalWrite(relay2Pins[actuatorNum - 1], LOW);
  Serial.println("Extending");
}

void retractLinearActuator(int actuatorNum) {
  digitalWrite(relay1Pins[actuatorNum - 1], LOW);
  digitalWrite(relay2Pins[actuatorNum - 1], HIGH);
  Serial.println("Retracting");
}

void stopLinearActuator(int actuatorNum) {
  digitalWrite(relay1Pins[actuatorNum - 1], LOW);
  digitalWrite(relay2Pins[actuatorNum - 1], LOW);
  Serial.println("Stopped");
}

void loop() {
  while (Serial.available() == 0) {
    // Wait for user input  
  }
  String mode = Serial.readString();
  if (mode == String("u\n") || mode == String("d\n") || mode == String("s\n")) {
    int actuatorNum = 1;
    if (mode == String("u\n")) {
      stopLinearActuator(actuatorNum);
      extendLinearActuator(actuatorNum);
    } else if (mode == String("d\n")) {
      stopLinearActuator(actuatorNum);
      retractLinearActuator(actuatorNum);
    } else if (mode == String("s\n")) {
      Serial.println("Stopping");
      stopLinearActuator(actuatorNum);
    }
  }
}

