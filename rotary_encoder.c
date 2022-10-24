// rotary_encoder.c
#include "pico/stdlib.h"

enum
{
	ROT_ENC_NULL = 0,
	ROT_ENC_CW = 1,
	ROT_ENC_CCW = -1,
	ROT_ENC_BUTTON = 2
};

struct rot_enc_t
{
	uint8_t data_pin;
	uint8_t clk_pin;
	uint8_t button_pin;
};

void rot_enc_init(struct rot_enc_t * rot_enc, uint8_t data_pin, uint8_t clk_pin, uint8_t button_pin)
{
	// Init variables
	rot_enc->data_pin = data_pin;
	rot_enc->clk_pin = clk_pin;
	rot_enc->button_pin = button_pin;
	
	// Init pins
	gpio_init(rot_enc->data_pin);
	gpio_set_dir(rot_enc->data_pin, GPIO_IN);
	gpio_pull_up(rot_enc->data_pin);
	gpio_init(rot_enc->clk_pin);
	gpio_set_dir(rot_enc->clk_pin, GPIO_IN);
	gpio_pull_up(rot_enc->clk_pin);
	gpio_init(rot_enc->button_pin);
	gpio_set_dir(rot_enc->button_pin, GPIO_IN);
	gpio_pull_up(rot_enc->button_pin);

}

int8_t rot_enc_read(struct rot_enc_t * rot_enc)
{
	const static uint8_t rot_enc_table[] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};
	static uint8_t prev_next_code = 0;
	static uint8_t store = 0;

	if (gpio_get(rot_enc->button_pin) == 0)
	{
		if (gpio_get(rot_enc->button_pin) == 0)
		{
			while (gpio_get(rot_enc->button_pin) == 0);
			return ROT_ENC_BUTTON;
		}
	}

	prev_next_code <<= 2;
	if (gpio_get(rot_enc->data_pin)) prev_next_code |= 0x02;
	if (gpio_get(rot_enc->clk_pin)) prev_next_code |= 0x01;
	prev_next_code &= 0x0f;

	// If valid then store as 16 bit data.
	if (rot_enc_table[prev_next_code] )
	{
		store <<= 4;
		store |= prev_next_code;

		if ((store & 0xff) == 0x2b) return ROT_ENC_CCW;
		if ((store & 0xff) == 0x17) return ROT_ENC_CW;
	}

  return ROT_ENC_NULL;
}

