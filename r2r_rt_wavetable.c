#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/interp.h"
#include "rotary_encoder.c"
#include "input.c"
#include "osc.c"
#include "env.c"
#include "rgb_led.c"

// Rotary encoder
#define ENC_CLK_PIN 6
#define ENC_DATA_PIN 7
#define ENC_BUT_PIN 3

// Input
#define CLK_PIN 21
#define LOAD_PIN 22
#define SER_READ 20

// Sound
#define FIRST_DAC_PIN 8
#define WAVETABLE_LENGTH 500
#define SAMPLE_RATE 100000

// volatile uint8_t wavetable[96] = {127, 135, 144, 152, 160, 168, 176, 183, 191, 198, 205, 211, 217, 223, 228, 233, 237, 241, 245, 248, 250, 252, 253, 254, 255, 254, 253, 252, 250, 248, 245, 241, 237, 233, 228, 223, 217, 211, 205, 198, 191, 183, 176, 168, 160, 152, 144, 135, 127, 119, 110, 102, 94, 86, 78, 71, 63, 56, 49, 43, 37, 31, 26, 21, 17, 13, 9, 6, 4, 2, 1, 0, 0, 0, 1, 2, 4, 6, 9, 13, 17, 21, 26, 31, 37, 43, 49, 56, 63, 71, 78, 86, 94, 102, 110, 119};

struct rot_enc_t rot_enc;

volatile int8_t wavetable[500] = {0, 2, 3, 5, 6, 8, 10, 11, 13, 14, 16, 17, 19, 21, 22, 24, 25, 27, 28, 30, 32, 33, 35, 36, 38, 39, 41, 42, 44, 45, 47, 48, 50, 51, 53, 54, 56, 57, 58, 60, 61, 63, 64, 65, 67, 68, 69, 71, 72, 73, 75, 76, 77, 78, 80, 81, 82, 83, 85, 86, 87, 88, 89, 90, 91, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 105, 106, 107, 108, 109, 110, 111, 111, 112, 113, 114, 114, 115, 116, 116, 117, 117, 118, 119, 119, 120, 120, 121, 121, 122, 122, 123, 123, 123, 124, 124, 124, 125, 125, 125, 126, 126, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 126, 126, 126, 126, 126, 125, 125, 125, 124, 124, 124, 123, 123, 123, 122, 122, 121, 121, 120, 120, 119, 119, 118, 117, 117, 116, 116, 115, 114, 114, 113, 112, 111, 111, 110, 109, 108, 107, 106, 105, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 91, 90, 89, 88, 87, 86, 85, 83, 82, 81, 80, 78, 77, 76, 75, 73, 72, 71, 69, 68, 67, 65, 64, 63, 61, 60, 58, 57, 56, 54, 53, 51, 50, 48, 47, 45, 44, 42, 41, 39, 38, 36, 35, 33, 32, 30, 28, 27, 25, 24, 22, 21, 19, 17, 16, 14, 13, 11, 10, 8, 6, 5, 3, 2, 0, -2, -3, -5, -6, -8, -10, -11, -13, -14, -16, -17, -19, -21, -22, -24, -25, -27, -28, -30, -32, -33, -35, -36, -38, -39, -41, -42, -44, -45, -47, -48, -50, -51, -53, -54, -56, -57, -58, -60, -61, -63, -64, -65, -67, -68, -69, -71, -72, -73, -75, -76, -77, -78, -80, -81, -82, -83, -85, -86, -87, -88, -89, -90, -91, -93, -94, -95, -96, -97, -98, -99, -100, -101, -102, -103, -104, -105, -105, -106, -107, -108, -109, -110, -111, -111, -112, -113, -114, -114, -115, -116, -116, -117, -117, -118, -119, -119, -120, -120, -121, -121, -122, -122, -123, -123, -123, -124, -124, -124, -125, -125, -125, -126, -126, -126, -126, -126, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -126, -126, -126, -126, -126, -125, -125, -125, -124, -124, -124, -123, -123, -123, -122, -122, -121, -121, -120, -120, -119, -119, -118, -117, -117, -116, -116, -115, -114, -114, -113, -112, -111, -111, -110, -109, -108, -107, -106, -105, -105, -104, -103, -102, -101, -100, -99, -98, -97, -96, -95, -94, -93, -91, -90, -89, -88, -87, -86, -85, -83, -82, -81, -80, -78, -77, -76, -75, -73, -72, -71, -69, -68, -67, -65, -64, -63, -61, -60, -58, -57, -56, -54, -53, -51, -50, -48, -47, -45, -44, -42, -41, -39, -38, -36, -35, -33, -32, -30, -28, -27, -25, -24, -22, -21, -19, -17, -16, -14, -13, -11, -10, -8, -6, -5, -3, -2};

