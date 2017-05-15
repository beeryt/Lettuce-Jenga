/*     Simple Stepper Motor Control Exaple Code
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */

#include <avr/io.h>
#include <util/delay.h>

// defines pins numbers
#define STEP_PIN PF1
#define STEP_PORT PORTF
#define DIR_PIN PF4
#define DIR_PORT PORTF

void digitalWrite(volatile uint8_t *port, uint8_t pin, uint8_t val)
{
  if (val) *port |= (1<<pin);
  else *port &= ~(1<<pin);
}

void setup(void) {
  // Sets the two pins as Outputs
  digitalWrite(&DDRF, STEP_PIN, 1); // OUTPUT
  digitalWrite(&DDRF, DIR_PIN, 1); // OUTPUT
}

void loop(void) {

  digitalWrite(&DIR_PORT, DIR_PIN, 1); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for (int i = 0; i < 400; ++i)
  {
    digitalWrite(&STEP_PORT, STEP_PIN, 1);
    _delay_us(1000);
    digitalWrite(&STEP_PORT, STEP_PIN, 0);
    _delay_us(1000);
  }
  _delay_ms(2000); // One second delay
  
  digitalWrite(&DIR_PORT, DIR_PIN, 0); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for (int i = 0; i < 800; ++i)
  {
    digitalWrite(&STEP_PORT, STEP_PIN, 1);
    _delay_us(1000);
    digitalWrite(&STEP_PORT, STEP_PIN, 0);
    _delay_us(1000);
  }
  _delay_ms(2000);
}

// int main(void)
// {
//   setup();
//   while (1) loop();
// }
