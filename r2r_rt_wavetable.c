#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define FIRST_DAC_PIN 7
#define WAVETABLE_LENGTH 96
#define SAMPLE_RATE 50000

volatile uint8_t wavetable[96] = {127, 135, 144, 152, 160, 168, 176, 183, 191, 198, 205, 211, 217, 223, 228, 233, 237, 241, 245, 248, 250, 252, 253, 254, 255, 254, 253, 252, 250, 248, 245, 241, 237, 233, 228, 223, 217, 211, 205, 198, 191, 183, 176, 168, 160, 152, 144, 135, 127, 119, 110, 102, 94, 86, 78, 71, 63, 56, 49, 43, 37, 31, 26, 21, 17, 13, 9, 6, 4, 2, 1, 0, 0, 0, 1, 2, 4, 6, 9, 13, 17, 21, 26, 31, 37, 43, 49, 56, 63, 71, 78, 86, 94, 102, 110, 119};

volatile float notes[108] = {16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87, 32.70, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27, 61.74, 65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01, 4434.92, 4698.63, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13};

volatile int current_note = 0;

volatile float phase = 0.0;

// Write sample to DAC. This function is called at sampling rate freq
bool write_sample(struct repeating_timer *t)
{
	// Linear interpolation
	int index_below = (int)(phase);
	int index_above = index_below + 1;
	if (index_above >= WAVETABLE_LENGTH)
		index_above = 0;

	float fraction_above = phase - index_below;
	float fraction_below = 1.0 - fraction_above;

	uint8_t val = (fraction_below * wavetable[index_below]) + (fraction_above * wavetable[index_above]);

	// Calculate sample to write	
	phase += WAVETABLE_LENGTH * notes[current_note] / SAMPLE_RATE;

	while (phase >= WAVETABLE_LENGTH){
		phase -= WAVETABLE_LENGTH;
	}

	// Write sample to DAC
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
	
	// Timer for sample rate. Gets called every 20us (50.000kHz)
	struct repeating_timer samplerate_timer;
	add_repeating_timer_us(-20, write_sample, NULL, &samplerate_timer);

    while (true) {
		sleep_ms(2000);
		current_note++;
		if (current_note == 108)
			current_note = 0;
    }

    return 0;
}

