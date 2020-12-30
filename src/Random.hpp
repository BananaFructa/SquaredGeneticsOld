#pragma once

class Random {
public:
	/// <returns>Returns a float value between -1 and 1</returns>
	static float RandomFloat() {
		return ((float)rand() / (float)RAND_MAX) * 2 - 1;
	}

	static float RandomPositiveFloat() {
		return ((float)rand() / (float)RAND_MAX);
	}
};