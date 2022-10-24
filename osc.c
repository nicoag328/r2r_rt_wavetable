// Oscillator module
// TODO Flexible sample-rate

#define SAMPLE_RATE 100000

volatile struct osc_t {
	int8_t sample;
	uint32_t phase;
	uint32_t phase_dx;
	uint32_t freq;
	volatile int8_t* wavetable;
	uint16_t wavetable_len;
} osc_a;

void osc_init(volatile struct osc_t* osc, volatile int8_t* wavetable, uint16_t wavetable_len)
{
	// Variable initialization
	osc->sample = 0;
	osc->phase = 0;
	osc->phase_dx = 0;
	osc->freq = 0;
	osc->wavetable = wavetable;
	osc->wavetable_len = wavetable_len;

	// Interpolator initialization
	interp_config cfg = interp_default_config();
	interp_config_set_blend(&cfg, true);
	interp_set_config(interp0, 0, &cfg);
	interp_config_set_signed(&cfg, true);
	interp_set_config(interp0, 1, &cfg);

}

void osc_set_freq(volatile struct osc_t* osc, uint32_t freq)
{
	osc->freq = freq;
	osc->phase_dx = (((uint64_t) (osc->wavetable_len)) * ((uint64_t) (osc->freq))) / (uint64_t)(SAMPLE_RATE);

}

int8_t osc_run(volatile struct osc_t* osc)
{
	// Hardware linear interpolation
	uint16_t lower_limit = osc->phase >> 8;
	uint16_t upper_limit = (osc->phase >> 8) + 1;

	if(upper_limit >= osc->wavetable_len)
		upper_limit = 0;

	interp0->base[0] = osc->wavetable[lower_limit];
	interp0->base[1] = osc->wavetable[upper_limit];
	interp0->accum[1] = osc->phase & 0XFF;
 
	// Write result
	osc->sample = interp0->peek[1];

	// Increment phase
	osc->phase += osc->phase_dx;

	// Reset phase when wavetable ends
	while (osc->phase >= osc->wavetable_len << 8)
		osc->phase -= osc->wavetable_len << 8;

	return osc->sample;
}

