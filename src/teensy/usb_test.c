#include "../salad_spinner_config.h"
#include "steppers.c" // not pretty just for testing

#include <usb_serial/usb_serial.h>

#include <util/delay.h>

#include <stdio.h>
#include <string.h>

int16_t recv_byte(void);
void print(const char*, ...);

void status_check(void);

int main(void)
{
	usb_init();
	setup();

	while (!usb_configured()); // wait
	_delay_ms(1000);

	while (1)
	{
		// wait for the user to run their terminal emulator program
		// which sets DTR to indicate it is ready to receive.
		while (!(usb_serial_get_control() & USB_SERIAL_DTR)); // wait

		// discard anything that was received prior.  Sometimes the
		// operating system or other software will send a modem
		// "AT command", which can still be buffered.
		usb_serial_flush_input();

		while (1)
		{
			print("Waiting for byte");
			int16_t byte = recv_byte();
			if (byte == -1)
			{
				print("Woops Did not receive a byte\n");
				usb_serial_flush_input();
				break;
			}
			print("Byte: 0x%02x\n", byte);

			switch (byte)
			{
				case STATUS_CHECK:
					status_check();
					break;
				case TEST_BYTE:
					print("Starting Loop"\n);
					loop();
					print("Loop Done\n");
					break;
				default:
					break;
			}
			usb_serial_flush_input();
		}
	}

	return 0;
}

/**
 * @brief      Returns to the Pi any status changes including errors or endstop
 *             positions
 *             
 *             Send everything everytime!
 */
void status_check()
{
}

void print(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[256];
	vsprintf(buffer, format, args);
	va_end(args);

	usb_serial_putchar(USB_DEBUG);

	for (unsigned int i = 0; i < strlen(buffer); ++i)
		usb_serial_putchar(buffer[i]);
}

int16_t recv_byte()
{
	int16_t r;
	while (1) // continue waiting on timeouts
	{
		r = usb_serial_getchar();
		if (r != -1) break;
		if (!usb_configured() || !(usb_serial_get_control() & USB_SERIAL_DTR))
			return -1; // connection terminated
	}
	return r;
}
