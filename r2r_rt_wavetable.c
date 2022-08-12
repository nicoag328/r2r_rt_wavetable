#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define FIRST_DAC_PIN 7
#define WAVETABLE_LENGTH 96
#define SAMPLE_RATE 100000

volatile uint8_t sine[96] = {127, 135, 144, 152, 160, 168, 176, 183, 191, 198, 205, 211, 217, 223, 228, 233, 237, 241, 245, 248, 250, 252, 253, 254, 255, 254, 253, 252, 250, 248, 245, 241, 237, 233, 228, 223, 217, 211, 205, 198, 191, 183, 176, 168, 160, 152, 144, 135, 127, 119, 110, 102, 94, 86, 78, 71, 63, 56, 49, 43, 37, 31, 26, 21, 17, 13, 9, 6, 4, 2, 1, 0, 0, 0, 1, 2, 4, 6, 9, 13, 17, 21, 26, 31, 37, 43, 49, 56, 63, 71, 78, 86, 94, 102, 110, 119};

volatile float phase = 0.0;
volatile float frequency = 3000.0;

// Write sample to DAC. This function is called at sampling rate freq
bool write_sample(struct repeating_timer *t)
{
	// Calculate sample to write	
	phase += WAVETABLE_LENGTH * frequency / SAMPLE_RATE;

	while (phase >= 96.0){
		phase -= 96.0;
	}

	// Write sample to DAC
	uint8_t val = sine[(int)(phase)];
	int32_t mask = val << FIRST_DAC_PIN;
	gpio_put_masked(255<<FIRST_DAC_PIN, mask);	
	return true;
}

int main() {

	// Init stdio
	stdio_init_all();
	
    // Init DAC output pins. TODO use gpio_set_dir_masked instead of for loop
    for (int gpio = FIRST_DAC_PIN; gpio < FIRST_DAC_PIN + 8; gpio++) {
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_OUT);
    }
	
	// Timer for sample rate. Gets called every 10us (100.000kHz)
	struct repeating_timer samplerate_timer;
	add_repeating_timer_us(-10, write_sample, NULL, &samplerate_timer);

    while (true) {
		sleep_ms(100);
    }

    return 0;
}

