#pragma once

namespace AFunctions {
	enum {
		Sigmoid,
		Tanh,
		LeakyReLU
	};
}

typedef int AFunction;

class FFNN {
private:

public:

	int LayerCount;
	int* LayerSizes;
	float** Layers;
	float** Biases;
	float*** Weights;
	AFunction* Activations;

	FFNN(int Layers[], AFunction Activations[], int size);
	float* Predict(float Input[]);
};