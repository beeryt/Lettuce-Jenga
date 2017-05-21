#ifndef COMM_STRUCTS_H
#define COMM_STRUCTS_H

#include <stdint.h>

#define USB_DEBUG 9
#define ENDSTOP_HIT 23
#define STATUS_CHECK 69

struct StepperMotor
{
	int delta;		// delta steps since homing
	int steps;				// steps left to execute
	int step_delay;			// time between step pulse changes
	uint8_t endstop_max;	// endstop for maximum movement
	uint8_t endstop_home;	// endstop for homing position
};

struct ArbiterStatus
{
	uint8_t arbiter_in;	// signal for turn start (lasts 1min or until arbiter_out goes high)
	uint8_t arbiter_out;// signal for turn end
};

#endif