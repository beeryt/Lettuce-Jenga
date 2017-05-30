//Code by Reichenstein7 (thejamerson.com)

//Keyboard Controls:
//
// 1 -Motor 1 Left
// 2 -Motor 1 Stop
// 3 -Motor 1 Right
//
// 4 -Motor 2 Left
// 5 -Motor 2 Stop
// 6 -Motor 2 Right

// Declare L298N Dual H-Bridge Motor Controller directly since there is not a library to load.

// Motor 1
int dir1PinA = 17;
int dir2PinA = 16;
int speedPinA = 15; // Needs to be a PWM pin to be able to control motor speed

// Motor 2
int dir1PinB = 12;
int dir2PinB = 13;
int speedPinB = 14; // Needs to be a PWM pin to be able to control motor speed

void setup() {  // Setup runs once per reset
// initialize serial communication @ 9600 baud:
Serial.begin(9600);

//Define L298N Dual H-Bridge Motor Controller Pins

pinMode(dir1PinA,OUTPUT);
pinMode(dir2PinA,OUTPUT);
pinMode(speedPinA,OUTPUT);
pinMode(dir1PinB,OUTPUT);
pinMode(dir2PinB,OUTPUT);
pinMode(speedPinB,OUTPUT);

}

void moveMotor(int speedPin, int dir1Pin, int dir2Pin, int dir, int dur, byte pwm_value=170)
{
  analogWrite(speedPin, pwm_value);
  digitalWrite(dir1Pin, !dir);
  digitalWrite(dir2Pin, dir);
  delay(dur);
  analogWrite(speedPin, 0);
}

void grip()
{
  moveMotor(speedPinA, dir1PinA, dir2PinA, HIGH, 750);
}

void ungrip()
{
  moveMotor(speedPinA, dir1PinA, dir2PinA, LOW, 500);
}

void pull()
{
  moveMotor(speedPinB, dir1PinB, dir2PinB, HIGH, 800);
}

void forward()
{
  moveMotor(speedPinB, dir1PinB, dir2PinB, LOW, 800);
}

void loop() {

// Initialize the Serial interface:

if (Serial.available() > 0) {
int inByte = Serial.read();

int val, dir, dur;

switch (inByte) {

case 'g': grip(); break;
case 'u': ungrip(); break;
case 'b': pull(); break;
case 'f': forward(); break;

//______________Motor 1______________

case '1': // Motor 1 Forward
val = Serial.parseInt();
dir = val > 0 ? 1 : 0;
dur = abs(val);
moveMotor(speedPinA, dir1PinA, dir2PinA, dir, dur);
break;

case '2': // Motor 1 Stop (Freespin)
val = Serial.parseInt();
dir = val > 0 ? 1 : 0;
dur = abs(val);
moveMotor(speedPinB, dir1PinB, dir2PinB, dir, dur);
break;

default:
// turn all the connections off if an unmapped key is pressed:
for (int thisPin = 2; thisPin < 11; thisPin++) {
digitalWrite(thisPin, LOW);
}
  }
    }
      }

