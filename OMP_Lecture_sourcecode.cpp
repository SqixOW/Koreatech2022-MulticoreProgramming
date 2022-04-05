#include <stdio.h>
#include <omp.h>
#include <Windows.h>

void omp_function_practice()
{
	#pragma omp parallel num_threads(4)
	{
		int max_thread_num = omp_get_num_threads();
		int current_thread_num = omp_get_thread_num();

		printf("%d %d\n", max_thread_num, current_thread_num);
	}
}

void if_clause_practice()
{
	omp_set_num_threads(2);
	int val = 4;
	#pragma omp parallel if(val)
	if (omp_in_parallel())
	{
		#pragma omp single
		printf_s("val = %d, parallelized with %d threads\n", val, omp_get_num_threads());
	}
	else
	{
		printf_s("val = %d, serialized\n", val);
	}
}

void loop_construct_practice()
{
	int max_threads = omp_get_max_threads();
	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < max_threads; i++)
		{
			printf("Thread %d executes loop iteration %d\n", omp_get_thread_num(), i);
		}
	}
}

void section_construct_practice()
{
	int max_threads = omp_get_max_threads();
	#pragma omp parallel num_threads(max_threads)
	{
		#pragma omp sections
		{
			#pragma omp section
			printf("Section A executed by thread %d\n", omp_get_thread_num());
			
			#pragma omp section
			printf("Section B executed by thread %d\n", omp_get_thread_num());
		}
	}
}

void single_construct_practice()
{
	int max_threads = omp_get_max_threads();
	int a;
	int b[24] = { 0 };
	#pragma omp parallel num_threads(max_threads)
	{
		#pragma omp single //nowait
		{
			Sleep(1);
			a = 10;
			printf("Single construct executed by thread %d\n", omp_get_thread_num());
			omp_get_thread_num();
		}

		#pragma omp for
		for (int i = 0; i < 24; i++)
		{
			b[i] = a;
		}
	}

	for (int i = 0; i < 24; i++)
		printf("b[%d] = %d\n", i, b[i]);
}

void shared_clause_practice()
{
	int a[24] = { 0 };
	int b[24] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	int c[24] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
	int max_threads = omp_get_max_threads();
	#pragma omp parallel num_threads(max_threads)
	{
		#pragma omp for 
		for (int i = 0; i < 24; i++)
		{
			a[i] = b[i] + c[i];
		}
	}

	for (int i = 0; i < 24; i++)
	{
		printf("%d ", a[i]);
	}
}

void private_clause_practice()
{
	int tid = 0;
	int priVar = 100;
	
	#pragma omp parallel for num_threads(4) private(tid, priVar)
	for (int i = 0; i < 4; i++)
	{
		tid = omp_get_thread_num();
		priVar = tid * 10;
		printf("[Thread %d] priVar = %d\n", tid, priVar);
	}
	printf("[After parallel region] priVar = %d\n", priVar);
}


void lastprivate_clause_practice()
{
	int tid = 0;
	int priVar = 100;

	#pragma omp parallel for num_threads(16) private(tid) lastprivate(priVar)
	for (int i = 0; i < 16; i++)
	{
		tid = omp_get_thread_num();
		priVar = tid * 10;
		printf("[Thread %d] priVar = %d\n", tid, priVar);
	}
	printf("[After parallel region] priVar = %d\n", priVar);
}

void firstprivate_clause_practice()
{
	int tid = 0;
	int priVar = 100;

#pragma omp parallel for num_threads(24) private(tid) firstprivate(priVar)
	for (int i = 0; i < 24; i++)
	{
		tid = omp_get_thread_num();
		priVar = priVar * 10 + i;
		printf("[Thread %d] priVar = %d\n", tid, priVar);
	}
	printf("[After parallel region] priVar = %d\n", priVar);
}

void barrier_clause_practice()
{
	int max_threads = 8;
	int tID = 0;
	#pragma omp parallel num_threads(max_threads) private(tID)
	{
		tID = omp_get_thread_num();
		if ((tID % 2) == 0) Sleep(10);
		printf("[%d] before \n", tID);
		#pragma omp barrier
		printf("[%d] after \n", tID);
	}
}

