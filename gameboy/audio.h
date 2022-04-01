#pragma once

#include "def.h"

/*
 * boilerplate for APU since maybe (just maybe) i'll
 * write an APU. but for right now this is an extreme
 * solution to pokemon centers freezing up :)
 */
struct apu
{
	u8 ch1_sweep;
	u8 ch1_length_duty;
	u8 ch1_volume_env;
	u8 ch1_freq_lo;
	u8 ch1_freq_hi;

	u8 ch2_length_duty;
	u8 ch2_volume_env;
	u8 ch2_freq_lo;
	u8 ch2_freq_hi;

	u8 ch3_on;
	u8 ch3_length;
	u8 ch3_out_level;
	u8 ch3_freq_lo;
	u8 ch3_freq_hi;
	u8 ch3_wave_pattern[0xf];

	u8 ch4_length;
	u8 ch4_volume_env;
	u8 ch4_poly_ctr;
	u8 ch4_counter;

	u8 ch_ctrl;
	u8 snd_output_select;
	u8 sound_on;
};