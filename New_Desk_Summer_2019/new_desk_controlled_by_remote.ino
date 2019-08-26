#include "OneButton.h"
#include <Pushbutton.h>
#include <NewPing.h>
#include <microsmooth.h>
#include <autotune.h>
#include <Chrono.h>
#include <LightChrono.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>

//CHANGE THESE VALUES
int short_height = 67;
int tall_height = 106;
#define CHANGE_TIME 30*60 //__ times 60 seconds
//CHANGE THESE VALUES

//Pins for Ultrasonic Sensor
#define TRIGGER_PIN  7
#define ECHO_PIN     6
#define MAX_DISTANCE 200

//Pins for buttons/desk movement
#define UP_BUTTON_PIN 5
#define DOWN_BUTTON_PIN 4
#define MOTOR_UP_PIN 3
#define MOTOR_DOWN_PIN 2


//Height and time tolerances/presettings
#define tolerance 1
#define short_delay_time 50
int prev_height = 0;
int set_new_height_time = 10;
int timeOutTime = 15;
bool snooze = false;

RF24 radio(10, 9); // CE, CSN         
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 00;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t node01 = 01;    // Address of the other node in Octal format

// Setup a new OneButton for user up.
OneButton up_button(UP_BUTTON_PIN, true);
// Setup a new OneButton for user down.
OneButton down_button(DOWN_BUTTON_PIN, true);

//Initialize timer for automatic height changes
Chrono changeTimer(Chrono::SECONDS);
Chrono timeOut(Chrono::SECONDS);
Chrono heightChangeTimer(Chrono::SECONDS);
//Initialize the ultrasonic sensor
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
uint16_t *history = ms_init(SGA);

// Return the distance in cm from the ultrasonic sensor
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

//Processes the height from the ultrasonic sensor
int get_height_cm() {
  int current_height_cm = getDistance (TRIGGER_PIN, ECHO_PIN);
  //int current_height_cm = sonar.ping_cm();
  int processed_value = sma_filter(current_height_cm, history);
  return  processed_value;
  //return current_height_cm;
}

void checkRadio(bool moving) {
  network.update();
  int remote = 0;
  while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    network.read(header, &remote, sizeof(remote)); // Read the incoming data
  }
  if (remote == 1 && !moving) {
    Serial.println("Remote Doublepress Up");
    up_doubleclick();
  } else if (remote == 2 && !moving) {
    Serial.println("Remote Up Longpress Start");
    up_longPressStart();
  } else if (remote == 3 && !moving) {
    Serial.println("Remote Up Longpress Stop");
    up_longPressStop();
  } else if (remote == 4 && !moving) {
    Serial.println("Remote Doublepress Up");
    down_doubleclick();
  } else if (remote == 5 && !moving) {
    Serial.println("Remote Up Longpress Start");
    down_longPressStart();
  } else if (remote == 6 && !moving) {
    Serial.println("Remote Up Longpress Stop");
    down_longPressStop();
  } else if (remote == 7) {
    snooze = true;
    Serial.println("Remote Stop");
    stop_desk();
  }
}

//Sets desk motor to move up
void raise_up() {
  digitalWrite(MOTOR_UP_PIN, HIGH);
}

//Sets desk motor to move down
void lower() {
  digitalWrite(MOTOR_DOWN_PIN, HIGH);
}

//Stops the desk motor
void stop_desk() {
  digitalWrite(MOTOR_UP_PIN, LOW);
  digitalWrite(MOTOR_DOWN_PIN, LOW);
}

//Sets the height of the desk to the given height
void set_desk_height(int height_in_cm) {
  int current_height_cm = get_height_cm();
  timeOut.restart(0);
  if (current_height_cm < height_in_cm - tolerance) {
    raise_up();
    while (current_height_cm < height_in_cm - tolerance && !timeOut.hasPassed(timeOutTime, false) && !snooze) {
      checkRadio(true);
      Serial.println(current_height_cm);
      current_height_cm = get_height_cm();
    }
  }
  else {
    lower();
    while (current_height_cm > height_in_cm + tolerance && !timeOut.hasPassed(timeOutTime, false) && !snooze) {
      checkRadio(true);
      Serial.println(current_height_cm);
      current_height_cm = get_height_cm();
    }
  }
  snooze = false;
  stop_desk();
}

