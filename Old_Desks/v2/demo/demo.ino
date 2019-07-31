const int numOfActuators = 1;
const int relay1Pins[] = {5};
const int relay2Pins[] = {6};

const int numOfSensors = 1;
const int ultrasoundTriggerPins[] = {9};
const int ultrasoundEchoPins[] = {10};

const double SPEED_OF_SOUND = 0.034;

const double MIN_DISTANCE_TO_FLOOR_RANGE[] = {48.00, 55.00};
const double AVG_DISTANCE_TO_FLOOR_RANGE[] = {68.00, 75.00};
const double MAX_DISTANCE_TO_FLOOR_RANGE[] = {93.00, 120.00};

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

  for (int i = 0; i < numOfSensors; ++i) {
    // initialize ultrasound pins
    pinMode(ultrasoundTriggerPins[i], OUTPUT);
    pinMode(ultrasoundEchoPins[i], INPUT);
    
    // set the trigger pins to low
    digitalWrite(ultrasoundTriggerPins[i], LOW); 
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

int readUltrasound(int sensorNum) {
  digitalWrite(ultrasoundTriggerPins[sensorNum - 1], LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(ultrasoundTriggerPins[sensorNum - 1], HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasoundTriggerPins[sensorNum - 1], LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration = pulseIn(ultrasoundEchoPins[sensorNum - 1], HIGH);

  return duration;
}

double findDistanceFromUltrasound(int sensorNum) {
  double totalDistance = 0;
  for (int i = 0; i < 5; ++i) {
    int duration = readUltrasound(sensorNum);
    
    // Calculating the distance: (1) Factor in speed of sound (2) Distance is travelled twice
    totalDistance += double(duration) * SPEED_OF_SOUND / 2.0;
  }

  double distance = totalDistance / 5.0;

  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  return distance;
}

bool deskAtMinHeight() {
  int distance = findDistanceFromUltrasound(1);
  return (distance >= (MIN_DISTANCE_TO_FLOOR_RANGE[0])) && (distance <= (MIN_DISTANCE_TO_FLOOR_RANGE[1]))
}

bool deskAtAvgHeight() {
  int distance = findDistanceFromUltrasound(1);
  return (distance >= (AVG_DISTANCE_TO_FLOOR_RANGE[0])) && (distance <= (AVG_DISTANCE_TO_FLOOR_RANGE[1]))
}

bool deskAtMaxHeight() {
  int distance = findDistanceFromUltrasound(1);
  return (distance >= (MAX_DISTANCE_TO_FLOOR_RANGE[0])) && (distance <= (MAX_DISTANCE_TO_FLOOR_RANGE[1]))
}

void moveDeskToMin() {
  while (!deskAtMinHeight()) {
    if (findDistanceFromUltrasound(1) > MIN_DISTANCE_TO_FLOOR_RANGE[1]) {
      retractLinearActuator(1);
    } else if (findDistanceFromUltrasound(1) < MIN_DISTANCE_TO_FLOOR_RANGE[0]) {
      stopLinearActuator(1);
    }
  }
  stopLinearActuator(1);
}

void moveDeskToAvg() {
  while (!deskAtAvgHeight()) {
    if (findDistanceFromUltrasound(1) > AVG_DISTANCE_TO_FLOOR_RANGE[1]) {
      retractLinearActuator(1);
    } else if (findDistanceFromUltrasound(1) < AVG_DISTANCE_TO_FLOOR_RANGE[0]) {
      extendLinearActuator(1);
    }
  }
  stopLinearActuator(1);
}

void moveDeskToMax() {
  while (!deskAtMaxHeight()) {
    if (findDistanceFromUltrasound(1) > MAX_DISTANCE_TO_FLOOR_RANGE[1]) {
      stopLinearActuator(1);
    } else if (findDistanceFromUltrasound(1) < MAX_DISTANCE_TO_FLOOR_RANGE[0]) {
      extendLinearActuator(1);
    }
  }
  stopLinearActuator(1);
}

void moveDesk(int goalHeight) {
  if (goalHeight == 0) {
    moveDeskToMin();
  } else if (goalHeight == 1) {
    moveDeskToAvg();
  } else if (goalHeight == 2) {
    moveDeskToMax();
  }
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
  
  if (mode == String("r\n")) {
    findDistanceFromUltrasound(1);
  }
  
  if (mode == String("min\n") || mode == String("avg\n") || mode == String("max\n")) {
    if (mode == String("min\n")) {
      moveDeskToMin();
    } else if (mode == String("avg\n")) {
      moveDeskToAvg();
    } else if (mode == String("max\n")) {
      moveDeskToMax();
    }
  }
}