void barrier_step_practice()
{
	int max_threads = 4;
	int a[4] = { 0 };
	int b[16] = { 0 };
	int tID = 0;
	#pragma omp parallel num_threads(max_threads) private(tID)
	{
		tID = omp_get_thread_num();
		a[tID] = tID * 10;

		#pragma omp barrier

		#pragma omp for
		for (int i = 0; i < 16; i++)
		{
			b[i] = 2 * a[(i + 1) % 4];
		}
	}

	for (int i = 0; i < 16; i++)
	{
		printf("b[%2d] b : %2d \n", i, b[i]);
	}
}


void critical_section_clause_practice()
{
	int max_threads = omp_get_max_threads();
	int ic = 0;
	#pragma omp parallel num_threads(max_threads)
	{
		for (int i = 0; i < 1024; i++)
		{
			#pragma omp master//critical//atomic
			ic += 1;
		}
		#pragma omp barrier
	}

	printf("count = %d\n", ic);
}


void producer_consumer_practice()
{
	int buffer = 0;
	omp_lock_t lock;
	omp_init_lock(&lock);
	bool isFinish = false;
	#pragma omp parallel sections shared(isFinish, lock) num_threads(2)
	{
		#pragma omp section
		{
			int numProduce = 10;
			while (numProduce > 1)
			{
				omp_set_lock(&lock);
				if (buffer == 0)
				{
					numProduce--;
					buffer = numProduce;
					printf("Produce push %d\n", buffer);
				}
				omp_unset_lock(&lock);
				Sleep(500);
			}
			isFinish = true;
		}

		#pragma omp section
		{
			int get = 0;
			while (!isFinish)
			{
				omp_set_lock(&lock);
				if (buffer > 0)
				{
					get = buffer;
					buffer = 0;
					printf("Consumer get %d\n", get);
				}
				omp_unset_lock(&lock);
				Sleep(500);
			}
		}
	}
	omp_destroy_lock(&lock);
	printf("Finished! \n");
}

void reduction_clause_practice()
{
	int sum = 0;
	#pragma omp parallel for num_threads(4) reduction(+:sum)
	for(int i = 0 ; i < 1024; i++)
	{
		int n = i * 2;
		sum += n;
	}
	printf("%d", sum);
}

void static_clause_practice()
{
	#pragma omp parallel for schedule(static, 4) num_threads(4)
	for(int i = 0 ; i < 12 ; i++)
	{
		int tID = omp_get_thread_num();
		printf("[%d] by thread %d\n", i, tID);
	}
}

void dynamic_clause_practice()
{
#pragma omp parallel for schedule(dynamic, 1) num_threads(3)
	for (int i = 0; i < 36; i++)
	{
		int tID = omp_get_thread_num();
		printf("[%d] by thread %d\n", i, tID);
	}
}

void guided_clause_practice()
{
#pragma omp parallel for schedule(guided, 1) num_threads(3)
	for (int i = 0; i < 36; i++)
	{
		int tID = omp_get_thread_num();
		printf("[%d] by thread %d\n", i, tID);
	}
}


void nested_parallel_practice()
{
	omp_set_nested(1);
	printf("Nested Parallelism : %s\n", omp_get_nested() ? "On" : "Off");

	#pragma omp parallel num_threads(4)
	{
		int parentID = omp_get_thread_num();
		printf("Lv 1 - Thread %d\n", parentID);

		#pragma omp parallel num_threads(2)
		{
			printf("\tLv 2 - Thread %d of %d\n", omp_get_thread_num(), parentID);
		}
	}
}

void main()
{
	//omp_function_practice();
	//if_clause_practice();
	//loop_construct_practice();
	//section_construct_practice();
	//single_construct_practice();
	//shared_clause_practice();
	private_clause_practice();
	lastprivate_clause_practice();
	firstprivate_clause_practice();
	//barrier_clause_practice();
	//barrier_step_practice();
	//critical_section_clause_practice();
	//producer_consumer_practice();
	//reduction_clause_practice();
	//static_clause_practice();
	//dynamic_clause_practice();
	//guided_clause_practice();
	//nested_parallel_practice();
}
