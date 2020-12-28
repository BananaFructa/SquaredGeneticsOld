#include "FFNN.h"
#include "../Random.hpp"

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

float UseActivation(float x, AFunction Af) {
	if (Af == AFunctions::Sigmoid) return Sigmoid(x);
	if (Af == AFunctions::LeakyReLU) return ReLU(x);
	if (Af == AFunctions::Tanh) return Tanh(x);
	return 0;
}

FFNN::FFNN (int Layers[], AFunction Activations[], int size) {
	this->LayerCount = size;
	this->LayerSizes = new int[size];
	for (int i = 0;i < size;++i) this->LayerSizes[i] = Layers[i];
	this->Biases = new float* [size-1];
	this->Layers = new float* [size];
	this->Weights = new float** [size-1];
	this->Activations = new AFunction[size-1];
	for (int i = 0;i < size - 1;++i) this->Activations[i] = Activations[i];
	for (int i = 0;i < size; ++i) {
		this->Layers[i] = new float[Layers[i]];
	}
	for (int i = 1;i < size; ++i) {
		Biases[i-1] = new float[Layers[i]];
		for (int l = 0;l < Layers[i];++l) Biases[i-1][l] = 0;
		if (i > 0) {
			Weights[i-1] = new float* [Layers[i-1]];
			for (int j = 0;j < Layers[i - 1]; ++j) {
				Weights[i - 1][j] = new float[Layers[i]];
				for (int l = 0;l < Layers[i];++l) Weights[i - 1][j][l] = 0;
			}
		}
	}
}

float* FFNN::Predict(float Input[]) {
	Layers[0] = Input; // Sets the input layer
	for (int i = 1;i < LayerCount;++i) {
		for (int j = 0;j < LayerSizes[i];++j) {
			float Sum = 0;
			for (int l = 0;l < LayerSizes[i - 1];++l) {
				Sum += Layers[i - 1][l] * Weights[i-1][l][j];
			}
			Sum += Biases[i - 1][j];
			Layers[i][j] = UseActivation(Sum, Activations[i - 1]);
		}
	}
	return Layers[LayerCount - 1];
}

void FFNN::RandomizeByChance(float c, float amplitude) {
	for (int i = 1;i < LayerCount; ++i) {
		for (int l = 0;l < LayerSizes[i];++l) {
			if (rand() < RAND_MAX * c) {
				Biases[i - 1][l] += Random::RandomFloat() * amplitude;
			}
		}
		if (i > 0) {
			for (int j = 0;j < LayerSizes[i - 1]; ++j) {
				for (int l = 0;l < LayerSizes[i];++l) {
					if (rand() < RAND_MAX * c) {
						Weights[i - 1][j][l] += Random::RandomFloat() * amplitude;
					}
				}
			}
		}
	}
}