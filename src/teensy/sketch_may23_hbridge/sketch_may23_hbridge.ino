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

void grip()
{
 
  analogWrite(speedPinA, 170);//Sets speed variable via PWM 
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  Serial.println("Motor 1 Grip"); // Prints out “Motor 1 Forward” on the serial monitor
  Serial.println("   "); // Creates a blank line printed on the serial monitor

  delay(750);
  
  analogWrite(speedPinA, 0);
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
}

void ungrip()
{
 
  analogWrite(speedPinA, 170);//Sets speed variable via PWM 
  digitalWrite(dir1PinA, HIGH);
  digitalWrite(dir2PinA, LOW);
  Serial.println("Motor Ungrip"); // Prints out “Motor 1 Forward” on the serial monitor
  Serial.println("   "); // Creates a blank line printed on the serial monitor

  delay(500);
  
  analogWrite(speedPinA, 0);
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
}

void pull()
{
 
  analogWrite(speedPinB, 170);//Sets speed variable via PWM 
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
  Serial.println("Motor 2 Back"); // Prints out “Motor 1 Forward” on the serial monitor
  Serial.println("   "); // Creates a blank line printed on the serial monitor

  delay(800);
  
  analogWrite(speedPinB, 0);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
}

void forward()
{
 
  analogWrite(speedPinB, 170);//Sets speed variable via PWM 
  digitalWrite(dir1PinB, HIGH);
  digitalWrite(dir2PinB, LOW);
  Serial.println("Motor 2 Forward"); // Prints out “Motor 1 Forward” on the serial monitor
  Serial.println("   "); // Creates a blank line printed on the serial monitor

  delay(800);
  
  analogWrite(speedPinB, 0);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH); 
}

void loop() {

// Initialize the Serial interface:

if (Serial.available() > 0) {
int inByte = Serial.read();
int speed; // Local variable

switch (inByte) {

case 'g': grip(); break;
case 'u': ungrip(); break;
case 'b': pull(); break;
case 'f': forward(); break;

//______________Motor 1______________

case '1': // Motor 1 Forward
analogWrite(speedPinA, 170);//Sets speed variable via PWM 
digitalWrite(dir1PinA, LOW);
digitalWrite(dir2PinA, HIGH);
Serial.println("Motor 1 Forward"); // Prints out “Motor 1 Forward” on the serial monitor
Serial.println("   "); // Creates a blank line printed on the serial monitor
break;

case '2': // Motor 1 Stop (Freespin)
analogWrite(speedPinA, 0);
digitalWrite(dir1PinA, LOW);
digitalWrite(dir2PinA, HIGH);
Serial.println("Motor 1 Stop");
Serial.println("   ");
break;

case '3': // Motor 1 Reverse
analogWrite(speedPinA, 170);
digitalWrite(dir1PinA, HIGH);
digitalWrite(dir2PinA, LOW);
Serial.println("Motor 1 Reverse");
Serial.println("   ");
break;

//______________Motor 2______________

case '4': // Motor 2 Forward
analogWrite(speedPinB, 127);
digitalWrite(dir1PinB, LOW);
digitalWrite(dir2PinB, HIGH);
Serial.println("Motor 2 Forward");
Serial.println("   ");
break;

case '5': // Motor 1 Stop (Freespin)
analogWrite(speedPinB, 0);
digitalWrite(dir1PinB, LOW);
digitalWrite(dir2PinB, HIGH);
Serial.println("Motor 2 Stop");
Serial.println("   ");
break;

case '6': // Motor 2 Reverse
analogWrite(speedPinB, 127);
digitalWrite(dir1PinB, HIGH);
digitalWrite(dir2PinB, LOW);
Serial.println("Motor 2 Reverse");
Serial.println("   ");
break;

default:
// turn all the connections off if an unmapped key is pressed:
for (int thisPin = 2; thisPin < 11; thisPin++) {
digitalWrite(thisPin, LOW);
}
  }
    }
      }

