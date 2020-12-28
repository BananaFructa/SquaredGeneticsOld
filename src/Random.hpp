#pragma once

namespace Random {
	/// <returns>Returns a float value between -1 and 1</returns>
	float RandomFloat() {
		return ((float)rand() / (float)RAND_MAX) * 2 - 1;
	}
}