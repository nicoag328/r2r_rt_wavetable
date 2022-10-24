#include "pico/stdlib.h"
#include "hardware/pwm.h"

struct rgb_led_t
{
	uint8_t pin_red;
	uint8_t pin_green;
	uint8_t pin_blue;

} rgb_led;

void rgb_led_init(struct rgb_led_t * rgb_led, uint8_t pin_red, uint8_t pin_green, uint8_t pin_blue)
{
	rgb_led->pin_red = pin_red;
	rgb_led->pin_green = pin_green;
	rgb_led->pin_blue = pin_blue;

	gpio_set_function(pin_red, GPIO_FUNC_PWM);
	gpio_set_function(pin_green, GPIO_FUNC_PWM);
	gpio_set_function(pin_blue, GPIO_FUNC_PWM);

	pwm_set_clkdiv(pwm_gpio_to_slice_num(pin_red), 100.0);
	pwm_set_clkdiv(pwm_gpio_to_slice_num(pin_green), 100.0);
	pwm_set_clkdiv(pwm_gpio_to_slice_num(pin_blue), 100.0);

	pwm_set_wrap(pwm_gpio_to_slice_num(pin_red), 255);
	pwm_set_wrap(pwm_gpio_to_slice_num(pin_green), 255);
	pwm_set_wrap(pwm_gpio_to_slice_num(pin_blue), 255);

	pwm_set_chan_level(pwm_gpio_to_slice_num(pin_red), PWM_CHAN_A, 255);
	pwm_set_chan_level(pwm_gpio_to_slice_num(pin_green), PWM_CHAN_B, 255);
	pwm_set_chan_level(pwm_gpio_to_slice_num(pin_blue), PWM_CHAN_A, 255);

	pwm_set_enabled(pwm_gpio_to_slice_num(pin_red), true);
	pwm_set_enabled(pwm_gpio_to_slice_num(pin_green), true);
	pwm_set_enabled(pwm_gpio_to_slice_num(pin_blue), true);


}

void rgb_led_set_color(struct rgb_led_t * rgb_led, uint8_t red, uint8_t green, uint8_t blue)
{
	pwm_set_chan_level(pwm_gpio_to_slice_num(rgb_led->pin_red), PWM_CHAN_A, red);
	pwm_set_chan_level(pwm_gpio_to_slice_num(rgb_led->pin_green), PWM_CHAN_B, green);
	pwm_set_chan_level(pwm_gpio_to_slice_num(rgb_led->pin_blue), PWM_CHAN_A, blue);


}
