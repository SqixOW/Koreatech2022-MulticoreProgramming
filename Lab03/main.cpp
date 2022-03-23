#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <omp.h>
#include "DS_timer.h"
#include "DS_definitions.h"

const int64_t FP_ARRAY_SIZE = 1000000;
//const int FP_ARRAY_SIZE = 1024 * 1024 * 1024;
const int MIN_BOUNDARY = 0;
const int MAX_BOUNDARY = 10;
const int RANGE = MAX_BOUNDARY - MIN_BOUNDARY;

int main(int argc, char** argv)
{
	std::random_device randomDevice;
	std::uniform_real_distribution<double> distr(MIN_BOUNDARY, MAX_BOUNDARY);
	int maxThreadNum = omp_get_max_threads();

	DS_timer timer(4);
	timer.setTimerName(0, (char*)"Serial");
	timer.setTimerName(1, (char*)"Version1");
	timer.setTimerName(2, (char*)"Version2");
	timer.setTimerName(3, (char*)"Version3");
	
	double* randomNumberArray = new double[FP_ARRAY_SIZE] {};

	//** Initialize Random Array **//

	for (int64_t i = 0; i < FP_ARRAY_SIZE; i++)
	{
		randomNumberArray[i] = round(distr(randomDevice) * 1000000) / 1000000;;
		//printf("%lf\n", randomNumberArray[i]);
	}
	

	//** 0. Serial **//

	int64_t* serialBin = new int64_t[RANGE] {0};
	timer.onTimer(0);
	for (int64_t i = 0; i < FP_ARRAY_SIZE; i++)
	{
		int serialBinIndex = int(randomNumberArray[i] * 1000000) / 1000000;
		serialBin[serialBinIndex] += 1;
	}
	timer.offTimer(0);
	printf("Serial Algorithm \n");
	for (int i = 0; i < RANGE; i++)
	{
		printf("[%d ~ %d) : %lld\n", i, i+1, serialBin[i]);
	}

	//** 1. Version 1 - Atomic **//

	int64_t* v1Bin = new int64_t[RANGE] {0};

	timer.onTimer(1);
	#pragma omp parallel for num_threads(maxThreadNum)
	for (int64_t i = 0 ; i < FP_ARRAY_SIZE; i++)
	{
		int v1BinIndex = int(randomNumberArray[i] * 1000000) / 1000000;
		#pragma omp atomic
		v1Bin[v1BinIndex] += 1;
	}
	timer.offTimer(1);

	printf("\nVersion 1 \n");
	for (int i = 0; i < RANGE; i++)
	{
		printf("[%d ~ %d) : %lld\n", i, i + 1, v1Bin[i]);
	}
	//** 2. Version 2 - barrier**//

	int64_t* v2Bin = new int64_t[RANGE] {0};

	timer.onTimer(2);
	#pragma omp parallel num_threads(maxThreadNum)
	{
		int64_t* localBin = new int64_t[RANGE]{ 0 };

		#pragma omp for
		for (int64_t i = 0; i < FP_ARRAY_SIZE; i++)
		{
			int v2BinIndex = int(randomNumberArray[i] * 1000000) / 1000000;
			localBin[v2BinIndex] += 1;
		}

		for (int i = 0; i < RANGE; i++)
		{
			#pragma omp atomic
			v2Bin[i] += localBin[i];
		}

		delete[] localBin;
		localBin = nullptr;
	}
	timer.offTimer(2);

	printf("\nVersion 2 \n");
	for (int i = 0; i < RANGE; i++)
	{
		printf("[%d ~ %d) : %lld\n", i, i + 1, v2Bin[i]);
	}

	//** 3. Version 3 - Global Sum**//
	const int OFFSET = 16;
	int64_t* v3Bin = new int64_t[RANGE * OFFSET]{ 0 };
	
	timer.onTimer(3);
	#pragma omp parallel num_threads(maxThreadNum)
	{
		int64_t* localBin = new int64_t[RANGE]{ 0 };
		int tID = omp_get_thread_num();

		#pragma omp for
		for (int64_t i = 0; i < FP_ARRAY_SIZE; i++)
		{
			int v3BinIndex = int(randomNumberArray[i] * 1000000) / 1000000;
			localBin[v3BinIndex] += 1;
		}

		int pivot = 16;
		for (int i = 0; i < RANGE; i++)
		{

		}

		delete[] localBin;
		localBin = nullptr;
	}
	timer.offTimer(3);

	printf("\nVersion 3 \n");
	for (int i = 0; i < RANGE; i++)
	{
		printf("[%d ~ %d) : %lld\n", i, i + 1, v3Bin[i]);
	}
	
	timer.printTimer();

	//** Memory delete **//

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