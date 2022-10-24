// input.c
#include "pico/stdlib.h"

struct input_t
{
	uint8_t clk_pin;
	uint8_t load_pin;
	uint8_t ser_read;
};

void input_init(struct input_t* input, uint8_t clk_pin, uint8_t load_pin, uint8_t ser_read)
{
	// Init variables
	input->clk_pin = clk_pin;
	input->load_pin = load_pin;
	input->ser_read = ser_read;
	
	// Init pins
	gpio_init(input->clk_pin);
	gpio_set_dir(input->clk_pin, GPIO_OUT);

	gpio_init(input->load_pin);
	gpio_set_dir(input->load_pin, GPIO_OUT);

	gpio_init(input->ser_read);
	gpio_set_dir(input->ser_read, GPIO_IN);

}

uint8_t input_read(struct input_t* input)
{

	// Load into register
	gpio_put(input->load_pin, false);
	busy_wait_at_least_cycles(15);

	gpio_put(input->load_pin, true);

	// Read register
	// uint32_t data = 0;
	uint8_t pos;
	for (pos = 20; pos > 0; pos--)
	{
		// Read data
		if(gpio_get(input->ser_read) == true)
			break;

		// Send clock signal
		gpio_put(input->clk_pin, true);
		busy_wait_at_least_cycles(15);
		gpio_put(input->clk_pin, false);
	}

	return pos;
}

