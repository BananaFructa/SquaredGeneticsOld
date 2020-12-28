#pragma once

#include <iterator>
#include <iostream>
#include <cmath>
#include <stdlib.h>

namespace AFunctions {
	enum {
		Sigmoid,
		Tanh,
		LeakyReLU
	};
}

typedef int AFunction;


class FFNN {
public:

	int LayerCount;
	int* LayerSizes;
	float** Layers;
	float** Biases;
	float*** Weights;
	AFunction* Activations;

	FFNN(int Layers[], AFunction Activations[], int size);
	FFNN(FFNN& ffnn);
	~FFNN();
	float* Predict(float* Input);
	void RandomizeByChance(float chance,float amplitude);
};