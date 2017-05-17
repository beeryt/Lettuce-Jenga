/*     Simple Stepper Motor Control Exaple Code
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */

// defines pins numbers
const int stepPin = A4; 
const int dirPin = A5;

void MoveMotor(int dir, int num_steps)
{
  digitalWrite(dirPin, dir);
  for (int i = 0; i < num_steps; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
  }
}
 
void setup() {
  // Sets the two pins as Outputs
  Serial.begin(9600);
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
}

void loop() {
  Serial.println("Boring loop");
  if (Serial.available() > 0)
  {
    Serial.println("Data incoming!");
    int steps = Serial.parseInt();
    int dir = steps > 0 ? 0 : 1;
    MoveMotor(dir, abs(steps));

//    char* str = new char[500];
//    sprintf(str, "Motor moved %sclockwise %d steps.", dir ? "counter" : "", abs(steps));
    Serial.print("Motor Moved ");
    Serial.print(steps);
    Serial.println(" steps.");
  }
//  MoveMotor(1, 400);
//  delay(2000); // One second delay
//  MoveMotor(0, 800);
//  delay(2000);
}
