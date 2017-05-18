/*     Simple Stepper Motor Control with timers
 *      
 * 
 *  
 */

#include "TimerOne.h"

// defines pins numbers
const int stepPin = A4; 
const int dirPin = A5;

volatile int steps_x = 0;             //current number of steps in queue
int steps_y = 0;
int steps_z = 0;
int dir_x;
int dir_y;
int dir_z;
int step_delay_x = 20;           //how many timer loops in a motor step; controls speed
int step_delay_y;
int step_delay_z;
volatile int count_x = 0;
int count_y = 0;
int count_z = 0;

void MoveMotor()  
{
  
  int dir_x = steps_x > 0 ? 0 : 1;        //0 if +; 1 if -
  
  if (count_x == step_delay_x && steps_x != 0)
  {
    count_x = 0;
  
    digitalWrite(dirPin, dir_x);
    if(digitalRead(stepPin) == LOW)
    {
      digitalWrite(stepPin, HIGH);
    }
    else if(digitalRead(stepPin) == HIGH)
    {
      digitalWrite(stepPin, LOW);
      steps_x += (dir_x) ? 1 : -1;
      Serial.print('.');
      if (steps_x == 0) Serial.println();
//      delta_x += (dir_x) ? 1 : -1;
    }
  }

  count_x++;
  count_y++;
  count_z++;
}
 
void setup() {
  // Sets the two pins as Outputs
  Serial.begin(9600);
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);

  Timer1.initialize(100);
  Timer1.attachInterrupt(MoveMotor);
  Timer1.start();
  Serial.println("Welcome to the world of tomorrow")
}

void loop() {
  if (Serial.available() > 0)
  {
    if (steps_x != 0) // we're still stepping. Kindly ignore the user.
    {
      Serial.println("We are still stepping at this time. Please kindly be ignored. Thank you, user.");
      while (Serial.read() != -1);
      return;
    }
    Serial.println("Data incoming!");
    int num = Serial.parseInt();
    Serial.print("Received: ");
    Serial.println(num);
    Serial.print("Starting motors");
    steps_x = num;
    count_x = 0;
  }
}
