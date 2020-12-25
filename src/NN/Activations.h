#pragma once

namespace af {
	float Sigmoid(float x) {
		return 1.0f / (1.0f + std::exp(-x));
	}

	float ReLU(float x) {
		bool pos = x >= 0;
		return pos * x + !pos * x * 0.1f;
	}

	float Tanh(float x) {
		return std::tanh(x);
	}

	float UseActivation(float x, AFunction af) {
		switch (af) {

		case AFunctions::Sigmoid:
			return Sigmoid(x);

		case AFunctions::LeakyReLU:
			return ReLU(x);

		case AFunctions::Tanh:
			return Tanh(x);

		default:
			return 0;
		}
	}
}