#ifndef COMM_STRUCTS_H
#define COMM_STRUCTS_H

#define USB_DEBUG 9
#define ENDSTOP_HIT 23
#define STATUS_CHECK 69

struct StepperStatus
{
	int delta_steps;	// delta steps since homing
	int steps_left;		// steps left to execute
	int step_delay;		// time between step pulse changes
	bool endstop_max;	// endstop for maximum movement
	bool endstop_home;	// endstop for homing position
};

struct ArbiterStatus
{
	bool arbiter_in;	// signal for turn start (lasts 1min or until arbiter_out goes high)
	bool arbiter_out;	// signal for turn end
}

#endif