// volatile float notes[108] = {16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87, 32.70, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27, 61.74, 65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01, 4434.92, 4698.63, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13};
volatile uint32_t notes[108] = {4186, 4434, 4698, 4979, 5274, 5588, 5919, 6272, 6646, 7040, 7460, 7903, 8371, 8870, 9398, 9956, 10547, 11174, 11840, 12544, 13289, 14080, 14917, 15805, 16745, 17741, 18796, 19912, 21097, 22351, 23680, 25088, 26580, 28160, 29834, 31608, 33487, 35479, 37588, 39823, 42191, 44700, 47360, 50176, 53158, 56320, 59668, 63217, 66977, 70958, 75177, 79649, 84385, 89403, 94717, 100352, 106317, 112640, 119337, 126433, 133952, 141919, 150356, 159296, 168768, 178806, 189437, 200701, 212636, 225280, 238676, 252869, 267904, 283835, 300713, 318595, 337539, 357609, 378875, 401403, 425272, 450560, 477353, 505736, 535808, 567670, 601426, 637189, 675077, 715221, 757750, 802806, 850545, 901120, 954703, 1011474, 1071619, 1135340, 1202849, 1274376, 1350154, 1430438, 1515497, 1605614, 1701089, 1802240, 1909407, 2022945};

volatile int func = 0;
volatile int note_offset = 44;


enum
{
	FUNC_WAVEFORM = 0,
	FUNC_ENVELOPE,
	FUNC_OCTAVE
};

// Write sample to DAC. This function is called at sampling rate freq
bool write_sample(struct repeating_timer *t)
{
	int8_t pre_val = osc_run(&osc_a);
	uint8_t val = env_run(&env_a, pre_val) + 127;

	// Write sample to DAC
	int32_t mask = val << FIRST_DAC_PIN;
	gpio_put_masked(255 << FIRST_DAC_PIN, mask);	
	return true;
}


int main() 
{

	// Init stdio
	stdio_init_all();

	// Init rotary encoder
	rot_enc_init(&rot_enc, ENC_DATA_PIN, ENC_CLK_PIN, ENC_BUT_PIN);
	//gpio_set_irq_enabled_with_callback(ENC_DATA_PIN, GPIO_IRQ_EDGE_FALL, true, irq_callback);
	//gpio_set_irq_enabled(ENC_CLK_PIN, GPIO_IRQ_EDGE_FALL, true);
	//gpio_set_irq_enabled(ENC_BUT_PIN, GPIO_IRQ_EDGE_FALL, true);

	// Init ADC
	adc_init();
	adc_gpio_init(26);
	adc_gpio_init(27);

	// Init RGB led pins
	rgb_led_init(&rgb_led, 16, 17, 18);
	rgb_led_set_color(&rgb_led, 0, 0, 255);

	// Init input
	struct input_t input;
	input_init(&input, CLK_PIN, LOAD_PIN, SER_READ);

    // Init DAC output pins. TODO use gpio_set_dir_masked instead of for loop
    for (int gpio = FIRST_DAC_PIN; gpio < FIRST_DAC_PIN + 8; gpio++) {
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_OUT);
    }

	// Init synth
	osc_init(&osc_a, wavetable, WAVETABLE_LENGTH);
	env_init(&env_a);

	// Timer for sample rate. Gets called every 10us (100.000kHz)
	struct repeating_timer samplerate_timer;
	add_repeating_timer_us(-10, write_sample, NULL, &samplerate_timer);

	int16_t pot_0 = 0;
	int16_t pot_1 = 0;
	
	// Main loop
    while (true) {

		int rotary = rot_enc_read(&rot_enc);

		switch(rotary)
		{
			case ROT_ENC_CW:
				if(func < 2)
					func++;
				else
					func = 0;
				break;
			case ROT_ENC_CCW:
				if(func > 0)
					func--;
				else
					func = 2;
				break;
			case ROT_ENC_BUTTON:
				break;
		}	

		// Read potentiometers
		// Potentiometer 0
		adc_select_input(0);
		pot_0 = adc_read() >> 4;
	
		// Potentiometer 1
		adc_select_input(1);
		pot_1 = adc_read() >> 4;
	
		printf("%d\n", pot_0);

		// Update parameters
		switch(func)
		{
			case FUNC_WAVEFORM:
				break;
			case FUNC_ENVELOPE:
				env_set_attack(&env_a, pot_0);
				env_set_release(&env_a, pot_1);
				break;
			case FUNC_OCTAVE:
				if(pot_0 >= 0 && pot_0 < 70)
					note_offset = 32;
				if(pot_0 >= 85 && pot_0 < 170)
					note_offset = 44;
				if(pot_0 >= 185 && pot_0 <= 255)
					note_offset = 56;
				break;
		}
	
		// Read keyboard
		uint8_t key = 21 - input_read(&input);
	
		// Update frequency and gate to synth
		if (key > 0 && key != 21)
		{
			env_gate(&env_a, true);
			osc_set_freq(&osc_a, notes[key + note_offset]);
		}
		else
		{
			env_gate(&env_a, false);
			env_a.state = ENV_RELEASE;
		}

    }

    return 0;
}

