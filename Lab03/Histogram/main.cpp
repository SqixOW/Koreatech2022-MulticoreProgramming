#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <omp.h>
#include "DS_timer.h"
#include "DS_definitions.h"

const int FP_ARRAY_SIZE = 10;
const int MIN_BOUNDARY = 0;
const int MAX_BOUNDARY = 10;
const int RANGE = MAX_BOUNDARY - MIN_BOUNDARY;

int main(int argc, char** argv)
{
	std::random_device randomDevice;
	std::uniform_real_distribution<double> distr(MIN_BOUNDARY, MAX_BOUNDARY);

	DS_timer timer(4);
	timer.setTimerName(0, (char*)"Serial");
	timer.setTimerName(1, (char*)"Version1");
	timer.setTimerName(2, (char*)"Version2");
	timer.setTimerName(3, (char*)"Version3");
	
	double* randomNumberArray = new double[FP_ARRAY_SIZE] {};

	for (int i = 0; i < FP_ARRAY_SIZE; i++)
	{
		randomNumberArray[i] = round(distr(randomDevice) * 1000000) / 1000000;;
		//printf("%lf\n", randomNumberArray[i]);
	}
	

	//** 0. Serial **//

	int* serialBin = new int[RANGE] {0};
	timer.onTimer(0);
	for (int i = 0; i < FP_ARRAY_SIZE; i++)
	{
		int serialBinIndex = trunc(randomNumberArray[i]);
		serialBin[serialBinIndex] += 1;
	}
	timer.onTimer(0);
	for (int i = 0; i < RANGE; i++)
	{
		printf("[%d ~ %d) : %d\n", i, i+1, serialBin[i]);
	}

	//** 1. Version 1 - lock **//

	int* v1Bin = new int[RANGE] {0};

	//** 2. Version 2 - barrier**//

	int* v2Bin = new int[RANGE] {0};

	//** 3. Version 3 - Global Sum**//

	int* v3Bin = new int[RANGE] {0};


	//** Memory cleaning **//

	delete[] randomNumberArray;
	delete[] serialBin;
	delete[] v1Bin;
	delete[] v2Bin;
	delete[] v3Bin;

	randomNumberArray = nullptr;
	serialBin = nullptr;
	v1Bin = nullptr;
	v2Bin = nullptr;
	v3Bin = nullptr;

	EXIT_WIHT_KEYPRESS;
}