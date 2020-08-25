#pragma once
#include <random>
#include <stdint.h>
#include <vector>

#include "GeometryComponents.h"

class Noise {

	float base_ampl, base_freq, persist;
	uint32_t layer_count, seed;
	std::vector<int> seed_list;


	float getRandFloat(unsigned int base, unsigned int seed) {
		int n = base;
		n = (n << 13) ^ n ^ seed;
		return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
	}

	int convertCoef(int x, int y) {
		return x + y * 57;
	}

	int convertCoef(Point p) {
		return convertCoef(p.x, p.y);
	}

	std::vector<float> getRandCoefArray(int base, std::vector<int> seed_list) {
		std::vector<float> output;
		for (int i = 0; i < seed_list.size(); i++) {
			output.push_back(getRandFloat(base, seed_list[i]));
		}
	}

	float getRoundedRandomCoef(float x, float y, float freq, float ampl, uint32_t seed) {
		x = floor(x / freq) * freq;
		y = floor(y / freq) * freq;
		int output = 0;

		output += (getRandFloat(convertCoef(x - freq, y - freq), seed) + getRandFloat(convertCoef(x + freq, y - freq), seed) + getRandFloat(convertCoef(x - freq, y + freq), seed) + getRandFloat(convertCoef(x + freq, y + freq), seed)) / 16 * ampl;
		output += (getRandFloat(convertCoef(x - freq, y), seed) + getRandFloat(convertCoef(x + freq, y), seed) + getRandFloat(convertCoef(x, y - freq), seed) + getRandFloat(convertCoef(x, y + freq), seed)) / 8 * ampl;
		output += getRandFloat(convertCoef(x, y), seed) / 4 * ampl;

		return output;
	}

	float getLineInterpolatedRandomCoef(float x, float y, float freq, float ampl, uint32_t seed) {
		float round_x = floor(x / freq) * freq;
		float round_y = floor(y / freq) * freq;

		return
			getRoundedRandomCoef(x, y, freq, ampl, seed) * (round_x + freq - x) * (round_y + freq - y) / freq / freq +
			getRoundedRandomCoef(x + freq, y, freq, ampl, seed) * (x - round_x) * (round_y + freq - y) / freq / freq +
			getRoundedRandomCoef(x, y + freq, freq, ampl, seed) * (round_x + freq - x) * (y - round_y) / freq / freq +
			getRoundedRandomCoef(x + freq, y + freq, freq, ampl, seed) * (x - round_x) * (y - round_y) / freq / freq;
	}


	int getHeight(Point p, uint32_t seed) {
		float freq = base_freq, ampl = base_ampl;
		int output = 0;
		for (int i = 0; i < layer_count; i++) {

			freq /= 2;
			output += getLineInterpolatedRandomCoef(p.x, p.y, freq, ampl, seed);

			ampl *= persist;
		}


		return output;
	}

public:

	Noise(float base_ampl, float base_freq, float persist, uint32_t layer_count, uint32_t seed, uint32_t seed_list_size) :
		base_ampl(base_ampl), base_freq(base_freq), persist(persist), layer_count(layer_count), seed(seed)
	{
		srand(seed);
		for (int i = 0; i < seed_list_size; i++) {
			seed_list.push_back(rand());
		}
	};

	std::vector<int> getTerritorialDistribution(Point p, int peak_threshold, int & max_ind_output, int & max_value_output) {
		std::vector<int> output;
		std::vector<int> coef_arr;
		int max_ind = 0, second_max_ind = 0;
		for (int i = 0; i < seed_list.size(); i++) {
			int height = getHeight(p, seed_list[i]);
			coef_arr.push_back(height);
			if (coef_arr[i] > coef_arr[max_ind]) {
				second_max_ind = max_ind;
				max_ind = i;
			}
			else if (coef_arr[i] > coef_arr[second_max_ind]) {
				second_max_ind = i;
			}
		}
		for (int i = 0; i < seed_list.size(); i++) {
			coef_arr[i] = (i == max_ind && coef_arr[max_ind] >= peak_threshold) ? (peak_threshold + (coef_arr[max_ind] - coef_arr[second_max_ind] - base_ampl / 2)) : -base_ampl * 2;
		}
		max_ind_output = max_ind;
		max_value_output = coef_arr[max_ind];
		return output;
	}

	int getHeight(Point p) {
		return getHeight(p, seed);
	}
};