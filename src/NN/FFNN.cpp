#include <iostream>
#include <iterator>
#include <cmath>
#include <stdlib.h>
#include "FFNN.h"
#include "Activations.h"
#include "../Random.h"

FFNN::FFNN (int Layers[], AFunction Activations[], int size) {
	this->LayerCount = size;
	this->LayerSizes = Layers;
	this->Biases = new float* [size-1];
	this->Layers = new float* [size];
	this->Weights = new float** [size-1];
	this->Activations = Activations;
	for (int i = 0;i < size; ++i) {
		this->Layers[i] = new float[Layers[i]];
	}
	for (int i = 1;i < size; ++i) {
		Biases[i-1] = new float[Layers[i]];
		for (int l = 0;l < Layers[i];++l) Biases[i-1][l] = Random::RandomFloat();
		if (i > 0) {
			Weights[i-1] = new float* [Layers[i-1]];
			for (int j = 0;j < Layers[i - 1]; ++j) {
				Weights[i - 1][j] = new float[Layers[i]];
				for (int l = 0;l < Layers[i];++l) Weights[i - 1][j][l] = Random::RandomFloat();
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
			Layers[i][j] = af::UseActivation(Sum, Activations[i - 1]);
		}
	}
	return Layers[LayerCount - 1];
}