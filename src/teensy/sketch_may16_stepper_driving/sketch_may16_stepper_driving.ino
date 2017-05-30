/*     Simple Stepper Motor Control with timers
 *      
 * 
 *  
 */

#include "salad_spinner_global.h"
 
#include <TimerOne.h>

const int debug_delay = 15000;

const int spin_x = A4;
const int dpin_x = A5;
const int hpin_x = A0;
const int sleepx = 12;

const int spin_y = A7;
const int dpin_y = A6;
const int hpin_y = A1;
const int sleepy = 13;

const int spin_z = A8;
const int dpin_z = A9;
const int hpin_z = A2;
const int sleepz = 14;

const int arbiter_in = 21;
const int arbiter_out = 18;

volatile bool turnStart = 0;
volatile bool turnEnd = 1;

volatile StepperMotor X, Y, Z;

volatile int count_x = 0;
volatile int count_y = 0;
volatile int count_z = 0;

/**
 * @brief      Initializes stepper instantiations with default values
 *
 *             could put live file input here, or support serial commands to
 *             change these
 */
void StepperInitialize()
{
  X.step_delay = 20;
  Y.step_delay = 50;
  Z.step_delay = 30;
}

/**
 * @brief      Prints motor info for debugging
 *
 */
void StepperDebug(volatile StepperMotor &m)
{
  char str[500];
  sprintf(str, "delta: %d, steps: %d, delay: %d\n", m.delta, m.steps, m.step_delay);
  // Serial.print(str);
}

/**
 * @brief      Toggles the step pin. Externally called as a pulse. Also handles
 *             decrementing the steps and delta variables.
 *
 * @param[in]  spin  The step pin
 * @param[in]  dpin  The direction pin
 * @param      m     The stepper motor struct
 */
void HandleMotor(const int spin, const int dpin, volatile StepperMotor &m)
{
  uint8_t dir = m.steps > 0 ? 0 : 1;
  digitalWrite(dpin, dir);
  if (digitalRead(spin) == HIGH)
  {
    digitalWrite(spin, LOW);
    m.steps += dir ? 1 : -1;
    m.delta += !dir ? 1 : -1; // opposite of steps here!
  }
  else digitalWrite(spin, HIGH);
}

/**
 * @brief      Motor callback to be called in a timer interrupt. Hosts 3 counter
 *             variables for each axis and calls the relevant motor toggle
 *             functions
 */
void MotorCallback()
{
  static int debug_counter = 0;
  if (debug_counter == debug_delay)
  {   
    // Serial.print("X: ");
    StepperDebug(X);
    // Serial.print("Y: ");
    StepperDebug(Y);
    // Serial.print("Z: ");
    StepperDebug(Z);
    // Serial.println();
  }
  
  if (count_x == X.step_delay && X.steps != 0)
    HandleMotor(spin_x, dpin_x, X);
  
  if (count_y == Y.step_delay && Y.steps != 0)
    HandleMotor(spin_y, dpin_y, Y);
  
  if (count_z == Z.step_delay && Z.steps != 0)
    HandleMotor(spin_z, dpin_z, Z);

  // Increment the count variables
  count_x++;
  count_y++;
  count_z++;
//  debug_counter++;

  // Roll over if > step_delay
  count_x %= X.step_delay + 1;
  count_y %= Y.step_delay + 1;
  count_z %= Z.step_delay + 1;
  debug_counter %= debug_delay + 1;
}
 
void setup() {
  // Sets the two pins as Outputs
  Serial.begin(9600);
  pinMode(11,OUTPUT);
  pinMode(spin_x,OUTPUT); 
  pinMode(dpin_x,OUTPUT);
  pinMode(hpin_x, INPUT_PULLUP);
  pinMode(spin_y,OUTPUT); 
  pinMode(dpin_y,OUTPUT);
  pinMode(hpin_y, INPUT_PULLUP);
  pinMode(spin_z,OUTPUT); 
  pinMode(dpin_z,OUTPUT);
  pinMode(hpin_z, INPUT_PULLUP);

  StepperInitialize();

  Timer1.initialize(100);
  Timer1.attachInterrupt(MotorCallback);
  Timer1.start();
  // Serial.println("Welcome to the world of tomorrow");
  StepperDebug(X);
  StepperDebug(Y);
  StepperDebug(Z);
}

void SerialFlush()
{
  while (Serial.available() > 0)
    Serial.read();
}

void MotorSteps(byte b)
{
    if (X.steps || Y.steps || Z.steps) // we're still stepping. Kindly ignore the user.
    {
      // Serial.println("Teensy is busy. Please kindly be ignored. Thank you, user.");
      return;
    }

    switch (b)
    {
      case 'x': X.steps = Serial.parseInt();
        break;
      case 'y': Y.steps = Serial.parseInt();
        break;
      case 'z': Z.steps = Serial.parseInt();
        break;
      default: return;
    }
}

void KillMotors()
{
  X.steps = 0;
  Y.steps = 0;
  Z.steps = 0;
  
  digitalWrite(spin_x, LOW);
  digitalWrite(spin_y, LOW);
  digitalWrite(spin_z, LOW);
}

void HomeX()
{
  X.delta = 0;
}

void HomeY()
{
  while (digitalRead(hpin_y) == HIGH)
  {
    Y.steps = 1;
    while (Z.steps); // wait
  }
  KillMotors();
  Y.delta = 0;
}

void HomeZ()
{
  while (digitalRead(hpin_z) == HIGH)
  {
    Z.steps = -1;
    while (Z.steps); // wait
  }
  KillMotors();
  Z.delta = 0;
}

void Home(byte b)
{
  KillMotors();
    
  switch (b)
  {
    case 'x': HomeX(); break;
    case 'y': HomeY(); break;
    case 'z': HomeZ(); break;
  }
}

void CheckSerial()
{
  if (Serial.available() > 0)
  {
    byte b = Serial.read();

    switch (b)
    {
      case 'x':
      case 'y':
      case 'z':
        MotorSteps(b);
        break;
      case 'h':
        Home(Serial.read());
        break;
      case '0': KillMotors();
        break;    
      default:
        break;
        // Serial.println("Command not recognized");
    }
    // Serial.print("X: ");
    StepperDebug(X);
    // Serial.print("Y: ");
    StepperDebug(Y);
    // Serial.print("Z: ");
    StepperDebug(Z);
    // Serial.println();

    SerialFlush();
  }
}

void loop()
{
  X.endstop_home = digitalRead(hpin_x);
  Y.endstop_home = digitalRead(hpin_y);
  Z.endstop_home = digitalRead(hpin_z);
  turnStart = digitalRead(arbiter_in);

  if (X.steps || Y.steps || Z.steps)
  {
    digitalWrite(11, HIGH);
  }
  else digitalWrite(11,LOW);

  CheckSerial();
}
