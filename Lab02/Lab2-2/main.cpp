#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "DS_timer.h"
#include "DS_definitions.h"

int main(int argc, char** argv)
{
	DS_timer timer(2);
	timer.setTimerName(0, (char*)"Serial");
	timer.setTimerName(1, (char*)"Parallel");

	double init_x_axis = double(strtol(argv[1], NULL, 10));
	double end_x_axis = double(strtol(argv[2], NULL, 10));
	double div = double(strtol(argv[3], NULL, 10));
	int div_index = int(div);
	if (div == 0)
	{
		printf("DivideByZeroError\n");
		return 0;
	}

	double x_range = (end_x_axis - init_x_axis) / div;
	double sum_serial = 0;
	double sum_parallel = 0;

	//** 1. Serial code **//

	timer.onTimer(0);

	for (int i = 0; i < div_index; i++)
	{
		double x1 = init_x_axis + i * x_range;
		double x2 = init_x_axis + (i + 1) * x_range;
		double area = x_range * 0.5 * (x1 * x1 + x2 * x2);
		sum_serial += area;
	}

	timer.offTimer(0);

	printf("Serial : %f\n", sum_serial);

	//** 2. Parallel code **//

	int max_threads = omp_get_max_threads();
	double* result_arr = new double[div_index] {};
	timer.onTimer(1);
	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < div_index; i++)
		{
			double x1 = init_x_axis + i * x_range;
			double x2 = init_x_axis + (i + 1) * x_range;
			double area = x_range * 0.5 * (x1 * x1 + x2 * x2);
			result_arr[i] += area;
		}
	}
	
	timer.offTimer(1);


	for (int i = 0; i < div_index; i++)
	{
		sum_parallel += result_arr[i];
	}

	printf("Parallel : %f\n", sum_parallel);
	//** 3. Result checking code **//
	if (sum_serial == sum_parallel) printf("Correct!");
	else printf("Not Correct!");

	timer.printTimer();
	EXIT_WIHT_KEYPRESS;
}