void setup() {
  SPI.begin();
  radio.begin();                  //Starting the Wireless communication
  network.begin(90, this_node); //Setting the address where we will send the data
  radio.setPALevel(RF24_PA_MAX);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.setDataRate(RF24_2MBPS);          //This sets the module as transmitter
  Serial.begin(9600);
  pinMode(MOTOR_DOWN_PIN, OUTPUT);
  pinMode(MOTOR_UP_PIN, OUTPUT);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, OUTPUT);
  
  // link the up button functions.
  up_button.attachClick(up_click);
  up_button.attachDoubleClick(up_doubleclick);
  up_button.attachLongPressStart(up_longPressStart);
  up_button.attachLongPressStop(up_longPressStop);
  up_button.attachDuringLongPress(up_longPress);

  // link the down button functions.
  down_button.attachClick(down_click);
  down_button.attachDoubleClick(down_doubleclick);
  down_button.attachLongPressStart(down_longPressStart);
  down_button.attachLongPressStop(down_longPressStop);
  down_button.attachDuringLongPress(down_longPress);
}

//Called to automatically set the desk height to change from down to up or vice versa
//Depends on where the desk is when called (aka, responds to user changes)
void switchPosition() {
  //warning_cue();
  if (get_height_cm() <= (tall_height + short_height) / 2) {
    Serial.println("MOVING UP - AUTOMATIC"); 
    set_desk_height(tall_height);
  }
  else {
    Serial.println("MOVING DOWN - AUTOMATIC");
    set_desk_height(short_height);
  }
  stop_desk();
}

void changePresets() {
  int current_height_cm = get_height_cm();
  if (prev_height == current_height_cm || prev_height == current_height_cm + 1 || prev_height == current_height_cm - 1) {
    if (heightChangeTimer.hasPassed(set_new_height_time, false)) {
      heightChangeTimer.restart(0);
      if (current_height_cm > (tall_height + short_height) / 2) {
        Serial.println("Tall Height Change");
        tall_height = current_height_cm;
      } else {
        Serial.println("Short Height Change");
        short_height = current_height_cm;
      }
    }
  } else {
    heightChangeTimer.restart(0);
  }
  prev_height = current_height_cm;
}

void loop() {
  snooze = false;
  Serial.println(get_height_cm());
  checkRadio(false);
  up_button.tick();
  down_button.tick();
  if (changeTimer.hasPassed(CHANGE_TIME, false)) {
    changeTimer.restart(0);
    switchPosition();
  }
  changePresets();
}

// Called on a single click of the user's up button
void up_click() {
  Serial.println("User Up click.");
  raise_up();
  delay(short_delay_time);
  stop_desk();
} 


// This function will be called when the user's up button was pressed 2 times in a short timeframe.
void up_doubleclick() {
  Serial.println("User Up doubleclick.");
  set_desk_height(tall_height);
} 

// This function will be called once, when the up button is pressed for a long time.
void up_longPressStart() {
  Serial.println("User Up longPress start");
  raise_up();
} 


// This function will be called often, while the up button is pressed for a long time.
void up_longPress() {

} 


// This function will be called once, when the up button is released after beeing pressed for a long time.
void up_longPressStop() {
  Serial.println("User Up longPress stop");
  stop_desk();
} 

// This function is called on a single press of the user's down button
void down_click() {
  Serial.println("User Down click.");
  lower();
  delay(short_delay_time);
  stop_desk();
} 


// This function will be called when the user's down button was pressed 2 times in a short timeframe.
void down_doubleclick() {
  Serial.println("User Down doubleclick.");
  set_desk_height(short_height);
} 

// This function will be called once, when the down button is pressed for a long time.
void down_longPressStart() {
  Serial.println("User Down longPress start");
  lower();
} 


// This function will be called often, while the down button is pressed for a long time.
void down_longPress() {

} 


// This function will be called once, when the down button is released after beeing pressed for a long time.
void down_longPressStop() {
  Serial.println("User Down longPress stop");
  stop_desk();
} 
