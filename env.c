// env.c

enum {
	ENV_ATTACK = 0,
	ENV_SUSTAIN,
	ENV_RELEASE
};


volatile struct env_t {
	int8_t sample;
	uint8_t state;
	uint32_t level;
	uint32_t attack_dx;
	uint32_t release_dx;

} env_a;

void env_init(volatile struct env_t* env)
{
	env->sample = 0;
	env->state = ENV_RELEASE;
	env->level = 0;
	env->attack_dx = 100;
	env->release_dx = 3;
}

void env_gate(volatile struct env_t* env, bool gate)
{
	if(gate == true)
	{
		if(env->state == ENV_RELEASE)
		{
			env->state = ENV_ATTACK;
		}
	}
	else
	{
		env->state = ENV_RELEASE;
	}	
}

void env_set_attack(volatile struct env_t* env, uint32_t attack_time)
{
	env->attack_dx = 0xFF<<11 / attack_time;
}

void env_set_release(volatile struct env_t* env, uint32_t release_time)
{
	env->release_dx = 0xFF<<11 / release_time;
}


// TODO Optimization
int8_t env_run(volatile struct env_t* env, int8_t input)
{
	if(env->state == ENV_ATTACK)
	{
		if (env->level >= 0xFF<<11)
			env->state = ENV_SUSTAIN;
		else
			env->level += env->attack_dx;
	}

	if(env->state == ENV_RELEASE)
	{
		if (env->level < env->release_dx)
			env->level = 0;
		else
			env->level -= env->release_dx;
	}

	bool is_neg = false;
	volatile uint32_t out;

	if(input < 0)
	{
		is_neg = true;
		out = input * -1;
	}
	else
		out = input;
	
	out = ((temp<<8) * (env->level>>11)) >> 16;
	int8_t out = temp;

	if(is_neg == true)
		 out *= -1;

	env->sample = out;
	return env->sample;
}

