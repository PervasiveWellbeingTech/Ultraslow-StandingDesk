 #include <Pushbutton.h>
#include <autotune.h>
#include <microsmooth.h>
#include <NewPing.h>
#include <Chrono.h>
#include <LightChrono.h>
/////////////UPDATE WITH EVERY RUN
int group = 4;
bool resetHeightAtBeginning = false;
////////////

//define pin name

const int numOfSensors = 1;
#define TRIGGER_PIN  11
#define ECHO_PIN     10
#define MAX_DISTANCE 200
#define UP_PIN 7
#define DOWN_PIN 8
#define offset 0 // recorded experimentally. offset is + when measured height is greater than actual height

int tolerance = 2; // cm
int dir_1 = 2;
int pwm_1 = 3;
int delayTime = 60 * 15; // 5 sec
int short_delay_time = 50; // milliseconds
int transition_time = 15;

int short_height = 66; // for michael : 68 - offset
int tall_height = 103; // for michael: 104 - offset


int first_change = 10 * 60; //sec
int second_change = 15 * 60; //sec
bool currentlyRaised = false;
bool first_change_done = false;
bool second_change_done = false;

/*
  1 - standard sit-stand desk, default is raised
  2 - standard sit-stand desk, default is lowered
  3 - online sit-stand desk, default is raised
  4 - online sit-stand desk, default is lowered
  5 - offline sit-stand desk, default is raised
  6 - offline sit-stand desk, default is lowered
*/

Pushbutton up_button(UP_PIN);
Pushbutton down_button(DOWN_PIN);

Chrono myChronoSeconds(Chrono::SECONDS);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
uint16_t *history = ms_init(SGA);

int getDistance(int trigPin, int echoPin) // returns the distance (cm)
{
  long duration, distance;

  digitalWrite(trigPin, HIGH); // We send a 10us pulse
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 20000); // We wait for the echo to come back, with a timeout of 20ms, which corresponds approximately to 3m

  // pulseIn will only return 0 if it timed out. (or if echoPin was already to 1, but it should not happen)
  if (duration == 0) // If we timed out
  {
    pinMode(echoPin, OUTPUT); // Then we set echo pin to output mode
    digitalWrite(echoPin, LOW); // We send a LOW pulse to the echo pin
    delayMicroseconds(200);
    pinMode(echoPin, INPUT); // And finaly we come back to input mode
  }

  distance = (duration / 2) / 29.1; // We calculate the distance (sound speed in air is aprox. 291m/s), /2 because of the pulse going and coming

  return distance; //We return the result. Here you can find a 0 if we timed out
}

int get_height_cm() {
  int current_height_cm = getDistance (TRIGGER_PIN, ECHO_PIN);
  //int current_height_cm = sonar.ping_cm();
  int processed_value = sma_filter(current_height_cm, history);
  return  processed_value;
  //return current_height_cm;
}

void raise_up() {
  digitalWrite(dir_1, LOW); //controls the direction the motor
  analogWrite(pwm_1, 255); //
}

void lower() {
  digitalWrite(dir_1, HIGH); //controls the direction the motor
  analogWrite(pwm_1, 255);
}

void stop_desk() {
  digitalWrite(dir_1, LOW); //controls the direction the motor
  analogWrite(pwm_1, 0);
}

void set_desk_height(int height_in_cm) {
  int current_height_cm = get_height_cm();
  if (current_height_cm < height_in_cm - tolerance) {
    while (current_height_cm < height_in_cm - tolerance) {
      raise_up();
      delay(short_delay_time);
      Serial.println(current_height_cm);
      current_height_cm = get_height_cm();
    }
  }
  else {

    while (current_height_cm > height_in_cm + tolerance) {
      lower();
      delay(short_delay_time);
      Serial.println(current_height_cm);
      current_height_cm = get_height_cm();
    }
  }

  stop_desk();

}
void setup() {
  //declare pins as INPUT/OUTPUT
  if (group % 2 == 1) {
    currentlyRaised = true;
  }
  else
    currentlyRaised = false;

  pinMode(pwm_1, OUTPUT);
  pinMode(dir_1, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, OUTPUT);

  Serial.begin(9600); //I am using Serial Monitor instead of LCD display
  Serial.write("Group: ");
  Serial.println(group);

  // LOW = UP
  // HIGH = DOWN
 

  if (resetHeightAtBeginning){
    if (currentlyRaised) {
    for (int i = 0; i < 5; i++)
      set_desk_height(tall_height);
  }
  else {
    for (int i = 0; i < 5; i++)
      set_desk_height(short_height);
  }
  
    }

  while (true){
    if (up_button.getSingleDebouncedPress())
  {
    Serial.println("STARTING");
    myChronoSeconds.restart(0);
    break;
  }

    }
  
}

void switchPosition() {
  if (currentlyRaised == false) {
    Serial.println("MOVING UP - AUTOMATIC"); 
    set_desk_height(tall_height);
    currentlyRaised = true;
  }
  else {
    Serial.println("MOVING DOWN - AUTOMATIC"); 
    set_desk_height(short_height);
    currentlyRaised = false;
  }

  stop_desk();
}


void loop() {
  delay(short_delay_time);
  Serial.println(get_height_cm());

  if (up_button.getSingleDebouncedPress())
  {
    Serial.println("UP BUTTON PRESSED");
    set_desk_height(tall_height);
  }
  else if (down_button.getSingleDebouncedPress())
  {
    Serial.println("DOWN BUTTON PRESSED");
    set_desk_height(short_height);
  }

  int current_height_cm = get_height_cm();

  if (group >= 3) {
    if (myChronoSeconds.hasPassed(second_change, false) && second_change_done == false) {
      Serial.println(second_change_done);
      switchPosition();
      second_change_done = true;
      Serial.println(second_change_done);
    }
    else if (myChronoSeconds.hasPassed(first_change, false) && first_change_done == false) {
      Serial.println(first_change_done);
      first_change_done = true;
      
      switchPosition();
      first_change_done = true;
      Serial.println(first_change_done);
    }
    stop_desk();
  }



  //pause

}

